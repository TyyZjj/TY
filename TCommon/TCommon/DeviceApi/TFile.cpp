#include <QDir>
#include <QProcess>
#include <QFileInfo>
#ifdef WIN32
#include <windows.h>
#ifdef CopyFile
#undef CopyFile
#endif // CopyFile
#ifdef CreateFile
#undef CreateFile
#endif // CreateFile
#ifdef MoveFile
#undef MoveFile
#endif // MoveFile
#endif // WIN32
#include "TFile.h"
#include "TAlgorithm.h"
#include "TSignDefine.h"


bool TFile::CopyFile(const QString &fileName, 
	const QString &newName, 
	bool replace /*= false*/)
{
	if (!QFile::exists(fileName))
		return false;
	QFileInfo fileInfo(fileName);
	if (!fileInfo.isFile())
		return false;

	QString strNewPath(QDir::toNativeSeparators(newName));
	QFileInfo newFileInfo(newName);
	if (QFile::exists(newName))
	{
		if (newFileInfo.isFile())
		{
			if (!replace)
				return false;
			QFile::remove(newName);
		}
		if (newFileInfo.isDir())
		{
			if (!strNewPath.endsWith(QDir::separator()))
				strNewPath += QDir::separator();
			strNewPath += fileInfo.fileName();
		}
			
	}
	else
	{
		QString strPath(newName);
		if (!newFileInfo.suffix().isEmpty())//������ʽ��׺ʱ, �϶�newName���ļ�·��, �������ļ���·��
			strPath = newFileInfo.absolutePath();
		QDir dir;
		if (!dir.exists(strPath) &&
			!dir.mkpath(strPath))
			return false;
	}
	return QFile::copy(fileName, strNewPath);
}

bool TFile::CopyFileWithWidget(const QString &fileName, 
	const QString &newName, 
	TFileMsgBox pQuestion /*= QMessageBox::question*/, 
	TFileMsgBox pInformation /*= QMessageBox::information*/, 
	QWidget* parent /*= nullptr*/)
{
	if (!QFile::exists(fileName))
	{
		if (pInformation != nullptr)
		{
			pInformation(parent, QString::fromLocal8Bit("����:"),
				QString::fromLocal8Bit("Դ�ļ�������."),
				QMessageBox::StandardButton::Ok,
				QMessageBox::StandardButton::Ok);
		}
		return false;
	}
	QFileInfo fileInfo(fileName);
	if (!fileInfo.isFile())
	{
		if (pInformation != nullptr)
		{
			pInformation(parent, QString::fromLocal8Bit("����:"),
				QString::fromLocal8Bit("Դ�ļ������ļ�."),
				QMessageBox::StandardButton::Ok,
				QMessageBox::StandardButton::Ok);
		}
		return false;
	}

	QString strNewPath(QDir::toNativeSeparators(newName));
	QFileInfo newFileInfo(newName);
	if (QFile::exists(newName))
	{
		if (newFileInfo.isFile())
		{
			if (pQuestion != nullptr)
			{
				QMessageBox::StandardButton btn = pQuestion(parent, QString::fromLocal8Bit("��ʾ:"),
					QString::fromLocal8Bit("�ļ��Ѵ���, �Ƿ��滻?"),
					QMessageBox::StandardButtons(QMessageBox::StandardButton::Yes |
						QMessageBox::StandardButton::No),
					QMessageBox::StandardButton::Yes);
				if (btn == QMessageBox::StandardButton::Yes)
					QFile::remove(newName);
				else
					return false;
			}
			else
				return false;
			
		}
		if (newFileInfo.isDir())
		{
			if (!strNewPath.endsWith(QDir::separator()))
				strNewPath += QDir::separator();
			strNewPath += fileInfo.fileName();
		}
	}
	else
	{
		QString strPath(newName);
		if (newFileInfo.suffix().isEmpty())
		{
			if (!fileInfo.suffix().isEmpty() &&
				pQuestion != nullptr)
			{
				QMessageBox::StandardButton btn = pQuestion(parent, QString::fromLocal8Bit("��ʾ:"),
					QString::fromLocal8Bit("Ŀ��·������ȷ,Ŀ��·�������Ƿ�Ŀ���ļ�����?"),
					QMessageBox::StandardButtons(QMessageBox::StandardButton::Yes |
						QMessageBox::StandardButton::No),
					QMessageBox::StandardButton::Yes);
				if (btn == QMessageBox::StandardButton::Yes)
					strPath = newFileInfo.absolutePath();
			}
		}
		else
			strPath = newFileInfo.absolutePath();
		QDir dir;
		if (!dir.exists(strPath) &&
			!dir.mkpath(strPath))
		{
			if (pInformation != nullptr)
			{
				pInformation(parent, QString::fromLocal8Bit("����:"),
					QString::fromLocal8Bit("����Ŀ���ļ���ʧ��."),
					QMessageBox::StandardButton::Ok,
					QMessageBox::StandardButton::Ok);
			}
			return false;
		}
	}
	return QFile::copy(fileName, strNewPath);

}

