

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name: rmain.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 往日详单规整系统调度
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
			modification: 修改内容
		 2. ... 
********************************************************************/


#include "cdef.h"


/********************************************************** 
function: DebugOutBuildCmd
description: 输出运行指令信息
Input:	prCmd: 描述运行指令各项内容的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutBuildCmd( PREBUILD_CMD prCmd )
{
	char bRunFlag;
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, 
"往日详单规整系统运行参数内容如下\n\n\
\t=== Build Detail Run Info ===\n\n\
\tRunLog    = [%s]\n\
\tCfgFile   = [%s]\n\
\tBuildDate = [%s]\n\
\tBuildKey  = [%s]\n\
\tHostName  = [%s]\n\
\tLoginFile = [%s]\n\
\tServer    = [%s]\n\n\
\t=== Build Detail Run Info ===\n\n",
				prCmd->szRunLog, prCmd->szCfgFile, prCmd->szBuildDate, prCmd->szBuildKey,
				prCmd->szHostName, prCmd->szLoginFile, prCmd->szServer );

	return MY_SUCCEED;
}

/********************************************************** 
function: CheckBuildCmd
description: 检验运行指令合法性
Input: prCmd: 描述运行指令各项内容的结构体
Output:
Return: MY_SUCCEED 合法 MY_FAIL 不合法
others:
***********************************************************/

int CheckBuildCmd( PSTORE_DATA psData )
{
	PREBUILD_CMD prCmd;
	
	prCmd = &(psData->rCmd);
	
	/* 校验处理日志目录 */
	if( !IsDir( prCmd->szRunLog ) )
	{
		fprintf( stderr, "gen\texit\t无效的运行日志目录!\n" );
		return MY_FAIL;
	}
	
	if( !IsDayDate( prCmd->szBuildDate ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "规整时间 %s 格式[YYYYMMDD]错误 !\n", prCmd->szBuildDate );
		MY_FAIL;
	}
	
	/* 获取数据库登陆用户名、口令 */
	if( MY_SUCCEED != GetLoginInfo( prCmd->szLoginFile, prCmd->szUserName, prCmd->szPassWord ) )
	{
		return MY_FAIL;
	}
	
	/* 获取详单格式描述信息文件 */
	if( MY_SUCCEED != GetIniInfo( prCmd->szCfgFile, "OPERINFO", prCmd->szOperFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单格式描述信息文件失败!\n" );
		return MY_FAIL;
	}

	return MY_SUCCEED;
}

/********************************************************** 
function: GetBuildCmd
description: 获取运行指令信息
Input: psData: 描述系统全局结构的结构体
       argc: 运行指令个数
       argv: 保存运行指令内容
Output:
Return: 0 MY_SUCCEED 1 MY_FAIL
others:
***********************************************************/

int GetBuildCmd( PSTORE_DATA psData, int argc, char * argv[] )
{
	PREBUILD_CMD prCmd;
	extern char * optarg;
	int opt, rflag, cflag, dflag, kflag, hflag, lflag, sflag;

	prCmd = &(psData->rCmd);
	
	rflag = 0;
	cflag = 0;
	dflag = 0;
	kflag = 0;
	hflag = 0;
	lflag = 0;
	sflag = 0;
	
	/* 获取运行程序名称 */
	memset( szBinName, 0, sizeof( szBinName ) );
	GetInDirName( szBinName, argv[0] );
	
	/* 获取运行命令 */
	while( -1 != ( opt = getopt( argc, argv, "R:r:C:c:D:d:K:k:H:h:L:l:S:s:" ) ) )
	{
		/* 分析运行命令 */
		switch( opt )
		{
			case 'R':
			case 'r':
				rflag = 1;
				strcpy( prCmd->szRunLog, optarg );

				strcpy( szLogPath, optarg );
				strcpy( szLogPrefix, "wrb" );
				break;
				
			case 'C':
			case 'c':
				cflag = 1;
				strcpy( prCmd->szCfgFile, optarg );
				break;
				
			case 'D':
			case 'd':
				dflag = 1;
				strcpy( prCmd->szBuildDate, optarg );
				break;

			case 'K':
			case 'k':
				kflag = 1;
				strcpy( prCmd->szBuildKey, optarg );
				break;
					
			case 'H':
			case 'h':
				hflag = 1;
				strcpy( prCmd->szHostName, optarg );
				break;
				
			case 'L':
			case 'l':
				lflag = 1;
				strcpy( prCmd->szLoginFile, optarg );
				break;
				
			case 'S':
			case 's':
				sflag = 1;
				strcpy( prCmd->szServer, optarg );
				break;

			default:
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "无效的往日详单规整系统运行参数 %c !\n", opt );
				break;
		}
	}

	/* 参数有效性检测 */
	if( !( rflag && cflag && dflag && kflag && hflag && lflag && sflag ) )
	{
		fprintf( stderr, "\n\n\t[usage]: %s -r Runlog -c Cfgfile -d buildDate -k buildKey -h Hostname -l Loginfile -s Server\n\n", szBinName );
   		fprintf( stderr, "\t往日详单规整系统运行参数不足!\n\n" );
   		exit(0);
	}
	
	/* 检查运行指令合法性 */
	if( MY_SUCCEED != CheckBuildCmd( psData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "无效的往日详单规整系统运行参数!\n\n" );
		exit(0);
	}

	DebugOutBuildCmd( prCmd );
	
	return MY_SUCCEED;
}

/**********************************************************
function: main
description: 主控函数
Input: argc: 参数数目
       argv: 参数值
Output:
Return:
others:
***********************************************************/

int main( int argc, char * argv[ ] )
{
	STORE_DATA sData;
	
	memset( &sData, 0, sizeof( STORE_DATA ) );
	memset( &(sData.rCmd), 0, sizeof( REBUILD_CMD ) );

	/* 获取运行指令 */
	GetBuildCmd( &sData, argc, argv );
	
	strcpy( sData.szHostName, sData.rCmd.szHostName );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "启动往日详单存储文件规整系统!\n\n" );
	
	/* 连接数据库 */
	ConnectDataBase( (sData.rCmd).szUserName, (sData.rCmd).szPassWord, (sData.rCmd).szServer );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取系统配置信息!\n\n" );
	
	/* 获取详单存储路径信息 */
	if( MY_SUCCEED != GetStoreControlInfo( &sData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单存储路径信息失败!\n" );
		DisConnectDataBase( );
		FreeMemoryData( &sData );
		return MY_FAIL;
	}
	
	/* 获取详单格式描述信息 */
	if( MY_SUCCEED != GetOperInfo( &sData, sData.rCmd.szOperFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单格式描述信息失败!\n\n" );
		DisConnectDataBase( );
		FreeMemoryData( &sData );
		exit(0);
	}
	
	/* 规整指定的当日详单存储文件 */
	if( MY_SUCCEED != RebuildDetailFile( &sData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往日详单存储文件规整失败!\n\n" );
		DisConnectDataBase( );
		FreeMemoryData( &sData );
		exit(0);
	}

	/* 断开数据库连接 */
	DisConnectDataBase( );
	
	/* 释放系统申请的内存空间 */
	FreeMemoryData( &sData );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "往日详单存储文件规整系统运行完毕, 成功退出!\n\n" );
	
	return 0;
}
