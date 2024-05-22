#ifdef WIN32
#include <QDir>
#include <windows.h>
#endif // WIN32
#ifdef GetDriveType
#undef GetDriveType
#endif // GetDriveType
#ifdef GetDiskFreeSpace
#undef GetDiskFreeSpace
#endif // GetDiskFreeSpace


#include <QOperatingSystemVersion>
#include "TDrive.h"
#include "TDefine.h"

unsigned long TDrive::GetDiskCount()
{
	int iDiskCount(0);
#ifdef WIN32
	unsigned long iDiskInfo = GetLogicalDrives();
	while (iDiskInfo > 0)
	{
		if (iDiskInfo & 1)
			++iDiskCount;
		iDiskInfo = iDiskInfo >> 1;
	}
#endif
	return iDiskCount;
}


QStringList TDrive::GetAllDriveName()
{
	QStringList lstDriveName;
#ifdef WIN32
	int iAllDriveLength = GetLogicalDriveStringsA(0, NULL);
	char* pAllDriveStr = new char[iAllDriveLength];
	GetLogicalDriveStringsA(iAllDriveLength, pAllDriveStr);
	for (int i = 0; i < iAllDriveLength / 4; i++)
	{
		char pDrive[3];
		pDrive[0] = pAllDriveStr[i * 4];
		pDrive[1] = pAllDriveStr[i * 4 + 1];
		pDrive[2] = pAllDriveStr[i * 4 + 2];
		QString strDriveName = QString::fromLatin1(pDrive, 3);
		lstDriveName.append(strDriveName);
	}
#endif
	return lstDriveName;
}


unsigned int TDrive::GetDriveType(const QString &strDriveName)
{
	unsigned int iDriveType(DRIVE_UNKNOWN);
#ifdef WIN32
	char* pDriveName = strDriveName.toLocal8Bit().data();
	iDriveType = GetDriveTypeA(pDriveName);
	if (iDriveType == DRIVE_FIXED)
		;
	else if (iDriveType == DRIVE_REMOVABLE)
		;
	else if (iDriveType == DRIVE_REMOTE)
		;
	else if (iDriveType == DRIVE_CDROM)
		;
	else if (iDriveType == DRIVE_RAMDISK)
		;
	else if (iDriveType == DRIVE_NO_ROOT_DIR)
		;
	else if (iDriveType == DRIVE_UNKNOWN)
		;
#endif
	return iDriveType;
}


unsigned long long TDrive::GetDiskFreeSpace(const QString &strDriver)
{
	unsigned long long freespace(0);
#ifdef WIN32
	LPCWSTR lpcwstrDriver = (LPCWSTR)strDriver.utf16();
	ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
	if (!GetDiskFreeSpaceExW(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes))
	{
		return 0;
	}
	freespace = (unsigned long long)liTotalFreeBytes.QuadPart / MB;
#endif
	return freespace;
}

QString TDrive::GetMaxFreeSpaceDisk(unsigned long long &iMaxFreeSpace)
{
	iMaxFreeSpace = 0;
	QString strMaxFreeSpaceDisk;
#ifdef WIN32
	QStringList lstDriveName = GetAllDriveName();
	for (auto strDriveName : lstDriveName)
	{
		unsigned int iDriveType = GetDriveType(strDriveName);
		if (iDriveType != DRIVE_FIXED)
			continue;
		int iFreeSpace = GetDiskFreeSpace(strDriveName);
		if (iFreeSpace > iMaxFreeSpace)
		{
			iMaxFreeSpace = iFreeSpace;
			strMaxFreeSpaceDisk = strDriveName;
		}
	}
#endif
	return strMaxFreeSpaceDisk;
}

void TDrive::GetDiskSpaceInfo(const QString &strDriveName, 
	int &iSizePercent, 
	double &dSizeUse,
	double &dSizeFree,
	double &dSizeAll)
{
#ifdef WIN32
	QDir dir(strDriveName);
	if (!dir.isRoot())
		return;

	QString dirName = dir.absolutePath();
	LPCWSTR lpcwstrDriver = (LPCWSTR)dirName.utf16();
	ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
	if (GetDiskFreeSpaceExW(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes)) {
		dSizeUse = (double)(liTotalBytes.QuadPart - liTotalFreeBytes.QuadPart) / GB;
		dSizeFree = (double)liTotalFreeBytes.QuadPart / GB;
		dSizeAll = (double)liTotalBytes.QuadPart / GB;
		iSizePercent = 100 - ((double)liTotalFreeBytes.QuadPart / liTotalBytes.QuadPart) * 100;
	}
#endif // WIN32

}


