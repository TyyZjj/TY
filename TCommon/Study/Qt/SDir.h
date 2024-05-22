#ifndef _SDIR_H_ 
#define _SDIR_H_

//QDir的学习注释.
/*
调试环境: F:\Data\2022\TTest目录下, 包含以下文件:
	->1			->文件夹
	->1.txt		->文件
	->1.lnk		->1.txt的快捷方式
	->2			->文件夹、隐藏
	->2.txt		->文件、隐藏
	->2.lnk		->2.txt的快捷方式
	->3			->文件夹
	->3.txt		->文件、只读
	->3..		->文件夹、3的软连接
	->3...lnk	->3..的快捷方式(双击进文件夹3, 右键属性为文件夹)
	->3.lnk		->3的快捷方式、隐藏
	->4			->文件夹、隐藏
	->4..		->文件夹、4的软连接
	->4.lnk		->4的快捷方式
	->4...lnk	->4..的快捷方式(双击进文件夹4, 右键属性为文件)
	->5..		->文件夹、软连接(无指向, 双击报错: 目录名称无效)
	->5			->文件
	->5...lnk	->5..的快捷方式(实际指向文件5.., 双击无反应, 右键属性为文件) Todo:可能是文件系统的BUG
	->6..		->文件夹、软连接(无指向, 双击报错: 不可用)
	->TTest.lnk	->TTest的快捷方式
*/


//一、Filter、Filters
/*
entryList时, Filter值必须附带Dirs/Files
enum Filter { 
	Dirs        = 0x001,
		1.列出目录下的文件夹. 不包含文件.
		2.不包含隐藏文件夹.
		3.文件夹快捷方式是否包含, 只和其指向的文件夹是否隐藏有关, 和其本身的隐藏属性无关.
		3.包含'.'和'..'文件夹, 不管其指向的文件夹是否是隐藏文件.
		4.'.'和'..'文件夹的快捷方式是否包含, 只和其最终指向的文件夹是否隐藏有关, 和其本身的隐藏属性无关.
		例:entryList =>	./../1/3/3../3...lnk/3.lnk/4../5../6../TTest.lnk

	Files       = 0x002,
		1.列出文件. 不包含文件夹,
		2.不包含隐藏文件.
		3.文件快捷方式是否包含,只和其指向的文件是否隐藏有关, 和其本身的隐藏属性无关.
		例:entryList => 1.lnk/1.txt/3.txt/5/5...lnk

	Drives      = 0x004,
		1.列出逻辑驱动器名称,该枚举变量在Linux/Unix中将被忽略. 如:"C:"、"D:". 
		2.参见:QDir::drives()
		Todo:entryList函数中貌似不生效.

	NoSymLinks  = 0x008,
		1.不列出符号链接(快捷方式)
		2.包含'.'和'..'文件夹
		例:entryList(Files | NoSymLinks) => 1.txt/3.txt/5/5..
		Todo: 5..右键属性为文件夹, 为什么此处能显示
		例:entryList(Dirs | NoSymLinks) => ./../1/3/3..
		Todo: 4../6..为什么不显示, 也搞不清楚. 虽然文件夹4是隐藏的, 但Dirs中就能显示.

	AllEntries  = Dirs | Files | Drives,
		
	TypeMask    = 0x00f = Dirs | Files | Drives | NoSymLink,
		例:entryList => ./../1/1.txt/3/3../3.txt/5/5..

	Readable    = 0x010,
		例:entryList(Dirs | Readable) => ./../1/3/3../3...lnk/3.lnk/4../5../TTest.lnk
		Todo: 5..还在, 6..没了. 都会报错啊, 虽然错误不一样
		例:entryList(Files | Readable) => 1.lnk/1.txt/3.txt/5/5...lnk

	Writable    = 0x020,
		例:entryList(Dirs | Writable) => ./../1/3/3../3...lnk/3.lnk/4../5../TTest.lnk
		例:entryList(Files | Writable) => 1.lnk/1.txt/5/5...lnk

	Executable  = 0x040,
		1.列出当前应用有执行权限的文件(.exe/.bat等)或目录
		例:entryList(AllEntries | Executable) => ./../1/3/3../3...lnk/3.lnk/4../TTest.lnk

	PermissionMask    = 0x070 = Readable | Writable | Executable,
		例:entryList(Dirs | PermissionMask) => ./../1/3/3../3...lnk/3.lnk/4../5../6../TTest.lnk
		Todo: 这就更奇妙了. PermissionMask = Readable | Writable | Executable, 不应该是三者的集合嘛. 6..怎么蹦出来了, BUG不少啊

	Modified    = 0x080,
		1.列出已被修改的文件，该值在Linux/Unix系统中将被忽略
		例:entryList(Dirs | Modified) => ./../1/3/3../3...lnk/3.lnk/4../5../6../TTest.lnk
		例:entryList(Files | Modified) => 1.lnk/1.txt/3.txt/5/5...lnk
		Think: 既然和直接检索Dirs和Files没有区别, 那么Modified的意义是什么

	Hidden      = 0x100,
		1.列出隐藏文件
		例:entryList(Dirs | Hidden) => ./../1/2/3/3../3...lnk/3.lnk/4/4../4...lnk/4.lnk/5../6../TTest.lnk
		例:entryList(Files | Hidden) => 1.lnk/1.txt/2.lnk/2.txt/3.txt/5/5...lnk

	System      = 0x200,
		1.列出系统文件

	AccessMask  = 0x3F0 = Readable | Writable | Executable | Modified | Hidden | System

	AllDirs       = 0x400,
		1.列出目录下的所有文件夹, 不对文件夹名臣过滤. 包含'.'和'..'文件夹, 不包含文件, 不包含隐藏文件夹
			  
	CaseSensitive = 0x800,
		1.设定过滤器为大小写敏感

	NoDot         = 0x2000,
	NoDotDot      = 0x4000,
	NoDotAndDotDot = NoDot | NoDotDot,

	NoFilter = -1
};
*/

