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
	// 参数: 	const QString & fileName: 源文件
	// 参数: 	const QString & newName: 目标文件
	// 参数: 	bool isReplace: 如果文件已存在, 是否替换
	// 返回:	bool
	// 功能:	文件拷贝, 仅用于文件的拷贝
	//************************************
	static bool CopyFile(const QString &fileName,
		const QString &newName,
		bool replace = false);

	//************************************
	// 参数: 	const QString & fileName: 源文件
	// 参数: 	const QString & newName: 目标文件
	// 参数: 	TFileShowQuestion pQuestion: 弹窗函数, 为nullptr时不弹窗
	// 参数: 	TFileShowQuestion pInformation: 弹窗函数, 为nullptr时不弹窗
	// 参数: 	QWidget * parent:父窗口, 仅在pQuestion/pInformation!=nullptr时生效
	// 返回:	bool
	// 功能:	文件拷贝, 仅用于文件的拷贝, 带弹窗
	//************************************
	static bool CopyFileWithWidget(const QString &fileName, 
		const QString &newName, 
		TFileMsgBox pQuestion = QMessageBox::question,
		TFileMsgBox pInformation = QMessageBox::information,
		QWidget* parent = nullptr);

	//************************************
	// 参数: 	QString strFilePath: 文件路径
	// 返回:	bool
	// 功能:	创建文件(普通文件, 特殊文件重写) 
	//************************************
	static bool CreateFile(const QString &strFilePath);

	//************************************
	// 参数: 	QString strFilePath
	// 返回:	bool
	// 功能:	使用WinApi删除文件/文件夹(包含文件夹内文件)至回收站
	//************************************
	static bool DeleteFileToCrash(const QString &strFilePath);

	//************************************
	// 参数: 	const QString & strDrivePath: 路径(指定驱动器/文件夹)
	// 返回:	long: 成功返回S_OK
	// 功能:	使用WinApi清除指定路径下的回收站
	//************************************
	static long EmptyRecycleBin(const QString &strPath = QString());

	//************************************
	// 参数: 	const QString & strFrom:
	// 参数: 	const QString & strTo:
	// 参数:	bool replace
	// 返回:	bool
	// 功能:	使用WinApi移动文件/文件夹
	//************************************
	static bool MoveFile(const QString &strFrom, const QString &strTo, bool replace = false);

#pragma region 压缩解压
	//************************************
	// 参数: 	const QString & strTargetPath: 目标文件(压缩后的)
	// 参数: 	const QStringList & lstSourcePath: 源文件/文件夹列表
	// 参数: 	bool replace: 如果目标文件已存在, 是否替换
	// 参数: 	bool delSource: 压缩后是否删除原文件
	// 返回:	bool
	// 功能:	压缩文件/文件夹(需要依赖7z.exe和7z.dll库, 相关软件参见..\3rdParty\7-Zip)
	//************************************
	static bool Compress(const QString& strTargetPath, 
		const QStringList& lstSourcePath, 
		bool replace = false,
		bool delSource = false);

	//************************************
	// 参数: 	const QString & strSourcePath: 压缩文件的目录
	// 参数: 	const QString & strTargetPath: 需要解压到的目录(文件夹)
	// 参数: 	bool replace: 如果文件已存在, 是否替换
	// 返回:	bool
	// 功能:	解压文件
	//************************************
	static bool Decompress(const QString& strSourcePath, 
		const QString& strTargetPath, 
		bool replace = false);
#pragma endregion

};


#endif // !_TFILE_H_ 

