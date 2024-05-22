#include "TDir.h"
#include "TFile.h"
#include "TDefine.h"
#include "TAlgorithm.h"

TDirNode::TDirNode(QString strDirPath, TDirNode* parent /*= nullptr*/)
{
	strDirPath = QDir::toNativeSeparators(strDirPath);
	if (!QFile::exists(strDirPath))
		return;

	this->fileInfo = QFileInfo(strDirPath);
	this->strDirName = fileInfo.fileName();
	if (parent)
		this->parent = parent;
	
}

TDirNode::~TDirNode()
{
	for (auto &child : lstChild)
	{
		delete child;
		child = nullptr;
	}
	parent = nullptr;
}

void TDirNode::setParent(TDirNode *parent)
{
	this->parent = parent;
}


bool isFileNameMatchNameFilter(const QString &strFilter, const QString &strFileName)
{
	QString strNameFilter = strFilter;
	if (strNameFilter.contains(SIGN_ASTERISK))//�����Ҫ���˸�ʽ
	{
		while (strNameFilter.contains(SIGN_ASTERISK))
			strNameFilter = strNameFilter.remove(SIGN_ASTERISK);
		if (strNameFilter.endsWith(SIGN_POINT))
		{
			if (strFileName.contains(strNameFilter))
				return true;
		}
		else
		{
			if (strFileName.endsWith(strNameFilter))
				return true;
		}
	}
	else//�����ļ���
	{
		if (strFileName.contains(strNameFilter))
			return true;
	}
	return false;
}
bool isFileInfoMatchNameFilter(const QString &strFilter, const QFileInfo &fileInfo)
{
	return isFileNameMatchNameFilter(strFilter, fileInfo.fileName());
}


QStringList TDir::GetFolderFileNameList(const QString &strFolderPath, 
	const QStringList &lstNameFilter /*= QStringList()*/, 
	QDir::Filters filters /*= QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden*/, 
	QDir::SortFlags sort /*= QDir::SortFlag::Name*/)
{
	QStringList lstFileName;
	QDir dir(strFolderPath);
	if (!dir.exists())
		return lstFileName;
	QFileInfo fileInfo(strFolderPath);
	if (fileInfo.isDir() &&
		!fileInfo.isSymLink() &&
		!strFolderPath.endsWith(SIGN_POINT))
		lstFileName = dir.entryList(filters,sort);

	if (lstFileName.count() && lstNameFilter.count())
		TListFind::Filter(lstFileName, lstNameFilter, isFileNameMatchNameFilter, false);
	return lstFileName;
}

QFileInfoList TDir::GetFolderFileInfoList(const QString &strFolderPath, 
	const QStringList &lstNameFilter /*= QStringList()*/, 
	QDir::Filters filters /*= QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden*/, 
	QDir::SortFlags sort /*= QDir::SortFlag::Name*/)
{
	QFileInfoList lstFileInfo;
	QDir dir(strFolderPath);
	if (!dir.exists())
		return lstFileInfo;
	QFileInfo fileInfo(strFolderPath);
	if (fileInfo.isDir() &&
		!fileInfo.isSymLink() &&
		!strFolderPath.endsWith(SIGN_POINT))
		lstFileInfo = dir.entryInfoList(filters, sort);

	if (lstFileInfo.count() && lstNameFilter.count())
		TListFind::Filter(lstFileInfo, lstNameFilter, isFileInfoMatchNameFilter, false);
	return lstFileInfo;
}

QStringList TDir::GetFolderAllFileNameList(const QString &strFolderPath, 
	const QStringList &lstNameFilter /*= QStringList()*/, 
	bool isContainFolder /*= false*/)
{
	QStringList lstFileName;
	QDir dir(strFolderPath);
	if (!dir.exists())
		return lstFileName;
	QFileInfo fileInfo(strFolderPath);
	if (fileInfo.isDir() &&
		!fileInfo.isSymLink() &&
		!strFolderPath.endsWith(SIGN_POINT))
	{
		QFileInfoList lstFileInfo;
		lstFileInfo = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
		for (QFileInfo& fileInfo : lstFileInfo)
		{
			if (fileInfo.isDir())
			{
				if (isContainFolder)
					lstFileName.append(fileInfo.fileName());
				if (!fileInfo.isSymLink() &&
					!strFolderPath.endsWith(SIGN_POINT))
				{
					QStringList lstFileNameTemp = GetFolderAllFileNameList(fileInfo.absoluteFilePath(), lstNameFilter, isContainFolder);
					lstFileName.append(lstFileNameTemp);
				}
			}
			else
				lstFileName.append(fileInfo.fileName());
		}
	}
	if (lstFileName.count() &&
		lstNameFilter.count())
		TListFind::Filter(lstFileName, lstNameFilter, isFileNameMatchNameFilter, false);

	return lstFileName;
}

