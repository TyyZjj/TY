#ifndef _TFILE_H_ 
#define _TFILE_H_

#include <QFile>
#include <QString>
#include <QMessageBox>
#include "SFile.h"

typedef QMessageBox::StandardButton(*TFileMsgBox)(QWidget *,
	const QString &,
	const QString&,
	QMessageBox::StandardButtons,
	QMessageBox::StandardButton);

class TFile : public QFile
{
public:
	//************************************
	// ����: 	const QString & fileName: Դ�ļ�
	// ����: 	const QString & newName: Ŀ���ļ�
	// ����: 	bool isReplace: ����ļ��Ѵ���, �Ƿ��滻
	// ����:	bool
	// ����:	�ļ�����, �������ļ��Ŀ���
	//************************************
	static bool CopyFile(const QString &fileName,
		const QString &newName,
		bool replace = false);

	//************************************
	// ����: 	const QString & fileName: Դ�ļ�
	// ����: 	const QString & newName: Ŀ���ļ�
	// ����: 	TFileShowQuestion pQuestion: ��������, Ϊnullptrʱ������
	// ����: 	TFileShowQuestion pInformation: ��������, Ϊnullptrʱ������
	// ����: 	QWidget * parent:������, ����pQuestion/pInformation!=nullptrʱ��Ч
	// ����:	bool
	// ����:	�ļ�����, �������ļ��Ŀ���, ������
	//************************************
	static bool CopyFileWithWidget(const QString &fileName, 
		const QString &newName, 
		TFileMsgBox pQuestion = QMessageBox::question,
		TFileMsgBox pInformation = QMessageBox::information,
		QWidget* parent = nullptr);

	//************************************
	// ����: 	QString strFilePath: �ļ�·��
	// ����:	bool
	// ����:	�����ļ�(��ͨ�ļ�, �����ļ���д) 
	//************************************
	static bool CreateFile(const QString &strFilePath);

	//************************************
	// ����: 	QString strFilePath
	// ����:	bool
	// ����:	ʹ��WinApiɾ���ļ�/�ļ���(�����ļ������ļ�)������վ
	//************************************
	static bool DeleteFileToCrash(const QString &strFilePath);

	//************************************
	// ����: 	const QString & strDrivePath: ·��(ָ��������/�ļ���)
	// ����:	long: �ɹ�����S_OK
	// ����:	ʹ��WinApi���ָ��·���µĻ���վ
	//************************************
	static long EmptyRecycleBin(const QString &strPath = QString());

	//************************************
	// ����: 	const QString & strFrom:
	// ����: 	const QString & strTo:
	// ����:	bool replace
	// ����:	bool
	// ����:	ʹ��WinApi�ƶ��ļ�/�ļ���
	//************************************
	static bool MoveFile(const QString &strFrom, const QString &strTo, bool replace = false);

#pragma region ѹ����ѹ
	//************************************
	// ����: 	const QString & strTargetPath: Ŀ���ļ�(ѹ�����)
	// ����: 	const QStringList & lstSourcePath: Դ�ļ�/�ļ����б�
	// ����: 	bool replace: ���Ŀ���ļ��Ѵ���, �Ƿ��滻
	// ����: 	bool delSource: ѹ�����Ƿ�ɾ��ԭ�ļ�
	// ����:	bool
	// ����:	ѹ���ļ�/�ļ���(��Ҫ����7z.exe��7z.dll��, �������μ�..\3rdParty\7-Zip)
	//************************************
	static bool Compress(const QString& strTargetPath, 
		const QStringList& lstSourcePath, 
		bool replace = false,
		bool delSource = false);

	//************************************
	// ����: 	const QString & strSourcePath: ѹ���ļ���Ŀ¼
	// ����: 	const QString & strTargetPath: ��Ҫ��ѹ����Ŀ¼(�ļ���)
	// ����: 	bool replace: ����ļ��Ѵ���, �Ƿ��滻
	// ����:	bool
	// ����:	��ѹ�ļ�
	//************************************
	static bool Decompress(const QString& strSourcePath, 
		const QString& strTargetPath, 
		bool replace = false);
#pragma endregion

};


#endif // !_TFILE_H_ 

