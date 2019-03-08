

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name: common.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 实现公用函数
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
			modification: 修改内容
		 2. ... 
********************************************************************/


#include "common.h"


/**********************************************************
function: MyLog
description: 写日志信息
Input: szSource: 信息来源
       szLevel: 信息级别
       szFmt: 信息格式
Output:
Return: MY_SUCCEED: 成功, 失败退出
others:
***********************************************************/

int MyLog( char * szSource, char * szLevel, char * szFmt, ... )
{
	va_list vaList;
	char szBuf[MAX_RECORD_LEN + 1];	
	char szTime[14 + 1];
	FILE * fp;

	memset( szTime, 0, sizeof( szTime ) );
	GetCurrTime( szTime );
	
	if( 0 == *(szSrcDir) )
	{
		sprintf( szBuf, "%s/%s%8.8s.runlog", szLogPath, szLogPrefix, szTime );
	}
	else
	{
		sprintf( szBuf, "%s/%s%8.8s.%s.runlog", szLogPath, szLogPrefix, szTime, szSrcDir );
	}

	fp = fopen( szBuf, "a" );
	if( NULL == fp )
	{
		fprintf( stderr, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s\t%s\t%s\t%s\n", 
					szTime,
					szTime + 4,
					szTime + 6,
					szTime + 8, 
					szTime + 10, 
					szTime + 12,
					"sys",
					"exit",
					"不能打开日志文件" );
		fprintf( stderr, "open [%s] failed!\n", szBuf );
		perror( "打开日志失败:" );

		fclose( fp );			
		exit(1);
	}

	sprintf( szBuf, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s\t%s\t%s\t", 
					szTime,
					szTime + 4,
					szTime + 6,
					szTime + 8, 
					szTime + 10, 
					szTime + 12,
					szSource,
					szLevel );
					
	fprintf( fp, "%s", szBuf );

	va_start( vaList, szFmt );

	vfprintf( fp, szFmt, vaList );
	vsprintf( szBuf, szFmt, vaList );

	va_end( vaList );

	if( 0 == strcmp( szLevel, INFO_LEVEL_EXIT ) )
	{
		sprintf( szBuf, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s\t%s\t%s\t%s\n", 
					szTime,
					szTime + 4,
					szTime + 6,
					szTime + 8, 
					szTime + 10, 
					szTime + 12,
					"sys",
					"exit",
					"发生异常情况, 程序停止运行, 退出!\n\n" );
		
		fprintf( fp, "%s", szBuf );
		
		fclose( fp );
		exit(1);
	}
	
	fclose( fp );
	
	return MY_SUCCEED;
}

/**********************************************************
function: TrimLeft
description: 去字符串左端空格
Input: szTrim: 待处理字符串
Output:
Return: szTrim: 处理后的字符串
others:
***********************************************************/

char * TrimLeft( char * szTrim )
{
	char * pLoc;

	pLoc = szTrim;

	while( isspace( (int)*pLoc ) ) pLoc++;

	strcpy( szTrim, pLoc );

	return szTrim;
}

/**********************************************************
function: TrimRight
description: 去字符串右端空格
Input: szTrim: 待处理字符串
Output:
Return: szTrim: 处理后的字符串
others:
***********************************************************/

char * TrimRight( char * szTrim )
{
	int nLen;

	nLen = strlen( szTrim );
	if ( nLen <= 0 )
	{
		return szTrim;
	}

	while( nLen > 0 && isspace( (int)szTrim[ nLen - 1 ] ) ) nLen--;

	szTrim[nLen] = '\0';

	return szTrim;
}

/**********************************************************
function: TrimLZero
description: 去字符串左端0字符
Input: szTrim: 待处理字符串
Output:
Return: szTrim: 处理后的字符串
others:
***********************************************************/

char * TrimLZero( char * szTrim )
{
	char * pLoc;

	pLoc = szTrim;

	while( (*pLoc) == '0' ) pLoc++;

	if( 0 == *(pLoc) || 0 == strlen( pLoc ) )
	{
		strcpy( szTrim, "0" );
	}
	else
	{
		strcpy( szTrim, pLoc );
	}	

	return szTrim;
}

