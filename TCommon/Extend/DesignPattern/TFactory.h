#ifndef _TFACTORY_H_
#define _TFACTORY_H_

#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include "TCommonDefine.h"

#pragma region �����೧
/*
*����ģʽʵ���๤��, ���̰߳�ȫ
*FactoryClass: �๤��������
*BaseClass: ���������
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
*��������ע�ᵽ������ȥ
*FactoryClass: �೧��
*T:������
*key: ��������
*...: �����๹�캯���Ĳ����б�
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