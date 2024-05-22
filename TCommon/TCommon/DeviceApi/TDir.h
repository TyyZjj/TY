#ifndef _TDIR_H_ 
#define _TDIR_H_

#include <QDir>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QFileInfo>
#include "SDir.h"

typedef QMessageBox::StandardButton(*TDirMsgBox)(QWidget *,
	const QString &,
	const QString&,
	QMessageBox::StandardButtons,
	QMessageBox::StandardButton);

typedef bool(*TRemoveCondition)(const QString &strDirPath);
typedef void(*TRemoveSortRule)(QFileInfoList &lstFileInfo);
typedef bool(*TRemoveRule)(const QFileInfoList &lstFileInfo, int iFileIndex);


struct TDirNode
{
	QString				strDirName;
	QFileInfo			fileInfo;
	TDirNode			*parent = nullptr;
	QList<TDirNode*>	lstChild;

	TDirNode(QString strDirPath, TDirNode* parent = nullptr);
	~TDirNode();
	void setParent(TDirNode *parent);
};


class TDir : public QDir
{
public:

	//************************************
	// 参数: 	const QString & strPath: 文件夹路径, 全路径
	// 参数: 	const QStringList & lstNameFilter: 文件名过滤关键词, 形如"*.txt"
	// 参数: 	QDir::Filters filters: 筛选规则
	// 参数: 	QDir::SortFlags sort: 排序规则
	// 返回:	QT_NAMESPACE::QStringList: 文件名列表
	// 功能:	获取文件夹下文件的文件名
	//************************************
	static QStringList GetFolderFileNameList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
		QDir::SortFlags sort = QDir::SortFlag::Name);

	//************************************
	// 参数: 	const QString & strFolderPath: 文件夹路径, 全路径
	// 参数: 	const QStringList & lstNameFilter: 名称过滤关键词, 形如"*.txt"
	// 参数: 	bool bSubFolder: 是否包含子文件夹
	// 参数: 	QDir::Filters filters: 筛选规则
	// 参数: 	QDir::SortFlags sort : 排序规则
	// 返回:	QT_NAMESPACE::QFileInfoList : 文件信息列表
	// 功能:	获取文件夹下文件的信息(不遍历文件夹)
	//************************************
	static QFileInfoList GetFolderFileInfoList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
		QDir::SortFlags sort = QDir::SortFlag::Name);

	//************************************
	// 参数: 	const QString & strFolderPath: 文件夹路径, 全路径
	// 参数: 	const QStringList & lstNameFilter: 文件名过滤关键词, 形如"*.txt"
	// 参数: 	bool isContainFolder: 是否包含文件夹
	// 返回:	QT_NAMESPACE::QStringList: 文件名列表
	// 功能:	获取文件夹下所有的文件名
	//************************************
	static QStringList GetFolderAllFileNameList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		bool isContainFolder = false);

	//************************************
	// 参数: 	const QString & strFolderPath: 文件夹路径, 全路径
	// 参数: 	const QStringList & lstNameFilter: 文件名过滤关键词, 形如"*.txt"
	// 参数: 	bool isContainFolder: 是否包含文件夹
	// 返回:	QT_NAMESPACE::QFileInfoList: 文件名列表
	// 功能:	获取文件夹下所有的文件信息(遍历文件夹)
	//************************************
	static QFileInfoList GetFolderAllFileInfoList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		bool isContainFolder = false);

	//************************************
	// 参数: 	const QString & strFolderPath: 文件夹路径, 全路径
	// 返回:	TDirNode
	// 功能:	获取文件夹下的树状结构
	//************************************
	static TDirNode* GetFolderDirTree(const QString &strFolderPath);

	//************************************
	// 参数: 	const QString & source: 源文件夹
	// 参数: 	const QString & destination: 目标文件夹
	// 参数: 	bool replace: 如果文件已存在, 是否替换
	// 参数: 	QFileInfoList & lstSucc: 拷贝成功的文件信息
	// 参数: 	QFileInfoList & lstFail: 拷贝失败的文件信息
	// 返回:	bool: 如果存在文件拷贝成功, 则返回成功; 如果只有一个文件夹, 则创建目标文件夹即成功;
	// 功能:	文件夹拷贝, 仅用于文件夹的拷贝(不可用于文件的拷贝)
	//************************************
	static bool CopyDir(const QString &source,
		const QString &destination,
		bool replace = false,
		QFileInfoList &lstSucc = QFileInfoList(),
		QFileInfoList &lstFail = QFileInfoList());

	//************************************
	// 参数: 	const QString & source: 源文件夹
	// 参数: 	const QString & destination: 目标文件夹
	// 参数: 	TDirMsgBox pQuestion: 弹窗函数, 为nullptr时不弹窗
	// 参数: 	TDirMsgBox pInformation: 弹窗函数, 为nullptr时不弹窗
	// 参数: 	QWidget * parent: 父窗口, 仅在pQuestion!=nullptr时生效
	// 参数: 	QFileInfoList & lstSucc: 拷贝成功的文件信息
	// 参数: 	QFileInfoList & lstFail: 拷贝失败的文件信息
	// 参数: 	int level: 层级, 函数内部递归调用使用, 外部调用不填
	// 返回:	bool: bool: 如果存在文件拷贝成功, 则返回成功; 如果只有一个文件夹, 则创建目标文件夹即成功;
	// 功能:	文件夹拷贝, 仅用于文件夹的拷贝(不可用于文件的拷贝).
	//				原文件夹/文件夹下文件不存在时, 弹出弹窗pInformation
	//				目标文件夹/文件夹下文件已存在时, 弹出弹窗pQuestion
	//************************************
	static bool CopyDirWithWidget(const QString &source,
		const QString &destination,
		TDirMsgBox pQuestion = QMessageBox::question,
		TDirMsgBox pInformation = QMessageBox::information,
		QWidget* parent = nullptr,
		QFileInfoList &lstSucc = QFileInfoList(),
		QFileInfoList &lstFail = QFileInfoList(),
		int level = 0);

	//************************************
	// 参数: 	const QString & source: 源文件/源文件夹
	// 参数: 	const QString & destination: 目标文件/目标文件夹
	// 参数: 	bool replace: 如果目标已存在, 是否替换
	// 参数: 	QFileInfoList & lstSucc: 拷贝成功的文件信息
	// 参数: 	QFileInfoList & lstFail: 拷贝失败的文件信息
	// 返回:	bool: 成功/失败
	// 功能:	文件/文件夹拷贝
	//************************************
	static bool Copy(const QString &source,
		const QString &destination,
		bool replace = false,
		QFileInfoList &lstSucc = QFileInfoList(),
		QFileInfoList &lstFail = QFileInfoList());

	//************************************
	// 参数: 	const QString & source: 源文件/源文件夹
	// 参数: 	const QString & destination: 目标文件/目标文件夹
	// 参数: 	TDirMsgBox pQuestion: 弹窗函数, 为nullptr时不弹窗
	// 参数: 	TDirMsgBox pInformation: 弹窗函数, 为nullptr时不弹窗
	// 参数: 	QWidget * parent: 父窗口, 仅在pQuestion!=nullptr时生效
	// 参数: 	QFileInfoList & lstSucc: 拷贝成功的文件信息
	// 参数: 	QFileInfoList & lstFail: 拷贝失败的文件信息
	// 返回:	bool
	// 功能:	文件/文件夹拷贝
	//************************************
	static bool CopyWithWidget(const QString &source,
		const QString &destination,
		TDirMsgBox pQuestion = QMessageBox::question,
		TDirMsgBox pInformation = QMessageBox::information,
		QWidget* parent = nullptr,
		QFileInfoList &lstSucc = QFileInfoList(),
		QFileInfoList &lstFail = QFileInfoList());

	//文件/文件夹大小
	static unsigned long long Size(const QString &strDirPath);

	//文件/文件夹是否为空 
	//文件夹为空, 返回true
	//文件大小为0, 返回true
	static bool Empty(const QString &strDirPath);

	//************************************
	// 参数: 	const QString & strFolderPath: 文件夹路径
	// 参数: 	const QString & strDirPath: 文件路径
	// 返回:	bool
	// 功能:	文件夹中是否包含此文件
	//************************************
	static bool	Contain(const QString &strFolderPath, const QString &strDirPath);


	//************************************
	// 参数: 	const QString & strPath: 一个路径
	// 返回:	QT_NAMESPACE::QString
	// 功能:	返回一个路径的驱动器路径
	//************************************
	static QString DrivePath(const QString &strPath);

	//************************************
	// 参数: 	const QString & strDirPath: 文件夹路径(必须是文件夹)
	// 参数: 	TRemoveCondition condition: 执行删除的条件(文件夹满足什么条件)
	// 参数: 	TRemoveSortRule sort: 文件夹内文件的排序规则
	// 参数: 	TRemoveRule rule: 删除文件的规则(文件满足什么条件)
	// 返回:	void
	// 功能:	按照一定规则删除文件夹下的文件
	//************************************
	static void RemoveDirByRule(const QString &strDirPath, 
		TRemoveCondition condition = nullptr, 
		TRemoveSortRule sort = nullptr,
		TRemoveRule rule = nullptr);
};


//************************************
// 参数: 	const QString & strDirPath: 文件夹路径(必须是文件夹)
// 参数: 	int iMaxDelete: 最大删除大小(单位MB) 
// 参数: 	int iFreeSpace: 剩余空间门限(单位MB)
// 返回:	void
// 功能:	磁盘将满时, 删除文件
//************************************
void RemoveDirByDisk(const QString &strDirPath, int iMaxDelete = 500, int iFreeSpace = 500);


//************************************
// 参数: 	const QString & strDirPath: : 文件夹路径(必须是文件夹)
// 参数: 	int iMaxSize: 文件夹最大大小(单位MB) 
// 返回:	void
// 功能:	文件夹超过大小时, 删除其中文件
//************************************
void RemoveDirBySize(const QString &strDirPath, int iMaxSize = 500);

#endif // !_TPATH_H_ 


