#ifndef _TDrive_H_
#define _TDrive_H_

#include <QString>
#include <QStringList>

//��������ص���
class TDrive
{
public:
	//��ȡ(������)��������
	static unsigned long GetDiskCount();

	//��ȡ����(������)���̵�����
	static QStringList GetAllDriveName();

	//************************************
	// ����: 	const QString & strDriveName: "C:\\"
	// ����:	unsigned int
	//		0(DRIVE_UNKNOWN: δ֪�豸),
	//		1(DRIVE_NO_ROOT_DIR: ·����Ч),
	//		2(DRIVE_REMOVABLE: ���ƶ�����),
	//		3(DRIVE_FIXED: Ӳ��),
	//		4(DRIVE_REMOTE: �������),
	//		5(DRIVE_CDROM: ����),
	//		6(DRIVE_RAMDISK: ����RAM����)
	// ����:	��ȡ������������
	//************************************
	static unsigned int GetDriveType(const QString &strDriveName);

	//��ȡ����ʣ��ռ�(��λMB)
	static unsigned long long GetDiskFreeSpace(const QString &strDriver);
	
	//************************************
	// ����: 	unsigned long long & iMaxFreeSpace: ���(��λMB)
	// ����:	QT_NAMESPACE::QString: �̷�����("C:\\")
	// ����:	��ȡʣ��ռ����Ĵ���(Ӳ��)
	//************************************
	static QString GetMaxFreeSpaceDisk(unsigned long long &iMaxFreeSpace);

	//************************************
	// ����: 	const QString & strDriveName: ������·�� QDir::drives()
	// ����: 	int & iSizePercent: ��ʹ�õı���
	// ����: 	QString & strSizeUse: ��ʹ�ÿռ�(��λGB)
	// ����: 	QString & strSizeFree: ����ռ�(��λGB)
	// ����: 	QString & strSizeAll: ���пռ�(��λGB)
	// ����:	void
	// ����:	��ȡ��ǰ����״̬
	//************************************
	static void GetDiskSpaceInfo(const QString &strDriveName,
		int &iSizePercent,
		double &dSizeUse,
		double &dSizeFree,
		double &dSizeAll);
private:

};




#endif
