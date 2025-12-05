#ifndef _TGENERICINVOKER_H_
#define _TGENERICINVOKER_H_

#include <any>
#include <map>
#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <typeindex>
#include <type_traits>
#include "TFuncTraits.h"

/*
* Taoyuyu 20251201
* 一个简易的RPC调用器, 能够自动根据函数签名进行调用
*/

#ifdef DEBUG_MODE
#include <execinfo.h>
void print_backtrace() {
	void* buffer[10];
	int nptrs = backtrace(buffer, 10);
	backtrace_symbols_fd(buffer, nptrs, STDERR_FILENO);
}
#endif


// ==========================================
// 支持返回值的调用实现 Taoyuyu 20251201
// ==========================================
template <typename FuncPtr, size_t... Is>
std::any call_impl(FuncPtr func,
	const std::vector<std::any>& args,
	std::index_sequence<Is...>)
{
	// 获取函数的返回值类型
	using R = typename FuncTraits<FuncPtr>::return_type;

	try
	{
		// 编译期分支:判断返回值是否为 void
		if constexpr (std::is_void_v<R>)
		{
			// 情况 A: 函数无返回值
			// 直接展开调用，不接收结果
			func(std::any_cast<typename FuncTraits<FuncPtr>::template arg_type<Is>>(args[Is])...);
			// 返回一个空的 any 表示 void
			return std::any{};
		}
		else
		{
			using StoredR = std::decay_t<R>;
			// 情况 B: 函数有返回值
			// 调用并捕获结果
			StoredR result = func(std::any_cast<typename FuncTraits<FuncPtr>::template arg_type<Is>>(args[Is])...);
			// 将结果包装进 any 返回
			return std::any(std::move(result));
		}
	}
	catch (const std::bad_any_cast& e)
	{
		std::cerr << "[call_impl] 参数类型不匹配: " << e.what() << std::endl;
		return std::any{};
	}

}

// 用户接口:现在返回 std::any
template <typename FuncPtr>
std::any invoke_function(FuncPtr func, const std::vector<std::any>& args) {
	using Traits = FuncTraits<FuncPtr>;

	// 安全检查
	if (args.size() != Traits::nArgs)
	{
		std::cerr << "[invoke_function] 参数数量不匹配!" << std::endl;
		return std::any{};
	}

	return call_impl(func, args, std::make_index_sequence<Traits::nArgs>{});
}

template<typename ClassType, typename FuncPtr, size_t... Is>
std::any call_member_impl(ClassType* obj, FuncPtr func,
	const std::vector<std::any>& args,
	std::index_sequence<Is...>)
{
	using R = typename FuncTraits<FuncPtr>::return_type;
	try {
		if constexpr (std::is_void_v<R>) {
			(obj->*func)(std::any_cast<typename FuncTraits<FuncPtr>::template arg_type<Is>>(args[Is])...);
			return std::any{};
		}
		else {
			using StoredR = std::decay_t<R>;
			StoredR result = (obj->*func)(std::any_cast<typename FuncTraits<FuncPtr>::template arg_type<Is>>(args[Is])...);
			return std::any(std::move(result));
		}
	}
	catch (const std::bad_any_cast& e) {
		std::cerr << "[call_member_impl] 参数类型不匹配: " << e.what() << std::endl;
		return std::any{};
	}
}

template<typename ClassType, typename FuncPtr>
std::any invoke_member_function(ClassType* obj, FuncPtr func, const std::vector<std::any>& args) {
	if (args.size() != FuncTraits<FuncPtr>::nArgs) {
		std::cerr << "[invoke_member_function] 参数数量不匹配!" << std::endl;
		return std::any{};
	}
	return call_member_impl(obj, func, args, std::make_index_sequence<FuncTraits<FuncPtr>::nArgs>{});
}

