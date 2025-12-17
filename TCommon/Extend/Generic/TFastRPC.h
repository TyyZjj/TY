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
// ==========================================
class TFastRPC {
public:
    // 定义通用的函数指针类型：接收参数指针数组和返回值指针
    // void** args: 指向参数地址的数组
    // void* result: 指向返回值内存的地址 (非引用返回值用于 placement new；引用返回值用于存储指针)
    using ThunkFunc = void(*)(void** args, void* result);

    // ThunkInfo 结构体未使用，但保留
    struct ThunkInfo {
        std::string name;
        ThunkFunc thunk;
        SignatureInfo signature;
    };

    static TFastRPC& Instance() {
        static TFastRPC instance;
        return instance;
    }

    // --------------------------------------------------------
    // 注册函数 (支持普通函数和成员函数)
    // --------------------------------------------------------
    template <auto FuncPtr>
    void Register(uint64_t id) {
        if (m_registry.find(id) != m_registry.end()) {
            std::cerr << "[TFastRPC] Warning: Function ID collision or duplicate registration." << std::endl;
            return;
        }
        // 生成并存储桩函数 (Thunk)
        m_registry[id] = &ThunkGenerator<FuncPtr>::Call;
    }

    // 辅助注册：使用字符串名称
    template <auto FuncPtr>
    void Register(const char* name) {
        Register<FuncPtr>(Hash(name));
    }

    // --------------------------------------------------------
    // 极速调用入口
    // Invoke<返回值类型>(函数名Hash, 参数...)
    // --------------------------------------------------------
    template <typename R = void, typename... Args>
    R Invoke(uint64_t id, Args&&... args) {
        // 1. 查找函数 (高性能 Hash Map 查找)
        auto it = m_registry.find(id);
        if (it == m_registry.end()) {
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

        ThunkFunc thunk = it->second;

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
            // 非引用返回值 (原代码不变)
            // 在栈上预留返回值的空间 (避免 any 的堆分配)
            char resultBuffer[sizeof(R)];
            thunk(argPtrs, (void*)resultBuffer);
            // 移动或拷贝构造回调用者
            // decltype(auto) 确保返回一个值类型 (int)
            return std::move(*reinterpret_cast<R*>(resultBuffer));
        }
    }

    // 字符串版本重载
    template <typename R = void, typename... Args>
    R Invoke(const char* name, Args&&... args) {
        return Invoke<R>(Hash(name), std::forward<Args>(args)...);
    }

private:
    std::unordered_map<uint64_t, ThunkFunc> m_registry;

    // 辅助函数：类型还原
    template<typename T>
    static decltype(auto) restore(void* ptr) {
        // 将 void* 转换回 T& 或 T&& (如果 T 是引用类型)
        // 这里的 remove_reference 是关键，确保 static_cast 的目标是指针类型
        return *static_cast<std::remove_reference_t<T>*>(ptr);
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

#endif // _TFAST_RPC_H_
#endif