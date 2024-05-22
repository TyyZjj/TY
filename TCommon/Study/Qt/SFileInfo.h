#pragma once
#ifndef _SFILEINFO_H_ 
#define _SFILEINFO_H_

//QFileInfo的学习注释
/*
调试环境: 
->F:\Data\2022\TTest目录下, 包含以下文件:
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

//一、函数
// QFileInfo(const QDir &dir, const QString &file);
/*
如果dir是相对路径, QFileInfo也会给定相对路径;
如果file是绝对路径,dir将被忽略;
*/

//二、
/*
->F:\\Data\\2019\\20190417_TestDB\\labeldata.txt.csv
	str = fileInfo.baseName();			labeldata
	str = fileInfo.absoluteFilePath();	F:/Data/2019/20190417_TestDB/labeldata.txt.csv
	str = fileInfo.canonicalFilePath();	F:/Data/2019/20190417_TestDB/labeldata.txt.csv		规范文件路径, 即没有符号连接或冗余的绝对路径或者带'.'的路径
	str = fileInfo.fileName();			labeldata.txt.csv
	str = fileInfo.completeBaseName();	labeldata.txt
	str = fileInfo.suffix();			csv
	str = fileInfo.bundleName();		""		IOS系统专用, 如"/Applications/Safari.app"返回"Safari".其他操作系统则返回空字符串
	str = fileInfo.completeSuffix();	txt.csv
	str = fileInfo.path();				F:/Data/2019/20190417_TestDB
	str = fileInfo.absolutePath();		F:/Data/2019/20190417_TestDB
	str = fileInfo.canonicalPath();		F:/Data/2019/20190417_TestDB

->F:\\Data\\2022\\TTest\\5...lnk
	str = fileInfo.baseName();			5
	str = fileInfo.absoluteFilePath();	F:/Data/2022/TTest/5...lnk
	str = fileInfo.canonicalFilePath();	F:/Data/2022/TTest/5
	str = fileInfo.fileName();			5...lnk
	str = fileInfo.completeBaseName();	5..
	str = fileInfo.suffix();			lnk
	str = fileInfo.bundleName();		""
	str = fileInfo.completeSuffix();	..lnk
	str = fileInfo.path();				F:/Data/2022/TTest
	str = fileInfo.absolutePath();		F:/Data/2022/TTest
	str = fileInfo.canonicalPath();		F:/Data/2022/TTest

->F:\\Data\\2022\\TTest\\3
	str = fileInfo.baseName();			3
	str = fileInfo.absoluteFilePath();	F:/Data/2022/TTest/3
	str = fileInfo.canonicalFilePath();	F:/Data/2022/TTest/3
	str = fileInfo.fileName();			3
	str = fileInfo.completeBaseName();	3
	str = fileInfo.suffix();			""
	str = fileInfo.bundleName();		""
	str = fileInfo.completeSuffix();	""
	str = fileInfo.path();				F:/Data/2022/TTest/
	str = fileInfo.absolutePath();		F:/Data/2022/TTest/
	str = fileInfo.canonicalPath();		F:/Data/2022/TTest/

Todo:带"."的文件夹中, canonicalFilePath的返回值并不靠谱, 测试过程中发现不确定的输出
->F:\\Data\\2022\\3..		实际指向-->F:\\Data\\2022\\TTest\\3
	str = fileInfo.baseName();			3
	str = fileInfo.absoluteFilePath();	F:/Data/2022/3..
	str = fileInfo.canonicalFilePath();	""
	str = fileInfo.fileName();			3..
	str = fileInfo.completeBaseName();	3.
	str = fileInfo.suffix();			""
	str = fileInfo.bundleName();		""
	str = fileInfo.completeSuffix();	.
	str = fileInfo.path();				F:/Data/2022
	str = fileInfo.absolutePath();		F:/Data/2022
	str = fileInfo.canonicalPath();		.
*/

//三、
/*
bool isNativePath(): 文件路径可以直接用于本机API，则返回true

bool makeAbsolute(): 将文件路径转换为绝对路径

bool isSymLink(): 是否是符号链接或快捷方式. 在windows下, 此函数不认为'..'文件是符号链接.
*/

#endif