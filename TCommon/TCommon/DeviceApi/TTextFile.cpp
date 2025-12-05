#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include "TDefine.h"
#include "TTextCodec.h"
#include "TTextFile.h"

#ifndef TEXT_BIG_FILE_SIZE
#define TEXT_BIG_FILE_SIZE (100*MB)
#endif

QByteArray TTextFile::GetFileContent(const QString &strFilePath)
{
	QFile file(strFilePath);
	QByteArray txt;
	if (file.open(QIODevice::ReadOnly))
		txt = file.readAll();
	file.close();
	return txt;
}

bool TTextFile::SetFileContent(QString strFilePath, QByteArray content)
{
	QFileInfo fileInfo(strFilePath);
	QString strPath = fileInfo.absolutePath();
	QDir dir;
	if (!dir.exists(strPath) &&
		!dir.mkpath(strPath))
		return false;

	QFile file(strFilePath);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly))
		return false;
	file.write(content);
	return true;
}

bool TTextFile::AppendText(const QString &strFilePath, 
	const QString &strContent, 
	bool isAutoDetectCode /*= true*/)
{
	if (!QFile::exists(strFilePath) &&
		!CreateFile(strFilePath))
		return false;
	
	QTextCodec *textCodec(nullptr);
#ifdef _TTEXTCODEC_H_//TTextCodec.h
	if (isAutoDetectCode)
	{
		QByteArray codec;
		QByteArray ba = TTextFile::GetFileContent(strFilePath);
		TTextCodec::GetCorrectUnicode(ba, codec);
		textCodec = TTextCodec::codecForName(codec);
	}
#endif // _TTEXTCODEC_H_

	QFile file(strFilePath);
	if (!file.open(QIODevice::Append | QIODevice::Text))
		return false;
	QTextStream textStream(&file);
	if (textCodec != nullptr)
		textStream.setCodec(textCodec);
	textStream << strContent;
	file.flush();
	file.close();
	return true;
}

QString TTextFile::ReadText(const QString& strFilePath)
{
	QByteArray ba;
	return ReadText(strFilePath, ba);
}

QString TTextFile::ReadText(const QString &strFilePath,
	QByteArray &codec/* = "UTF-8" "GBK"*/ /*= QByteArray()*/)
{
	QByteArray ba = GetFileContent(strFilePath);
	QString txt(ba);
#ifdef _TTEXTCODEC_H_//TTextCodec.h
	if (!ba.isEmpty())
		txt = TTextCodec::GetCorrectUnicode(ba, codec);
#endif // _TTEXTCODEC_H_
	return txt;
}

