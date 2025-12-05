#include "CSecs.h"
#include <QDebug>


QByteArray CSecs::PackageValueHead(nSecsType type, int lenth)
{
	QByteArray ba;

	int iSize(0);
	if (lenth == 0)
	{
		ba.insert(0, lenth);
		iSize++;
	}
	else
	{
		
		while (lenth > 0)
		{
			ba.insert(0, lenth & 0xFF);
			lenth = lenth >> 8;
			iSize++;
		}
	}

	
	ba.insert(0, type + iSize);

	return ba;
}

QByteArray CSecs::PackageValue(const QString &str, int iLength, char fill /*= ' '*/)
{
	QByteArray ba;
	if (iLength <= 0)
		return ba;
	ba = PackageValueHead(nSecsType::ASCII, iLength);
	ba += GetByteArray(str, iLength, fill);

	return ba;
}

QByteArray CSecs::PackageValue(int bnum)
{
	QByteArray ba;

	//ba = PackageValueHead(nSecsType::Binary, 1);

	ba += IntToByte(bnum, 1);
	return ba;
}

QByteArray CSecs::PackageHead(QByteArray stream, 
	QByteArray function, 
	int iSendFlowNum, 
	int iBodyByteCount, 
	QByteArray ptype /*= Hex0*/, 
	QByteArray stype /*= Hex0*/, 
	QByteArray session1 /*= Hex0*/, 
	QByteArray session2 /*= Hex0*/)
{
	QByteArray ba;
	if (stream.isEmpty() ||
		function.isEmpty())
		return ba;
	ba += session1;
	ba += session2;
	ba += stream;
	ba += function;
	ba += ptype;
	ba += stype;
	//iSendFlowNum++;
	ba += IntToByte(iSendFlowNum);

	iBodyByteCount += ba.count();
	ba = IntToByte(iBodyByteCount) + ba;
	return ba;
}

QByteArray CSecs::PackageHead(int iStream, 
	int iFunction, 
	int iSendFlowNum, 
	int iBodyByteCount, 
	QByteArray ptype /*= Hex0*/, 
	QByteArray stype /*= Hex0*/, 
	QByteArray session1 /*= Hex0*/, 
	QByteArray session2 /*= Hex0*/)
{
	QByteArray ba;
	if (iStream <= 0 ||
		iFunction <= 0)
		return ba;
	ba += session1;
	ba += session2;
	ba += (char)iStream;
	ba += (char)iFunction;
	ba += ptype;
	ba += stype;
	//iSendFlowNum++;
	ba += IntToByte(iSendFlowNum);

	iBodyByteCount += ba.count();
	ba = IntToByte(iBodyByteCount) + ba;
	return ba;
}


bool CSecs::ParseData(const QByteArray &data, sSecsData &secsData)
{
	//int iLength = data.left(sSecsData::nSecsHeader::LengthCount).toHex().toInt(nullptr, 16);
	int iLength = ByteToInt(data.mid(sSecsData::nSecsHeader::LengthIndex, sSecsData::nSecsHeader::LengthCount));
	if (iLength < (int)sSecsData::nSecsHeader::SecsHeaderLength - 
		(int)sSecsData::nSecsHeader::LengthCount)
		return false;
	if ((iLength + sSecsData::nSecsHeader::LengthCount) > data.count())
		return false;

	//int iSessionId = data.mid(sSecsData::nSecsHeader::SessionIdIndex, 
	//	sSecsData::nSecsHeader::SessionIdCount).toHex().toInt(nullptr, 16);
	int iSessionId = ByteToInt(data.mid(sSecsData::nSecsHeader::SessionIdIndex, sSecsData::nSecsHeader::SessionIdCount));
	//int iPtype = data.mid(sSecsData::nSecsHeader::PtypeIndex, 1).toHex().toInt(nullptr, 16);
	int iPtype = ByteToInt(data.mid(sSecsData::nSecsHeader::PtypeIndex, 1));
	//int iStype = data.mid(sSecsData::nSecsHeader::StypeIndex, 1).toHex().toInt(nullptr, 16);
	int iStype = ByteToInt(data.mid(sSecsData::nSecsHeader::StypeIndex, 1));
	//int iFlowNum = data.mid(sSecsData::nSecsHeader::SystemByteIndex, 
	//	sSecsData::nSecsHeader::SystemByteCount).toHex().toInt(nullptr, 16);
	int iFlowNum = ByteToInt(data.mid(sSecsData::nSecsHeader::SystemByteIndex, sSecsData::nSecsHeader::SystemByteCount));
	//int iStreamId = data.mid(sSecsData::nSecsHeader::StreamIdIndex, 1).toHex().toInt(nullptr, 16);
	int iStreamId = ByteToInt(data.mid(sSecsData::nSecsHeader::StreamIdIndex, 1));
	//int iFunctionId = data.mid(sSecsData::nSecsHeader::FunctionIdIndex, 1).toHex().toInt(nullptr, 16);
	int iFunctionId = ByteToInt(data.mid(sSecsData::nSecsHeader::FunctionIdIndex, 1));
	if (iStreamId > 0x80)
		iStreamId -= 0x80;

	secsData.iLength = iLength + sSecsData::nSecsHeader::LengthCount;
	secsData.iSession1 = iSessionId >> 8;
	secsData.iSession2 = (iSessionId & 0xFF);
	secsData.iStreamId = iStreamId;
	secsData.iFunctionId = iFunctionId;
	secsData.iPtype = iPtype;
	secsData.iStype = iStype;
	secsData.iFlowNum = iFlowNum;
	secsData.strFlowName = QString("S%1F%2").arg(iStreamId).arg(iFunctionId);

	int iOffset(sSecsData::nSecsHeader::SecsHeaderLength);
	if (iOffset >= secsData.iLength)
		return true;
	bool ret = ParseValue(data, iOffset, secsData.secsdata);
	if (!ret)
		qCritical() << QString::fromUtf8("发生严重解析错误, 位置: %1.").arg(iOffset);
	
	return ret;
}

