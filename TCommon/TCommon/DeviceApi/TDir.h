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
	// ����: 	const QString & strPath: �ļ���·��, ȫ·��
	// ����: 	const QStringList & lstNameFilter: �ļ������˹ؼ���, ����"*.txt"
	// ����: 	QDir::Filters filters: ɸѡ����
	// ����: 	QDir::SortFlags sort: �������
	// ����:	QT_NAMESPACE::QStringList: �ļ����б�
	// ����:	��ȡ�ļ������ļ����ļ���
	//************************************
	static QStringList GetFolderFileNameList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
		QDir::SortFlags sort = QDir::SortFlag::Name);

	//************************************
	// ����: 	const QString & strFolderPath: �ļ���·��, ȫ·��
	// ����: 	const QStringList & lstNameFilter: ���ƹ��˹ؼ���, ����"*.txt"
	// ����: 	bool bSubFolder: �Ƿ�������ļ���
	// ����: 	QDir::Filters filters: ɸѡ����
	// ����: 	QDir::SortFlags sort : �������
	// ����:	QT_NAMESPACE::QFileInfoList : �ļ���Ϣ�б�
	// ����:	��ȡ�ļ������ļ�����Ϣ(�������ļ���)
	//************************************
	static QFileInfoList GetFolderFileInfoList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
		QDir::SortFlags sort = QDir::SortFlag::Name);

	//************************************
	// ����: 	const QString & strFolderPath: �ļ���·��, ȫ·��
	// ����: 	const QStringList & lstNameFilter: �ļ������˹ؼ���, ����"*.txt"
	// ����: 	bool isContainFolder: �Ƿ�����ļ���
	// ����:	QT_NAMESPACE::QStringList: �ļ����б�
	// ����:	��ȡ�ļ��������е��ļ���
	//************************************
	static QStringList GetFolderAllFileNameList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		bool isContainFolder = false);

	//************************************
	// ����: 	const QString & strFolderPath: �ļ���·��, ȫ·��
	// ����: 	const QStringList & lstNameFilter: �ļ������˹ؼ���, ����"*.txt"
	// ����: 	bool isContainFolder: �Ƿ�����ļ���
	// ����:	QT_NAMESPACE::QFileInfoList: �ļ����б�
	// ����:	��ȡ�ļ��������е��ļ���Ϣ(�����ļ���)
	//************************************
	static QFileInfoList GetFolderAllFileInfoList(const QString &strFolderPath,
		const QStringList &lstNameFilter = QStringList(),
		bool isContainFolder = false);

	//************************************
	// ����: 	const QString & strFolderPath: �ļ���·��, ȫ·��
	// ����:	TDirNode
	// ����:	��ȡ�ļ����µ���״�ṹ
	//************************************
	static TDirNode* GetFolderDirTree(const QString &strFolderPath);

	//************************************
	// ����: 	const QString & source: Դ�ļ���
	// ����: 	const QString & destination: Ŀ���ļ���
	// ����: 	bool replace: ����ļ��Ѵ���, �Ƿ��滻
	// ����: 	QFileInfoList & lstSucc: �����ɹ����ļ���Ϣ
	// ����: 	QFileInfoList & lstFail: ����ʧ�ܵ��ļ���Ϣ
	// ����:	bool: ��������ļ������ɹ�, �򷵻سɹ�; ���ֻ��һ���ļ���, �򴴽�Ŀ���ļ��м��ɹ�;
	// ����:	�ļ��п���, �������ļ��еĿ���(���������ļ��Ŀ���)
	//************************************
	static bool CopyDir(const QString &source,
		const QString &destination,
		bool replace = false,
		QFileInfoList &lstSucc = QFileInfoList(),
		QFileInfoList &lstFail = QFileInfoList());

	//************************************
	// ����: 	const QString & source: Դ�ļ���
	// ����: 	const QString & destination: Ŀ���ļ���
	// ����: 	TDirMsgBox pQuestion: ��������, Ϊnullptrʱ������
	// ����: 	TDirMsgBox pInformation: ��������, Ϊnullptrʱ������
	// ����: 	QWidget * parent: ������, ����pQuestion!=nullptrʱ��Ч
	// ����: 	QFileInfoList & lstSucc: �����ɹ����ļ���Ϣ
	// ����: 	QFileInfoList & lstFail: ����ʧ�ܵ��ļ���Ϣ
	// ����: 	int level: �㼶, �����ڲ��ݹ����ʹ��, �ⲿ���ò���
	// ����:	bool: bool: ��������ļ������ɹ�, �򷵻سɹ�; ���ֻ��һ���ļ���, �򴴽�Ŀ���ļ��м��ɹ�;
	// ����:	�ļ��п���, �������ļ��еĿ���(���������ļ��Ŀ���).
	//				ԭ�ļ���/�ļ������ļ�������ʱ, ��������pInformation
	//				Ŀ���ļ���/�ļ������ļ��Ѵ���ʱ, ��������pQuestion
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
	// ����: 	const QString & source: Դ�ļ�/Դ�ļ���
	// ����: 	const QString & destination: Ŀ���ļ�/Ŀ���ļ���
	// ����: 	bool replace: ���Ŀ���Ѵ���, �Ƿ��滻
	// ����: 	QFileInfoList & lstSucc: �����ɹ����ļ���Ϣ
	// ����: 	QFileInfoList & lstFail: ����ʧ�ܵ��ļ���Ϣ
	// ����:	bool: �ɹ�/ʧ��
	// ����:	�ļ�/�ļ��п���
	//************************************
	static bool Copy(const QString &source,
		const QString &destination,
		bool replace = false,
		QFileInfoList &lstSucc = QFileInfoList(),
		QFileInfoList &lstFail = QFileInfoList());

	//************************************
	// ����: 	const QString & source: Դ�ļ�/Դ�ļ���
	// ����: 	const QString & destination: Ŀ���ļ�/Ŀ���ļ���
	// ����: 	TDirMsgBox pQuestion: ��������, Ϊnullptrʱ������
	// ����: 	TDirMsgBox pInformation: ��������, Ϊnullptrʱ������
	// ����: 	QWidget * parent: ������, ����pQuestion!=nullptrʱ��Ч
	// ����: 	QFileInfoList & lstSucc: �����ɹ����ļ���Ϣ
	// ����: 	QFileInfoList & lstFail: ����ʧ�ܵ��ļ���Ϣ
	// ����:	bool
	// ����:	�ļ�/�ļ��п���
	//************************************
	static bool CopyWithWidget(const QString &source,
		const QString &destination,
		TDirMsgBox pQuestion = QMessageBox::question,
		TDirMsgBox pInformation = QMessageBox::information,
		QWidget* parent = nullptr,
		QFileInfoList &lstSucc = QFileInfoList(),
		QFileInfoList &lstFail = QFileInfoList());

	//�ļ�/�ļ��д�С
	static unsigned long long Size(const QString &strDirPath);

	//�ļ�/�ļ����Ƿ�Ϊ�� 
	//�ļ���Ϊ��, ����true
	//�ļ���СΪ0, ����true
	static bool Empty(const QString &strDirPath);

	//************************************
	// ����: 	const QString & strFolderPath: �ļ���·��
	// ����: 	const QString & strDirPath: �ļ�·��
	// ����:	bool
	// ����:	�ļ������Ƿ�������ļ�
	//************************************
	static bool	Contain(const QString &strFolderPath, const QString &strDirPath);


	//************************************
	// ����: 	const QString & strPath: һ��·��
	// ����:	QT_NAMESPACE::QString
	// ����:	����һ��·����������·��
	//************************************
	static QString DrivePath(const QString &strPath);

	//************************************
	// ����: 	const QString & strDirPath: �ļ���·��(�������ļ���)
	// ����: 	TRemoveCondition condition: ִ��ɾ��������(�ļ�������ʲô����)
	// ����: 	TRemoveSortRule sort: �ļ������ļ����������
	// ����: 	TRemoveRule rule: ɾ���ļ��Ĺ���(�ļ�����ʲô����)
	// ����:	void
	// ����:	����һ������ɾ���ļ����µ��ļ�
	//************************************
	static void RemoveDirByRule(const QString &strDirPath, 
		TRemoveCondition condition = nullptr, 
		TRemoveSortRule sort = nullptr,
		TRemoveRule rule = nullptr);
};


//************************************
// ����: 	const QString & strDirPath: �ļ���·��(�������ļ���)
// ����: 	int iMaxDelete: ���ɾ����С(��λMB) 
// ����: 	int iFreeSpace: ʣ��ռ�����(��λMB)
// ����:	void
// ����:	���̽���ʱ, ɾ���ļ�
//************************************
void RemoveDirByDisk(const QString &strDirPath, int iMaxDelete = 500, int iFreeSpace = 500);


//************************************
// ����: 	const QString & strDirPath: : �ļ���·��(�������ļ���)
// ����: 	int iMaxSize: �ļ�������С(��λMB) 
// ����:	void
// ����:	�ļ��г�����Сʱ, ɾ�������ļ�
//************************************
void RemoveDirBySize(const QString &strDirPath, int iMaxSize = 500);

#endif // !_TPATH_H_ 


