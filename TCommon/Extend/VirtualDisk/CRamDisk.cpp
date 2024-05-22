#include "CRamDisk.h"
#include <windows.h>
#ifdef RAMDISK_LIB
#include "ntumapi.h"
#include "imdisk.h"
#endif
#include <QDir>
#include <QProcess>

CRamDisk*	CRamDisk::m_pInstance = nullptr;

CRamDisk * CRamDisk::GetKernel()
{
	if (m_pInstance == nullptr)
		m_pInstance = new CRamDisk;
	return m_pInstance;
}

void CRamDisk::DestroyKernel()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void CRamDisk::setImDiskExePath(QString strImDiskExePath)
{
	this->strImDiskExePath = strImDiskExePath;
}

#ifdef RAMDISK_LIB
BOOL
ImDiskCliCheckDriverVersion(HANDLE Device)
{
	DWORD VersionCheck = 0;
	DWORD BytesReturned;
	if (!DeviceIoControl(Device,
		IOCTL_IMDISK_QUERY_VERSION,
		NULL, 0,
		&VersionCheck, sizeof VersionCheck,
		&BytesReturned, NULL))
		return FALSE;

	if (BytesReturned < sizeof VersionCheck)
		return FALSE;

	if (VersionCheck != IMDISK_DRIVER_VERSION)
		return FALSE;

	return true;
}

BOOL
ImDiskCliValidateDriveLetterTarget(
	LPCWSTR DriveLetter,
	LPCWSTR ValidTargetPath)
{
	WCHAR target[MAX_PATH];

	if (QueryDosDevice(DriveLetter, target, _countof(target)))
	{
		if (wcscmp(target, ValidTargetPath) == 0)
		{
			return TRUE;
		}

		//ImDiskOemPrintF(stderr,
		//	"Drive letter %1!ws! points to '%2!ws!' instead of expected '%3!ws!'. "
		//	"Will attempt to redefine drive letter.",
		//	DriveLetter, target, ValidTargetPath);
	}
	else if (GetLastError() != ERROR_FILE_NOT_FOUND)
	{
		;// PrintLastError(L"Error verifying temporary drive letter:");
	}

	return FALSE;
}

int
ImDiskCliFormatDisk(
	LPCWSTR DevicePath,
	WCHAR DriveLetter,
	LPCWSTR FormatOptions)
{
	static const WCHAR format_mutex[] = L"ImDiskFormat";

	static const WCHAR format_cmd_prefix[] = L"format.com ";

	WCHAR temporary_mount_point[] = { 255, L':', 0 };

#pragma warning(suppress: 6305)
	LPWSTR format_cmd = (LPWSTR)_alloca(sizeof(format_cmd_prefix) +
		sizeof(temporary_mount_point) + (wcslen(FormatOptions) << 1));

	STARTUPINFO startup_info = { sizeof(startup_info) };
	PROCESS_INFORMATION process_info;

	BOOL temp_drive_defined = FALSE;
	HANDLE hMutex = CreateMutex(NULL, FALSE, format_mutex);
	if (hMutex == NULL)
	{
		return 1;
	}

	switch (WaitForSingleObject(hMutex, INFINITE))
	{
	case WAIT_OBJECT_0:
	case WAIT_ABANDONED:
		break;

	default:
		//PrintLastError(L"Error, mutex object failed:");
		CloseHandle(hMutex);
		return 2;
	}

	if (DriveLetter != 0)
		temporary_mount_point[0] = DriveLetter;

	if (!ImDiskCliValidateDriveLetterTarget(temporary_mount_point,
		DevicePath))
	{
		if (!DefineDosDevice(DDD_RAW_TARGET_PATH,
			temporary_mount_point,
			DevicePath))
		{
			//PrintLastError(L"Error defining drive letter:");
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			return 3;
		}

		if (!ImDiskCliValidateDriveLetterTarget(temporary_mount_point,
			DevicePath))
		{
			if (!DefineDosDevice(DDD_REMOVE_DEFINITION |
				DDD_EXACT_MATCH_ON_REMOVE |
				DDD_RAW_TARGET_PATH,
				temporary_mount_point,
				DevicePath))
				;// PrintLastError(L"Error undefining temporary drive letter:");

			ReleaseMutex(hMutex);
			CloseHandle(hMutex);

			return 4;
		}
	}

	wcscpy(format_cmd, format_cmd_prefix);
	wcscat(format_cmd, temporary_mount_point);
	wcscat(format_cmd, L" ");
	wcscat(format_cmd, FormatOptions);

	if (CreateProcess(NULL, format_cmd, NULL, NULL, TRUE, 0, NULL, NULL,
		&startup_info, &process_info))
	{
		CloseHandle(process_info.hThread);
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
	}

	if (!ReleaseMutex(hMutex))
		return 5;

	if (!CloseHandle(hMutex))
		return 6;

	return 0;
}