/**********************************************************
function: GetIniInfo
description: 获取配置文件某一字段取值
Input: szFileName: 配置文件名
	   szLValue: 字段名
	   szRValue: 字段值
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetIniInfo( char * szFileName, char * szLValue, char * szRValue )
{
	FILE * fp;
	char szRec[MAX_RECORD_LEN + 1];
	char szVal[MAX_RECORD_LEN + 1];
	char * szPos;
	
	fp = fopen( szFileName, "r" );
	if( NULL == fp )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "INI 文件 %s 打开失败!\n", szFileName );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	while( NULL != fgets( szRec, sizeof( szRec ), fp ) )
	{
		szPos = NULL;
		if( NULL != ( szPos = ( char * ) strchr( szRec, '=' ) ) )
		{

			memcpy( szVal, szRec, szPos - szRec );
			szVal[szPos - szRec] = 0;
			TrimLeft( TrimRight( szVal ) );
			if( strcmp( szVal, szLValue ) == 0 )
			{
				strcpy( szRValue, szPos + 1 );
				TrimLeft( TrimRight( szRValue ) );
				fclose( fp );
				return MY_SUCCEED;
			}
			else
			{
				continue;
			}
		}
	}
	
	fclose( fp );
	
	return MY_FAIL;
}

/**********************************************************
function: IsFloat
description: 判断字符串值是否为浮点数
Input: szNum: 字符串
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsFloat( char * szNum )
{
    int nFlag;

    nFlag = 0;

    if ( 0 == *(szNum) || 0 == strlen( szNum ) )
    {
        return MY_FALSE;
    }

    while ( *szNum )
    {
        if ( !isdigit( (int)(*szNum) ) )
        {
            if( '.' != (*szNum) || nFlag || 0 == *(szNum + 1) )
            {
                return MY_FALSE;
            }

            nFlag++;
        }

        szNum++;
    }

	return MY_TRUE;
}

/**********************************************************
function: IsNum
description: 判断字符串值是否为数值
Input: szNum: 字符串
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsNum( char * szNum )
{
	if ( 0 == *(szNum) || 0 == strlen( szNum ) )
	{
		return MY_SUCCEED;
	}

	while ( *szNum )
	{
		if ( isdigit( (int)(*szNum) ) )
		{
			szNum++;
		}
		else
		{
			return MY_FALSE;
		}
	}

	return MY_TRUE;
}

/**********************************************************
function: IsStr
description: 判断字符串值是否合法
Input: szNum: 字符串
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsStr( char * szStr )
{
	int nOffset;
	
	nOffset = 0;
	
	if( 0 == strlen( szStr ) || 0 == *(szStr) )
	{
		return MY_FALSE;
	}
	
	while( nOffset < strlen( szStr ) )
	{
		if( ( szStr[nOffset] >= 'a' &&  szStr[nOffset] <= 'z' )
		 || ( szStr[nOffset] >= 'A' &&  szStr[nOffset] <= 'Z' )
		 || ( szStr[nOffset] >= '0' &&  szStr[nOffset] <= '9' )
		 || '_' == szStr[nOffset]
		  )	
		{
			nOffset++;
		}
		else
		{
			return MY_FALSE;
		}
	}

	return MY_TRUE;
}

/**********************************************************
function: IsLeapYear
description: 判断是否为闰年
Input: nYear: 年
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsLeapYear( int nYear )
{
	if( ( nYear % 4 == 0 && nYear % 100 != 0 ) || nYear % 400 == 0 )
	{
		return MY_TRUE;
	}
	else
	{
		return MY_FALSE;
	}
}

/**********************************************************
function: GetFileCount
description: 获取文件记录数
Input: szPath: 文件路径
Output:
Return: lCount: 记录数
others:
***********************************************************/

long GetFileCount( char * szPath )
{
	FILE * fp;
	int lCount;
	char szBuf[MAX_RECORD_LEN + 1];
	
	fp = fopen( szPath, "r" );
	
	if( NULL == fp )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "打开文件 %s 失败!\n", szPath );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return -1;
	}
	
	lCount = 0;
	
	while( NULL != fgets( szBuf, sizeof( szBuf ), fp ) )
	{
		if( 0 != strncmp( szBuf, FILE_NOTE_FLAG, strlen( FILE_NOTE_FLAG ) ) )
		{
			lCount++;
		}
	}
	
	fclose( fp );
	
	return lCount;
}

