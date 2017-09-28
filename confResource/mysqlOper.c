/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : mysqlOper.c
* Descripe     : operate mysql database
******************************************************/

#include "mysqlOper.h"

static char error_info[LOGINFO_LENGTH];

bool startMysql()
{
    char config_value[CONFIG_VALUE_MAX_NUM];
    if(!getConfValueByLabelAndKey("mysqlInfo", "bind_address", bind_address))
		RecordLog("get mysqlInfo->bind_address failure\n");
	if(getConfValueByLabelAndKey("mysqlInfo", "port", config_value))
    {
		port = StrToInt(config_value);
    }
    else
        RecordLog("get mysqlInfo->port failure\n");
	if(!getConfValueByLabelAndKey("mysqlInfo", "user", user))
		RecordLog("get mysqlInfo->user failure\n");
	if(!getConfValueByLabelAndKey("mysqlInfo", "pass", pass))
		RecordLog("get mysqlInfo->pass failure\n");
	if(!getConfValueByLabelAndKey("mysqlInfo", "database", database))
		RecordLog("get mysqlInfo->database failure\n");
    
    mysql_library_init(0, NULL, NULL);
	mysqlConnect = mysql_init(&db);
    if(mysqlConnect == NULL)
    {
        RecordLog("init mysql failure\n");
        return false;
    }
	if(NULL == mysql_real_connect((MYSQL *)mysqlConnect, bind_address, user, pass, database, port, NULL, 0))
	{
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "connect failed: %s\n", mysql_error(mysqlConnect));
		RecordLog(error_info);
		return false;
	}
    
    return true;
}

void stopMysql()
{
    mysql_close(mysqlConnect);
    mysql_library_end();
}

bool executeSQLCommand(MYSQL *mysqlConn, char *command)
{
    if(mysqlConn != NULL)
    {
        if(mysql_real_query(mysqlConn, command, strlen(command)) != 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "execute command failed: %s\n", mysql_error(mysqlConn));
            RecordLog(error_info);
            return false;
        }
    }
    else
    {
        MYSQL temp_db;
        MYSQL *tempMysqlConnect = NULL;
        tempMysqlConnect = mysql_init(&temp_db);
        if(tempMysqlConnect == NULL)
        {
            RecordLog("init mysql failure\n");
            return false;
        }
        if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
            RecordLog(error_info);
            mysql_close(tempMysqlConnect);
            return false;
        }
        if(mysql_real_query(tempMysqlConnect, command, strlen(command)) != 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
            RecordLog(error_info);
            mysql_close(tempMysqlConnect);
            return false;
        }
        mysql_close(tempMysqlConnect);
    }
    
    
    return true;
}
