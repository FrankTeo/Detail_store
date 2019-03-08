

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name: amain.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 往月详单归档、压缩系统调度
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
			modification: 修改内容
		 2. ... 
********************************************************************/


#include "cdef.h"


/********************************************************** 
function: DebugOutArchiveCmd
description: 输出运行指令信息
Input:	paCmd: 描述运行指令各项内容的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutArchiveCmd( PARCHIVE_CMD paCmd )
{
	char bRunFlag;
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, 
"往日详单规整系统运行参数内容如下\n\n\
\t=== Archive Detail Run Info ===\n\n\
\tRunLog     = [%s]\n\
\tArchMonth  = [%s]\n\
\tFileSystem = [%s]\n\n\
\t=== Archive Detail Run Info ===\n\n",
				paCmd->szRunLog, paCmd->szArchMonth, paCmd->szFileSystem );

	return MY_SUCCEED;
}

/********************************************************** 
function: CheckArchiveCmd
description: 检验运行指令合法性
Input: paCmd: 描述运行指令各项内容的结构体
Output:
Return: MY_SUCCEED 合法 MY_FAIL 不合法
others:
***********************************************************/

int CheckArchiveCmd( PSTORE_DATA psData )
{
	PARCHIVE_CMD paCmd;
	
	paCmd = &(psData->aCmd);
	
	/* 校验处理日志目录 */
	if( !IsDir( paCmd->szRunLog ) )
	{
		fprintf( stderr, "gen\texit\t无效的运行日志目录!\n" );
		return MY_FAIL;
	}
	
	if( !IsMonthDate( paCmd->szArchMonth ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "规整时间 %s 格式[YYYYMM]错误 !\n", paCmd->szArchMonth );
		MY_FAIL;
	}
	
	/* 获取数据库登陆用户名、口令 */
	if( !IsDir( paCmd->szFileSystem ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单存储文件系统 %s 无效!\n", paCmd->szFileSystem );
		MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetArchiveCmd
description: 获取运行指令信息
Input: psData: 描述系统全局结构的结构体
       argc: 运行指令个数
       argv: 保存运行指令内容
Output:
Return: 0 MY_SUCCEED 1 MY_FAIL
others:
***********************************************************/

int GetArchiveCmd( PSTORE_DATA psData, int argc, char * argv[] )
{
	PARCHIVE_CMD paCmd;
	extern char * optarg;
	int opt, rflag, mflag, fflag;

	paCmd = &(psData->aCmd);
	
	rflag = 0;
	mflag = 0;
	fflag = 0;
	
	/* 获取运行程序名称 */
	memset( szBinName, 0, sizeof( szBinName ) );
	GetInDirName( szBinName, argv[0] );
	
	/* 获取运行命令 */
	while( -1 != ( opt = getopt( argc, argv, "R:r:M:m:F:f:" ) ) )
	{
		/* 分析运行命令 */
		switch( opt )
		{
			case 'R':
			case 'r':
				rflag = 1;
				strcpy( paCmd->szRunLog, optarg );

				strcpy( szLogPath, optarg );
				strcpy( szLogPrefix, "war" );
				break;
				
			case 'M':
			case 'm':
				mflag = 1;
				strcpy( paCmd->szArchMonth, optarg );
				break;
				
			case 'F':
			case 'f':
				fflag = 1;
				strcpy( paCmd->szFileSystem, optarg );
				break;

			default:
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "无效的往月详单归档、压缩系统运行参数 %c !\n", opt );
				break;
		}
	}

	/* 参数有效性检测 */
	if( !( rflag && mflag && fflag ) )
	{
		fprintf( stderr, "\n\n\t[usage]: %s -r Runlog -m archMonth -f Filesystem\n\n", szBinName );
   		fprintf( stderr, "\t往月详单归档、压缩系统运行参数不足!\n\n" );
   		exit(0);
	}
	
	/* 检查运行指令合法性 */
	if( MY_SUCCEED != CheckArchiveCmd( psData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "无效的往月详单归档、压缩系统运行参数!\n\n" );
		exit(0);
	}

	DebugOutArchiveCmd( paCmd );
	
	return MY_SUCCEED;
}

/********************************************************** 
function: FreeArchiveMemory
description: 释放系统内存空间
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeArchiveMemory( PSTORE_DATA psData )
{
	FreeMonthMemory( psData );
	
	if( NULL != psData->szArchDetail )
	{
		free( psData->szArchDetail );
		psData->szArchDetail = NULL;
	}
		
	if( NULL != psData->szCompDetail )
	{
		free( psData->szCompDetail );
		psData->szCompDetail = NULL;
	}
	
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
	memset( &(sData.aCmd), 0, sizeof( ARCHIVE_CMD ) );
	
	/* 获取运行指令 */
	GetArchiveCmd( &sData, argc, argv );

	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "启动往月详单归档、压缩系统!\n\n" );

	/* 详单归档、压缩指定月的详单存储文件 */
	if( MY_SUCCEED != ArchiveMonthDetail( &sData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往月详单归档、压缩失败!\n\n" );
		FreeArchiveMemory( &sData );
		exit(0);
	}

	/* 释放系统内存空间 */
	FreeArchiveMemory( &sData );

	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "往月详单归档、压缩系统运行完毕, 成功退出!\n\n" );
	
	return 0;
}
