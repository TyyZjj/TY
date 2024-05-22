#ifndef _TTEXTFILE_H_
#define _TTEXTFILE_H_

#include <tuple>
#include "TFile.h"

typedef bool(*TTextFileIsReplaceFun)(const QString &text/*全文本*/, const QString &strKey/*查找目标*/);
typedef QString(*TTextFileReplaceNewFun)(const QString &text/*全文本*/, const QString &strKey/*查找目标*/);
typedef QString(*TTextFileReplaceNewFunReg)(const QString &text/*全文本*/, const QRegExp &rx/*查找正则表达式*/);
class TTextFile : public TFile
{
public:
	//读取文件内容(字节流)
	static QByteArray GetFileContent(const QString &strFilePath);

	//写文件(字节流)
	static bool SetFileContent(QString strFilePath, QByteArray content);

	//************************************
	// 参数: 	const QString & strFilePath: 文件路径
	// 参数: 	const QString & strContent: 文本内容
	// 参数: 	bool isAutoDetectCode: 是否自动检测编码格式
	// 返回:	bool
	// 功能:	文本末尾追加内容
	//************************************
	static bool AppendText(const QString &strFilePath,
		const QString &strContent,
		bool isAutoDetectCode = true);

	//************************************
	// 参数: 	const QString & strFilePath: 文件路径
	// 参数: 	const QByteArray & codec:输入输出,  默认编码格式, 
	//					如果该编码格式匹配该文本, 则使用该编码格式读取;
	//					如果该编码格式不匹配该文本, 则使用匹配的编码格式读取;
	// 返回:	QT_NAMESPACE::QString
	// 功能:	读取文本(字符串), 不清楚文本的编码格式时, 可以使用该函数
	//************************************
	static QString ReadText(const QString &strFilePath,
		QByteArray &codec/* = "UTF-8" "GBK"*/ = QByteArray());

	static bool WriteText(const QString &strFilePath,
		const QString &strContent,
		const QByteArray &codec/* = "UTF-8" "GBK"*/);

	//************************************
	// 参数: 	const QString &strFilePath: 文件路径
	// 参数: 	int pos: 插入位置
	// 参数: 	const QString &strData: 插入数据
	// 返回:	bool
	// 功能:	在文件的某个位置插入字符
	//************************************
	static bool InsertText(const QString &strFilePath, int pos,
		const QString &strData);

	//************************************
	// 参数: 	const QString & strFilePath: 文件路径
	// 参数: 	int line: 行,从1开始
	// 参数: 	int linePos: 在行内的位置
	// 参数: 	const QString & strData: 插入数据
	// 返回:	bool
	// 功能:	在文件的指定行的某个位置插入字符
	//************************************
	static bool InsertText(const QString &strFilePath, 
		int line,
		int linePos,
		const QString &strData);

	//************************************
	// 参数: 	const QString & strFilePath: 文件路径
	// 参数: 	const QList<std::tuple<int, int, QString>>& lstTuple: std::tuple<行号从1开始, 在行内的位置, 插入数据>
	// 返回:	bool
	// 功能:	在文件的指定一些行的指定位置插入指定字符
	//************************************
	static bool InsertText(const QString &strFilePath,
		const QList<std::tuple<int, int, QString>> &lstTuple);

	
	//************************************
	// 参数: 	const QString & strFilePath: 文本路径
	// 参数: 	const QString & strKey: 查找目标
	// 参数: 	const QString & strNew: 替换文本
	// 参数: 	bool isReplaceAll: 是否替换所有
	// 参数: 	QList<int> &lstReplacedPos: 返回值, 替换的位置
	// 参数: 	TTextFileIsReplaceFun Fun: 判断函数, 对与查找到的文本位置做额外的替换判断
	//				应该替换返回true, 否则返回false
	// 返回:	bool
	// 功能:	替换文本
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QString &strKey,
		const QString &strNew = QString(),
		bool isReplaceAll = true,
		QList<int> &lstReplacedPos = QList<int>(),
		TTextFileIsReplaceFun Fun = nullptr);

	//************************************
	// 参数: 	const QString & strFilePath: 文本路径
	// 参数: 	const QRegExp & rx: 正则表达式
	// 参数: 	const QString & strNew: 替换文本
	// 参数: 	bool isReplaceAll: 是否替换所有
	// 参数: 	QList<int> & lstReplacedPos: 替换的位置
	// 返回:	bool
	// 功能:	替换文本
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QRegExp &rx,
		const QString &strNew = QString(),
		bool isReplaceAll = true,
		QList<int> &lstReplacedPos = QList<int>());

	//************************************
	// 参数: 	const QString & strFilePath: 文本路径
	// 参数: 	const QString & strKey: 查找目标
	// 参数: 	TTextFileReplaceNewFun fun: 处理函数, 返回替换后的文本
	// 返回:	bool
	// 功能:	替换文本
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QString &strKey,
		TTextFileReplaceNewFun Fun);

	//************************************
	// 参数: 	const QString & strFilePath: 文本路径
	// 参数: 	const QRegExp & rx: 正则表达式
	// 参数: 	TTextFileReplaceNewFunReg Fun: 处理函数, 返回替换后的文本
	// 返回:	bool
	// 功能:	替换文本
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QRegExp &rx,
		TTextFileReplaceNewFunReg Fun);

private:
};


#endif
