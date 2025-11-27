#pragma once
#include <QList>
#include <QString>
#include <QByteArray>
#include <QDebug>


#define Hex0	QByteArray::fromHex("00")
#define VALUELOG(Type,Length,Value) QString("%1,%2 %3").arg(Type).arg(QString::number(Length)).arg(Value) 

enum nSecsType
{
	List = 0,				// List 000000 00
	Binary = 0x20,			// Binary 001000 00
	Boolean = 0x24,			// Boolean 001001 00
	ASCII = 0x40,			// ASCII 010000 00
	JIS8 = 0x44,			// JIS8 010001 00
	I8 = 0x60,			// I8 011000 00
	I1 = 0x64,			// I1 011001 00
	I2 = 0x68,			// I2 011010 00
	I4 = 0x70,			// I4 011100 00
	F8 = 0x80,			// F8 100000 00
	F4 = 0x90,			// F4 100100 00
	U8 = 0xA0,			// U8 101000 00
	U1 = 0xA4,			// U1 101001 00
	U2 = 0xA8,			// U2 101010 00
	U4 = 0xB0,			// U4 101100 00
	NONE = 0xFF,

};




struct sSecsItem
{
	//enum nValueType
	//{
	//	CharType = 0,//字符 ByteArray
	//	BinaryType,//二进制
	//	ListType,//列表
	//};

	nSecsType nType = nSecsType::List;
	int iValueSize = 0;

	QByteArray bytearray;
	QList<sSecsItem> lstSecsItem;



	friend QString typeToName(const nSecsType& nType)
	{
		QString name = "NONE";
		switch (nType)
		{
		case List:
			name = "L";
			break;
		case Binary:
			name = "B";
			break;
		case Boolean:
			name = "BL";
			break;
		case ASCII:
			name = "A";
			break;
		case JIS8:
			name = "J";
			break;
		case I8:
			name = "I8";
			break;
		case I1:
			name = "I1";
			break;
		case I2:
			name = "I2";
			break;
		case I4:
			name = "I4";
			break;
		case F8:
			name = "F8";
			break;
		case F4:
			name = "F4";
			break;
		case U8:
			name = "U8";
			break;
		case U1:
			name = "U1";
			break;
		case U2:
			name = "U2";
			break;
		case U4:
			name = "U4";
			break;
		default:
			break;
		}

		return name;
	};


	friend QDebug& operator<<(QDebug out, const sSecsItem& info)
	{
		if (info.nType == nSecsType::List)
		{
			out << VALUELOG(typeToName(info.nType), info.iValueSize, "") << endl;
			for (auto item : info.lstSecsItem)
			{
				out << item ;
			}
		}
		else
		{
			out << VALUELOG(typeToName(info.nType),info.bytearray.count(), QString(info.bytearray)) << endl;
		}
		return out;
	}

};

struct sSecsData
{
	enum nSecsHeader
	{
		/*
		-----------------------
		byte1	Length
		--------
		byte2
		--------
		byte3
		--------
		byte4
		-----------------------
		byte5	Session
		--------
		byte6
		-----------------------
		byte7	Stream
		-----------------------
		byte8	Function
		-----------------------
		byte9	Ptype
		-----------------------
		byte10	Stype
		-----------------------
		byte11	SystemByte
		--------
		byte12
		--------
		byte13
		--------
		byte14
		-----------------------
		*/

		LengthIndex = 0,	//长度起始位
		LengthCount = 4,	//长度字节数
		SessionIdIndex = 4,	//Session起始位
		SessionIdCount = 2,	//Session字节数
		StreamIdIndex = 6,	//Stream起始位
		FunctionIdIndex = 7,//Function起始位
		PtypeIndex = 8,		//Ptype起始位
		StypeIndex = 9,		//Stype起始位  Stype  0=Data Message; 1=Select request; 2=Select reply; 5=Link test request; 6=Link test reply; 9=Separate request
		SystemByteIndex = 10,//系统字节(计数器)起始位
		SystemByteCount = 4, //系统字节(计数器)字节数
		SecsHeaderLength = 14,
	};

	

	QString strFlowName;
	int	iLength = 0;	//整个消息字节长度(包括14位Secs头)
	int	iSession1 = 0x00;
	int iSession2 = 0x00;
	int iStreamId = 0;
	int iFunctionId = 0;
	int iPtype = 0x00;
	int iStype = 0x00;
	int	iFlowNum = 0;

	sSecsItem	secsdata;