/**********************************************************
function: GetCurrTime
description: 获取当前系统时间
Input: szStr: 保存时间的字符串
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/

int GetCurrTime( char * szStr )
{
	time_t tTime;
	struct tm * tmTime;
	/*
	struct timeval mTime;
	*/

	time(&tTime);
	tmTime = localtime(&tTime);
	/*
	gettimeofday(&mTime, NULL);
	*/

	sprintf(szStr, "%04d%02d%02d%02d%02d%02d",
		tmTime->tm_year + 1900, tmTime->tm_mon + 1, 
		tmTime->tm_mday, tmTime->tm_hour, 
		tmTime->tm_min, tmTime->tm_sec
		/* 山西精确到MicroSecond */
		);

	return MY_SUCCEED;
}

/**********************************************************
function: GetCurrMicroSecond
description: 获取当前系统时间的微秒
Input: szStr: 保存时间的字符串
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/
int GetCurrMicroSecond( char *szStr )
{
	struct timeval mTime;

	gettimeofday( &mTime, NULL );
	sprintf( szStr, "%06d", mTime.tv_usec );

	return MY_SUCCEED ;

}

/**********************************************************
function: GetTimeInterval
description: 获取时间间隔
Input: szStartTime: 开始时间
       szEndTime: 结束时间
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/

time_t GetTimeInterval( char * szStartTime, char * szEndTime )
{
	time_t stTime, etTime;
	struct tm stmTime, etmTime;
	char szBuf[14 + 1];

	memset( &stmTime, 0, sizeof( struct tm ) );
	memset( &etmTime, 0, sizeof( struct tm ) );
	
	strncpy( szBuf, szStartTime + 12, 2 ); szBuf[2] = 0;
	stmTime.tm_sec = atoi( szBuf );
	
	strncpy( szBuf, szStartTime + 10, 2 ); szBuf[2] = 0;
	stmTime.tm_min = atoi( szBuf );
	
	strncpy( szBuf, szStartTime +  8, 2 ); szBuf[2] = 0;
	stmTime.tm_hour = atoi( szBuf );
	
	strncpy( szBuf, szStartTime +  6, 2 ); szBuf[2] = 0;
	stmTime.tm_mday = atoi( szBuf );

	strncpy( szBuf, szStartTime +  4, 2 ); szBuf[2] = 0;
	stmTime.tm_mon = atoi( szBuf ) - 1;

	strncpy( szBuf, szStartTime,   4    ); szBuf[4] = 0;
	stmTime.tm_year = atoi( szBuf ) - 1900;

	stTime =  mktime( &stmTime );

	strncpy( szBuf, szEndTime + 12, 2 ); szBuf[2] = 0;
	etmTime.tm_sec = atoi( szBuf );
	
	strncpy( szBuf, szEndTime + 10, 2 ); szBuf[2] = 0;
	etmTime.tm_min = atoi( szBuf );
	
	strncpy( szBuf, szEndTime +  8, 2 ); szBuf[2] = 0;
	etmTime.tm_hour = atoi( szBuf );
	
	strncpy( szBuf, szEndTime +  6, 2 ); szBuf[2] = 0;
	etmTime.tm_mday = atoi( szBuf );

	strncpy( szBuf, szEndTime +  4, 2 ); szBuf[2] = 0;
	etmTime.tm_mon = atoi( szBuf ) - 1;

	strncpy( szBuf, szEndTime,   4    ); szBuf[4] = 0;
	etmTime.tm_year = atoi( szBuf ) - 1900;

	etTime =  mktime( &etmTime );
	
	return ( etTime - stTime );	
}

/**********************************************************
function: GetSecondTime
description: 获取时间间隔
Input: szTime: 处理时间
Output:
Return: 秒数
others:
***********************************************************/

