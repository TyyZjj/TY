#ifndef _SDIR_H_ 
#define _SDIR_H_

//QDir��ѧϰע��.
/*
���Ի���: F:\Data\2022\TTestĿ¼��, ���������ļ�:
	->1			->�ļ���
	->1.txt		->�ļ�
	->1.lnk		->1.txt�Ŀ�ݷ�ʽ
	->2			->�ļ��С�����
	->2.txt		->�ļ�������
	->2.lnk		->2.txt�Ŀ�ݷ�ʽ
	->3			->�ļ���
	->3.txt		->�ļ���ֻ��
	->3..		->�ļ��С�3��������
	->3...lnk	->3..�Ŀ�ݷ�ʽ(˫�����ļ���3, �Ҽ�����Ϊ�ļ���)
	->3.lnk		->3�Ŀ�ݷ�ʽ������
	->4			->�ļ��С�����
	->4..		->�ļ��С�4��������
	->4.lnk		->4�Ŀ�ݷ�ʽ
	->4...lnk	->4..�Ŀ�ݷ�ʽ(˫�����ļ���4, �Ҽ�����Ϊ�ļ�)
	->5..		->�ļ��С�������(��ָ��, ˫������: Ŀ¼������Ч)
	->5			->�ļ�
	->5...lnk	->5..�Ŀ�ݷ�ʽ(ʵ��ָ���ļ�5.., ˫���޷�Ӧ, �Ҽ�����Ϊ�ļ�) Todo:�������ļ�ϵͳ��BUG
	->6..		->�ļ��С�������(��ָ��, ˫������: ������)
	->TTest.lnk	->TTest�Ŀ�ݷ�ʽ
*/


//һ��Filter��Filters
/*
entryListʱ, Filterֵ���븽��Dirs/Files
enum Filter { 
	Dirs        = 0x001,
		1.�г�Ŀ¼�µ��ļ���. �������ļ�.
		2.�����������ļ���.
		3.�ļ��п�ݷ�ʽ�Ƿ����, ֻ����ָ����ļ����Ƿ������й�, ���䱾������������޹�.
		3.����'.'��'..'�ļ���, ������ָ����ļ����Ƿ��������ļ�.
		4.'.'��'..'�ļ��еĿ�ݷ�ʽ�Ƿ����, ֻ��������ָ����ļ����Ƿ������й�, ���䱾������������޹�.
		��:entryList =>	./../1/3/3../3...lnk/3.lnk/4../5../6../TTest.lnk

	Files       = 0x002,
		1.�г��ļ�. �������ļ���,
		2.�����������ļ�.
		3.�ļ���ݷ�ʽ�Ƿ����,ֻ����ָ����ļ��Ƿ������й�, ���䱾������������޹�.
		��:entryList => 1.lnk/1.txt/3.txt/5/5...lnk

	Drives      = 0x004,
		1.�г��߼�����������,��ö�ٱ�����Linux/Unix�н�������. ��:"C:"��"D:". 
		2.�μ�:QDir::drives()
		Todo:entryList������ò�Ʋ���Ч.

	NoSymLinks  = 0x008,
		1.���г���������(��ݷ�ʽ)
		2.����'.'��'..'�ļ���
		��:entryList(Files | NoSymLinks) => 1.txt/3.txt/5/5..
		Todo: 5..�Ҽ�����Ϊ�ļ���, Ϊʲô�˴�����ʾ
		��:entryList(Dirs | NoSymLinks) => ./../1/3/3..
		Todo: 4../6..Ϊʲô����ʾ, Ҳ�㲻���. ��Ȼ�ļ���4�����ص�, ��Dirs�о�����ʾ.

	AllEntries  = Dirs | Files | Drives,
		
	TypeMask    = 0x00f = Dirs | Files | Drives | NoSymLink,
		��:entryList => ./../1/1.txt/3/3../3.txt/5/5..

	Readable    = 0x010,
		��:entryList(Dirs | Readable) => ./../1/3/3../3...lnk/3.lnk/4../5../TTest.lnk
		Todo: 5..����, 6..û��. ���ᱨ��, ��Ȼ����һ��
		��:entryList(Files | Readable) => 1.lnk/1.txt/3.txt/5/5...lnk

	Writable    = 0x020,
		��:entryList(Dirs | Writable) => ./../1/3/3../3...lnk/3.lnk/4../5../TTest.lnk
		��:entryList(Files | Writable) => 1.lnk/1.txt/5/5...lnk

	Executable  = 0x040,
		1.�г���ǰӦ����ִ��Ȩ�޵��ļ�(.exe/.bat��)��Ŀ¼
		��:entryList(AllEntries | Executable) => ./../1/3/3../3...lnk/3.lnk/4../TTest.lnk

	PermissionMask    = 0x070 = Readable | Writable | Executable,
		��:entryList(Dirs | PermissionMask) => ./../1/3/3../3...lnk/3.lnk/4../5../6../TTest.lnk
		Todo: ��͸�������. PermissionMask = Readable | Writable | Executable, ��Ӧ�������ߵļ�����. 6..��ô�ĳ�����, BUG���ٰ�

	Modified    = 0x080,
		1.�г��ѱ��޸ĵ��ļ�����ֵ��Linux/Unixϵͳ�н�������
		��:entryList(Dirs | Modified) => ./../1/3/3../3...lnk/3.lnk/4../5../6../TTest.lnk
		��:entryList(Files | Modified) => 1.lnk/1.txt/3.txt/5/5...lnk
		Think: ��Ȼ��ֱ�Ӽ���Dirs��Filesû������, ��ôModified��������ʲô

	Hidden      = 0x100,
		1.�г������ļ�
		��:entryList(Dirs | Hidden) => ./../1/2/3/3../3...lnk/3.lnk/4/4../4...lnk/4.lnk/5../6../TTest.lnk
		��:entryList(Files | Hidden) => 1.lnk/1.txt/2.lnk/2.txt/3.txt/5/5...lnk

	System      = 0x200,
		1.�г�ϵͳ�ļ�

	AccessMask  = 0x3F0 = Readable | Writable | Executable | Modified | Hidden | System

	AllDirs       = 0x400,
		1.�г�Ŀ¼�µ������ļ���, �����ļ�����������. ����'.'��'..'�ļ���, �������ļ�, �����������ļ���
			  
	CaseSensitive = 0x800,
		1.�趨������Ϊ��Сд����

	NoDot         = 0x2000,
	NoDotDot      = 0x4000,
	NoDotAndDotDot = NoDot | NoDotDot,

	NoFilter = -1
};
*/

