#pragma once
#ifndef _SFILEDIALOG_H_
#define _SFILEDIALOG_H_

//QFileDialog��ѧϰע��.


//һ��Option��Options
/*
	enum Option
	{
		ShowDirsOnly                = 0x00000001,
			1����ʾ�ļ���
			2������FileMode=Directoryʱ��Ч
			3��Ĭ��Option()ʱ, �ļ����ļ��ж�����ʾ
		DontResolveSymlinks         = 0x00000002,
			1����������������
			2��Ĭ��Option()ʱ, �������������
		DontConfirmOverwrite        = 0x00000004,
			1������ļ�����, ����ȷ��
			2��Ĭ��Option()ʱ, ������ȷ��
		DontUseSheet                = 0x00000008,//Todo,����
		DontUseNativeDialog         = 0x00000010,
			1����ʹ�ñ����ļ��Ի���(��ʱ���ԶԶԻ������ʽ��������)
			2��Ĭ��Option()ʱ, ��ʹ�ñ����ļ��Ի���(�޷����öԻ�����ʽ)
		ReadOnly                    = 0x00000020,
			1��ֻ����ʽ��(�����½�/ɾ���ļ�/�ļ���)
		HideNameFilterDetails       = 0x00000040,
			1�������ļ���ɸѡ��
		DontUseCustomDirectoryIcons = 0x00000080
			1��ʹ��Ĭ��Ŀ¼ͼ��.(ϵͳ�����û����ò�ͬ��ͼ��,�Զ���ͼ��Ĳ��һ����������������ɺܴ�Ӱ��)
	};
*/

//����ѡ���ļ���ѡ���ļ��С�ѡ��·����ѡ���ļ���
//1��getOpenFileName			ѡ��һ���ļ�	
//2��getOpenFileNames			ѡ��һ���ļ�
//3��getOpenFileUrl				ѡ��һ���ļ�. ��getOpenFileName����������, �˺�������ѡ��Զ���ļ�
//4��getOpenFileUrls			ѡ��һ���ļ�
//5��getSaveFileName			ѡ�񱣴���ļ���
//6��getSaveFileUrl				ѡ�񱣴���ļ���, ��getSaveFileName����������, �˺�������ѡ��Զ���ļ�
//7��getExistingDirectory		ѡ��һ���ļ���
//8��getExistingDirectoryUrl	ѡ��һ���ļ���, ��getExistingDirectoryUrl����������, �˺�������ѡ��Զ��·��

#endif