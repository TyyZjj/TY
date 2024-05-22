#ifndef _TFACTORY_H_
#define _TFACTORY_H_

#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include "TCommonDefine.h"


/*
*����ģʽʵ���๤��
*FactoryClass: �๤��������
*BaseClass: ���������
*/
#define KernelFactory(FactoryClass, BaseClass)\
class FactoryClass\
{\
public:\
	Kernel(FactoryClass)\
\
	template<typename T>\
	struct sRegisterT{\
		sRegisterT(const QString& key){\
			FactoryClass::GetKernel()->m_mapRegistor.insert(key, [] { return new T(); });}\
\
		template<typename... Args>\
		sRegisterT(const QString& key, Args... args){\
			FactoryClass::GetKernel()->m_mapRegistor.insert(key, [&] { return new T(args...); });}\
	};\
\
	BaseClass* Produce(const QString& key){\
		if (!m_mapRegistor.contains(key)){return nullptr;}\
		return m_mapRegistor.value(key)();}\
\
	QSharedPointer<BaseClass> ProduceSharePointer(const QString& key){\
		return QSharedPointer<BaseClass>(Produce(key));}\
\
	QStringList GetLstRegister(){\
		return m_mapRegistor.keys();}\
private:\
	FactoryClass() {};\
	Q_DISABLE_COPY(FactoryClass)\
	QMap<QString, std::function<BaseClass* ()>> m_mapRegistor;\
};

#define REGISTER_VNAME(T) reg_msg_##T##_
/*
*��������ע�ᵽ������ȥ
*FactoryClass: �೧��
*T:������
*key: ��������
*...: �����๹�캯���Ĳ����б�
*/
#define REGISTER(FactoryClass, T, key, ...) static FactoryClass::sRegisterT<T> REGISTER_VNAME(T)(key, ##__VA_ARGS__);

/*����
class Base{};
class A :public Base{};
KernelFactory(FactoryName, Base)
REGISTER(FactoryName, A, "A")
*/

#endif //_TFACTORY_H_