//// ==========================================
//// 新增：成员函数专用注册器 (Taoyuyu 20251205) 已舍弃
//// ==========================================
//template<typename ClassType, typename FuncPtr>
//class MemberFunctionWrapper {
//public:
//	MemberFunctionWrapper(ClassType* obj, FuncPtr func)
//		: m_obj(obj), m_func(func) {
//	}
//
//	// 通用调用入口
//	std::any operator()(const std::vector<std::any>& args) {
//		return invoke_member_function(m_obj, m_func, args);
//	}
//
//private:
//	template<size_t... Is>
//	std::any call_impl(ClassType* obj, FuncPtr func,
//		const std::vector<std::any>& args,
//		std::index_sequence<Is...>) {
//		using R = typename FuncTraits<FuncPtr>::return_type;
//		try {
//			if constexpr (std::is_void_v<R>) {
//				// 调用成员函数：(object->*func)(args...)
//				(obj->*func)(std::any_cast<typename FuncTraits<FuncPtr>::template arg_type<Is>>(args[Is])...);
//				return std::any{};
//			}
//			else {
//				using StoredR = std::decay_t<R>;
//				StoredR result = (obj->*func)(std::any_cast<typename FuncTraits<FuncPtr>::template arg_type<Is>>(args[Is])...);
//				return std::any(std::move(result));
//			}
//		}
//		catch (const std::bad_any_cast& e) {
//			std::cerr << "[call_impl] 参数类型不匹配: " << e.what() << std::endl;
//			return std::any{};
//		}
//	}
//
//	std::any invoke_member_function(ClassType* obj, FuncPtr func, const std::vector<std::any>& args) {
//		if (args.size() != FuncTraits<FuncPtr>::nArgs) {
//			std::cerr << "[invoke_member_function] 参数数量不匹配!" << std::endl;
//			return std::any{};
//		}
//		return call_impl(obj, func, args,
//			std::make_index_sequence<FuncTraits<FuncPtr>::nArgs>{});
//	}
//
//	ClassType* m_obj;
//	FuncPtr m_func;
//};


#include <mutex>
#include <shared_mutex>
class TGenericInvoker
{
public:
	static TGenericInvoker* GetKernel()
	{
		static TGenericInvoker* instance = nullptr;
		static std::once_flag flag;
		std::call_once(flag, []() {
			instance = new TGenericInvoker;
			});
		return instance;
	}

	using DispatcherFunc = std::function<std::any(const std::vector<std::any>&)>;
	struct DispatcherEntry {
		DispatcherFunc dispatcher;
		SignatureInfo signature;
	};
	using DispatcherList = std::list<DispatcherEntry>;

#pragma region 命名函数(仅匹配形参)
	// 注册命名函数 (支持重载)(仅根据参数类型调用)
	template<typename FuncPtr>
	bool Register(const std::string& name, FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();
		using C = FuncTraits<FuncPtr>::class_type;
		if constexpr (std::is_void_v<C>)//判断是否是全局函数
		{
			entry.dispatcher = [func](const std::vector<std::any>& dynamic_args) -> std::any {
				return invoke_function(func, dynamic_args);
				};
		}
		else
		{
			entry.dispatcher = [func](const std::vector<std::any>& args) -> std::any {
				C* obj = std::any_cast<C*>(args[0]);
				// 关键安全检查:空指针保护 ======
				if (!obj)
				{
					std::cerr << "[Register] 空对象指针调用: "
						<< typeid(C).name() << " 类的成员函数" << std::endl;
					throw std::runtime_error("Cannot call member function on null object pointer");
				}
				std::vector<std::any> realArgs(args.begin() + 1, args.end());
				return invoke_member_function(obj, func, realArgs);
				};
		}

		std::unique_lock lock(m_mutex);
		auto it = m_mapCommandRegistry.find(name);
		if (it != m_mapCommandRegistry.end())
		{
			//遍历DispatcherList，是否有相同的func
			for (const auto& item : it->second)
			{
				if (entry.signature.rpEquals(item.signature))
				{
					std::cerr << "[Register] 已经注册过相同签名的命令!" << std::endl;
					return false;
				}
			}
			it->second.push_back(entry);
		}
		else
			m_mapCommandRegistry[name].push_back(entry);
		return true;
	}