bool TFile::CreateFile(const QString &strFilePath)
{
	QFileInfo fileInfo(strFilePath);
	QString strPath = fileInfo.absolutePath();
	QDir dir;
	if (!dir.exists(strPath) &&
		!dir.mkpath(strPath))
		return false;

	QFile file(strFilePath);
	if (file.exists())
		return true;
	if (!file.open(QIODevice::NewOnly))
		return false;
	file.close();
	return true;
}


bool TFile::DeleteFileToCrash(const QString &strFilePath)
{
#ifdef WIN32
	if (!QFile::exists(strFilePath))
		return false;

	LPSTR lpSourceFile = QDir::toNativeSeparators(strFilePath).toLocal8Bit().data();
	CHAR ToBuf[MAX_PATH];
	CHAR FromBuf[MAX_PATH];
	ZeroMemory(ToBuf, sizeof(ToBuf));
	ZeroMemory(FromBuf, sizeof(FromBuf));
	lstrcpyA(FromBuf, lpSourceFile);

	/*
typedef struct _SHFILEOPSTRUCTA
{
	HWND            hwnd;	:�Ի���Ĵ��ھ��, ����ʾ�й��ļ�����״̬����Ϣ
	UINT            wFunc;	:ָʾҪִ�еĲ�����ֵ.����ֵ֮һ:
								FO_COPY:��pFrom��Ա��ָ�����ļ����Ƶ�pTo��Ա��ָ����λ��
								FO_DELETE: ɾ��pFrom��ָ�����ļ�
								FO_MOVE: ��pFrom��ָ�����ļ��ƶ���pTo��ָ����λ��
								FO_RENAME: ������pFrom��ָ�����ļ�. ����ʹ�ô˱�־ͨ������������������������ļ�. ����FOU_MOVE
	PCZZSTR         pFrom;	:ָ��һ������Դ�ļ���ָ��, ����Ӧ��ȫ·��, �Է�����Ľ��
	PCZZSTR         pTo;	:ָ��Ŀ���ļ���Ŀ¼����ָ��. ��δʹ�ô˲���, ���뽫������ΪNULL. ������ʹ��ͨ���.
								���ƺ��ƶ���������ָ�������ڵ�Ŀ��Ŀ¼. ����Щ�����, ϵͳ�᳢�Դ�������, ͨ������ʾһ���Ի���, ѯ���û��Ƿ�Ҫ������Ŀ¼. ����Ҫ�Ի����Ծ�Ĭ��ʽ����Ŀ¼, ����fFlags������FOF_NOCONFIRMMKDIR��־.
								���ƺ��ƶ�����, ���fFlags��Աָ��FOF_MULTIDESTFILES, ����԰������Ŀ���ļ���.
								ʹ��ȫ·��, ����ֹʹ�����·��, �����ܲ�������Ԥ��Ľ��.
	FILEOP_FLAGS    fFlags;	:�����ļ������ı�־���˳�Ա����ͬʱʹ�����±�־:
								FOF_ALLOWUNDO: �������, �뱣��������Ϣ. (���·������ȫ·��, �����ᱻ����)
								FOF_CONFIRMMOUSE: ��ʹ��
								FOF_FILESONLY: ���������ļ�(�ļ��в�����)
								FOF_MULTIDESTFILES: pTo��Աָ�����Ŀ���ļ�(pFrom�е�ÿ��Դ�ļ�һ��), �����Ǵ������Դ�ļ���һ��Ŀ¼
								FOF_NOCONFIRMATION: ������ʾ���κζԻ���, ��Yes to All�ظ�
								FOF_NOCONFIRMMKDIR: ���������Ҫ������Ŀ¼, ����Ҫ�û�ȷ��.
								FOF_NO_CONNECTED_ELEMENTS: ��Ҫһ�������Ӷ���ļ�, ֻ�ƶ�ָ���ļ���
								FOF_NOCOPYSECURITYATTRIBS: �������ļ��İ�ȫ����. Ŀ���ļ����������ļ��еİ�ȫ����.
								FOF_NOERRORUI: �����������, ����ʾ�Ի���.
								FOF_NORECURSEREPARSE: ��ʹ��.
								FOF_NORECURSION: ���ڵ�ǰĿ¼��ִ�иò���, ��Ҫ�ݹ���Ŀ¼, ����Ĭ����Ϊ.
								FOF_NO_UI: ��Ĭ��ʽִ�в���, ����ʾUI.  == FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR
								FOF_RENAMEONCOLLISION: ���Ŀ���Ѵ���, �����ƶ������ƻ�������������ָ��һ��.
								FOF_SILENT: ����ʾ���ȶԻ���.
								FOF_SIMPLEPROGRESS: ��ʾ���ȶԻ���, ������ʾ����ʱ�ĵ����ļ���
								FOF_WANTMAPPINGHANDLE: ���ָ����FOF_RENAMEONCOLLISION, �����������κ��ļ�, �뽫����������ƺ������Ƶ�����ӳ����󴫵ݸ�hNameMappings��Ա. 
										��������Ҫ�ö���ʱ������ʹ��SHFreeNameMappings�ͷŸö���
								FOF_WANTNUKEWARNING: ����ļ���ɾ�������ڼ䱻��������, ���Ǳ�����, �뷢�;���. �˱�־���ָ���FOF_NOCONFIRMATION
	BOOL            fAnyOperationsAborted; ����������ʱ, ����κ��ļ����������֮ǰ����ֹ, �򷵻�TRUE; ���򷵻�FALSE.
										�û���ͨ��UI�ֶ���ֹ����, ���������FOF_NOERRORUI��FOF_NOCONFIRMATION��־, ��ϵͳ�����Զ���ֹ����
	LPVOID          hNameMappings; ����������ʱ, �ó�ԱΪ����ӳ�����ľ��, �ö�������������ļ����¾�����.
										����fFlags��Ա����FOF_WANTMAPPINGHANDLE��־ʱ, ��ʹ�ô˳�Ա
	PCSTR           lpszProgressTitle; ָ����ȶԻ�������ָ��. ����һ����null��β���ַ���. ����fFlags����FOF_SIMPLEPROGRESS��־ʱ, ��ʹ�ô˳�Ա
} SHFILEOPSTRUCTA

	ע:����ȷ��Դ·����Ŀ��·����˫����ֹ.
		��ͨ�ַ�������һ�����ַ���β.
		�����pFrom��pTo�д��ݸ�ֵ, �������޷��ҵ��ַ���ĩβ,�����������ڴ��ж�ȡ,ֱ�������˫��ֵ. ��ᵼ�»��������, �����ܵ��²�������ݵ�����ɾ��.
	*/
	SHFILEOPSTRUCTA opRecycle;
	ZeroMemory(&opRecycle, sizeof(opRecycle));
	opRecycle.hwnd = Q_NULLPTR;
	opRecycle.wFunc = FO_DELETE;
	opRecycle.pFrom = FromBuf;
	opRecycle.pTo = ToBuf;
	opRecycle.fFlags = FOF_ALLOWUNDO | FOF_SILENT;
	opRecycle.fAnyOperationsAborted = FALSE;
	opRecycle.hNameMappings = Q_NULLPTR;
	opRecycle.lpszProgressTitle = Q_NULLPTR;
	int ret = SHFileOperationA(&opRecycle);
	return (ret == 0 &&
		opRecycle.fAnyOperationsAborted == FALSE);
#endif
	return false;
}

