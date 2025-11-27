#ifndef _PERMISSION_H_
#define _PERMISSION_H_
#include <QMap>
#include <QString>
#include "loginmanager_global.h"

//内容的权限等级
enum nContentPermissionLevel
{
	nNoPermission = 0,	//没有权限查看
	nOnlyRead,			//只读
	nReadWrite,			//读写
	nOperator = nReadWrite, //可操作
	nAllocation,		//可分配
};
typedef nContentPermissionLevel CPL;

//权限内容(某个功能对应的权限)
struct LOGINMANAGER_EXPORT sContentPermission
{
	QString strName;	//权限名称
	QString strParent;	//所属的画面/分类名称
	QMap<int, int>	mapDefaultUserPermissionLevel; //此功能对应 的默认的用户组的权限
	/*= [&]()->QMap<int, int> {
		QMap<int, int> map;
		map.insert(DUGL::nUserLevel_Invalid, CPL::nNoPermission);
		map.insert(DUGL::nUserLevel_Operator, CPL::nNoPermission);
		map.insert(DUGL::nUserLevel_Technician, CPL::nOnlyRead);
		map.insert(DUGL::nUserLevel_Engineer, CPL::nReadWrite);
		map.insert(DUGL::nUserLevel_Supervisor, CPL::nReadWrite);
		map.insert(DUGL::nUserLevel_Admin, CPL::nAllocation);
		map.insert(DUGL::nUserLevel_Develop, CPL::nAllocation);
		return map;
	}();*/
	CPL iLevel = CPL::nNoPermission;

	bool operator<(const sContentPermission& other)const
	{
		if (strName == other.strName)
			return strParent < other.strParent;
		return
			strName < other.strName;
	}
	bool operator==(const sContentPermission& other)const
	{
		return strName == other.strName &&
			strParent == other.strParent;
	}
};
typedef sContentPermission CP;
typedef QSet<CP> CPSet;
typedef QList<CP> CPList;


//用户组权限
class CUserGroupPermission
{
	QString					strUserGroupName;
	QMap<QString, CPSet>	mapPermission;	//各功能对应的权限
};
typedef CUserGroupPermission UP;

#endif // !_PERMISSION_H_
