#ifndef _TTEXTFILE_H_
#define _TTEXTFILE_H_

#include <tuple>
#include "TFile.h"

typedef bool(*TTextFileIsReplaceFun)(const QString &text/*ȫ�ı�*/, const QString &strKey/*����Ŀ��*/);
typedef QString(*TTextFileReplaceNewFun)(const QString &text/*ȫ�ı�*/, const QString &strKey/*����Ŀ��*/);
typedef QString(*TTextFileReplaceNewFunReg)(const QString &text/*ȫ�ı�*/, const QRegExp &rx/*����������ʽ*/);
class TTextFile : public TFile
{
public:
	//��ȡ�ļ�����(�ֽ���)
	static QByteArray GetFileContent(const QString &strFilePath);

	//д�ļ�(�ֽ���)
	static bool SetFileContent(QString strFilePath, QByteArray content);

	//************************************
	// ����: 	const QString & strFilePath: �ļ�·��
	// ����: 	const QString & strContent: �ı�����
	// ����: 	bool isAutoDetectCode: �Ƿ��Զ��������ʽ
	// ����:	bool
	// ����:	�ı�ĩβ׷������
	//************************************
	static bool AppendText(const QString &strFilePath,
		const QString &strContent,
		bool isAutoDetectCode = true);

	//************************************
	// ����: 	const QString & strFilePath: �ļ�·��
	// ����: 	const QByteArray & codec:�������,  Ĭ�ϱ����ʽ, 
	//					����ñ����ʽƥ����ı�, ��ʹ�øñ����ʽ��ȡ;
	//					����ñ����ʽ��ƥ����ı�, ��ʹ��ƥ��ı����ʽ��ȡ;
	// ����:	QT_NAMESPACE::QString
	// ����:	��ȡ�ı�(�ַ���), ������ı��ı����ʽʱ, ����ʹ�øú���
	//************************************
	static QString ReadText(const QString &strFilePath,
		QByteArray &codec/* = "UTF-8" "GBK"*/ = QByteArray());

	static bool WriteText(const QString &strFilePath,
		const QString &strContent,
		const QByteArray &codec/* = "UTF-8" "GBK"*/);

	//************************************
	// ����: 	const QString &strFilePath: �ļ�·��
	// ����: 	int pos: ����λ��
	// ����: 	const QString &strData: ��������
	// ����:	bool
	// ����:	���ļ���ĳ��λ�ò����ַ�
	//************************************
	static bool InsertText(const QString &strFilePath, int pos,
		const QString &strData);

	//************************************
	// ����: 	const QString & strFilePath: �ļ�·��
	// ����: 	int line: ��,��1��ʼ
	// ����: 	int linePos: �����ڵ�λ��
	// ����: 	const QString & strData: ��������
	// ����:	bool
	// ����:	���ļ���ָ���е�ĳ��λ�ò����ַ�
	//************************************
	static bool InsertText(const QString &strFilePath, 
		int line,
		int linePos,
		const QString &strData);

	//************************************
	// ����: 	const QString & strFilePath: �ļ�·��
	// ����: 	const QList<std::tuple<int, int, QString>>& lstTuple: std::tuple<�кŴ�1��ʼ, �����ڵ�λ��, ��������>
	// ����:	bool
	// ����:	���ļ���ָ��һЩ�е�ָ��λ�ò���ָ���ַ�
	//************************************
	static bool InsertText(const QString &strFilePath,
		const QList<std::tuple<int, int, QString>> &lstTuple);

	
	//************************************
	// ����: 	const QString & strFilePath: �ı�·��
	// ����: 	const QString & strKey: ����Ŀ��
	// ����: 	const QString & strNew: �滻�ı�
	// ����: 	bool isReplaceAll: �Ƿ��滻����
	// ����: 	QList<int> &lstReplacedPos: ����ֵ, �滻��λ��
	// ����: 	TTextFileIsReplaceFun Fun: �жϺ���, ������ҵ����ı�λ����������滻�ж�
	//				Ӧ���滻����true, ���򷵻�false
	// ����:	bool
	// ����:	�滻�ı�
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QString &strKey,
		const QString &strNew = QString(),
		bool isReplaceAll = true,
		QList<int> &lstReplacedPos = QList<int>(),
		TTextFileIsReplaceFun Fun = nullptr);

	//************************************
	// ����: 	const QString & strFilePath: �ı�·��
	// ����: 	const QRegExp & rx: ������ʽ
	// ����: 	const QString & strNew: �滻�ı�
	// ����: 	bool isReplaceAll: �Ƿ��滻����
	// ����: 	QList<int> & lstReplacedPos: �滻��λ��
	// ����:	bool
	// ����:	�滻�ı�
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QRegExp &rx,
		const QString &strNew = QString(),
		bool isReplaceAll = true,
		QList<int> &lstReplacedPos = QList<int>());

	//************************************
	// ����: 	const QString & strFilePath: �ı�·��
	// ����: 	const QString & strKey: ����Ŀ��
	// ����: 	TTextFileReplaceNewFun fun: ������, �����滻����ı�
	// ����:	bool
	// ����:	�滻�ı�
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QString &strKey,
		TTextFileReplaceNewFun Fun);

	//************************************
	// ����: 	const QString & strFilePath: �ı�·��
	// ����: 	const QRegExp & rx: ������ʽ
	// ����: 	TTextFileReplaceNewFunReg Fun: ������, �����滻����ı�
	// ����:	bool
	// ����:	�滻�ı�
	//************************************
	static bool ReplaceText(const QString &strFilePath,
		const QRegExp &rx,
		TTextFileReplaceNewFunReg Fun);

private:
};


#endif