long TFile::EmptyRecycleBin(const QString &strPath /*= QString()*/)
{
#ifdef WIN32
	QString strTempPath = QDir::toNativeSeparators(strPath);
	/*
	hwnd: �����ھ��
	pszRootPath: ���������ļ��к����ļ�������, ����c:\\windows\\system. ���ΪNULL���������������ϵ����л���վ�������
	dwFlags: 
		SHERB_NOCONFIRMATION: ����ʾȷ��ɾ������ĶԻ���
		SHERB_NOPROGRESSUI: ����ʾ���ȶԻ���
		SHERB_NOSOUND:	������ɺ󽫲���������
	*/
	HRESULT ret = SHEmptyRecycleBinA(NULL, strTempPath.toLocal8Bit().data(), SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
	if (ret == S_OK)
		;
	return ret;
#endif
}

bool TFile::MoveFile(const QString &strFrom, const QString &strTo, bool replace /*= false*/)
{
#ifdef WIN32
	if (!QFile::exists(strFrom))
		return false;
	QString strfrom = QDir::toNativeSeparators(strFrom);
	QString strto = QDir::toNativeSeparators(strTo);
	QFileInfo fileInfoFrom(strfrom);
	QFileInfo fileInfoTo(strto);

	if (QFile::exists(strto))
	{
		if (fileInfoFrom.isFile())
		{
			if (fileInfoTo.isDir())
			{
				if (!strto.endsWith(QDir::separator()))
					strto += QDir::separator();
				strto += fileInfoFrom.fileName();
			}
			if (fileInfoTo.isFile())
			{
				if (replace)
					QFile::remove(strto);
				else
					return false;
			}
		}
		//�ļ�����Ĭ��Ŀ���ļ������Ƽ���Ŀ������
	}
	
	CHAR FromBuf[MAX_PATH];
	CHAR ToBuf[MAX_PATH];
	ZeroMemory(FromBuf, sizeof(FromBuf));
	ZeroMemory(ToBuf, sizeof(ToBuf));
	lstrcpyA(FromBuf, strfrom.toLocal8Bit().data());
	lstrcpyA(ToBuf, strto.toLocal8Bit().data());

	SHFILEOPSTRUCTA opMove;
	ZeroMemory(&opMove, sizeof(opMove));
	opMove.hwnd = NULL;
	opMove.wFunc = FO_MOVE;
	opMove.pFrom = FromBuf;    // �ļ�������\0\0��β
	opMove.pTo = ToBuf;        // �ļ�������\0\0��β
	opMove.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOCONFIRMMKDIR;
	opMove.hNameMappings = NULL;
	opMove.lpszProgressTitle = NULL;
	int ret = SHFileOperationA(&opMove);
	return (ret == 0);
#endif
}

bool TFile::Compress(const QString& strTargetPath, 
	const QStringList& lstSourcePath,
	bool replace /*= false*/,
	bool delSource /*= false*/)
{
	if (lstSourcePath.isEmpty())
		return false;
	bool isExists = TListFind::Find(lstSourcePath, [](const QString strSourceFile)->bool {
		return QFile::exists(strSourceFile);});
	if (!isExists)
		return false;

	QString strTarget = strTargetPath;
	QFileInfo targetFileInfo(strTarget);

	if (!QFile::exists(strTarget))//Ŀ�겻����
	{
		if (targetFileInfo.suffix().isNull())//��Ŀ���׺��Ϊ��, ���϶�Ŀ�����ļ���
		{
			if (!QDir().mkpath(strTarget))
				return false;
		}
		else//�϶�Ŀ�����ļ�
		{
			if (!QFile::exists(targetFileInfo.absolutePath()) &&
				!QDir().mkpath(targetFileInfo.absoluteFilePath()))
				return false;
		}
	}

	targetFileInfo.setFile(strTarget);
	if (QFile::exists(strTarget))
	{
		if (targetFileInfo.isDir())//Ŀ��·�����ļ���
		{
			QFileInfo firstSourceFileInfo(lstSourcePath.first());
			if (!strTarget.endsWith(QDir::separator()))
				strTarget += QDir::separator();
			if (lstSourcePath.count() == 1)
				strTarget += firstSourceFileInfo.baseName() + ".zip";
			else
				strTarget += firstSourceFileInfo.absoluteDir().dirName() + ".zip";
			if (QFile::exists(strTarget))//�鿴�ļ��Ƿ����
			{
				if (replace)
					QFile::remove(strTarget);
				else
					return false;
			}
		}
		else
		{
			if (replace)
				QFile::remove(strTarget);
			else
				return false;
		}
	}

	int ret = 0;
	QProcess p;
	QString command = "7z.exe";
	QStringList args;
	args.append("a");
	args.append(strTarget);
	args.append(lstSourcePath);
	if (delSource)
		args.append("-sdel");
	ret = p.execute(command, args);
	return ret == 0;
}

bool TFile::Decompress(const QString& strSourcePath, 
	const QString& strTargetPath, 
	bool replace /*= false*/)
{
	if (!QFile::exists(strSourcePath))
		return false;

	QFileInfo sourceFileInfo(strSourcePath);
	if (!sourceFileInfo.isFile())
		return false;

	if (!QFile::exists(strTargetPath) &&
		!QDir().mkpath(strTargetPath))
		return false;

	QFileInfo targetFileInfo(strTargetPath);
	if (!targetFileInfo.isDir())
		return false;

	int ret(0);
	QProcess p;
	QString command = "7z.exe";
	QStringList args;
	args.append("x");
	args.append(strSourcePath);
	if (replace)
		args.append("-aoa");
	else
		args.append("-aos");
	args.append(QString::fromLocal8Bit("-o%1").arg(strTargetPath));
	ret = p.execute(command, args);

	return ret == 0;
}


