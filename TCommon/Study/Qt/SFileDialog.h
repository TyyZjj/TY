#pragma once
#ifndef _SFILEDIALOG_H_
#define _SFILEDIALOG_H_

//QFileDialog的学习注释.


//一、Option、Options
/*
	enum Option
	{
		ShowDirsOnly                = 0x00000001,
			1、显示文件夹
			2、仅在FileMode=Directory时生效
			3、默认Option()时, 文件和文件夹都会显示
		DontResolveSymlinks         = 0x00000002,
			1、不解析符号链接
			2、默认Option()时, 会解析符号链接
		DontConfirmOverwrite        = 0x00000004,
			1、如果文件存在, 不再确认
			2、默认Option()时, 会请求确认
		DontUseSheet                = 0x00000008,//Todo,不管
		DontUseNativeDialog         = 0x00000010,
			1、不使用本机文件对话框(此时可以对对话框的样式进行设置)
			2、默认Option()时, 是使用本机文件对话框(无法设置对话框样式)
		ReadOnly                    = 0x00000020,
			1、只读方式打开(不能新建/删除文件/文件夹)
		HideNameFilterDetails       = 0x00000040,
			1、隐藏文件名筛选器
		DontUseCustomDirectoryIcons = 0x00000080
			1、使用默认目录图标.(系统允许用户设置不同的图标,自定义图标的查找会对驱动器的性能造成很大影响)
	};
*/

//二、选择文件、选择文件夹、选择路径、选择文件名
//1、getOpenFileName			选择一个文件	
//2、getOpenFileNames			选择一堆文件
//3、getOpenFileUrl				选择一个文件. 和getOpenFileName的区别在于, 此函数可以选择远程文件
//4、getOpenFileUrls			选择一堆文件
//5、getSaveFileName			选择保存的文件名
//6、getSaveFileUrl				选择保存的文件名, 和getSaveFileName的区别在于, 此函数可以选择远程文件
//7、getExistingDirectory		选择一个文件夹
//8、getExistingDirectoryUrl	选择一个文件夹, 和getExistingDirectoryUrl的区别在于, 此函数可以选择远程路径

#endif