time_t GetSecondTime( char * szTime )
{
	struct tm tmTime;
	char szBuf[14 + 1];

	memset( &tmTime, 0, sizeof( struct tm ) );
	
	strncpy( szBuf, szTime + 12, 2 ); szBuf[2] = 0;
	tmTime.tm_sec = atoi( szBuf );
	
	strncpy( szBuf, szTime + 10, 2 ); szBuf[2] = 0;
	tmTime.tm_min = atoi( szBuf );
	
	strncpy( szBuf, szTime +  8, 2 ); szBuf[2] = 0;
	tmTime.tm_hour = atoi( szBuf );
	
	strncpy( szBuf, szTime +  6, 2 ); szBuf[2] = 0;
	tmTime.tm_mday = atoi( szBuf );

	strncpy( szBuf, szTime +  4, 2 ); szBuf[2] = 0;
	tmTime.tm_mon = atoi( szBuf ) - 1;

	strncpy( szBuf, szTime,   4    ); szBuf[4] = 0;
	tmTime.tm_year = atoi( szBuf ) - 1900;

	return ( mktime( &tmTime ) );
}

/**********************************************************
function: GetLastDay
description: 获取参照时间的上一天的时间
Input: szLastTime: 上一天
       szCurrTime: 参照时间
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/

int GetLastDay( char * szLastTime, char * szCurrTime )
{
	time_t tTime;
	struct tm * tmTime;
	struct tm otmTime;
	char szBuf[128];
	char szTime[14 + 1];
	int nLen;
	int nYear, nMon, nDay, nHour, nMin, nSec;

	memset( &otmTime, 0, sizeof( struct tm ) );	
	
	nLen = strlen( szCurrTime );
	
	if( nLen > 6 )
	{
		memcpy( szBuf, szCurrTime + 6, 2 ); szBuf[2] = 0;
		otmTime.tm_mday = atoi( szBuf );
	}
	else
	{
		otmTime.tm_mday = 1;
	}
	
	if( nLen > 4 )
	{
		memcpy( szBuf, szCurrTime + 4, 2 ); szBuf[2] = 0;
		otmTime.tm_mon = atoi( szBuf ) - 1;
	}
	
	memcpy( szBuf, szCurrTime, 4 ); szBuf[4] = 0;
	otmTime.tm_year = atoi( szBuf ) - 1900;

	tTime =  mktime( &otmTime );
	tTime -= 86400;
	tmTime = localtime( &tTime );

	sprintf( szTime, "%04d%02d%02d%02d%02d%02d",
							tmTime->tm_year + 1900, 
							tmTime->tm_mon + 1, 
							tmTime->tm_mday, 
							tmTime->tm_hour, 
							tmTime->tm_min, 
							tmTime->tm_sec);
	
	strcpy( szLastTime, szTime );

	return MY_SUCCEED;
}

/**********************************************************
function: GetNextDay
description: 获取参照时间的下一天的时间
Input: szNextDay: 下一天
       szCurrTime: 参照时间
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/

int GetNextDay( char * szNextDay, char * szCurrDate )
{
	time_t tTime;
	struct tm * tmTime;
	struct tm otmTime;
	char szBuf[14 + 1];
/*
	otmTime.tm_sec  = 0;
	otmTime.tm_min  = 0;
	otmTime.tm_hour = 1;
*/	
	memset( &otmTime, 0, sizeof( struct tm ) );

	strncpy( szBuf, szCurrDate + 6, 2 ); szBuf[2] = 0;
	otmTime.tm_mday = atoi( szBuf );

	strncpy( szBuf, szCurrDate + 4, 2 ); szBuf[2] = 0;
	otmTime.tm_mon = atoi( szBuf ) - 1;

	strncpy( szBuf, szCurrDate, 4 ); szBuf[4] = 0;
	otmTime.tm_year = atoi( szBuf ) - 1900;

	tTime =  mktime( &otmTime );
	tTime += 86400;
	tmTime = localtime( &tTime );

	sprintf( szNextDay, "%04d%02d%02d",
							tmTime->tm_year + 1900, 
							tmTime->tm_mon + 1, 
							tmTime->tm_mday );

	return MY_SUCCEED;
}