	template<typename FuncPtr>
	bool UnRegister(const std::string& name, FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();

		std::unique_lock lock(m_mutex);
		auto it = m_mapCommandRegistry.find(name);
		if (it != m_mapCommandRegistry.end())
		{
			//遍历DispatcherList，是否有相同的func
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				if (entry.signature.rpEquals(it2->signature))
				{
					it->second.erase(it2);
					return true;
				}
			}
		}
		return false;
	}

	//变参模板 Invoke (具名调用), 能够自动根据函数命名和函数签名(仅能通过入参类型进行判断, 不包含返回值类型、所属类的判断)进行调用
	//如果要调用的是成员函数, args中第一个参数必须是对象指针
	template <typename... Args>
	std::any Invoke(const std::string& name, Args&&... args)
	{
		// 1. 将参数包展开并构造 vector<any>
		// 利用 std::forward 完美转发参数
		std::vector<std::any> argList = { std::any(std::forward<Args>(args))... };

		// 2. 调用内部实现
		return Invoke_impl(name, argList);
	}
#pragma endregion

#pragma region 命名函数(精准匹配)
	// 注册准确匹配的命名函数 (支持重载)
	template<typename FuncPtr>
	bool RegisterExact(const std::string& name, FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();
		using C = FuncTraits<FuncPtr>::class_type;
		if constexpr (std::is_void_v<C>)//判断是否是全局函数
		{
			entry.dispatcher = [func](const std::vector<std::any>& dynamic_args) -> std::any {
				return invoke_function(func, dynamic_args);
				};
		}
		else
		{
			entry.dispatcher = [func](const std::vector<std::any>& args) -> std::any {
				C* obj = std::any_cast<C*>(args[0]);
				// 关键安全检查:空指针保护 ======
				if (!obj)
				{
					std::cerr << "[RegisterExact] 空对象指针调用: "
						<< typeid(C).name() << " 类的成员函数" << std::endl;
					throw std::runtime_error("Cannot call member function on null object pointer");
				}
				std::vector<std::any> realArgs(args.begin() + 1, args.end());
				return invoke_member_function(obj, func, realArgs);
				};
		}
		std::unique_lock lock(m_mutex);
		auto it = m_mapExactCommandRegistry.find(name);
		if (it != m_mapExactCommandRegistry.end())
		{
			//遍历DispatcherList，是否有相同的func
			for (const auto& item : it->second)
			{
				if (item.signature == entry.signature)
				{
					std::cerr << "[RegisterExact] 已经注册过相同签名的命令!" << std::endl;
					return false;
				}
			}
			it->second.push_back(entry);
		}
		else
			m_mapExactCommandRegistry[name].push_back(entry);
		return true;
	}

	template<typename FuncPtr>
	bool UnRegisterExact(const std::string& name, FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();

		std::unique_lock lock(m_mutex);
		auto it = m_mapExactCommandRegistry.find(name);
		if (it != m_mapExactCommandRegistry.end())
		{
			//遍历DispatcherList，是否有相同的func
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				if (entry.signature == it2->signature)
				{
					it->second.erase(it2);
					return true;
				}
			}
		}
		return false;
	}

	//  精确匹配具名函数 (指定返回值 R 和 类类型 C)
	//  用法: InvokeExact<int, MyClass>("Func", 10);
	//  如果是普通函数，C 填 void: InvokeExact<int, void>("Func", 10);
	template <typename R, typename C = void, typename... Args>
	std::any InvokeExact(const std::string& name, Args&&... args)
	{
		std::vector<std::any> argList = { std::any(std::forward<Args>(args))... };

		// 构造期望的签名特征
		std::type_index typeRet = std::type_index(typeid(R));
		std::type_index typeClass = std::type_index(typeid(C));

		return InvokeExact_impl(name, typeRet, typeClass, argList);
	}
