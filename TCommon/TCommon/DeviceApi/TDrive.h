#ifndef _TDrive_H_
#define _TDrive_H_

#include <QString>
#include <QStringList>

//驱动器相关的类
class TDrive
{
public:
	//获取(驱动器)磁盘数量
	static unsigned long GetDiskCount();

	//获取所有(驱动器)磁盘的名称
	static QStringList GetAllDriveName();

	//************************************
	// 参数: 	const QString & strDriveName: "C:\\"
	// 返回:	unsigned int
	//		0(DRIVE_UNKNOWN: 未知设备),
	//		1(DRIVE_NO_ROOT_DIR: 路径无效),
	//		2(DRIVE_REMOVABLE: 可移动磁盘),
	//		3(DRIVE_FIXED: 硬盘),
	//		4(DRIVE_REMOTE: 网络磁盘),
	//		5(DRIVE_CDROM: 光驱),
	//		6(DRIVE_RAMDISK: 虚拟RAM磁盘)
	// 功能:	获取驱动器的类型
	//************************************
	static unsigned int GetDriveType(const QString &strDriveName);

	//获取磁盘剩余空间(单位MB)
	static unsigned long long GetDiskFreeSpace(const QString &strDriver);
	
	//************************************
	// 参数: 	unsigned long long & iMaxFreeSpace: 输出(单位MB)
	// 返回:	QT_NAMESPACE::QString: 盘符名称("C:\\")
	// 功能:	获取剩余空间最大的磁盘(硬盘)
	//************************************
	static QString GetMaxFreeSpaceDisk(unsigned long long &iMaxFreeSpace);

	//************************************
	// 参数: 	const QString & strDriveName: 驱动器路径 QDir::drives()
	// 参数: 	int & iSizePercent: 已使用的比例
	// 参数: 	QString & strSizeUse: 已使用空间(单位GB)
	// 参数: 	QString & strSizeFree: 空余空间(单位GB)
	// 参数: 	QString & strSizeAll: 所有空间(单位GB)
	// 返回:	void
	// 功能:	获取当前磁盘状态
	//************************************
	static void GetDiskSpaceInfo(const QString &strDriveName,
		int &iSizePercent,
		double &dSizeUse,
		double &dSizeFree,
		double &dSizeAll);
private:

};




#endif