/**********************************************************
function: GetNextMonth
description: 获取参照时间的下一天的时间
Input: szNextMonth: 下一天
       szCurrTime: 参照时间
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/

int GetNextMonth( char * szNextMonth, char * szCurrDate )
{
	int nYear, nMon, nLastDay;
	char szYear[DATE_YEAR_LEN + 1];
	char szMonth[DATE_MON_LEN + 1];

	strncpy( szYear, szCurrDate, DATE_YEAR_LEN );
	szYear[DATE_YEAR_LEN] = 0;
	nYear = atoi( szYear );
	strncpy( szMonth, szCurrDate + DATE_YEAR_LEN, DATE_MON_LEN );
	szMonth[DATE_MON_LEN] = 0;
	nMon = atoi( szMonth );
	
	nMon++;
	
	if( nMon > 12 )
	{
		nMon = 1;
		nYear++;
	}
	
	sprintf( szNextMonth, "%04d%02d", nYear, nMon );
	
	return MY_SUCCEED;
}

/**********************************************************
function: GetMonthLastDay
description: 获取某一月的最后一天
Input: szLastDay: 最后一天
       szTime: 参照时间
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/

int GetMonthLastDay( char * szLastDay, char * szTime )
{
	int nYear, nMon, nLastDay;
	char szYear[DATE_YEAR_LEN + 1];
	char szMonth[DATE_MON_LEN + 1];

	strncpy( szYear, szTime, DATE_YEAR_LEN );
	szYear[DATE_YEAR_LEN] = 0;
	nYear = atoi( szYear );
	strncpy( szMonth, szTime + DATE_YEAR_LEN, DATE_MON_LEN );
	szMonth[DATE_MON_LEN] = 0;
	nMon = atoi( szMonth );
	
	if( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 )
	{
		nLastDay = 30;
	}
	else if( nMon == 2 )
	{
		if( IsLeapYear( nYear ) )
		{
			nLastDay = 29;
		}
		else
		{
			nLastDay = 28;
		}
	}
	else
	{
		nLastDay = 31;
	}
	
	sprintf( szLastDay, "%04d%02d%02d", nYear, nMon, nLastDay );
	
	return MY_SUCCEED;
}

/**********************************************************
function: FileIsExist
description: 判断文件是否存在
Input: szFile: 文件名
Output:
Return: MY_TRUE: 存在 MY_FALSE: 不存在
others:
***********************************************************/

int FileIsExist( char * szFile )
{
	if( 0 == access( szFile, F_OK ) )
	{
		return MY_TRUE;
	}
	else
	{
		return MY_FALSE;
	}
}

/**********************************************************
function: GetFileSize
description: 获取文件的大小
Input: szFile: 文件名
Output:
Return: 文件大小
others:
***********************************************************/

long GetFileSize( char * szFile )
{
	struct stat statBuf;
	
	lstat( szFile, &statBuf );
	
	return ( statBuf.st_size );
}

/**********************************************************
function: IsDir
description: 判断文件是否为目录文件
Input: szDirName: 文件名
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsDir( char * szDirName )
{
	struct stat statMode;

	if ( (lstat( szDirName, &statMode ) == 0) && (statMode.st_mode & S_IFDIR) )
	{
		return (MY_TRUE);
	}
	else
	{
		return (MY_FALSE);
	}
}

/**********************************************************
function: ChkLogPath
description: 检验日志文件路径有效性
Input: szPath: 日志文件目录
Output:
Return: MY_SUCCEED: 有效 MY_FAIL: 无效
others:
***********************************************************/

int ChkLogPath( char * szPath )
{
	FILE * fp;
	char szBuf[MAX_RECORD_LEN + 1];
	char szTime[14 + 1];
	
	if( !IsDir( szPath ) )
	{
		return MY_FAIL;
	}
	
	memset( szTime, 0, sizeof( szTime ) );
	GetCurrTime( szTime );
	
	if( 0 == *(szSrcDir) )
	{
		sprintf( szBuf, "%s/%s%8.8s.runlog", szLogPath, szLogPrefix, szTime );
	}
	else
	{
		sprintf( szBuf, "%s/%s%8.8s.%s.runlog", szLogPath, szLogPrefix, szTime, szSrcDir );
	}
	
	fp = fopen( szBuf, "a" );
	if( NULL == fp ) 
	{
		return MY_FAIL;
	}
	
	fclose( fp );
	
	return MY_SUCCEED;
}

