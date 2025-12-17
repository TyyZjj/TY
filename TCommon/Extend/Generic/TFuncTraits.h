#pragma once
#include <any>
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <type_traits>

// ==========================================================
// 用于存储函数签名的信息
// Taoyuyu 20251201
// 针对普通函数指针和成员函数指针进行提取函数的签名信息, 包括返回值类型和参数类型列表
// ==========================================================

enum class nRefType {
    Value = 0,  // T
    LValueRef,  // T&
    RValueRef   // T&&
};

struct TypeInfo {
    std::type_index type = typeid(void);
    nRefType ref_category = nRefType::Value;
    bool is_const = false;

    // 辅助对比
    bool operator==(const TypeInfo& other) const {
        return type == other.type && ref_category == other.ref_category && is_const == other.is_const;
    }
};

template <typename T>
struct TypeExtractor {
    static TypeInfo get() {
        TypeInfo info;
        // 1. 提取基础类型（去掉引用和 const）
        using BaseType = std::remove_cv_t<std::remove_reference_t<T>>;
        info.type = typeid(BaseType);

        // 2. 识别引用类型
        if (std::is_lvalue_reference_v<T>) {
            info.ref_category = nRefType::LValueRef;
        }
        else if (std::is_rvalue_reference_v<T>) {
            info.ref_category = nRefType::RValueRef;
        }
        else {
            info.ref_category = nRefType::Value;
        }

        // 3. 识别是否为 const (注意：需要针对引用和非引用分别判断)
        // 对于引用类型，我们需要看它指向的类型是否为 const
        using NonRefT = std::remove_reference_t<T>;
        if (std::is_const_v<NonRefT>) {
            info.is_const = true;
        }

        return info;
    }
};


// 用于存储函数签名信息的结构体 todo Taoyuyu 是否要添加所处的类型, 以便于区分普通函数和成员函数???
struct SignatureInfo {
    //std::type_index class_type = typeid(void);// 所属类类型, 普通函数为 void 20251204 Taoyuyu
    //std::type_index return_type = typeid(void);
    //std::vector<std::type_index> param_types;
    //std::type_index class_type_pointer = typeid(void*);

	TypeInfo class_type_info;
    TypeInfo return_type_info;
    std::vector<TypeInfo> param_types_info;
    TypeInfo class_type_pointer_info;

    // 为了方便，定义一个简单的比较运算符
    bool operator==(const SignatureInfo& other) const
    {
        //return class_type == other.class_type &&
        //    return_type == other.return_type &&
        //    param_types == other.param_types;
        return class_type_info == other.class_type_info &&
            return_type_info == other.return_type_info &&
			param_types_info == other.param_types_info;
    }

    bool operator!=(const SignatureInfo& other) const
    {
        //return class_type != other.class_type ||
        //    return_type != other.return_type ||
        //    param_types != other.param_types;
		return class_type_info != other.class_type_info ||
			return_type_info != other.return_type_info ||
			param_types_info != other.param_types_info;
    }

    // 用于比较两个 SignatureInfo 是否相等，不考虑所属类类型 r:return_type p:param_types
    bool rpEquals(const SignatureInfo& other) const
    {
        //return return_type == other.return_type &&
        //    param_types == other.param_types;
        return return_type_info == other.return_type_info &&
			param_types_info == other.param_types_info;
    }

    // 辅助调试打印
    std::string toString() const {
        std::string s = "Class: " + std::string(class_type_info.type.name()) +
            " | Ret: " + std::string(return_type_info.type.name()) + " | Args: [";
        for (auto& t : param_types_info) s += std::string(t.type.name()) + " ";
        s += "]";
        return s;
    }
};

// 基础模板
template <typename T>
struct FuncTraits {
    using return_type = void;
};

// =========================================================
// 宏定义：用于快速生成 普通函数 / noexcept / 成员函数 的特化
// =========================================================

// 1. 普通函数指针 & 静态成员函数
#define SPECIALIZE_FREE_FUNC(IS_NOEXCEPT) \
template <typename R, typename... Args> \
struct FuncTraits<R(*)(Args...) IS_NOEXCEPT> { \
    using class_type = void; \
    static constexpr size_t nArgs = sizeof...(Args); \
    using return_type = R; \
    using args_tuple = std::tuple<Args...>;\
	template <size_t N>\
    using arg_type = std::tuple_element_t<N, args_tuple>;\
    static SignatureInfo get_signature_info() { \
        return SignatureInfo{ \
            /*std::type_index(typeid(void)),*/ \
            /*std::type_index(typeid(R)),*/ \
            /*{ std::type_index(typeid(std::decay_t<Args>))... },*/ \
            /*std::type_index(typeid(void*)),*/ \
            TypeExtractor<void>::get(),\
			TypeExtractor<R>::get(),\
			{ TypeExtractor<Args>::get()... },\
			TypeExtractor<void*>::get() \
        }; \
    } \
};