//******Taoyuyu***********************   
// 日期: 2023/08/01
// 功能: 创建内存硬盘. 参照ImDisk源码编写, 阉割了部分功能. 此处仅适用于创建无映射文件的内存硬盘
// 返回: int
// 参数: LPDWORD DeviceNumber: 可选，在输入时传递设备号用于创建设备。如果要自动分配设备号，则可以传递NULL指针。在输出时，如果成功创建设备，该参数将包含已创建设备的设备号。
// 参数: PDISK_GEOMETRY DiskGeometry: 可选，虚拟磁盘的几何属性，例如磁盘大小等。如果指定了这些值为0，则将由驱动程序自动选择合适的值。
// 参数: PLARGE_INTEGER ImageOffset: 此处无效，如果虚拟磁盘是基于映像文件的，则可以使用此参数指定映像文件的偏移量。如果不是映像文件，则可以传递NULL。
// 参数: DWORD Flags: 此处仅适用IMDISK_TYPE_VM
//			用于指定虚拟磁盘的类型、设备类型和其他选项的位掩码。通常可以将这些标志保持为0，让驱动程序自动选择合适的值。
//		1. `IMDISK_DEVICE_TYPE_HD`: 表示创建的虚拟磁盘是硬盘类型。
//		2. `IMDISK_DEVICE_TYPE_FD`: 表示创建的虚拟磁盘是软盘类型。
//		3. `IMDISK_DEVICE_TYPE_CD`: 表示创建的虚拟磁盘是光盘类型（CD / DVD / BD）。
//		4. `IMDISK_OPTION_RO`: 表示创建的虚拟磁盘是只读的，即不允许写入数据。
//		5. `IMDISK_OPTION_REMOVABLE`: 表示创建的虚拟磁盘被视为可移动设备。这将影响操作系统对于设备的某些行为，例如是否显示“安全删除硬件”选项。
//		6. `IMDISK_TYPE_VM`: 表示虚拟磁盘是基于虚拟内存的，而不是映像文件。
//		7. `IMDISK_TYPE_FILE`: 表示虚拟磁盘是基于映像文件的。
//		8. `IMDISK_FILE_TYPE_AWEALLOC`: 表示虚拟磁盘映像文件使用AWE（Address Windowing Extensions）内存分配方式。这种方式可以更高效地处理大型映像文件。
// 参数: LPCWSTR FileName: 此处无效，如果虚拟磁盘是基于映像文件的，则传递映像文件的路径。如果是内存磁盘，则可以传递NULL。
// 参数: BOOL NativePath: 此处无效, 如果虚拟磁盘是基于映像文件的，则传递映像文件的路径。如果是内存磁盘，则可以传递NULL。
// 参数: LPWSTR MountPoint: 可选，指定驱动器号（例如F:或F:\）或空目录的路径（在另一个NTFS卷上）来挂载新的虚拟设备。
// 参数: BOOL NumericPrint: 此处无效
// 参数: LPWSTR FormatOptions: 格式化磁盘时, 需要进行Dos指令"/fs:ntfs /q /y"
// 参数: BOOL SaveSettings: 此处无效
//************************************
int
ImDiskCliCreateDevice(
	LPDWORD DeviceNumber,
	PDISK_GEOMETRY DiskGeometry,
	PLARGE_INTEGER ImageOffset,
	DWORD Flags,
	LPCWSTR FileName,
	BOOL NativePath,
	LPWSTR MountPoint,
	BOOL NumericPrint,
	LPWSTR FormatOptions,
	BOOL SaveSettings)
{
	PIMDISK_CREATE_DATA create_data;
	HANDLE driver;
	UNICODE_STRING file_name;
	DWORD dw;
	WCHAR device_path[MAX_PATH];

	//应该使用RtlInitUnicodeString初始化file_name, 但是RtlInitUnicodeString依赖WDK, 此处省略, 暂时写死
	file_name.Length = sizeof(IMDISK_CTL_DEVICE_NAME) - 2;
	file_name.MaximumLength = 36;
	file_name.Buffer = new WCHAR[file_name.MaximumLength];
	memcpy_s(file_name.Buffer, file_name.Length, IMDISK_CTL_DEVICE_NAME, file_name.Length);

	for (;;)
	{
		driver = ImDiskOpenDeviceByName(&file_name,
			GENERIC_READ | GENERIC_WRITE);

		if (driver != INVALID_HANDLE_VALUE)
			break;

		WCHAR svc[] = IMDISK_DRIVER_NAME;
		if (!ImDiskStartService(svc))
			return 1;
		Sleep(0);
	}

	if (!ImDiskCliCheckDriverVersion(driver))
	{
		CloseHandle(driver);
		return 2;
	}

	create_data = (PIMDISK_CREATE_DATA)_alloca(sizeof(IMDISK_CREATE_DATA));
	ZeroMemory(create_data, sizeof(IMDISK_CREATE_DATA));
	create_data->DriveLetter = MountPoint[0];
	create_data->DeviceNumber = *DeviceNumber;
	create_data->DiskGeometry = *DiskGeometry;
	create_data->ImageOffset = *ImageOffset;
	create_data->Flags = Flags;
	create_data->FileNameLength = 0;

	if (!DeviceIoControl(driver,
		IOCTL_IMDISK_CREATE_DEVICE,
		create_data,
		sizeof(IMDISK_CREATE_DATA) +
		create_data->FileNameLength,
		create_data,
		sizeof(IMDISK_CREATE_DATA) +
		create_data->FileNameLength,
		&dw,
		NULL))
	{
		CloseHandle(driver);
		return 3;
	}

	CloseHandle(driver);

	*DeviceNumber = create_data->DeviceNumber;
	// Build device path, e.g. \Device\ImDisk2
	_snwprintf(device_path, sizeof(device_path) / sizeof(*device_path) - 1,
		IMDISK_DEVICE_BASE_NAME L"%u", create_data->DeviceNumber);
	device_path[sizeof(device_path) / sizeof(*device_path) - 1] = 0;

	if (FormatOptions != NULL)
	{
		return ImDiskCliFormatDisk(device_path,
			create_data->DriveLetter,
			FormatOptions);
	}

	return 0;
}
#endif // RAMDISK_LIB