//����SortFlag
/*
enum SortFlag {
	Name = 0x00,		����������
	Time = 0x01,		��ʱ������(�޸�ʱ��)
	Size = 0x02,		���ļ���С����
	Unsorted = 0x03 = Name | Time | Size,	������
	SortByMask = 0x03,

	DirsFirst = 0x04,	������Ŀ¼,Ȼ�����ļ�
	Reversed = 0x08,	����
	IgnoreCase = 0x10,	�����ִ�Сд��������
	DirsLast = 0x20,	�������ļ�,Ȼ����Ŀ¼
	LocaleAware = 0x40,	���ݱ������ý����ʵ�����(������)
	Type = 0x80,		����
	NoSort = -1
};
*/

//��������
//(һ).QDir(const QString &path = QString())
/*
pathΪ��ʱ, ���ص�ǰĿ¼. QDir::currentPath()
*/

//(��).·��
/*
QDir dir("C:\\Users\\Taoyuyu\\Desktop\\/shoushu.xls");
	str = dir.path();				C:/Users/Taoyuyu/Desktop//shoushu.xls
	
	str = dir.absolutePath();		C:/Users/Taoyuyu/Desktop/shoushu.xls
		1.����·��(һ����"/"����������ʶ��ʼ��·��),�ɰ�����������, ����"."��".."���߶���ָ���

	str = dir.canonicalPath();		C:/Users/Taoyuyu/Desktop/shoushu.xls
		1.���ع淶��·��, Ҳ����û�з������ӻ��߶����"."��".."Ԫ�ص�·��
		2.û�з������ӵ�ϵͳ��, ��absolutePath()����ֵһ��

	str = dir.dirName();			shoushu.xls

QDir::currentPath()		E:/Git/LY-Code/RS-Keyboard/RS-Keyboard		vs����ʱ��ʵ������ʱ���ص�·����ͬ
	�μ�QCoreApplication::applicationDirPath(), QApplication::applicationDirPath()
QDir::homePath()		C:/Users/Taoyuyu
QDir::rootPath()		C:/
QDir::tempPath()		C:/Users/Taoyuyu/AppData/Local/Temp
*/

//dir.rename("F:\\Old", "F:\\New")	·����Ҫ��ȫ·��

//QDir::drives() �������е�������(������Ӳ��)


#endif // !_TPATH_H_ 