// 2. 成员函数指针 (Const / Non-Const)
#define SPECIALIZE_MEM_FUNC(CV_QUALIFIER, IS_NOEXCEPT) \
template <typename C, typename R, typename... Args> \
struct FuncTraits<R(C::*)(Args...) CV_QUALIFIER IS_NOEXCEPT> { \
    using class_type = C; \
    static constexpr size_t nArgs = sizeof...(Args); \
    using return_type = R; \
    using args_tuple = std::tuple<Args...>;\
    template <size_t N>\
    using arg_type = std::tuple_element_t<N, args_tuple>;\
    static SignatureInfo get_signature_info() { \
        return SignatureInfo{ \
            /*std::type_index(typeid(C)),*/ \
            /*std::type_index(typeid(R)),*/ \
            /*{ std::type_index(typeid(std::decay_t<Args>))... },*/ \
            /*std::type_index(typeid(C*)),*/ \
			TypeExtractor<C>::get(),\
			TypeExtractor<R>::get(),\
			{ TypeExtractor<Args>::get()... },\
			TypeExtractor<C*>::get() \
        }; \
    } \
};

// C++17 noexcept 支持
#if __cplusplus >= 201703L
#define NOEXCEPT_SPEC noexcept
#else
#define NOEXCEPT_SPEC
#endif

// 1. 普通函数 / 静态成员函数
SPECIALIZE_FREE_FUNC()              // R(*)(Args...)
#if __cplusplus >= 201703L
SPECIALIZE_FREE_FUNC(noexcept)       // R(*)(Args...) noexcept
#endif

// 2. 成员函数 (非 const)
SPECIALIZE_MEM_FUNC(, )             // R(C::*)(Args...)
#if __cplusplus >= 201703L
SPECIALIZE_MEM_FUNC(, noexcept)     // R(C::*)(Args...) noexcept
#endif

// 3. 成员函数 (const)
SPECIALIZE_MEM_FUNC(const, )         // R(C::*)(Args...) const
#if __cplusplus >= 201703L
SPECIALIZE_MEM_FUNC(const, noexcept) // R(C::*)(Args...) const noexcept
#endif

// 清理宏
#undef SPECIALIZE_FREE_FUNC
#undef SPECIALIZE_MEM_FUNC
#undef NOEXCEPT_SPEC

#if 0 //20251217之前的版本
// 用于存储函数签名信息的结构体 todo Taoyuyu 是否要添加所处的类型, 以便于区分普通函数和成员函数???
struct SignatureInfo {
	std::type_index class_type = typeid(void);// 所属类类型, 普通函数为 void 20251204 Taoyuyu
    std::type_index return_type = typeid(void);
    std::vector<std::type_index> param_types;
    std::type_index class_type_pointer = typeid(void*);

    // 为了方便，定义一个简单的比较运算符
    bool operator==(const SignatureInfo& other) const 
    {
        return class_type == other.class_type &&
            return_type == other.return_type &&
            param_types == other.param_types;
    }
    
    bool operator!=(const SignatureInfo& other) const 
	{
		return class_type != other.class_type ||
			return_type != other.return_type ||
			param_types != other.param_types;
	}

    // 用于比较两个 SignatureInfo 是否相等，不考虑所属类类型 r:return_type p:param_types
    bool rpEquals(const SignatureInfo& other) const
    {
        return return_type == other.return_type &&
			param_types == other.param_types;
    }

    // 辅助调试打印
    std::string ToString() const {
        std::string s = "Class: " + std::string(class_type.name()) +
            "Ret: " + std::string(return_type.name()) + ", Args: [";
        for (auto& t : param_types) s += std::string(t.name()) + " ";
        s += "]";
        return s;
    }
};

// 定义基础模板（未定义实现，强制匹配失败）
template <typename T>
struct FuncTraits
{
	using return_type = void;
    return_type* ret = nullptr;
};

// 针对【普通函数指针】进行偏特化匹配
// R 是返回值，Args... 是参数包
template <typename R, typename... Args>
struct FuncTraits<R(*)(Args...)> {
    using class_type = void;
    // 获取参数个数
    static constexpr size_t nArgs = sizeof...(Args);
    // 获取返回值类型
    using return_type = R;
    // 将参数类型存入 std::tuple 以便后续提取
    using args_tuple = std::tuple<Args...>;
    // 辅助工具：获取第 N 个参数的类型
    template <size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;

    // 静态方法：提取编译期签名信息并封装为 SignatureInfo
    static SignatureInfo get_signature_info() {
        return SignatureInfo{
            std::type_index(typeid(void)),  // 提取所属类类型，这里为 void，因为普通函数没有所属类
            std::type_index(typeid(R)),  // 提取返回值类型
            // 提取所有参数类型 (使用参数包展开)
            //{ std::type_index(typeid(Args))... }
            // 注意：这里使用 std::decay_t，因为 std::any 存储的是退化后的类型 Taoyuyu 20251201
            // 这样 func(const int&) 和 func(int) 都能匹配到 std::any(int)
            { std::type_index(typeid(std::decay_t<Args>))... },
            std::type_index(typeid(void*))
        };
    }
    return_type* ret = nullptr;
};