/**********************************************************
function: BackGround
description: 程序转入后台运行
Input: 
Output:
Return: SUCCEED: 成功, 失败则程序退出
others:
***********************************************************/

int BackGround( void )
{
	pid_t pid;

	pid = fork();
	
	if ( 0 > pid )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "程序转入后台失败\n\n" );
		exit( 1 );
	}
	else if ( pid > 0 )
	{
		exit( 0 );
	}
	else
	{
		setsid();
		chdir( "/" );
		umask( 0 );
	}

	return MY_SUCCEED;
}

/**********************************************************
function: IsDate
description: 判断是否为标准日期
Input: szDate: 日期
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsDate( char * szDate )
{
	char szBuf[128];
	int nYear, nMon, nDay, nHour, nMin, nSec;
	int nLen;

	if( !IsNum( szDate ) )
	{
		return MY_FALSE;
	}
	
	nLen = strlen( szDate );

	if( DATE_STD_LEN != nLen )
	{
		return MY_FALSE;
	}
	
	memcpy( szBuf, szDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
	memcpy( szBuf, szDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );
	memcpy( szBuf, szDate +  6, 2 ); szBuf[2] = 0;	nDay  = atoi( szBuf );
	memcpy( szBuf, szDate +  8, 2 ); szBuf[2] = 0;	nHour = atoi( szBuf );
	memcpy( szBuf, szDate + 10, 2 ); szBuf[2] = 0;	nMin  = atoi( szBuf );
	memcpy( szBuf, szDate + 12, 2 ); szBuf[2] = 0;	nSec  = atoi( szBuf );

	if( nMon > 12 || nDay > 31 || nHour > 23 || nMin > 59 || nSec > 59 )
	{
		return MY_FALSE;
	}
	
	if( ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 ) && nDay > 30 )
	{
		return MY_FALSE;
	}

	if( nMon == 2 )
	{
		if( IsLeapYear( nYear ) )
		{
			return (nDay > 29) ? MY_FALSE : MY_TRUE;   
		}	
		else 
		{
			return (nDay > 28) ? MY_FALSE : MY_TRUE;
		}	
	}
	
	return MY_TRUE;
}

/**********************************************************
function: IsDate
description: 判断是否为精确到小时的日期
Input: szDate: 日期
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsHourDate( char * szDate )
{
	char szBuf[128];
	int nYear, nMon, nDay, nHour, nMin, nSec;
	int nLen;

	if( !IsNum( szDate ) )
	{
		return MY_FALSE;
	}
	
	nLen = strlen( szDate );

	if( ( DATE_YEAR_LEN + DATE_MON_LEN + DATE_DAY_LEN + DATE_HOUR_LEN ) != nLen )
	{
		return MY_FALSE;
	}
	
	memcpy( szBuf, szDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
	memcpy( szBuf, szDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );
	memcpy( szBuf, szDate +  6, 2 ); szBuf[2] = 0;	nDay  = atoi( szBuf );
	memcpy( szBuf, szDate +  8, 2 ); szBuf[2] = 0;	nHour = atoi( szBuf );

	if( nMon > 12 || nDay > 31 || nHour > 23 )
	{
		return MY_FALSE;
	}
	
	if( ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 ) && nDay > 30 )
	{
		return MY_FALSE;
	}

	if( nMon == 2 )
	{
		if( IsLeapYear( nYear ) )
		{
			return (nDay > 29) ? MY_FALSE : MY_TRUE;   
		}	
		else 
		{
			return (nDay > 28) ? MY_FALSE : MY_TRUE;
		}	
	}
	
	return MY_TRUE;
}

/**********************************************************
function: IsDate
description: 判断是否为精确到日的日期
Input: szDate: 日期
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsDayDate( char * szDayDate )
{
	char szBuf[128];
	int nYear, nMon, nDay;
	int nLen;

	if( !IsNum( szDayDate ) )
	{
		return MY_FALSE;
	}
	
	nLen = strlen( szDayDate );

	if( ( DATE_YEAR_LEN + DATE_MON_LEN + DATE_DAY_LEN ) != nLen )
	{
		return MY_FALSE;
	}
	
	memcpy( szBuf, szDayDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
	memcpy( szBuf, szDayDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );
	memcpy( szBuf, szDayDate +  6, 2 ); szBuf[2] = 0;	nDay  = atoi( szBuf );

	if( nMon > 12 || nDay > 31 )
	{
		return MY_FALSE;
	}
	
	if( ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 ) && nDay > 30 )
	{
		return MY_FALSE;
	}

	if( nMon == 2 )
	{
		if( IsLeapYear( nYear ) )
		{
			return (nDay > 29) ? MY_FALSE : MY_TRUE;   
		}	
		else 
		{
			return (nDay > 28) ? MY_FALSE : MY_TRUE;
		}	
	}
	
	return MY_TRUE;
}

/**********************************************************
function: IsDate
description: 判断是否为精确到月的日期
Input: szDate: 日期
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsMonthDate( char * IsMonthDate )
{
	char szBuf[128];
	int nYear, nMon;
	int nLen;

	if( !IsNum( IsMonthDate ) )
	{
		return MY_FALSE;
	}
	
	nLen = strlen( IsMonthDate );

	if( ( DATE_YEAR_LEN + DATE_MON_LEN ) != nLen )
	{
		return MY_FALSE;
	}
	
	memcpy( szBuf, IsMonthDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
	memcpy( szBuf, IsMonthDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );

	if( nMon > 12 )
	{
		return MY_FALSE;
	}
	
	return MY_TRUE;
}

/**********************************************************
function: GetInDirName
description: 获取路径的 BaseName
Input: szFullDir: 路径
Output: szDirName: BaseName
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetInDirName( char * szDirName, char * szFullDir )
{
	char szDir[MAX_PATH_LEN + 1];
	int nLen;
	
	strcpy( szDir, szFullDir );
	nLen = strlen( szDir );
	
	while( nLen )
	{
        if( '/' == szDir[nLen - 1] )
        {
            szDir[nLen - 1] = 0;
        }
        else
        {
            break;
        }

        nLen--;
	}
	
	strcpy( szDirName, strrchr( szDir, '/' ) + 1 );
	
	if( 0 == *( szDirName ) )
	{
		strcpy( szDirName, szFullDir );
	}
	
	return MY_SUCCEED ;
}

/**********************************************************
function: GetLoginInfo
description: 获取登陆口令文件中的用户名、密码
Input: szLoginFile: 登陆口令文件
Output: szUser: 用户名
		szPwD: 密码 
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetLoginInfo( char * szLoginFile, char * szUser, char * szPwd )
{
	FILE * fpLogin;
	char szKey[MAX_RECORD_LEN + 1];
    char szStr[MAX_RECORD_LEN + 1];
    int nLoop;

	if( NULL == ( fpLogin = fopen( szLoginFile, "r" ) ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "口令文件 %s 打开失败!\n", szLoginFile );
        MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
    strcpy( szKey, "DFJLT" );
    
    fgets( szUser, MAX_RECORD_LEN, fpLogin );
    fgets( szUser, MAX_RECORD_LEN, fpLogin );
    fgets( szPwd , MAX_RECORD_LEN, fpLogin );
    fgets( szPwd , MAX_RECORD_LEN, fpLogin );
 
    szUser[strlen(szUser) - 1] = 0;
    szPwd[strlen(szPwd) - 1] = 0;
   
    fclose( fpLogin );

    strcpy( szStr, szUser );

    for( nLoop = 0; nLoop < strlen( szStr ); nLoop++ )
    {
        szStr[nLoop] = szStr[nLoop] ^ szKey[nLoop%strlen(szKey)];
    }

	strcpy( szUser, szStr );
	
	strcpy( szStr, szPwd );

    for( nLoop = 0; nLoop < strlen( szStr ); nLoop++ )
    {
        szStr[nLoop] = szStr[nLoop] ^ szKey[nLoop%strlen(szKey)];
    }

	strcpy( szPwd, szStr );

	
    return MY_SUCCEED;
}

/**********************************************************
function: MoveFile
description: 移动文件
Input: szSrcFile: 源文件
	   szDestFile: 目标文件
Output: 
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/


int MoveFile( char * szSrcFile, char * szDstFile )
{
    if( -1 == rename( szSrcFile, szDstFile ) )
    {
        MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "移动文件 %s 至 %s 失败!\n", szSrcFile, szDstFile );
        MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**********************************************************
function: IsSameFileSystem
description: 判断两个目录是否在同一个文件系统
Input: szDir1: 目录1
	   szDir2: 目录2
Output: 
Return: MY_TRUE: 是 MY_FALSE: 否
others:
***********************************************************/