QFileInfoList TDir::GetFolderAllFileInfoList(const QString &strFolderPath, 
	const QStringList &lstNameFilter /*= QStringList()*/, 
	bool isContainFolder /*= false*/)
{
	QFileInfoList lstFileInfo;
	QDir dir(strFolderPath);
	if (!dir.exists())
		return lstFileInfo;

	QFileInfo fileInfo(strFolderPath);
	if (fileInfo.isDir() &&
		!fileInfo.isSymLink() &&
		!strFolderPath.endsWith(SIGN_POINT))
	{
		QFileInfoList lstFileInfoSub = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
		for (QFileInfo& fileInfo : lstFileInfoSub)
		{
			if (fileInfo.isDir())
			{
				if (isContainFolder)
					lstFileInfo.append(fileInfo);
				if (!fileInfo.isSymLink() &&
					!strFolderPath.endsWith(SIGN_POINT))
				{
					QFileInfoList lstFileInfoNext = GetFolderAllFileInfoList(fileInfo.absoluteFilePath(), lstNameFilter, isContainFolder);
					lstFileInfo.append(lstFileInfoNext);
				}
			}
			else
				lstFileInfo.append(fileInfo);
		}
	}
	if (lstFileInfo.count() &&
		lstNameFilter.count())
		TListFind::Filter(lstFileInfo, lstNameFilter, isFileInfoMatchNameFilter, false);
	return lstFileInfo;
}

TDirNode* TDir::GetFolderDirTree(const QString &strFolderPath)
{
	QString strDirPath = QDir::toNativeSeparators(strFolderPath);
	TDirNode *topNode = new TDirNode(strFolderPath);
	if (topNode->fileInfo.isDir() &&
		!topNode->fileInfo.isSymLink() &&
		!topNode->strDirName.endsWith(SIGN_POINT))
	{
		QDir dir(strDirPath);
		QStringList lstDirName = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
		if (!strDirPath.endsWith(QDir::separator()))
			strDirPath += QDir::separator();
		for (auto strDirName : lstDirName)
		{
			TDirNode *dirNode = GetFolderDirTree(strDirPath + strDirName);
			dirNode->setParent(topNode);
			topNode->lstChild.append(dirNode);
		}
	}
	return topNode;
}

bool TDir::CopyDir(const QString &source, 
	const QString &destination, 
	bool replace /*= false*/, 
	QFileInfoList &lstSucc /*= QFileInfoList()*/, 
	QFileInfoList &lstFail /*= QFileInfoList()*/)
{
	QDir dir;
	if (!dir.exists(source))
		return false;

	QFileInfo sourceFileInfo(source);
	if (!sourceFileInfo.isDir() ||
		sourceFileInfo.isSymLink() ||
		source.endsWith(SIGN_POINT))
		return false;

	if (!dir.exists(destination))
	{
		bool ret = dir.mkpath(destination);
		if (!ret)
		{
			QFileInfoList lstFileInfo = GetFolderAllFileInfoList(source, QStringList());
			lstFail.append(lstFileInfo);
			return false;
		}
	}

	QString strSrcPath = QDir::toNativeSeparators(source);
	if (!strSrcPath.endsWith(QDir::separator()))
		strSrcPath += QDir::separator();
	QString strDstPath = QDir::toNativeSeparators(destination);
	if (!strDstPath.endsWith(QDir::separator()))
		strDstPath += QDir::separator();

	QStringList lstFileName = GetFolderFileNameList(source, QStringList());
	for (QStringList::size_type index = 0; index < lstFileName.count(); ++index)
	{
		QString strFileName = lstFileName.at(index);
		QString strSrcFilePath = strSrcPath + strFileName;
		QString strDstFilePath = strDstPath + strFileName;
		QFileInfo fileInfo(strSrcFilePath);
		if (fileInfo.isFile() || fileInfo.isSymLink())
		{
			bool ret = TFile::CopyFile(strSrcFilePath, strDstFilePath, replace);
			if (ret)
				lstSucc.append(fileInfo);
			else
				lstFail.append(fileInfo);
		}
		else if (fileInfo.isDir())
		{
			QFileInfoList lstSucc2, lstFail2;
			CopyDir(strSrcFilePath, strDstFilePath, replace, lstSucc2, lstFail2);
			lstSucc.append(lstSucc2);
			lstFail.append(lstFail2);
		}
	}

	if (lstSucc.isEmpty() && lstFail.isEmpty())//��һ���ļ���, ����û���ļ�
		return true;

	return !lstSucc.isEmpty();
}

