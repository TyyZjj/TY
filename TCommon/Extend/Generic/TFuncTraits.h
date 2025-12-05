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
struct FuncTraits;

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
};