	friend QDebug& operator<<(QDebug out, const sSecsData& info)
	{
		out << info.strFlowName << endl
			<< info.secsdata << endl
			
			;
		return out;
	}


};





class CSecs
{
public:

	//************************************
	// 函数:		IntToByte
	// 参数: 	const int iNum	数字
	// 参数: 	int iByteCount  转换成数据后的长度
	// 返回:		QByteArray	数据
	// 功能:		
	//************************************
	static QByteArray IntToByte(const int iNum, int iByteCount = 4);
	//************************************
	// 函数:		ByteToInt
	// 参数: 	const QByteArray & ba	数据
	// 返回:		int	数字
	// 功能:		
	//************************************
	static int ByteToInt(const QByteArray& ba);

	//************************************
	// 参数: 	nSecsType type
	// 参数: 	int iLength: 长度 16进制
	// 返回:		QByteArray:
	// 功能:		打包类型
	//************************************
	static QByteArray PackageValueHead(nSecsType type,int lenth);

	//************************************
	// 参数: 	const QString & str: 字段值
	// 参数: 	int iLength: 长度 16进制
	// 参数:	char fill:填充字节
	// 返回:	QByteArray:
	// 功能:	打包某个字段
	//************************************
	static QByteArray PackageValue(const QString &str, int iLength, char fill = ' ');

	static QByteArray PackageValue(int bnum);

	//************************************
	// 参数: 	QByteArray stream: S
	// 参数: 	QByteArray function: F
	// 参数: 	int iSendFlowNum: 参见m_iSendFlowNum
	// 参数: 	int iBodyByteCount: 消息主题字节长度
	// 参数: 	QByteArray ptype: 参见nSecsHeader
	// 参数: 	QByteArray stype: 参见nSecsHeader, Stype  0=Data Message; 1=Select request; 2=Select reply; 5=Link test request; 6=Link test reply; 9=Separate request
	// 参数: 	QByteArray session1: 参见nSecsHeader
	// 参数: 	QByteArray session2: 参见nSecsHeader
	// 返回:	QT_NAMESPACE::QByteArray
	// 功能:	打包消息头
	//************************************
	static QByteArray PackageHead(QByteArray stream,
		QByteArray function,
		int iSendFlowNum,
		int iBodyByteCount,
		QByteArray ptype = Hex0,
		QByteArray stype = Hex0,
		QByteArray session1 = Hex0,
		QByteArray session2 = Hex0);
	static QByteArray PackageHead(int iStream,
		int iFunction,
		int iSendFlowNum,
		int iBodyByteCount,
		QByteArray ptype = Hex0,
		QByteArray stype = Hex0,
		QByteArray session1 = Hex0,
		QByteArray session2 = Hex0);

	//************************************
	// 参数: 	const QByteArray & data
	// 参数: 	sSecsData & secsData: 解析出来的Secs数据
	// 返回:	bool: 是否成功
	// 功能:	解析SECS数据
	//************************************
	static bool ParseData(const QByteArray &data, sSecsData &secsData);
private:



	//************************************
	// Method:    GetByteArray 根据输入的字符获得指定长度的ByteArray, 舍去超出的长度, 长度不足以fill补全, 补在后方
	// FullName:  CMesCore::GetByteArray
	// Returns:   bool
	// Parameter: const QString & str:输入的字符
	// Parameter: int len:长度
	// Parameter: char fill:填充字节
	//************************************
	static QByteArray GetByteArray(const QString &str, int len, char fill = ' ');

	//************************************
	// 参数: 	const QByteArray & data: 输入
	// 参数: 	int & iOffset: 输入/输出 偏移量
	// 参数: 	sSecsItem::nValueType & type: 输出, 值的类型
	// 参数: 	int & iBegin:  输出, 开始字段 0x41/0x42/0x21/0x01
	// 参数: 	int & iHeadSize:  输出, 数值头占用的字节大小
	// 参数: 	int & iValueSize: 输出, 数值占用字节的大小
	// 返回:	bool 成功/失败
	// 功能:	解析开始的字段
	//************************************
	static bool ParseValueHead(const QByteArray &data, int &iOffset, nSecsType &ntype, int &iBegin, int &iHeadSize, int &iValueSize);

	//************************************
	// 参数: 	const QByteArray & data: 输入
	// 参数: 	sSecsItem & secsItem: secs数据
	// 参数: 	int iMaxLength: data的最大长度
	// 返回:	bool: 是否成功
	// 功能:	解析Secs数据中的消息体
	//************************************
	static bool ParseValue(const QByteArray &data, int &iOffset, sSecsItem &secsItem);

};



