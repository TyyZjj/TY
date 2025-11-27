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
		if (!newFileInfo.suffix().isEmpty())//包含格式后缀时, 认定newName是文件路径, 否则是文件夹路径
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
			pInformation(parent, QString::fromUtf8("错误:"),
				QString::fromUtf8("源文件不存在."),
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
			pInformation(parent, QString::fromUtf8("错误:"),
				QString::fromUtf8("源文件不是文件."),
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
				QMessageBox::StandardButton btn = pQuestion(parent, QString::fromUtf8("提示:"),
					QString::fromUtf8("文件已存在, 是否替换?"),
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
				QMessageBox::StandardButton btn = pQuestion(parent, QString::fromUtf8("提示:"),
					QString::fromUtf8("目标路径不明确,目标路径名称是否即目标文件名称?"),
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
				pInformation(parent, QString::fromUtf8("错误:"),
					QString::fromUtf8("创建目标文件夹失败."),
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
	HWND            hwnd;	:对话框的窗口句柄, 以显示有关文件操作状态的信息
	UINT            wFunc;	:指示要执行的操作的值.下列值之一:
								FO_COPY:将pFrom成员中指定的文件复制到pTo成员中指定的位置
								FO_DELETE: 删除pFrom中指定的文件
								FO_MOVE: 将pFrom中指定的文件移动到pTo中指定的位置
								FO_RENAME: 重命名pFrom中指定的文件. 不能使用此标志通过单个函数调用重命名多个文件. 改用FOU_MOVE
	PCZZSTR         pFrom;	:指向一个或多个源文件的指针, 名称应是全路径, 以防意外的结果
	PCZZSTR         pTo;	:指向目标文件或目录名的指针. 如未使用此参数, 则须将其设置为NULL. 不允许使用通配符.
								复制和移动操作可以指定不存在的目标目录. 在这些情况下, 系统会尝试创建它们, 通常会显示一个对话框, 询问用户是否要创建新目录. 若不要对话框并以静默方式创建目录, 请在fFlags中设置FOF_NOCONFIRMMKDIR标志.
								复制和移动操作, 如果fFlags成员指定FOF_MULTIDESTFILES, 则可以包含多个目标文件名.
								使用全路径, 不禁止使用相对路径, 但可能产生不可预测的结果.
	FILEOP_FLAGS    fFlags;	:控制文件操作的标志。此成员可以同时使用以下标志:
								FOF_ALLOWUNDO: 如果可能, 请保留撤消信息. (如果路径不是全路径, 则此项会被忽略)
								FOF_CONFIRMMOUSE: 不使用
								FOF_FILESONLY: 仅适用于文件(文件夹不适用)
								FOF_MULTIDESTFILES: pTo成员指定多个目标文件(pFrom中的每个源文件一个), 而不是存放所有源文件的一个目录
								FOF_NOCONFIRMATION: 对于显示的任何对话框, 用Yes to All回复
								FOF_NOCONFIRMMKDIR: 如果操作需要创建新目录, 不需要用户确认.
								FOF_NO_CONNECTED_ELEMENTS: 不要一次性连接多个文件, 只移动指定文件、
								FOF_NOCOPYSECURITYATTRIBS: 不复制文件的安全属性. 目标文件接收其新文件夹的安全属性.
								FOF_NOERRORUI: 如果发生错误, 不显示对话框.
								FOF_NORECURSEREPARSE: 不使用.
								FOF_NORECURSION: 仅在当前目录中执行该操作, 不要递归子目录, 这是默认行为.
								FOF_NO_UI: 静默方式执行操作, 不显示UI.  == FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR
								FOF_RENAMEONCOLLISION: 如果目标已存在, 请在移动、复制或重命名操作中指定一个.
								FOF_SILENT: 不显示进度对话框.
								FOF_SIMPLEPROGRESS: 显示进度对话框, 但不显示操作时的单个文件名
								FOF_WANTMAPPINGHANDLE: 如果指定了FOF_RENAMEONCOLLISION, 并重命名了任何文件, 请将包含其旧名称和新名称的名称映射对象传递给hNameMappings成员. 
										当不再需要该对象时，必须使用SHFreeNameMappings释放该对象
								FOF_WANTNUKEWARNING: 如果文件在删除操作期间被永久销毁, 而非被回收, 请发送警告. 此标志部分覆盖FOF_NOCONFIRMATION
	BOOL            fAnyOperationsAborted; 当函数返回时, 如果任何文件操作在完成之前被中止, 则返回TRUE; 否则返回FALSE.
										用户可通过UI手动中止操作, 如果设置了FOF_NOERRORUI或FOF_NOCONFIRMATION标志, 则系统可以自动中止操作
	LPVOID          hNameMappings; 当函数返回时, 该成员为名称映射对象的句柄, 该对象包含重命名文件的新旧名称.
										仅当fFlags成员包含FOF_WANTMAPPINGHANDLE标志时, 才使用此成员
	PCSTR           lpszProgressTitle; 指向进度对话框标题的指针. 这是一个以null结尾的字符串. 仅当fFlags包含FOF_SIMPLEPROGRESS标志时, 才使用此成员
} SHFILEOPSTRUCTA

	注:必须确保源路径和目标路径以双空终止.
		普通字符串仅以一个空字符结尾.
		如果在pFrom或pTo中传递该值, 函数将无法找到字符串末尾,并将继续在内存中读取,直到随机的双空值. 这会导致缓冲区溢出, 并可能导致不相关数据的意外删除.
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
	hwnd: 父窗口句柄
	pszRootPath: 驱动器、文件夹和子文件夹名称, 例如c:\\windows\\system. 如果为NULL，则所有驱动器上的所有回收站都将清空
	dwFlags: 
		SHERB_NOCONFIRMATION: 不显示确认删除对象的对话框
		SHERB_NOPROGRESSUI: 不显示进度对话框
		SHERB_NOSOUND:	操作完成后将不播放声音
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
		//文件夹则默认目标文件夹名称即是目标名称
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
	opMove.pFrom = FromBuf;    // 文件名请以\0\0结尾
	opMove.pTo = ToBuf;        // 文件名请以\0\0结尾
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

	if (!QFile::exists(strTarget))//目标不存在
	{
		if (targetFileInfo.suffix().isNull())//且目标后缀名为空, 则认定目标是文件夹
		{
			if (!QDir().mkpath(strTarget))
				return false;
		}
		else//认定目标是文件
		{
			if (!QFile::exists(targetFileInfo.absolutePath()) &&
				!QDir().mkpath(targetFileInfo.absoluteFilePath()))
				return false;
		}
	}

	targetFileInfo.setFile(strTarget);
	if (QFile::exists(strTarget))
	{
		if (targetFileInfo.isDir())//目标路径是文件夹
		{
			QFileInfo firstSourceFileInfo(lstSourcePath.first());
			if (!strTarget.endsWith(QDir::separator()))
				strTarget += QDir::separator();
			if (lstSourcePath.count() == 1)
				strTarget += firstSourceFileInfo.baseName() + ".zip";
			else
				strTarget += firstSourceFileInfo.absoluteDir().dirName() + ".zip";
			if (QFile::exists(strTarget))//查看文件是否存在
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
	args.append(QString::fromUtf8("-o%1").arg(strTargetPath));
	ret = p.execute(command, args);

	return ret == 0;
}