QByteArray CSecs::IntToByte(const int iNum, int iByteCount /*= 4*/)
{
	QByteArray ba;
	int i = iNum;
	for (int iByteIndex = 0; iByteIndex < iByteCount; ++iByteIndex)
	{
		ba = (char)(i & 0xFF) + ba;
		i = i >> 8;
	}
	return ba;
}

int CSecs::ByteToInt(const QByteArray& ba)
{
	//int num = 0;
	//int length = ba.length();
	//for (int i = 0; i < length; i++)
	//{
	//	int offset = (length - 1 - i) * 8;
	//	num |= (ba[i] & 0xFF) << offset;
	//}
	//return num;

	return ba.toHex().toInt(nullptr, 16);
}

QByteArray CSecs::GetByteArray(const QString &str, int len, char fill /*= ' '*/)
{
	QByteArray ba(str.toLocal8Bit());
	ba = ba.leftJustified(len, fill, true);
	return ba;
}

bool CSecs::ParseValueHead(const QByteArray &data, 
	int &iOffset, 
	nSecsType &ntype,
	int &iBegin, 
	int &iHeadSize, 
	int &iValueSize)
{
	int iDataCount = data.count();
	if ((iOffset + 1) >= iDataCount)
		return false;

	iBegin = ByteToInt(data.mid(iOffset, 1));

	// Bits - 7 6 5 4 3 2  Bits - 1 0
	ntype = (nSecsType)(iBegin & 0b11111100);
	iHeadSize = iBegin & 0b00000011;

	if ((iOffset + iHeadSize) > iDataCount)//长度校验
		return false;

	//取出长度
	QByteArray bytesize = data.mid(iOffset + 1, iHeadSize);
	iValueSize = ByteToInt(bytesize);

	if ((iOffset + iHeadSize + iValueSize) > data.count())
		return false;

	iOffset += iHeadSize + 1;

	return true;
}

bool CSecs::ParseValue(const QByteArray &data, int &iOffset, sSecsItem &secsItem)
{
	nSecsType ntype;
	int iBegin(0), iHeadSize(0), iValueSize(0);
	bool ret = ParseValueHead(data, iOffset, ntype, iBegin, iHeadSize, iValueSize);
	if (!ret)
		return false;

	secsItem.nType = ntype;
	int iDataCount = data.count();
	if (ntype == nSecsType::List)
	{
		secsItem.iValueSize = iValueSize;
		for (int index = 0; index < iValueSize; index++)
		{
			sSecsItem secsItemTemp;
			ret &= ParseValue(data, iOffset, secsItemTemp);
			if (!ret)
				return false;
			secsItem.lstSecsItem.append(secsItemTemp);
		}
	}
	else
	{
		secsItem.iValueSize = iValueSize;
		if ((iOffset + iValueSize) > iDataCount)
			return false;
		secsItem.bytearray = data.mid(iOffset, iValueSize);
		iOffset += iValueSize;
	}

	return true;
}