int IsSameFileSystem( char * szDir1, char * szDir2 )
{
	struct stat statMode1;
	struct stat statMode2;

	if( 0 != lstat( szDir1, &statMode1 ) || 0 != lstat( szDir2, &statMode2 ) )
	{
		return MY_FALSE;
	}
	
	
	if( statMode1.st_dev != statMode2.st_dev )
	{
		return MY_FALSE;
	}
	
	return MY_TRUE;
}

/**********************************************************
function: IsRecordLine
description: 判断记录是否为有效记录
Input: szRecord: 记录内容
Output: 
Return: MY_TRUE: 是 MY_FALSE: 否
others:
***********************************************************/

int IsRecordLine( char * szRecord )
{
	if( 0 == strncmp( szRecord, FILE_NOTE_FLAG, strlen( FILE_NOTE_FLAG ) ) )
	{
		return MY_FALSE;;
	}
		
	TrimLeft( TrimRight( szRecord ) );
		
	if( 0 == *( szRecord ) )
	{
		return MY_FALSE;;
	}

	return MY_TRUE;
}

/**********************************************************
function: GetTimeData
description: 分解时间字符串各子项, 保存在指定的时间结构体中
Input: szTime: 时间字符串
	   ptData; 时间结构体
Output: 
Return: MY_SUCCEED: 成功
others:
***********************************************************/