#pragma endregion

#pragma region 匿名函数(仅匹配形参)
	// 注册匿名/全局匹配函数 (仅根据参数类型调用)
	template<typename FuncPtr>
	bool RegisterAny(FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();
		using C = FuncTraits<FuncPtr>::class_type;
		if constexpr (std::is_void_v<C>)//判断是否是全局函数
		{
			entry.dispatcher = [func](const std::vector<std::any>& dynamic_args) -> std::any {
				return invoke_function(func, dynamic_args);
				};
		}
		else
		{
			entry.dispatcher = [func](const std::vector<std::any>& args) -> std::any {
				C* obj = std::any_cast<C*>(args[0]);
				// 关键安全检查:空指针保护 ======
				if (!obj) 
				{
					std::cerr << "[RegisterAny] 空对象指针调用: "
						<< typeid(C).name() << " 类的成员函数" << std::endl;
					throw std::runtime_error("Cannot call member function on null object pointer");
				}
				std::vector<std::any> realArgs(args.begin() + 1, args.end());
				return invoke_member_function(obj, func, realArgs);
				};
		}

		std::unique_lock lock(m_mutex);
		// 检查非命名列表
		for (const auto& item : m_lstCommandRegistry) 
		{
			if (entry.signature.rpEquals(item.signature)) {
				std::cerr << "[RegisterAny] Anonymous function with same signature already registered." << std::endl;
				return false;
			}
		}
		m_lstCommandRegistry.push_back(entry);
		return true;
	}

	template<typename FuncPtr>
	bool UnRegisterAny(FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();
		//using C = FuncTraits<FuncPtr>::class_type;
		//if constexpr (std::is_void_v<C>)
		//{
		//	entry.dispatcher = [func](const std::vector<std::any>& dynamic_args) -> std::any {
		//		return invoke_function(func, dynamic_args);
		//		};
		//}
		//else
		//{
		//	entry.dispatcher = [func](const std::vector<std::any>& args) -> std::any {
		//		C* obj = std::any_cast<C*>(args[0]);
		//
		//		std::vector<std::any> realArgs(args.begin() + 1, args.end());
		//		return invoke_member_function(obj, func, realArgs);
		//		};
		//}

		std::unique_lock lock(m_mutex);
		// 检查非命名列表
		for (auto it = m_lstCommandRegistry.begin(); it != m_lstCommandRegistry.end(); ++it)
		{
			if (entry.signature.rpEquals(it->signature)) {
				m_lstCommandRegistry.erase(it);
				return true;
			}
		}
		return false;
	}

	//  变参模板 InvokeAny (匿名/类型匹配调用) 如果要调用的是成员函数, args中第一个参数必须是对象指针
	template <typename... Args>
	std::any InvokeAny(Args&&... args)
	{
		// 1. 将参数包展开并构造 vector<any>
		std::vector<std::any> argList = { std::any(std::forward<Args>(args))... };

		// 2. 调用内部实现
		return InvokeAny_impl(argList);
	}
#pragma endregion

