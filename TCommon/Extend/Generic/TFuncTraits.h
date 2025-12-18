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

#pragma region 函数签名
// 哈希组合函数：参考 Boost 实现
template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    // 0x9e3779b9 是黄金分割比，防止哈希碰撞聚集
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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

// 针对 TypeInfo 的哈希器
struct TypeInfoHasher {
    std::size_t operator()(const TypeInfo& info) const {
        std::size_t seed = 0;
        hash_combine(seed, info.type);
        hash_combine(seed, static_cast<int>(info.ref_category));
        hash_combine(seed, info.is_const);
        return seed;
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

struct SignatureHasher {
    std::size_t operator()(const SignatureInfo& info) const {
        std::size_t seed = 0;
        TypeInfoHasher typeHasher;

        // 1. 哈希所属类
        hash_combine(seed, typeHasher(info.class_type_info));

        // 2. 哈希返回值
        hash_combine(seed, typeHasher(info.return_type_info));

        // 3. 哈希参数列表 (Order Matters!)
        for (const auto& param : info.param_types_info) {
            hash_combine(seed, typeHasher(param));
        }

        return seed;
    }
};

namespace std {
    template <>
    struct hash<SignatureInfo> {
        size_t operator()(const SignatureInfo& info) const {
            SignatureHasher hasher;
            return hasher(info);
        }
    };
}


// ============================================================
// 签名匹配器
// ============================================================
class SignatureMatcher {
public:
    template <typename R, typename... Args>
    static bool Match(const SignatureInfo& info) {
        // 1. 检查返回值类型 (如果 InvokeOverload 指定了 R 且不是 void)
        if constexpr (!std::is_void_v<R>) {
            TypeInfo current_ret = TypeExtractor<R>::get();
            if (current_ret != info.return_type_info) {
                return false; // 返回值类型不匹配
            }
        }

        // 2. 准备实参的类型列表
        std::vector<TypeInfo> args_info = { TypeExtractor<Args>::get()... };

        // 3. 判断 info 是否指向成员函数
        bool is_target_member_func = (info.class_type_info.type != typeid(void));

        if (is_target_member_func) {
            // === 情况 A: 目标是成员函数 ===
            // 要求：
            // 1. Args 总数 = 参数表长度 + 1 (因为 Args[0] 是对象指针)
            // 2. Args[0] 必须是指向 class_type 的指针
            // 3. Args[1...] 必须与 param_types 匹配

            if (args_info.size() != info.param_types_info.size() + 1) {
                return false; // 参数数量不对
            }

            // 检查 Args[0] 是否为合法的对象指针
            // 注意：args_info[0].type 应该是 ClassType*，而 info.class_type_info.type 是 ClassType
            // 这里我们需要比较 typeid(Args[0]) 是否等于 info.class_type_pointer_info (结构体里存了这个)
            // 或者比较 remove_pointer 后是否一致。

            // 假设 SignatureInfo 中有 class_type_pointer (typeid(C*))
            if (args_info[0].type != info.class_type_pointer_info.type) {
                // 严格匹配：实参必须是精确的类指针。
                // 如果需要支持派生类指针调用基类函数，这里需要改为 dynamic_cast 逻辑或 is_base_of 逻辑，
                // 但 typeid 只能做精确匹配。
                return false;
            }

            // 检查剩余参数
            for (size_t i = 0; i < info.param_types_info.size(); ++i) {
                // args_info 从索引 1 开始，info.param_types_info 从索引 0 开始
                if (args_info[i + 1] != info.param_types_info[i]) {
                    return false;
                }
            }

        }
        else {
            // === 情况 B: 目标是普通函数 / 静态函数 ===
            // 要求：Args 总数 = 参数表长度，且一一对应

            if (args_info.size() != info.param_types_info.size()) {
                return false;
            }

            for (size_t i = 0; i < info.param_types_info.size(); ++i) {
                if (args_info[i] != info.param_types_info[i]) {
                    return false;
                }
            }
        }

        return true;
    }
};
#pragma endregion

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

//以下是20251218之前的代码

#if 0 
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

#pragma region 返回值偏特化

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