//二、SortFlag
/*
enum SortFlag {
	Name = 0x00,		按名称排序
	Time = 0x01,		按时间排序(修改时间)
	Size = 0x02,		按文件大小排序
	Unsorted = 0x03 = Name | Time | Size,	不排序
	SortByMask = 0x03,

	DirsFirst = 0x04,	首先是目录,然后是文件
	Reversed = 0x08,	倒序
	IgnoreCase = 0x10,	不区分大小写进行排序
	DirsLast = 0x20,	首先是文件,然后是目录
	LocaleAware = 0x40,	根据本地设置进行适当排序(不常用)
	Type = 0x80,		类型
	NoSort = -1
};
*/

//三、函数
//(一).QDir(const QString &path = QString())
/*
path为空时, 返回当前目录. QDir::currentPath()
*/

//(二).路径
/*
QDir dir("C:\\Users\\Taoyuyu\\Desktop\\/shoushu.xls");
	str = dir.path();				C:/Users/Taoyuyu/Desktop//shoushu.xls
	
	str = dir.absolutePath();		C:/Users/Taoyuyu/Desktop/shoushu.xls
		1.绝对路径(一个由"/"或驱动器标识开始的路径),可包含符号连接, 不含"."、".."或者多个分隔符

	str = dir.canonicalPath();		C:/Users/Taoyuyu/Desktop/shoushu.xls
		1.返回规范的路径, 也就是没有符号连接或者多余的"."、".."元素的路径
		2.没有符号连接的系统上, 和absolutePath()返回值一样

	str = dir.dirName();			shoushu.xls

QDir::currentPath()		E:/Git/LY-Code/RS-Keyboard/RS-Keyboard		vs调试时和实际运行时返回的路径不同
	参见QCoreApplication::applicationDirPath(), QApplication::applicationDirPath()
QDir::homePath()		C:/Users/Taoyuyu
QDir::rootPath()		C:/
QDir::tempPath()		C:/Users/Taoyuyu/AppData/Local/Temp
*/

//dir.rename("F:\\Old", "F:\\New")	路径需要是全路径

//QDir::drives() 返回所有的驱动器(不光是硬盘)


#endif // !_TPATH_H_ 