bool TDir::CopyDirWithWidget(const QString &source, 
	const QString &destination, 
	TDirMsgBox pQuestion /*= QMessageBox::question*/, 
	TDirMsgBox pInformation /*= QMessageBox::information*/,
	QWidget* parent /*= nullptr*/, 
	QFileInfoList &lstSucc /*= QFileInfoList()*/, 
	QFileInfoList &lstFail /*= QFileInfoList()*/, int level /*= 0*/)
{
	//1.ȫ��/ȫ��
	static QMessageBox::StandardButton folderBtn, fileBtn;
	if (level == 0)
	{
		folderBtn = QMessageBox::StandardButton::NoButton;
		fileBtn = QMessageBox::StandardButton::NoButton;
	}

	//2.Դ�ļ��в�����
	QDir dir;
	if (!dir.exists(source))
	{
		if (pInformation != nullptr)
		{
			pInformation(parent, QString::fromLocal8Bit("����:"),
				QString::fromLocal8Bit("Դ�ļ��в�����."),
				QMessageBox::StandardButton::Ok,
				QMessageBox::StandardButton::Ok);
		}
		return false;
	}

	QFileInfo sourceFileInfo(source);
	if (!sourceFileInfo.isDir())
	{
		if (pInformation != nullptr)
		{
			pInformation(parent, QString::fromLocal8Bit("����:"),
				QString::fromLocal8Bit("Դ�ļ��в����ļ���."),
				QMessageBox::StandardButton::Ok,
				QMessageBox::StandardButton::Ok);
		}
		return false;
	}

	//3.Ŀ���ļ���
	if (dir.exists(destination))
	{
		QMessageBox::StandardButton folderBtnTemp = folderBtn;
		if (folderBtnTemp == QMessageBox::StandardButton::NoButton)
		{
			if (pQuestion != nullptr)
			{
				folderBtnTemp = pQuestion(parent, QString::fromLocal8Bit("��ʾ:"),
					QString::fromLocal8Bit("�ļ����Ѵ���, �Ƿ�ϲ�?"),
					QMessageBox::StandardButtons(QMessageBox::StandardButton::Yes |
						QMessageBox::StandardButton::YesToAll |
						QMessageBox::StandardButton::No |
						QMessageBox::StandardButton::NoToAll),
					QMessageBox::StandardButton::Yes);
			}
			else
				folderBtnTemp = QMessageBox::StandardButton::Yes;
		}

		if (folderBtnTemp == QMessageBox::StandardButton::YesToAll)
		{
			folderBtn = folderBtnTemp;
		}
		else if (folderBtnTemp == QMessageBox::StandardButton::No)
		{
			QFileInfoList lstFileInfo = GetFolderAllFileInfoList(source, QStringList());
			lstFail.append(lstFileInfo);
			return false;
		}
		else if (folderBtnTemp == QMessageBox::StandardButton::NoToAll)
		{
			folderBtn = folderBtnTemp;
			QFileInfoList lstFileInfo = GetFolderAllFileInfoList(source, QStringList());
			lstFail.append(lstFileInfo);
			return false;
		}
	}
	else
	{
		bool ret = dir.mkpath(destination);
		if (!ret)
		{
			QFileInfoList lstFileInfo = GetFolderAllFileInfoList(source, QStringList());
			lstFail.append(lstFileInfo);
			if (pInformation != nullptr)
			{
				pInformation(parent, QString::fromLocal8Bit("����:"),
					QString::fromLocal8Bit("����Ŀ¼ʧ��. Ŀ¼:%1 .").arg(destination),
					QMessageBox::StandardButton::Ok,
					QMessageBox::StandardButton::Ok);
			}
			return false;
		}
	}

	//4.�ļ������ݵĿ���
	QString strSrcPath = QDir::toNativeSeparators(source);
	if (!strSrcPath.endsWith(QDir::separator()))
		strSrcPath += QDir::separator();
	QString strDstPath = QDir::toNativeSeparators(destination);
	if (!strDstPath.endsWith(QDir::separator()))
		strDstPath += QDir::separator();

	QStringList lstFileName = GetFolderFileNameList(source, QStringList());
	for (QStringList::size_type index = 0; index < lstFileName.count(); ++index)
	{
		QString strFileName = lstFileName.at(index);
		QString strSrcFilePath = strSrcPath + strFileName;
		QString strDstFilePath = strDstPath + strFileName;
		QFileInfo fileInfo(strSrcFilePath);
		if (!QFile::exists(strSrcFilePath))
		{
			if (pInformation != nullptr)
			{
				pInformation(parent, QString::fromLocal8Bit("����:"),
					QString::fromLocal8Bit("%1������. ·��:%2 .").arg(fileInfo.fileName()).arg(strSrcFilePath),
					QMessageBox::StandardButtons(QMessageBox::StandardButton::Ok),
					QMessageBox::StandardButton::Ok);
			}
			lstFail.append(fileInfo);
			continue;
		}

		//5.�ļ�����
		if (fileInfo.isFile() || fileInfo.isSymLink())
		{
			if (QFile::exists(strDstFilePath))
			{
				QMessageBox::StandardButton fileBtnTemp = fileBtn;
				if (fileBtnTemp == QMessageBox::StandardButton::NoButton)
				{
					if (pQuestion != nullptr)
					{
						fileBtnTemp = pQuestion(parent, QString::fromLocal8Bit("��ʾ:"),
							QString::fromLocal8Bit("�ļ��Ѵ���, �Ƿ��滻?"),
							QMessageBox::StandardButtons(QMessageBox::StandardButton::Yes |
								QMessageBox::StandardButton::YesToAll |
								QMessageBox::StandardButton::No |
								QMessageBox::StandardButton::NoToAll),
							QMessageBox::StandardButton::Yes);
					}
					else
						fileBtnTemp = QMessageBox::StandardButton::No;
				}

				if (fileBtnTemp == QMessageBox::StandardButton::YesToAll)
				{
					fileBtn = fileBtnTemp;
				}
				else if (fileBtnTemp == QMessageBox::StandardButton::No)
				{
					lstFail.append(fileInfo);
					continue;
				}
				else if (fileBtnTemp == QMessageBox::StandardButton::NoToAll)
				{
					fileBtn = fileBtnTemp;
					lstFail.append(fileInfo);
					continue;
				}
			}
			bool ret = TFile::CopyFile(strSrcFilePath, strDstFilePath, true);
			if (ret)
				lstSucc.append(fileInfo);
			else
				lstFail.append(fileInfo);
		}
		//6.�ļ��п���
		else if (fileInfo.isDir())
		{
			QFileInfoList lstSucc2, lstFail2;
			CopyDirWithWidget(strSrcFilePath,
				strDstFilePath,
				pQuestion,
				pInformation,
				parent,
				lstSucc2,
				lstFail2,
				level + 1);
			lstSucc.append(lstSucc2);
			lstFail.append(lstFail2);
		}
	}

	if (lstSucc.isEmpty() && lstFail.isEmpty())//��һ���ļ���, ����û���ļ�
		return true;

	return !lstSucc.isEmpty();
}

