#ifndef _TCOMMONDEFINE_H_
#define _TCOMMONDEFINE_H_


#include <QString>
#include <QMetaEnum>
#include <QSettings>
#include <QStringList>


#pragma region QMetaEnum
template<typename T>
static QStringList typeNameList()
{
	QStringList list;
	QMetaEnum metaEnum = QMetaEnum::fromType<T>();

	for (int i = 0; i < metaEnum.keyCount(); i++) {
		list += metaEnum.valueToKey(metaEnum.value(i));
	}
	return list;
};
template<typename T>
static T nameToType(const QString& name, bool *isok = nullptr)
{
	QMetaEnum metaEnum = QMetaEnum::fromType<T>();
	T type = (T)metaEnum.keyToValue(name.toLocal8Bit(), isok);
	return type;
};
template<typename T>
static QString typeToName(const T& type)
{
	QMetaEnum metaEnum = QMetaEnum::fromType<T>();
	QString name = metaEnum.valueToKey(type);
	return name;
};
#pragma endregion


#pragma region Kernel
// 单实例类宏实现方式
//Q_DISABLE_COPY(ClassName); 建议在单例类内增加此宏
#define Kernel(ClassName) \
static ClassName* GetKernel(){\
static ClassName * instance = nullptr; \
if(!instance)\
{instance = new ClassName;}\
return instance;\
}\
static void DestroyKernel(){\
 ClassName * instance = ClassName::GetKernel(); \
if(instance)\
{delete instance;instance=nullptr;}\
}

/*范例
class AAA
{
public:
	Kernel(AAA)
private:
	AAA(){}
	Q_DISABLE_COPY(AAA);
};
*/
#pragma endregion


#pragma region 读写INI
#include <QMap>
#include <QString>
#include <QVariant>
#include <QSettings>
#include <QSharedPointer>

//读写INI
//////////////////////////////////////////////////////////////////////////
//使用示例
//INI_BEGIN(AAAA, "C:\\xxxx\\xxx\\xxxx.ini")
//INI_PARAM(Section1, bool, Name1, false)
//INI_PARAM(Section1, int, Name2, 0)
//INI_PARAM(Section2, bool, Name3, false)
//INI_END
//////////////////////////////////////////////////////////////////////////
//INI(AAAA)->getSection1Name1()
//INI(AAAA)->setSection1Name1(true)

#define INI_BEGIN(IniName,FilePath) \
class Inier##IniName {\
public:\
    Kernel(Inier##IniName)\
private:\
    Inier##IniName() {\
        m_pConfig = QSharedPointer<QSettings>::create(FilePath, QSettings::IniFormat);\
        for (QString key: m_pConfig->allKeys()) m_mapVals.insert(key, m_pConfig->value(key));\
    }\
    ~Inier##IniName() {}\
public:

#define INI_PARAM(Section, Type, Name, DefaultVal)\
    Type get##Section##Name() { \
        if(!m_mapVals.contains(#Section"/"#Name)) setValue(#Section"/"#Name, DefaultVal);\
        return m_mapVals.value(#Section"/"#Name, DefaultVal).value<Type>(); }\
    void set##Section##Name(const Type &Name) {\
        Type val = get##Name();\
        if (val == Name) return;\
            val = Name;\
        m_mapVals.insert(#Section"/"#Name, Name);\
        setValue(#Section"/"#Name, Name);\
    }

#define INI_END \
    void setValue(const QString &name, const QVariant &val) { \
		m_pConfig->setValue(name, val); m_pConfig->sync(); }\
    QVariant value(const QString& name, const QVariant& defaultVal) {\
    return m_pConfig->value(name, defaultVal); }\
private:\
    QSharedPointer<QSettings> m_pConfig; \
	QMap<QString, QVariant> m_mapVals; \
};

#define INI(IniName) Inier##IniName::GetKernel()
#pragma endregion


#pragma region 序列化数据
#include <QIODevice>
#include <QByteArray>
#include <QDataStream>
//************************************
// 函数:  	LibPublic::byteToTemp
// 参数:    const QByteArray & data
// 参数:    T & t
// 返回:    bool
// 功能:		序列化数据
//************************************
template<typename  T >
static bool byteToTemp(const QByteArray& data, T& t)
{
	QDataStream ds(data);
	ds >> t;
	return true;
}
//************************************
// 函数:  	LibPublic::tempTobyte
// 参数:    const T & t
// 参数:    QByteArray & byte
// 返回:    bool
// 功能:		反序列化
//************************************
template<typename  T >
static bool tempTobyte(const T& t, QByteArray& byte)
{
	QDataStream ds(&byte, QIODevice::WriteOnly);
	ds << t;
	return true;
};
#pragma endregion

#endif //_TCOMMONDEFINE_H_