#pragma region 匿名函数(精准匹配)
	// 注册准确匹配的匿名/全局匹配函数
	template<typename FuncPtr>
	bool RegisterAnyExact(FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();
		using C = FuncTraits<FuncPtr>::class_type;
		if constexpr (std::is_void_v<C>)//判断是否是全局函数
		{
			entry.dispatcher = [func](const std::vector<std::any>& dynamic_args) -> std::any {
				return invoke_function(func, dynamic_args);
				};
		}
		else
		{
			entry.dispatcher = [func](const std::vector<std::any>& args) -> std::any {
				C* obj = std::any_cast<C*>(args[0]);
				// 关键安全检查:空指针保护 ======
				if (!obj)
				{
					std::cerr << "[RegisterAnyExact] 空对象指针调用: "
						<< typeid(C).name() << " 类的成员函数" << std::endl;
					throw std::runtime_error("Cannot call member function on null object pointer");
				}
				std::vector<std::any> realArgs(args.begin() + 1, args.end());
				return invoke_member_function(obj, func, realArgs);
				};
		}

		std::unique_lock lock(m_mutex);
		// 检查非命名列表
		for (const auto& item : m_lstExactCommandRegistry) 
		{
			if (item.signature == entry.signature) {
				std::cerr << "[RegisterAnyExact] Anonymous function with same signature already registered." << std::endl;
				return false;
			}
		}
		m_lstExactCommandRegistry.push_back(entry);
		return true;
	}

	template<typename FuncPtr>
	bool UnRegisterAnyExact(FuncPtr func)
	{
		DispatcherEntry entry;
		entry.signature = FuncTraits<FuncPtr>::get_signature_info();

		std::unique_lock lock(m_mutex);
		// 检查非命名列表
		for (auto it = m_lstExactCommandRegistry.begin(); it != m_lstExactCommandRegistry.end(); ++it)
		{
			if (entry.signature == it->signature) {
				m_lstExactCommandRegistry.erase(it);
				return true;
			}
		}
		return false;
	}

	//  精确匹配匿名函数 (指定返回值 R 和 类类型 C), 如果要调用的是成员函数, args中第一个参数必须是对象指针
	template <typename R, typename C = void, typename... Args>
	std::any InvokeAnyExact(Args&&... args)
	{
		std::vector<std::any> argList = { std::any(std::forward<Args>(args))... };

		std::type_index typeRet = std::type_index(typeid(R));
		std::type_index typeClass = std::type_index(typeid(C));

		return InvokeAnyExact_impl(argList, typeRet, typeClass);
	}
#pragma endregion

	// 获取命令列表
	DispatcherList GetCommandList(const std::string& name)
	{
		std::shared_lock lock(m_mutex);
		auto it = m_mapCommandRegistry.find(name);
		if (it != m_mapCommandRegistry.end())
			return it->second;
		return {};
	}

	DispatcherList GetExactCommandList(const std::string& name)
	{
		std::shared_lock lock(m_mutex);
		auto it = m_mapExactCommandRegistry.find(name);
		if (it != m_mapExactCommandRegistry.end())
			return it->second;
		return {};
	}

	// 获取命令列表
	DispatcherList GetCommandList()
	{
		std::shared_lock lock(m_mutex);
		return m_lstCommandRegistry;
	}

	DispatcherList GetExactCommandList()
	{
		std::shared_lock lock(m_mutex);
		return m_lstExactCommandRegistry;
	}

	// 清除命令
	void ClearCommands()
	{
		std::unique_lock lock(m_mutex);
		m_mapCommandRegistry.clear();
		m_lstCommandRegistry.clear();
		m_mapExactCommandRegistry.clear();
		m_lstExactCommandRegistry.clear();
	}