int GetTimeData( char * szTime, PTIME_DATA ptData )
{
	strncpy( ptData->szYear, szTime     , DATE_YEAR_LEN ); ptData->szYear[DATE_YEAR_LEN] = 0;
	strncpy( ptData->szMon , szTime +  4, DATE_MON_LEN  ); ptData->szMon[DATE_MON_LEN]   = 0;
	strncpy( ptData->szDay , szTime +  6, DATE_DAY_LEN  ); ptData->szDay[DATE_DAY_LEN]   = 0;
	strncpy( ptData->szHour, szTime +  8, DATE_HOUR_LEN ); ptData->szHour[DATE_HOUR_LEN] = 0;
	strncpy( ptData->szMin , szTime + 10, DATE_MIN_LEN  ); ptData->szMin[DATE_MIN_LEN]   = 0;
	strncpy( ptData->szSec , szTime + 12, DATE_SEC_LEN  ); ptData->szSec[DATE_SEC_LEN]   = 0;
	
	return MY_SUCCEED;
}
	
/**********************************************************
function: IsMobileHead
description: 判断记录是否为移动号码
Input: szRecord: 记录内容
Output: 
Return: MY_TRUE: 是 MY_FALSE: 否
others:
***********************************************************/

int IsMobileHead( char * szHead )
{
/*
	if( 0 != strncmp( szHead, MOBILE_HEAD_FLAG, strlen( MOBILE_HEAD_FLAG ) ) 
	&&  0 != strncmp( szHead, VIRTUL_HEAD_FLAG, strlen( VIRTUL_HEAD_FLAG ) ) )
	{
		return MY_FALSE;
	}
*/
	if( !IsNum( szHead ) ) {
		return MY_FALSE;
	}

	return MY_TRUE;
}

/**********************************************************
function: IsVirtulHead
description: 判断记录是否为虚拟号码
Input: szRecord: 记录内容
Output: 
Return: MY_TRUE: 是 MY_FALSE: 否
others:
***********************************************************/

int IsVirtulHead( char * szHead )
{
	if( 0 != strncmp( szHead, VIRTUL_HEAD_FLAG, strlen( VIRTUL_HEAD_FLAG ) ) )
	{
		return MY_FALSE;
	}

	return MY_TRUE;
}