bool TDir::Copy(const QString &source, 
	const QString &destination, 
	bool replace /*= false*/, 
	QFileInfoList &lstSucc /*= QFileInfoList()*/, 
	QFileInfoList &lstFail /*= QFileInfoList()*/)
{
	if (!QFile::exists(source))
		return false;

	bool ret(false);
	QFileInfo sourceFileInfo(source);
	if (sourceFileInfo.isFile() || sourceFileInfo.isSymLink())
	{
		ret = TFile::CopyFile(source, destination, replace);
		if (ret)
			lstSucc.append(sourceFileInfo);
		else
			lstFail.append(sourceFileInfo);
	}
	else if (sourceFileInfo.isDir())
	{
		ret = CopyDir(source, destination, replace, lstSucc, lstFail);
	}

	return ret;
}

bool TDir::CopyWithWidget(const QString &source, 
	const QString &destination, 
	TDirMsgBox pQuestion /*= QMessageBox::question*/, 
	TDirMsgBox pInformation /*= QMessageBox::information*/, 
	QWidget* parent /*= nullptr*/, 
	QFileInfoList &lstSucc /*= QFileInfoList()*/, 
	QFileInfoList &lstFail /*= QFileInfoList()*/)
{
	if (!QFile::exists(source))
	{
		if (pInformation != nullptr)
		{
			pInformation(parent, QString::fromLocal8Bit("����:"),
				QString::fromLocal8Bit("ԭ�ļ�������."),
				QMessageBox::StandardButton::Ok,
				QMessageBox::StandardButton::Ok);
		}
		return false;
	}

	bool ret(false);
	QFileInfo sourceFileInfo(source), destinationFileInfo(destination);
	if (sourceFileInfo.isFile() || sourceFileInfo.isSymLink())
	{
		ret = TFile::CopyFileWithWidget(source,
			destination,
			pQuestion,
			pInformation,
			parent);
		if (ret)
			lstSucc.append(sourceFileInfo);
		else
			lstFail.append(sourceFileInfo);
	}
	else if (sourceFileInfo.isDir())
	{
		ret = CopyDirWithWidget(source,
			destination,
			pQuestion,
			pInformation,
			parent,
			lstSucc,
			lstFail,
			0);
	}

	return ret;
}