private:
	TGenericInvoker() = default;
	~TGenericInvoker() = default;
	TGenericInvoker(const TGenericInvoker&) = delete;
	TGenericInvoker& operator=(const TGenericInvoker&) = delete;



	// 辅助：检查运行时参数 args 是否匹配编译期签名 info
	bool isSignatureMatch(const SignatureInfo& info, const std::vector<std::any>& args) const
	{
		//判定info.class_type是否为void
		if (info.class_type == typeid(void))
		{
			if (info.param_types.size() != args.size())
				return false;
			for (size_t i = 0; i < args.size(); ++i)
			{
				// 注意：args[i].type() 返回的是实际持有值的类型
				if (args[i].type() != info.param_types[i])
				{
					return false;
				}
			}
		}
		else
		{
			if (info.param_types.size() + 1 != args.size())
				return false;
			// 检查第一个参数是否为类类型 一个是类型对应的type_index, 一个是类型指针对应的type_index
			if (args[0].type() != info.class_type_pointer)
				return false;
			for (size_t i = 0; i < info.param_types.size(); ++i)
			{
				// 注意：args[i].type() 返回的是实际持有值的类型
				if (args[i + 1].type() != info.param_types[i])
				{
					return false;
				}
			}
		}
		return true;
	}

	bool isExactMatch(const SignatureInfo& info,
		std::type_index reqRet,
		std::type_index reqClass,
		const std::vector<std::any>& args) const
	{
		// 1. 检查返回值类型
		if (info.return_type != reqRet)
			return false;
		// 2. 检查所属类类型
		if (info.class_type != reqClass)
			return false;
		// 3. 检查参数
		return isSignatureMatch(info, args);
	}

	std::any Invoke_impl(const std::string& name, const std::vector<std::any>& args)
	{
		std::shared_lock lock(m_mutex);
		auto it = m_mapCommandRegistry.find(name);
		if (it == m_mapCommandRegistry.end()) {
			std::cerr << "[Invoke_impl] Command '" << name << "' not found." << std::endl;
			return std::any{};
		}
		for (const auto& entry : it->second) {
			if (isSignatureMatch(entry.signature, args)) {
				try {
					return entry.dispatcher(args);
				}
				catch (const std::exception& e) {
					std::cerr << "[Invoke_impl] function call failed: " << e.what() << std::endl;
#ifdef DEBUG_MODE
					print_backtrace();
#endif
					return std::any{}; // 或抛出封装后的异常
				}
			}
		}
		std::cerr << "[Invoke_impl] No matching overload for '" << name << "'" << std::endl;
		return std::any{};
	}

	// 精确调用的内部实现 Taoyuyu 20251205
	std::any InvokeExact_impl(const std::string& name,
		std::type_index retType,
		std::type_index classType,
		const std::vector<std::any>& args)
	{
		std::shared_lock lock(m_mutex);
		auto it = m_mapExactCommandRegistry.find(name);
		if (it == m_mapExactCommandRegistry.end()) {
			std::cerr << "[InvokeExact_impl] Command '" << name << "' not found." << std::endl;
			return std::any{};
		}

		for (const auto& entry : it->second) {
			// 使用 isExactMatch 进行严格筛选
			if (isExactMatch(entry.signature, retType, classType, args)) {
				try {
					return entry.dispatcher(args);
				}
				catch (const std::exception& e) {
					std::cerr << "[InvokeExact_impl] function call failed: " << e.what() << std::endl;
#ifdef DEBUG_MODE
					print_backtrace();
#endif
					return std::any{}; // 或抛出封装后的异常
				}
			}
		}
		std::cerr << "[InvokeExact_impl] No exact match (Ret/Class/Args) for '" << name << "'" << std::endl;
		return std::any{};
	}

	std::any InvokeAny_impl(const std::vector<std::any>& args)
	{
		std::shared_lock lock(m_mutex);
		for (const auto& entry : m_lstCommandRegistry) {
			if (isSignatureMatch(entry.signature, args)) {
				try {
					return entry.dispatcher(args);
				}
				catch (const std::exception& e) {
					std::cerr << "[InvokeAny_impl] function call failed: " << e.what() << std::endl;
#ifdef DEBUG_MODE
					print_backtrace();
#endif
					return std::any{}; // 或抛出封装后的异常
				}
			}
		}
		std::cerr << "[InvokeAny_impl] No anonymous function match found." << std::endl;
		return std::any{};
	}

	// 精确调用的内部实现 (匿名) Taoyuyu 20251205
	std::any InvokeAnyExact_impl(const std::vector<std::any>& args,
		std::type_index retType,
		std::type_index classType)
	{
		std::shared_lock lock(m_mutex);
		for (const auto& entry : m_lstExactCommandRegistry) {
			if (isExactMatch(entry.signature, retType, classType, args)) {
				try {
					return entry.dispatcher(args);
				}
				catch (const std::exception& e) {
					std::cerr << "[InvokeAnyExact_impl] function call failed: " << e.what() << std::endl;
#ifdef DEBUG_MODE
					print_backtrace();
#endif
					return std::any{}; // 或抛出封装后的异常
				}
			}
		}
		std::cerr << "[InvokeAnyExact_impl] No exact anonymous match found." << std::endl;
		return std::any{};
	}