// ======================= 针对【非 const 成员 函数指针】进行偏特化匹配 Taoyuyu 20251204补充=======================
// T 为 R(C::*)(Args...)，其中 C 是类名
template <typename C, typename R, typename... Args>
struct FuncTraits<R(C::*)(Args...)> 
{
    // 成员函数需要知道所属的类类型
    using class_type = C;
    // 获取参数个数
    static constexpr size_t nArgs = sizeof...(Args);
    // 获取返回值类型
    using return_type = R;
    // 将参数类型存入 std::tuple
    using args_tuple = std::tuple<Args...>;

    template <size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;
    
    // 静态方法：提取编译期签名信息并封装为 SignatureInfo
    static SignatureInfo get_signature_info() {
        return SignatureInfo{
            std::type_index(typeid(C)),
            std::type_index(typeid(R)),  
            { std::type_index(typeid(std::decay_t<Args>))... },
            std::type_index(typeid(C*))
        };
    }
    return_type* ret = nullptr;
};

// ======================= 针对【const 成员函数指针】进行偏特化匹配 Taoyuyu 20251204补充=======================
// T 为 R(C::*)(Args...) const
template <typename C, typename R, typename... Args>
struct FuncTraits<R(C::*)(Args...) const> {
    using class_type = C;
    // 获取参数个数
    static constexpr size_t nArgs = sizeof...(Args);
    // 获取返回值类型
    using return_type = R;
    // 将参数类型存入 std::tuple
    using args_tuple = std::tuple<Args...>;

    template <size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;
    static SignatureInfo get_signature_info() {
        return SignatureInfo{
            std::type_index(typeid(C)),
            std::type_index(typeid(R)),
            { std::type_index(typeid(std::decay_t<Args>))... },
            std::type_index(typeid(C*))
        };
    }
    return_type* ret = nullptr;
};
#endif

#pragma region next

// --- 1. 基础模板：处理普通值类型 T (如 int, MyType) ---
template <typename R>
struct ReturnWrapper {
    // 存储类型：存储 R 的值拷贝
    using storage_type = R;

    // 包装函数：将原始返回值 r 复制到 storage_type 中
    static storage_type wrap(R r) {
        // 
        return r; // 执行值拷贝
    }
};

// --- 2. 偏特化：处理引用类型 T& (如 int&, const MyType&) ---
template <typename R>
struct ReturnWrapper<R&> {
    // 存储类型：使用 std::reference_wrapper<R> 保持引用语义
    using storage_type = std::reference_wrapper<R>;

    // 包装函数：将引用 r 包装起来
    static storage_type wrap(R& r) {
        // 
        return std::ref(r); // 返回 std::reference_wrapper 对象
    }
};

// --- 3. 偏特化：处理右值引用类型 T&& (如 int&&) ---
template <typename R>
struct ReturnWrapper<R&&> {
    // 通常右值引用返回用于移动语义，但为了统一，我们在此也存储引用包装器
    // 另一种选择是使用 std::decay_t<R> 并直接移动语义，但在此选择存储引用包装器
    using storage_type = std::reference_wrapper<R>;

    static storage_type wrap(R&& r) {
        // 对于右值引用，通常我们不希望长期持有，但如果必须，std::ref 也能工作
        return std::ref(r);
    }
};

// --- 4. 偏特化：处理指针类型 T* (如 int*, MyType*) ---
template <typename R>
struct ReturnWrapper<R*> {
    // 存储类型：直接存储原始指针 R*
    using storage_type = R*;

    // 包装函数：直接返回指针 p
    static storage_type wrap(R* p) {
        return p;
    }
};

template <typename T>
struct TFuncTraits;

// 针对【普通函数指针】进行偏特化匹配
// R 是返回值，Args... 是参数包
template <typename R, typename... Args>
struct TFuncTraits<R(*)(Args...)> {
    using class_type = void;
    // 获取参数个数
    static constexpr size_t nArgs = sizeof...(Args);
    // 获取返回值类型
    using return_type = R;
    // 将参数类型存入 std::tuple 以便后续提取
    using args_tuple = std::tuple<Args...>;
    // 辅助工具：获取第 N 个参数的类型
    template <size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;
};

// ======================= 针对【非 const 成员 函数指针】进行偏特化匹配 Taoyuyu 20251204补充=======================
// T 为 R(C::*)(Args...)，其中 C 是类名
template <typename C, typename R, typename... Args>
struct TFuncTraits<R(C::*)(Args...)>
{
    // 成员函数需要知道所属的类类型
    using class_type = C;
    // 获取参数个数
    static constexpr size_t nArgs = sizeof...(Args);
    // 获取返回值类型
    using return_type = R;
    // 将参数类型存入 std::tuple
    using args_tuple = std::tuple<Args...>;

    template <size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;
};

// ======================= 针对【const 成员函数指针】进行偏特化匹配 Taoyuyu 20251204补充=======================
// T 为 R(C::*)(Args...) const
template <typename C, typename R, typename... Args>
struct TFuncTraits<R(C::*)(Args...) const> {
    using class_type = C;
    // 获取参数个数
    static constexpr size_t nArgs = sizeof...(Args);
    // 获取返回值类型
    using return_type = R;
    // 将参数类型存入 std::tuple
    using args_tuple = std::tuple<Args...>;

    template <size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;
};
#pragma endregion