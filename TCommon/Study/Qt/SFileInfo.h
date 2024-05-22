#pragma once
#ifndef _SFILEINFO_H_ 
#define _SFILEINFO_H_

//QFileInfo��ѧϰע��
/*
���Ի���: 
->F:\Data\2022\TTestĿ¼��, ���������ļ�:
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

//һ������
// QFileInfo(const QDir &dir, const QString &file);
/*
���dir�����·��, QFileInfoҲ��������·��;
���file�Ǿ���·��,dir��������;
*/

//����
/*
->F:\\Data\\2019\\20190417_TestDB\\labeldata.txt.csv
	str = fileInfo.baseName();			labeldata
	str = fileInfo.absoluteFilePath();	F:/Data/2019/20190417_TestDB/labeldata.txt.csv
	str = fileInfo.canonicalFilePath();	F:/Data/2019/20190417_TestDB/labeldata.txt.csv		�淶�ļ�·��, ��û�з������ӻ�����ľ���·�����ߴ�'.'��·��
	str = fileInfo.fileName();			labeldata.txt.csv
	str = fileInfo.completeBaseName();	labeldata.txt
	str = fileInfo.suffix();			csv
	str = fileInfo.bundleName();		""		IOSϵͳר��, ��"/Applications/Safari.app"����"Safari".��������ϵͳ�򷵻ؿ��ַ���
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

Todo:��"."���ļ�����, canonicalFilePath�ķ���ֵ��������, ���Թ����з��ֲ�ȷ�������
->F:\\Data\\2022\\3..		ʵ��ָ��-->F:\\Data\\2022\\TTest\\3
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

//����
/*
bool isNativePath(): �ļ�·������ֱ�����ڱ���API���򷵻�true

bool makeAbsolute(): ���ļ�·��ת��Ϊ����·��

bool isSymLink(): �Ƿ��Ƿ������ӻ��ݷ�ʽ. ��windows��, �˺�������Ϊ'..'�ļ��Ƿ�������.
*/

#endif