private:
	//命名函数注册表, 同一种命名的函数可以注册多个 如"Add"对应&Add(int,int)、 &Add(double,double), 
	//同时int Add(int,int)、 int Max(int, int)也可以分别注册为"Add" "Max"
	std::map<std::string, DispatcherList> m_mapCommandRegistry;
	//非命名函数注册表 仅根据函数签名(返回值、参数类型)进行注册, 同一种签名的函数只能注册一个, 如int Add(int,int)、 int Max(int, int)仅能存一个 
	std::list<DispatcherEntry> m_lstCommandRegistry;


	//精确匹配注册, 对于一个函数指针(R)(C::*)(params), 会精确匹配他的返回值R和所属类C和参数params
	std::map<std::string, DispatcherList> m_mapExactCommandRegistry;	//执行精确匹配的命名函数注册表
	std::list<DispatcherEntry> m_lstExactCommandRegistry;	//执行精确匹配的非命名函数注册表 
	mutable std::shared_mutex m_mutex;
};


//Sample
/*
int Add(int a, int b)
{
	return a + b;
}

TestApp* TestApp::newTestApp(TestApp* p) { return p; };
static int TestApp::GetTestIndex() { return 1; }//静态成员函数

TestApp::TestApp(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::TestAppClass())
{
	ui->setupUi(this);

	//TGenericInvoker::GetKernel()->RegisterAny(&Add);							//匿名函数注册 + 全局函数 + 仅匹配参数类型
	//TGenericInvoker::GetKernel()->RegisterAny(&TestApp::newTestApp);			//匿名函数注册 + 成员函数 + 仅匹配参数类型
	//TGenericInvoker::GetKernel()->RegisterAny(&TestApp::GetTestIndex);		//匿名函数注册 + 静态成员函数 + 仅匹配参数类型
	//TGenericInvoker::GetKernel()->RegisterAnyExact(&TestApp::newTestApp);		//匿名函数注册 + 成员函数 + 匹配参数类型、返回值、所属类
	//TGenericInvoker::GetKernel()->RegisterAnyExact(&TestApp::GetTestIndex);	//匿名函数注册 + 静态成员函数 + 匹配参数类型、返回值、所属类
	//TGenericInvoker::GetKernel()->Register("Add", &Add);						//命名函数注册 + 全局函数 + 仅匹配参数类型
	//TGenericInvoker::GetKernel()->Register("newTestApp", &TestApp::newTestApp);//命名函数注册 + 成员函数 + 仅匹配参数类型
	//TGenericInvoker::GetKernel()->RegisterExact("Add", &Add);					//命名函数注册 + 全局函数 + 匹配参数类型、返回值、所属类
	//TGenericInvoker::GetKernel()->RegisterExact("newTestApp", &TestApp::newTestApp);//命名函数注册 + 成员函数 + 匹配参数类型、返回值、所属类
}

TestApp::~TestApp()
{
	delete ui;
}

void TestApp::on_pushButton_clicked()
{
	//CGlobalMessage::GetKernel()->notify("TestMessage");
	//TGenericInvoker::GetKernel()->InvokeAny(3, 5);
	//TGenericInvoker::GetKernel()->InvokeAny();
	//TGenericInvoker::GetKernel()->InvokeAnyExact<TestApp*, TestApp>(this, this);
	//TGenericInvoker::GetKernel()->InvokeAnyExact<int, void>();
	//TGenericInvoker::GetKernel()->Invoke("Add", 3, 5);
	//TGenericInvoker::GetKernel()->Invoke("newTestApp", this, this);
	//TGenericInvoker::GetKernel()->InvokeExact<int, void>("Add", 3, 5);
	//TGenericInvoker::GetKernel()->InvokeExact<TestApp*, TestApp>("newTestApp", this, this);

}
*/

#endif // !_TGENERICINVOKER_H_