bool CRamDisk::create(int iSizeInMB, QString &strLetterName)
{
	if (strLetterName.isEmpty())
	{
#ifdef RAMDISK_LIB
		strLetterName = ImDiskFindFreeDriveLetter();
		strLetterName += ":";
#else
		return false;
#endif
	}

	if (strLetterName.count() != 2 &&
		strLetterName.count() != 3)
		return false;

	if (strLetterName.count() == 3)
	{
		if (!strLetterName.endsWith("\\") &&
			!strLetterName.endsWith("/"))
			return false;
		else
			strLetterName.chop(1);
	}

	QFileInfoList lstFileInfo = QDir::drives();
	QStringList lstDriveName;
	for (auto fileInfo : lstFileInfo)
		lstDriveName.append(fileInfo.absoluteFilePath());
	if (lstDriveName.contains(strLetterName))
		return true;

	if (iSizeInMB < 0)
		return false;

#ifdef RAMDISK_LIB
#pragma region API调用1
	WCHAR mount_point[] = L"Z:";
	int mp_size = strLetterName.toWCharArray(mount_point);
	DWORD device_number = IMDISK_AUTO_DEVICE_NUMBER;
	DISK_GEOMETRY disk_geometry = { 0 };
	LARGE_INTEGER image_offset = { 0 };
	disk_geometry.Cylinders.QuadPart = iSizeInMB;
	if (false/*GB*/)
		disk_geometry.Cylinders.QuadPart <<= 30;
	else if (true/*MB*/)
		disk_geometry.Cylinders.QuadPart <<= 20;
	else //KB
		disk_geometry.Cylinders.QuadPart <<= 10;
	image_offset.QuadPart = iSizeInMB;
	if (false/*GB*/)
		image_offset.QuadPart <<= 30;
	else if (true/*MB*/)
		image_offset.QuadPart <<= 20;
	else //KB
		image_offset.QuadPart <<= 10;
	WCHAR format_options[] = L"/fs:ntfs /q /y";
	int ret = ImDiskCliCreateDevice(&device_number,
		&disk_geometry,
		&image_offset,
		IMDISK_TYPE_VM,
		NULL,
		FALSE,
		mount_point,
		FALSE,
		format_options,
		FALSE);
	if (ret == 0)
		return true;
#pragma endregion
#pragma region API调用2 
#if 0
	//不清楚是什么情况, ImDiskCreateDeviceEx只能创建磁盘, 但是无法格式化磁盘, 系统会弹出格式化磁盘的弹窗
// Create a memory-based virtual disk
	DWORD deviceNumber = -1; // Variable to store the created device number
	DISK_GEOMETRY diskGeometry = { 0 }; // Use default disk geometry
	diskGeometry.Cylinders.QuadPart = iSizeInMB;
	if (false/*GB*/)
		diskGeometry.Cylinders.QuadPart <<= 30;
	else if (true/*MB*/)
		diskGeometry.Cylinders.QuadPart <<= 20;
	else //KB
		diskGeometry.Cylinders.QuadPart <<= 10;

	LARGE_INTEGER image_offset = { 0 };
	image_offset.QuadPart = 100;
	if (false/*GB*/)
		image_offset.QuadPart <<= 30;
	else if (true/*MB*/)
		image_offset.QuadPart <<= 20;
	else //KB
		image_offset.QuadPart <<= 10;
	WCHAR mountPoint[] = L"Z:";
	int size = strLetterName.toWCharArray(mountPoint);
	/*
参数解释：
- `hWndStatusText`: 可选，如果指定了一个窗口句柄，函数将向该窗口发送`WM_SETTEXT`消息以显示状态消息文本。如果该参数为NULL，则函数将以非交互方式运行。
- `DeviceNumber`: 可选，在输入时传递设备号用于创建设备。如果要自动分配设备号，则可以传递NULL指针。在输出时，如果成功创建设备，该参数将包含已创建设备的设备号。
- `DiskGeometry`: 可选，虚拟磁盘的几何属性，例如磁盘大小等。如果指定了这些值为0，则将由驱动程序自动选择合适的值。
- `ImageOffset`: 可选，如果虚拟磁盘是基于映像文件的，则可以使用此参数指定映像文件的偏移量。如果不是映像文件，则可以传递NULL。
- `Flags`: 用于指定虚拟磁盘的类型、设备类型和其他选项的位掩码。通常可以将这些标志保持为0，让驱动程序自动选择合适的值。
	1. `IMDISK_DEVICE_TYPE_HD`: 表示创建的虚拟磁盘是硬盘类型。
	2. `IMDISK_DEVICE_TYPE_FD`: 表示创建的虚拟磁盘是软盘类型。
	3. `IMDISK_DEVICE_TYPE_CD`: 表示创建的虚拟磁盘是光盘类型（CD/DVD/BD）。
	4. `IMDISK_OPTION_RO`: 表示创建的虚拟磁盘是只读的，即不允许写入数据。
	5. `IMDISK_OPTION_REMOVABLE`: 表示创建的虚拟磁盘被视为可移动设备。这将影响操作系统对于设备的某些行为，例如是否显示“安全删除硬件”选项。
	6. `IMDISK_TYPE_VM`: 表示虚拟磁盘是基于虚拟内存的，而不是映像文件。
	7. `IMDISK_TYPE_FILE`: 表示虚拟磁盘是基于映像文件的。
	8. `IMDISK_FILE_TYPE_AWEALLOC`: 表示虚拟磁盘映像文件使用AWE（Address Windowing Extensions）内存分配方式。这种方式可以更高效地处理大型映像文件。
- `FileName`: 可选，如果虚拟磁盘是基于映像文件的，则传递映像文件的路径。如果是内存磁盘，则可以传递NULL。
- `NativePath`: 一个布尔值，指示FileName参数是否是一个原生的NT路径（例如\??\C:\imagefile.img），如果是，则设为TRUE；否则，设为FALSE。
- `MountPoint`: 可选，指定驱动器号（例如F:或F:\）或空目录的路径（在另一个NTFS卷上）来挂载新的虚拟设备。
	*/
	BOOL success = ImDiskCreateDeviceEx(NULL, &deviceNumber, &diskGeometry, NULL/*不创建映像文件*//*image_offset*/,
		IMDISK_DEVICE_TYPE_HD | IMDISK_TYPE_VM, NULL, FALSE, mountPoint);
	return success;
#endif // 0

#pragma endregion
#else
	if (!QFile::exists(strImDiskExePath) &&
		!QFileInfo(strImDiskExePath).isExecutable())
		return false;

	const char* mountPoint = "/DIRECTORY"; // Replace with the desired directory

	//Build the command to create the virtual disk
	std::string command = strImDiskExePath.toStdString();
	command += " -a -t vm -m ";
	command += strLetterName.toStdString();
	command += " -s ";
	command += std::to_string(iSizeInMB) + "M"; // Convert disk size to string
	command += " -p \"/fs:ntfs /q /y\""; // Optional: Format as NTFS

	// Create the virtual disk
	int result = std::system(command.c_str());
	if (result == 0) {
		// Optionally, you can mount the disk to a directory
		std::string mountCommand = strImDiskExePath.toStdString();
		mountCommand += " -a ";
		mountCommand += strLetterName.toStdString();
		mountCommand += " -m ";
		mountCommand += mountPoint;
		result = std::system(mountCommand.c_str());
		return result;
	}
#endif // RAMDISK_LIB

	return false;
}

CRamDisk::CRamDisk(QObject *parent)
	: QObject(parent)
{
}

CRamDisk::~CRamDisk()
{
}