unsigned long long TDir::Size(const QString &strDirPath)
{
	QFileInfo fileInfo(strDirPath);
	if (fileInfo.isDir())
	{
		if (fileInfo.absoluteFilePath().endsWith(SIGN_POINT))
			return 0;//����'.'�ļ���
		if (fileInfo.isSymLink())
			return 0;

		QFileInfoList lstFileInfo = GetFolderAllFileInfoList(strDirPath);
		unsigned long long iSize(0);
		for (QFileInfo &fileInfo : lstFileInfo)
		{
			if (fileInfo.isFile())
				iSize += fileInfo.size();
		}
		return iSize;
	}
	else
	{
		return fileInfo.size();
	}
}

bool TDir::Empty(const QString &strDirPath)
{
	QFileInfo fileInfo(strDirPath);
	if (fileInfo.isDir())
	{
		if (fileInfo.absoluteFilePath().endsWith("."))
			return 0;//����'.'�ļ���
		if (fileInfo.isSymLink())
			return 0;

		return GetFolderFileNameList(strDirPath).isEmpty();
	}
	else
	{
		return (fileInfo.size() == 0);
	}
}

bool TDir::Contain(const QString &strFolderPath, const QString &strDirPath)
{
	QDir dir;
	if (!dir.exists(strFolderPath) ||
		!QFile::exists(strDirPath))//�ļ���/�ļ������� ������"."�ļ���
		return false;

	QString strFolderPathTemp = QDir::toNativeSeparators(strFolderPath);
	QString strDirPathTemp = QDir::toNativeSeparators(strDirPath);
	QFileInfo folderInfo(strFolderPathTemp);
	strFolderPathTemp = folderInfo.absoluteFilePath();
	QFileInfo fileInfo(strDirPathTemp);
	strDirPathTemp = fileInfo.absoluteFilePath();

	return strDirPathTemp.contains(strFolderPathTemp, Qt::CaseSensitivity::CaseInsensitive);
}

QString TDir::DrivePath(const QString &strPath)
{
	QString strPathTemp = strPath;
	int iDepth(0);
	while (!QFileInfo(strPathTemp).isRoot())
	{
		iDepth++;
		strPathTemp = QFileInfo(strPathTemp).absolutePath();
		if (iDepth > std::numeric_limits<char>::max())//�ݹ�ջ��������, ��ֹ��ѭ��
			return QString();
	}
	return strPathTemp;
}

