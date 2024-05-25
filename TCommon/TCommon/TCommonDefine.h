#ifndef _TCOMMONDEFINE_H_
#define _TCOMMONDEFINE_H_

#pragma region QMetaEnum
#include <QString>
#include <QMetaEnum>
#include <QStringList>

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
// ��ʵ�����ʵ�ַ�ʽ
//Q_DISABLE_COPY(ClassName); �����ڵ����������Ӵ˺�
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

/*����
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


#pragma region ��дINI
#include <QMap>
#include <QString>
#include <QVariant>
#include <QSettings>
#include <QSharedPointer>

//��дINI
//////////////////////////////////////////////////////////////////////////
//ʹ��ʾ��
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


#pragma region ���л�����
#include <QIODevice>
#include <QByteArray>
#include <QDataStream>
//************************************
// ����:  	LibPublic::byteToTemp
// ����:    const QByteArray & data
// ����:    T & t
// ����:    bool
// ����:		���л�����
//************************************
template<typename  T >
static bool byteToTemp(const QByteArray& data, T& t)
{
	QDataStream ds(data);
	ds >> t;
	return true;
}
//************************************
// ����:  	LibPublic::tempTobyte
// ����:    const T & t
// ����:    QByteArray & byte
// ����:    bool
// ����:		�����л�
//************************************
template<typename  T >
static bool tempTobyte(const T& t, QByteArray& byte)
{
	QDataStream ds(&byte, QIODevice::WriteOnly);
	ds << t;
	return true;
};
#pragma endregion


#pragma region ���ز��
#include <QList>
#include <QLibrary>
#include <QStringList>
#include <QPluginLoader>

template<typename  T >
static void getPluginObject(QList<T*>& lst, const QString& path, QStringList filter = QStringList() /*<< "RS-*.dll" << "RS-*.DLL"*/)
{
	QStringList list = GetPathFileNames(path, filter, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	for (QString var : list)
	{
		QString pluginPath = path + "//" + var;
		if (!QLibrary::isLibrary(pluginPath))
			continue;
		QPluginLoader loader(pluginPath);
		if (!loader.load())
		{
			qWarning() << loader.errorString();
			continue;
		}
		QObject *instance = loader.instance();
		if (loader.isLoaded())
		{

			T * plugin = qobject_cast<T*>(instance);
			if (plugin)
			{
				lst.append(plugin);
			}
			else
			{
				//instance->deleteLater();
				//delete instance;
			}
		}
		else
		{

		}
	}
}

template<typename  T >
static void destroyPluginObject(QList<T*>& lst)
{
	qDeleteAll(lst);
	lst.clear();
}

#pragma endregion


#pragma region MD5
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QCryptographicHash>
QByteArray getFileMd5(QString filePath)
{
	QFile localFile(filePath);
	if (!localFile.open(QFile::ReadOnly))
	{
		qWarning() << "file open error.";
		return 0;
	}

	QCryptographicHash ch(QCryptographicHash::Md5);
	quint64 totalBytes = 0;
	quint64 bytesWritten = 0;
	quint64 bytesToWrite = 0;
	quint64 loadSize = 1024 * 4;
	QByteArray buf;
	totalBytes = localFile.size();
	bytesToWrite = totalBytes;

	while (1)
	{
		if (bytesToWrite > 0)
		{
			buf = localFile.read(qMin(bytesToWrite, loadSize));
			ch.addData(buf);
			bytesWritten += buf.length();
			bytesToWrite -= buf.length();
			buf.resize(0);
		}
		else
		{
			break;
		}

		if (bytesWritten == totalBytes)
		{
			break;
		}
	}

	localFile.close();
	QByteArray md5 = ch.result();
	return md5.toHex().constData();
}
QByteArray getStrMd5(QString str)
{
	QByteArray ba;
	ba.append(str);
	QByteArray hba = QCryptographicHash::hash(ba, QCryptographicHash::Md5);
	return hba.toHex().constData();
}
#pragma endregion
#endif //_TCOMMONDEFINE_H_