bool TTextFile::WriteText(const QString &strFilePath, 
	const QString &strContent, 
	const QByteArray &codec/* = "UTF-8" "GBK"*/)
{
	if (!QFile::exists(strFilePath) &&
		!CreateFile(strFilePath))
		return false;

	QTextCodec *textCodec(nullptr);
	if (!codec.isEmpty())
		textCodec->codecForName(codec);

	QFile file(strFilePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;
	QTextStream textStream(&file);
	if (textCodec != nullptr)
		textStream.setCodec(textCodec);
	textStream << strContent;
	file.flush();
	file.close();
	return true;
}

bool TTextFile::InsertText(const QString &strFilePath, 
	int pos, const QString &strData)
{
	if (!QFile::exists(strFilePath))
		return false;

	QFileInfo fileInfo(strFilePath);
	if (!fileInfo.isFile())
		return false;
	
	unsigned long long size = fileInfo.size();
	if (size < TEXT_BIG_FILE_SIZE)
	{
		QByteArray codec;
		QString txt = ReadText(strFilePath, codec);
		if (txt.count() >= pos)
			txt.insert(pos, strData);
		else
			return false;
		return WriteText(strFilePath, txt, codec);
	}
	else
	{
		QFile file(strFilePath);
		if (!file.open(QIODevice::ReadOnly))
			return false;

		QString strTempFilePath = QDir::tempPath() + QDir::separator()
			+ SIGN_TILDE + fileInfo.fileName();
		if (QFile::exists(strTempFilePath) &&
			!QFile::remove(strTempFilePath))
			return false;
		QFile tempFile(strTempFilePath);
		if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
			return false;

		QTextCodec *codec(nullptr);
		QTextStream textStream(&file), tempTextStream(&tempFile);
		textStream.setAutoDetectUnicode(true);
		int iTotalPos(0);
		while (!textStream.atEnd())
		{
			if (iTotalPos != 0)
			{
				int iLineFeedCount = QString(LINE_FEED).count();
				if (iTotalPos < pos &&
					iTotalPos + iLineFeedCount > pos)
					return false;//插入位置刚好在换行符的位置
				iTotalPos += iLineFeedCount;
				tempTextStream << endl;
			}

			QString strLine = textStream.readLine();
			int iCount = strLine.count();
			if (iCount > 0 &&
				codec == nullptr)
			{
				codec = textStream.codec();
				tempTextStream.setCodec(codec);
			}

			if (iTotalPos <= pos &&
				iTotalPos + iCount > pos)
				strLine = strLine.insert(pos - iTotalPos, strData);
			iTotalPos += iCount;
			tempTextStream << strLine;
		}
		file.close();
		tempFile.flush();
		tempFile.close();
		
		return CopyFile(strTempFilePath, strFilePath, true);
	}
}

bool TTextFile::InsertText(const QString &strFilePath, 
	int line, int linePos, const QString &strData)
{
	if (!QFile::exists(strFilePath))
		return false;

	QFileInfo fileInfo(strFilePath);
	if (!fileInfo.isFile())
		return false;

	QFile file(strFilePath);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QString strTempFilePath = QDir::tempPath() + QDir::separator()
		+ SIGN_TILDE + fileInfo.fileName();
	if (QFile::exists(strTempFilePath) &&
		!QFile::remove(strTempFilePath))
		return false;
	QFile tempFile(strTempFilePath);
	if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextCodec *codec(nullptr);
	QTextStream textStream(&file), tempTextStream(&tempFile);
	textStream.setAutoDetectUnicode(true);
	int iLine(0);
	while (!textStream.atEnd())
	{
		if (iLine > 0)
			tempTextStream << endl;

		QString strLine = textStream.readLine();
		int iCount = strLine.count();
		if (iCount > 0 &&
			codec == nullptr)
		{
			codec = textStream.codec();
			tempTextStream.setCodec(codec);
		}

		iLine++;
		if (iLine == line)
		{
			if (iCount >= linePos)
				strLine = strLine.insert(linePos, strData);
			else
				return false;
		}
		tempTextStream << strLine;
	}
	file.close();
	tempFile.flush();
	tempFile.close();

	return CopyFile(strTempFilePath, strFilePath, true);
}

bool TTextFile::InsertText(const QString &strFilePath, 
	const QList<std::tuple<int, int, QString>> &lstTuple)
{
	if (!QFile::exists(strFilePath))
		return false;

	QFileInfo fileInfo(strFilePath);
	if (!fileInfo.isFile())
		return false;

	QFile file(strFilePath);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QString strTempFilePath = QDir::tempPath() + QDir::separator()
		+ SIGN_TILDE + fileInfo.fileName();
	if (QFile::exists(strTempFilePath) &&
		!QFile::remove(strTempFilePath))
		return false;
	QFile tempFile(strTempFilePath);
	if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextCodec *codec(nullptr);
	QTextStream textStream(&file), tempTextStream(&tempFile);
	textStream.setAutoDetectUnicode(true);
	int iLine(0);
	while (!textStream.atEnd())
	{
		if (iLine > 0)
			tempTextStream << endl;

		QString strLine = textStream.readLine();
		int iCount = strLine.count();
		if (iCount > 0 &&
			codec == nullptr)
		{
			codec = textStream.codec();
			tempTextStream.setCodec(codec);
		}

		iLine++;
		for (int index = 0; index < lstTuple.count(); index++)
		{
			const std::tuple<int, int, QString> &t = lstTuple.at(index);
			int line = std::get<0>(t);
			if (iLine == line)
			{
				int linePos = std::get<1>(t);
				QString strData = std::get<2>(t);
				if (iCount >= linePos)
					strLine = strLine.insert(linePos, strData);
				else
					return false;
			}
		}
		tempTextStream << strLine;
	}
	file.close();
	tempFile.flush();
	tempFile.close();

	return CopyFile(strTempFilePath, strFilePath, true);
}

bool TTextFile::ReplaceText(const QString& strFilePath, 
	const QString& strKey, 
	const QString& strNew, 
	bool isReplaceAll, 
	TTextFileIsReplaceFun Fun)
{
	QList<int> lstReplacedPos;
	return ReplaceText(strFilePath, 
		strKey,
		lstReplacedPos,
		strNew,
		isReplaceAll,
		Fun);
}

bool TTextFile::ReplaceText(const QString &strFilePath,
	const QString &strKey, 
	QList<int>& lstReplacedPos /*= QList<int>()*/,
	const QString &strNew /*= QString()*/, 
	bool isReplaceAll /*= true*/, 
	TTextFileIsReplaceFun Fun /*= nullptr*/)
{
	QByteArray codec;
	QString txt = ReadText(strFilePath, codec);
	if (txt.isEmpty())
		return false;

	while (txt.contains(strKey))
	{
		if (Fun(txt, strKey))
		{
			int iReplacedPos = txt.indexOf(strKey);
			lstReplacedPos.append(iReplacedPos);
			txt.replace(strKey, strNew);
		}
		if (!isReplaceAll)
			break;
	}
	return WriteText(strFilePath, txt, codec);
}

bool TTextFile::ReplaceText(const QString& strFilePath, 
	const QRegExp& rx, 
	const QString& strNew, 
	bool isReplaceAll)
{
	QList<int> lstReplacedPos;
	return ReplaceText(strFilePath,
		rx,
		lstReplacedPos,
		strNew,
		isReplaceAll);
}

bool TTextFile::ReplaceText(const QString &strFilePath,
	const QRegExp &rx, 
	QList<int>& lstReplacedPos /*= QList<int>()*/,
	const QString &strNew /*= QString()*/, 
	bool isReplaceAll /*= true*/)
{
	QByteArray codec;
	QString txt = ReadText(strFilePath, codec);
	if (txt.isEmpty())
		return false;

	while (txt.contains(rx))
	{
		int iReplacedPos = txt.indexOf(rx);
		lstReplacedPos.append(iReplacedPos);
		txt.replace(rx, strNew);
		if (!isReplaceAll)
			break;
	}
	return WriteText(strFilePath, txt, codec);
}

bool TTextFile::ReplaceText(const QString &strFilePath, 
	const QString &strKey, 
	TTextFileReplaceNewFun Fun)
{
	QByteArray codec;
	QString txt = ReadText(strFilePath, codec);
	if (txt.isEmpty())
		return false;

	if (txt.contains(strKey))
		txt = Fun(txt, strKey);

	return WriteText(strFilePath, txt, codec);
}

bool TTextFile::ReplaceText(const QString &strFilePath, 
	const QRegExp &rx, 
	TTextFileReplaceNewFunReg Fun)
{
	QByteArray codec;
	QString txt = ReadText(strFilePath, codec);
	if (txt.isEmpty())
		return false;
	
	if (txt.contains(rx))
		txt = Fun(txt, rx);

	return WriteText(strFilePath, txt, codec);
}

