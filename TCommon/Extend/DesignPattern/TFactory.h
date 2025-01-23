#ifndef _TFACTORY_H_
#define _TFACTORY_H_

#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include "TCommonDefine.h"

#pragma region 单例类厂
/*
*单例模式实现类工厂, 非线程安全
*FactoryClass: 类工厂的类名
*BaseClass: 基类的类名
*/
#define KernelFactory(FactoryClass, BaseClass)\
template<typename KEY>\
class FactoryClass\
{\
public:\
	Kernel(FactoryClass)\
\
	template<typename T>\
	struct sRegisterT{\
		sRegisterT(const KEY& key){\
			FactoryClass::GetKernel()->m_mapRegistor.insert(key, [] { return new T(); });}\
\
		template<typename... Args>\
		sRegisterT(const KEY& key, Args... args){\
			FactoryClass::GetKernel()->m_mapRegistor.insert(key, [&] { return new T(args...); });}\
	};\
\
	BaseClass* Produce(const KEY& key){\
		if (!m_mapRegistor.contains(key)){return nullptr;}\
		return m_mapRegistor.value(key)();}\
\
	QSharedPointer<BaseClass> ProduceSharePointer(const QString& key){\
		return QSharedPointer<BaseClass>(Produce(key));}\
\
	QList<KEY> GetLstRegister(){\
		return m_mapRegistor.keys();}\
private:\
	FactoryClass() {};\
	Q_DISABLE_COPY(FactoryClass)\
	QMap<KEY, std::function<BaseClass* ()>> m_mapRegistor;\
};

#define REGISTER_VNAME(T) reg_msg_##T##_
/*
*将派生类注册到基类中去
*FactoryClass: 类厂类
*T:派生类
*key: 派生类名
*...: 派生类构造函数的参数列表
*/
#define REGISTER(FactoryClass, T, KEY, key, ...) \
static FactoryClass<KEY>::sRegisterT<T> REGISTER_VNAME(T)(key, ##__VA_ARGS__);

//Sample:
//class Base {};
//class A :public Base {};
//KernelFactory(FactoryName, Base)
//REGISTER(FactoryName, A, QString, "A")
#pragma endregion

#endif //_TFACTORY_H_