void TDir::RemoveDirByRule(const QString &strDirPath, 
	TRemoveCondition condition /*= nullptr*/, 
	TRemoveSortRule sort /*= nullptr*/, 
	TRemoveRule rule /*= nullptr*/)
{
	if (!QDir().exists(strDirPath))
		return;

	if (condition != nullptr &&
		!condition(strDirPath))//�ļ��в�����ɾ������
		return;

	QFileInfoList lstFileInfo = GetFolderAllFileInfoList(strDirPath, QStringList(), true);
	if (sort != nullptr)
		sort(lstFileInfo);

	for (int index = 0; index < lstFileInfo.count(); ++index)
	{
		if (rule != nullptr &&
			!rule(lstFileInfo, index))
			continue;

		const QFileInfo &fileInfo = lstFileInfo.at(index);
		QString strPath = fileInfo.absoluteFilePath();
		if (fileInfo.isFile())
			QFile::remove(strPath);
		else
			QDir(strPath).removeRecursively();
	}
}

#include "TDrive.h"
#include <QMutex>
#include <QDateTime>

void RemoveDirByDisk(const QString &strDirPath, 
	int iMaxDelete /*= 500*/, int iFreeSpace /*= 500*/)
{
	static QMutex mutex;
	mutex.lock();
	static int iMaxDeleteStatic, iFreeSpaceStatic;
	iMaxDeleteStatic = iMaxDelete;
	iFreeSpaceStatic = iFreeSpace;
	TDir::RemoveDirByRule(strDirPath,
		[](const QString &strDirPath) ->bool {
		QDir dir(strDirPath);
		QString strRootPath = dir.rootPath();
		int iCurrentFreeSpace = TDrive::GetDiskFreeSpace(strRootPath);
		if (iCurrentFreeSpace < iFreeSpaceStatic)
			return true;
		else
			return false;
	},
		[](QFileInfoList &lstFileInfo) {
		std::sort(lstFileInfo.begin(), lstFileInfo.end(), [](QFileInfo left, QFileInfo right)->bool {
			return left.lastModified() < right.lastModified();//������޸�ʱ������
		});
	},
		[](const QFileInfoList &lstFileInfo, int iFileIndex)->bool {
		static int iDeleteSpace;
		if (iFileIndex == 0)
			iDeleteSpace = 0;
		QFileInfo fileInfo = lstFileInfo.at(iFileIndex);
		QString strPath = fileInfo.absoluteFilePath();
		if (fileInfo.isFile())
		{
			if (iDeleteSpace / MB < iMaxDeleteStatic)
			{
				if (QFile::exists(strPath))
				{
					iDeleteSpace += fileInfo.size();
				}
				return true;
			}
		}
		if (fileInfo.isDir())//���ܴ���û�б�ɾ�����ļ���, ����
		{
			QDir dir(strPath);
			if (dir.exists() && TDir::Empty(strPath))
			{
				return true;
			}
		}
		return false;
	});

	mutex.unlock();
}

void RemoveDirBySize(const QString &strDirPath, int iMaxSize /*= 500*/)
{
	static QMutex mutex;
	mutex.lock();
	static int iMaxSizeStatic, iDeleteSize;
	iMaxSizeStatic = iMaxSize;
	iDeleteSize = 0;
	TDir::RemoveDirByRule(strDirPath,
		[](const QString &strDirPath) ->bool {
		int iSize = (int)TDir::Size(strDirPath);
		if (iSize > iMaxSizeStatic)
		{
			iDeleteSize = iSize / MB - iMaxSizeStatic;
			return true;
		}
		return false;
	},
		[](QFileInfoList &lstFileInfo) {
		std::sort(lstFileInfo.begin(), lstFileInfo.end(), [](QFileInfo left, QFileInfo right)->bool {
			return left.lastModified() < right.lastModified();//������޸�ʱ������
		});
	},
		[](const QFileInfoList &lstFileInfo, int iFileIndex)->bool {
		static int iDeleteSpace;
		if (iFileIndex == 0)
			iDeleteSpace = 0;
		QFileInfo fileInfo = lstFileInfo.at(iFileIndex);
		QString strPath = fileInfo.absoluteFilePath();
		if (fileInfo.isFile())
		{
			if (iDeleteSpace / MB < iDeleteSize)
			{
				if (QFile::exists(strPath))
				{
					iDeleteSpace += fileInfo.size();
				}
				return true;
			}
		}
		if (fileInfo.isDir())//���ܴ���û�б�ɾ�����ļ���, ����
		{
			QDir dir(strPath);
			if (dir.exists() && TDir::Empty(strPath))
			{
				return true;
			}
		}
		return false;
	});

	mutex.unlock();
}
