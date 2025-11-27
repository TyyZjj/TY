#ifndef DBDATADEFINE_H
#define DBDATADEFINE_H

#include <QDateTime>

#pragma region 数据库相关
struct sDBInfo
{
	QString         strDbType;
	QString         strDbName;
	QString         strDbAddress;
	QString			strDbSvrName = "";
	int				iDbPort = 0;
	QString			strDbUserID = "";
	QString			StrDbUserPwd = "";
	bool			bDbIsTrans = false;		//是否启用事务模式


	//供map的key值使用
	bool operator< (const sDBInfo& other) const   //注: const 不能省
	{
		if (strDbType < other.strDbType)
		{
			return true;
		}
		else if (strDbType == other.strDbType)
		{
			if (strDbName < other.strDbName)
			{
				return true;
			}
		}
		return false;
	}
};

QByteArray GetFileContent(const QString& strFilePath);
bool SetFileContent(QString strFilePath, QByteArray content);

#endif // DBDATADEFINE_H