#if 1
#ifndef _TFAST_RPC_H_
#define _TFAST_RPC_H_

#include <vector>
#include <iostream>
#include <tuple>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <utility> // for std::forward, std::addressof
#include "TFuncTraits.h"

// ==========================================
// 1. 编译期字符串哈希 (FNV-1a 算法)
// 用于将函数名 "FuncName" 转换为唯一的 uint64_t ID
// ==========================================
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

constexpr uint64_t Hash(const char* const str, const uint64_t value = val_64_const) noexcept {
    return (str[0] == '\0') ? value : Hash(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
}

// 方便的字面量操作符: "FuncName"_hash
constexpr uint64_t operator"" _hash(const char* str, size_t) noexcept {
    return Hash(str);
}

// ==========================================
// 2. 核心调用器实现
// 不支持多态（继承关系）： 如果你注册了 void Func(Base* b)，但调用 Invoke(id, new Derived())，由于 typeid(Base*) != typeid(Derived*)，匹配会失败。C++ 原生支持这种调用，但本库不支持
// 不支持隐式转换： 注册 void Func(double d)，调用 Invoke(id, 1) (int)，匹配失败
// 出于性能考虑，m_simple_registry和m_registry没有加锁, 确保所有 Register 调用都在单线程初始化阶段完成
// ==========================================
class TFastRPC {
public:
    // 定义通用的函数指针类型：接收参数指针数组和返回值指针
    // void** args: 指向参数地址的数组
    // void* result: 指向返回值内存的地址 (非引用返回值用于 placement new；引用返回值用于存储指针)
    using ThunkFunc = void(*)(void** args, void* result);

    struct ThunkInfo {
		std::string name;   // 函数名称 (仅用于调试)
        ThunkFunc thunk;    // 函数桩 (Thunk) 指针
		SignatureInfo signature; // 函数签名信息
    };

    static TFastRPC& Instance() {
        static TFastRPC instance;
        return instance;
    }

    // --------------------------------------------------------
    // 注册函数 (支持普通函数和成员函数) 同一个name只能注册一个函数,函数重载必须使用不同的name
    // --------------------------------------------------------
    template <auto FuncPtr>
    void Register(const char* name, uint64_t id) {
        if (m_simple_registry.find(id) != m_simple_registry.end()) {
            std::cerr << "[TFastRPC] Warning: Function ID collision or duplicate registration." << std::endl;
            return;
        }
        // 生成并存储桩函数 (Thunk)
		ThunkInfo thunkInfo;
		thunkInfo.name = name; // 未使用名称
		thunkInfo.thunk = &ThunkGenerator<FuncPtr>::Call;
        auto signature = FuncTraits<decltype(FuncPtr)>::get_signature_info();
		thunkInfo.signature = signature;
        m_simple_registry[id] = thunkInfo;
    }

    // 辅助注册：使用字符串名称
    template <auto FuncPtr>
    void Register(const char* name) {
        Register<FuncPtr>(name, Hash(name));
    }

    template <auto FuncPtr>
    void RegisterOverload(const char* name, uint64_t id)
    {
        //if (m_registry.find(id) != m_registry.end()) {
        //    std::cerr << "[TFastRPC] Warning: Function ID collision or duplicate registration." << std::endl;
        //    return;
        //}
		auto& signatureFuncMap = m_registry[id];
		auto signature = FuncTraits<decltype(FuncPtr)>::get_signature_info();
		auto it = signatureFuncMap.find(signature);
		if (it != signatureFuncMap.end()) {
			std::cerr << "[TFastRPC] Warning: Function signature collision or duplicate registration. name:" <<
				it->second.name << "," << signature.toString() << std::endl;
			return;
		}
		ThunkInfo thunkInfo;
		thunkInfo.name = name; // 未使用名称
		thunkInfo.thunk = &ThunkGenerator<FuncPtr>::Call;
		thunkInfo.signature = signature;
		signatureFuncMap[signature] = thunkInfo;
    }

    // 注册函数 (支持普通函数和成员函数) 同一个name可以对应多个函数重载, 但是函数调用时不适用高性能的Hash Map查找
    template <auto FuncPtr>
    void RegisterOverload(const char* name) {
        RegisterOverload<FuncPtr>(name, Hash(name));
    }

    // --------------------------------------------------------
    // 极速调用入口, 同一种name只能注册一个函数
    // Invoke<返回值类型>(函数名Hash, 参数...)
    // --------------------------------------------------------
    template <typename R = void, typename... Args>
    R Invoke(uint64_t id, Args&&... args) {
        // 1. 查找函数 (高性能 Hash Map 查找)
        auto it = m_simple_registry.find(id);
        if (it == m_simple_registry.end()) {
            std::cerr << "[TFastRPC] Error: Function not found!" << std::endl;
            if constexpr (std::is_void_v<R>) {
                return;
            }
            // 修正点：非 void 返回值
            else {
                // 对于值类型（R=int），返回 int{} (0)
                if constexpr (!std::is_reference_v<R>) {
                    return R{};
                }
                // 对于引用类型（R=int&），安全做法是抛异常或返回默认值
                else {
                    // ⚠️ 注意：返回引用必须绑定到一个对象。
                    // 由于找不到函数，这里无法返回有效的引用，因此抛出异常是最好的选择。
                    throw std::runtime_error("RPC function not found and cannot return a default reference.");
                }
            }
        }

        ThunkFunc thunk = it->second.thunk;

        if constexpr (sizeof...(Args) > 0)
        {
            // 2. 构造参数指针栈数组 (Zero Allocation)
            void* argPtrs[] = { (void*)std::addressof(args)... };

            // 3. 处理返回值与调用
            if constexpr (std::is_void_v<R>) {
                thunk(argPtrs, nullptr);
            }
            else if constexpr (std::is_reference_v<R>) { // 修正点：处理引用返回值
                // 引用返回值只存储一个指针
                char resultBuffer[sizeof(void*)];
                thunk(argPtrs, (void*)resultBuffer);

                // 取出存储的地址，并解引用为 R (例如 int&)
                void* returned_ptr = *reinterpret_cast<void**>(resultBuffer);
                // 强转为 R 的实际类型指针，并解引用
                // decltype(auto) 确保返回一个引用 (int&)
                return *static_cast<std::remove_reference_t<R>*>(returned_ptr);
            }
            else {
                //// 非引用返回值 (原代码不变)
                //// 在栈上预留返回值的空间 (避免 any 的堆分配)
                //char resultBuffer[sizeof(R)];
                //thunk(argPtrs, (void*)resultBuffer);
                //// 移动或拷贝构造回调用者
                //// decltype(auto) 确保返回一个值类型 (int)
                //return std::move(*reinterpret_cast<R*>(resultBuffer));
                // 栈上分配空间
                alignas(R) char resultBuffer[sizeof(R)];//如果 R 是一个不可移动且不可拷贝的类型（或者移动构造函数被删除了）,这段代码将无法编译或产生错误行为
                thunk(argPtrs, (void*)resultBuffer);

                R* obj_ptr = reinterpret_cast<R*>(resultBuffer);
                R ret_val = std::move(*obj_ptr);
                obj_ptr->~R();

                return ret_val;
            }
        }
        else
        {
            void* argPtrs[] = { nullptr };//args不能为空
            if constexpr (std::is_void_v<R>) {
                thunk(argPtrs, nullptr);
            }
            else if constexpr (std::is_reference_v<R>) { // 修正点：处理引用返回值
                char resultBuffer[sizeof(void*)];
                thunk(argPtrs, (void*)resultBuffer);
                void* returned_ptr = *reinterpret_cast<void**>(resultBuffer);
                return *static_cast<std::remove_reference_t<R>*>(returned_ptr);
            }
            else {
                alignas(R) char resultBuffer[sizeof(R)];//如果 R 是一个不可移动且不可拷贝的类型（或者移动构造函数被删除了）,这段代码将无法编译或产生错误行为
                thunk(argPtrs, (void*)resultBuffer);

                R* obj_ptr = reinterpret_cast<R*>(resultBuffer);
                R ret_val = std::move(*obj_ptr);
                obj_ptr->~R();

                return ret_val;
            }
        }
    }

    // 字符串版本重载
    template <typename R = void, typename... Args>
    R Invoke(const char* name, Args&&... args) {
        return Invoke<R>(Hash(name), std::forward<Args>(args)...);
    }

    //考虑到性能, 此功能已舍弃, 不再支持注册重载的函数, 重载的函数请使用不同的name注册 20251218
    template <typename R = void, typename... Args>
    R InvokeOverload(uint64_t id, Args&&... args)
    {
        auto it = m_registry.find(id);
        if (it == m_registry.end())
        {
            std::cerr << "[TFastRPC] Error: name id not found!" << std::endl;
            if constexpr (std::is_void_v<R>)
            {
                return;
            }
            // 修正点：非 void 返回值
            else
            {
                // 对于值类型（R=int），返回 int{} (0)
                if constexpr (!std::is_reference_v<R>)
                {
                    return R{};
                }
                // 对于引用类型（R=int&），安全做法是抛异常或返回默认值
                else
                {
                    // ⚠️ 注意：返回引用必须绑定到一个对象。
                    // 由于找不到函数，这里无法返回有效的引用，因此抛出异常是最好的选择。
                    throw std::runtime_error("RPC function not found and cannot return a default reference.");
                }
            }
        }

        SignatureFuncMap & signatureFuncMap = it->second;
        //遍历signatureFuncMap
        for (auto it2 = signatureFuncMap.begin(); it2 != signatureFuncMap.end(); ++it2)
        {
            ThunkInfo& thunkInfo = it2->second;
			bool bMatch = SignatureMatcher::Match<R, Args...>(thunkInfo.signature);
			if (bMatch)
			{
                if constexpr (sizeof...(Args) > 0)
                {
                    void* argPtrs[sizeof...(Args)] = { (void*)std::addressof(args)... };//args不能为空

                    if constexpr (std::is_void_v<R>)
                    {
                        thunkInfo.thunk(argPtrs, nullptr);
                        return;
                    }
                    else if constexpr (std::is_reference_v<R>)
                    { // 修正点：处理引用返回值
                        // 引用返回值只存储一个指针
                        char resultBuffer[sizeof(void*)];
                        thunkInfo.thunk(argPtrs, (void*)resultBuffer);

                        // 取出存储的地址，并解引用为 R (例如 int&)
                        void* returned_ptr = *reinterpret_cast<void**>(resultBuffer);
                        // 强转为 R 的实际类型指针，并解引用
                        // decltype(auto) 确保返回一个引用 (int&)
                        return *static_cast<std::remove_reference_t<R>*>(returned_ptr);
                    }
                    else
                    {
#pragma region 之前的代码, 会导致内存泄漏
                        //// 非引用返回值 (原代码不变)
                        //// 在栈上预留返回值的空间 (避免 any 的堆分配)
                        //char resultBuffer[sizeof(R)];
                        //thunkInfo.thunk(argPtrs, (void*)resultBuffer);

                        //// 移动或拷贝构造回调用者
                        //// decltype(auto) 确保返回一个值类型 (int)
                        //return std::move(*reinterpret_cast<R*>(resultBuffer));
#pragma endregion
                    // 栈上分配空间
                        alignas(R) char resultBuffer[sizeof(R)];//如果 R 是一个不可移动且不可拷贝的类型（或者移动构造函数被删除了）,这段代码将无法编译或产生错误行为
                        thunkInfo.thunk(argPtrs, (void*)resultBuffer);

                        R* obj_ptr = reinterpret_cast<R*>(resultBuffer);
                        R ret_val = std::move(*obj_ptr);
                        obj_ptr->~R();

                        return ret_val;
                    }
                }  
                else
                {
                    void* argPtrs[] = { nullptr };//args不能为空
                    if constexpr (std::is_void_v<R>)
                    {
                        thunkInfo.thunk(argPtrs, nullptr);
                        return;
                    }
                    else if constexpr (std::is_reference_v<R>)
                    {
                        char resultBuffer[sizeof(void*)];
                        thunkInfo.thunk(argPtrs, (void*)resultBuffer);
                        void* returned_ptr = *reinterpret_cast<void**>(resultBuffer);
                        return *static_cast<std::remove_reference_t<R>*>(returned_ptr);
                    }
                    else
                    {
                    // 栈上分配空间
                        alignas(R) char resultBuffer[sizeof(R)];//如果 R 是一个不可移动且不可拷贝的类型（或者移动构造函数被删除了）,这段代码将无法编译或产生错误行为
                        thunkInfo.thunk(argPtrs, (void*)resultBuffer);
                        R* obj_ptr = reinterpret_cast<R*>(resultBuffer);
                        R ret_val = std::move(*obj_ptr);
                        obj_ptr->~R();

                        return ret_val;
                    }
                }
			}
        }

        std::cerr << "[TFastRPC] Error: function not found!" << std::endl;
        if constexpr (std::is_void_v<R>)
        {
            return;
        }
        // 修正点：非 void 返回值
        else
        {
            // 对于值类型（R=int），返回 int{} (0)
            if constexpr (!std::is_reference_v<R>)
            {
                return R{};
            }
            // 对于引用类型（R=int&），安全做法是抛异常或返回默认值
            else
            {
                // ⚠️ 注意：返回引用必须绑定到一个对象。
                // 由于找不到函数，这里无法返回有效的引用，因此抛出异常是最好的选择。
                throw std::runtime_error("RPC function not found and cannot return a default reference.");
            }
        }
    }

    template <typename R = void, typename... Args>
    R InvokeOverload(const char* name, Args&&... args)
    {
        return InvokeOverload<R>(Hash(name), std::forward<Args>(args)...);
    }

private:
    std::unordered_map<uint64_t, ThunkInfo> m_simple_registry;

    //需要极致性能的不要调用
    typedef std::unordered_map<SignatureInfo, ThunkInfo> SignatureFuncMap;
	//typedef std::vector<ThunkInfo> ThunkInfoList;
	std::unordered_map<uint64_t, SignatureFuncMap> m_registry; 
    //std::unordered_map <uint64_t, ThunkInfoList> m_registry;

#pragma region 之前的代码, 会导致右值引用参数无法正确传递
    //// 辅助函数：类型还原
    //template<typename T>
    //static decltype(auto) restore(void* ptr) {
    //    // 将 void* 转换回 T& 或 T&& (如果 T 是引用类型)
    //    // 这里的 remove_reference 是关键，确保 static_cast 的目标是指针类型
    //    return *static_cast<std::remove_reference_t<T>*>(ptr);
    //}
#pragma endregion
    template<typename T>
    static T&& restore(void* ptr) { // 返回 T&&，利用引用折叠
        // 先转回原始类型的指针，解引用得到左值，再强转为 T&&
        return static_cast<T&&>(*static_cast<std::remove_reference_t<T>*>(ptr));
    }

    // ==========================================
    // Thunk 生成器：利用模板元编程自动展开参数
    // ==========================================
    template <auto Func>
    struct ThunkGenerator;

    // 特化：普通函数指针
    template <typename R, typename... Params, R(*Func)(Params...)>
    struct ThunkGenerator<Func> {
        static void Call(void** args, void* result) {
            CallImpl(args, result, std::make_index_sequence<sizeof...(Params)>{});
        }

        template <size_t... Is>
        static void CallImpl(void** args, void* result, std::index_sequence<Is...>) {
            if constexpr (std::is_void_v<R>) {
                Func(restore<Params>(args[Is])...);
            }
            else if constexpr (std::is_reference_v<R>) { // 修正点：引用返回值
                decltype(auto) returned_ref = Func(restore<Params>(args[Is])...);
                // 将引用所指对象的地址存储在 result 缓冲区 (void*)
                void* returned_ptr = static_cast<void*>(std::addressof(returned_ref));
                *static_cast<void**>(result) = returned_ptr;
            }
            else {
                // 非引用返回值：使用 placement new 构造
                new (result) R(Func(restore<Params>(args[Is])...));
            }
        }
    };

    // 特化：非 const 成员函数指针 (第一个参数必须是对象指针)
    template <typename C, typename R, typename... Params, R(C::* Func)(Params...)>
    struct ThunkGenerator<Func> {
        static void Call(void** args, void* result) {
            CallImpl(args, result, std::make_index_sequence<sizeof...(Params)>{});
        }
        template <size_t... Is>
        static void CallImpl(void** args, void* result, std::index_sequence<Is...>) {
            void* this_ptr = args[0];
            if (this_ptr == nullptr) 
            {
                throw std::invalid_argument("[TFastRPC] Error: 'this' pointer is null for member function call.");
                // 或者如果不使用异常，可以在这里做错误标记，但通常 RPC 遇到这种情况无法继续
            }
            C* obj = static_cast<C*>(args[0]); // args[0] 是 this 指针
            if constexpr (std::is_void_v<R>) {
                (obj->*Func)(restore<Params>(args[Is + 1])...); // 参数偏移+1
            }
            else if constexpr (std::is_reference_v<R>) { // 修正点：引用返回值
                decltype(auto) returned_ref = (obj->*Func)(restore<Params>(args[Is + 1])...);
                void* returned_ptr = static_cast<void*>(std::addressof(returned_ref));
                *static_cast<void**>(result) = returned_ptr;
            }
            else {
                new (result) R((obj->*Func)(restore<Params>(args[Is + 1])...));
            }
        }
    };

    // 特化：const 成员函数指针
    template <typename C, typename R, typename... Params, R(C::* Func)(Params...) const>
    struct ThunkGenerator<Func> {
        static void Call(void** args, void* result) {
            CallImpl(args, result, std::make_index_sequence<sizeof...(Params)>{});
        }

        template <size_t... Is>
        static void CallImpl(void** args, void* result, std::index_sequence<Is...>) {
            void* this_ptr = args[0];
            if (this_ptr == nullptr)
            {
                throw std::invalid_argument("[TFastRPC] Error: 'this' pointer is null for member function call.");
                // 或者如果不使用异常，可以在这里做错误标记，但通常 RPC 遇到这种情况无法继续
            }
            const C* obj = static_cast<const C*>(args[0]); // args[0] 强转为 const C*
            if constexpr (std::is_void_v<R>) {
                (obj->*Func)(restore<Params>(args[Is + 1])...);
            }
            else if constexpr (std::is_reference_v<R>) { // 修正点：引用返回值
                decltype(auto) returned_ref = (obj->*Func)(restore<Params>(args[Is + 1])...);
                void* returned_ptr = static_cast<void*>(std::addressof(returned_ref));
                *static_cast<void**>(result) = returned_ptr;
            }
            else {
                new (result) R((obj->*Func)(restore<Params>(args[Is + 1])...));
            }
        }
    };
};

//Sample
/*
int &Add(int &a, int &b)
{
    b++;
    return b;
}
TestApp* TestApp::newTestApp(TestApp* p){return p;};
TestApp& TestApp::refTestApp(TestApp &app){return app;};
TestApp& TestApp::refTestApp(TestApp& app, int a){return app;}
static int TestApp::GetTestIndex() {return 1;}
TFastRPC::Instance().Register<&Add>("Add");
TFastRPC::Instance().Register<static_cast<TestApp& (TestApp::*)(TestApp&)>(&TestApp::refTestApp)>("refTestApp");
TFastRPC::Instance().Register<&TestApp::newTestApp>("newTestApp");

TFastRPC::Instance().RegisterOverload<&TestApp::GetTestIndex>("GetTestIndex");
TFastRPC::Instance().RegisterOverload<&TestApp::newTestApp>("newTestApp");
TFastRPC::Instance().RegisterOverload<static_cast<TestApp& (TestApp::*)(TestApp&)>(&TestApp::refTestApp)>("refTestApp");
TFastRPC::Instance().RegisterOverload<static_cast<TestApp& (TestApp::*)(TestApp&, int)>(&TestApp::refTestApp)>("refTestApp");


TFastRPC::Instance().Invoke<int&>("Add", b, b);
TestApp& app_ref = TFastRPC::Instance().Invoke<TestApp&>("refTestApp", this, *this);
TestApp* app_ptr = TFastRPC::Instance().Invoke<TestApp*>("newTestApp", this, this);

int a = TFastRPC::Instance().InvokeOverload<int>("GetTestIndex");
TestApp& app_ref2 = TFastRPC::Instance().InvokeOverload<TestApp&>("refTestApp", this, *this);
TestApp& app_ref3 = TFastRPC::Instance().InvokeOverload<TestApp&>("refTestApp", this, *this, 1);
TestApp* app_ptr4 = TFastRPC::Instance().InvokeOverload<TestApp*>("newTestApp", this, this);
*/


#endif // _TFAST_RPC_H_
#endif