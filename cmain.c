

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name: cmain.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 当日详单存储系统调度
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
			modification: 修改内容
		 2. ... 
********************************************************************/


#include "cdef.h"


/********************************************************** 
function: DebugOutStoreCmd
description: 输出运行指令信息
Input:	psCmd: 描述运行指令各项内容的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutStoreCmd( PSTORE_CMD psCmd )
{
	char bRunFlag;
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, 
"当日详单存储系统运行参数内容如下\n\n\t=== Store Detail Run Info ===\n\n\
\tProcLog   = [%s]\n\tRunLog    = [%s]\n\tLogTmp    = [%s]\n\tInDir     = [%s]\n\
\tBakDir    = [%s]\n\tTmpBak    = [%s]\n\tOutDir    = [%s]\n\tTmpDir    = [%s]\n\
\tHostName  = [%s]\n\tLoginFile = [%s]\n\tServer    = [%s]\n\tCtrlFlag  = [%s]\n\
\tCfgFile   = [%s]\n\tInterval  = [%d]\n\n\t=== Store Detail Run Info ===\n\n",
				psCmd->szProcLog, psCmd->szRunLog, psCmd->szLogTmp, psCmd->szInDir, 
				psCmd->szBakDir, psCmd->szTmpBak, psCmd->szOutDir, psCmd->szTmpDir, 
				psCmd->szHostName, psCmd->szLoginFile, psCmd->szServer, 
				psCmd->szCtrlFlag, psCmd->szCfgFile, psCmd->nInterval );

	return MY_SUCCEED;
}

/********************************************************** 
function: CheckStoreCmd
description: 检验运行指令合法性
Input: psCmd: 描述运行指令各项内容的结构体
Output:
Return: MY_SUCCEED 合法 MY_FAIL 不合法
others:
***********************************************************/

int CheckStoreCmd( PSTORE_DATA psData )
{
	PSTORE_CMD psCmd;
	
	psCmd = &(psData->sCmd);
	
	/* 校验处理日志目录 */
	if( !IsDir( psCmd->szProcLog ) )
	{
		fprintf( stderr, "gen\texit\t无效的处理日志目录!\n" );
		return MY_FAIL;
	}
	
	/* 校验运行日志目录 */
	if( !IsDir( psCmd->szRunLog ) )
	{
		fprintf( stderr, "gen\texit\t无效的运行日志目录!\n" );
		return MY_FAIL;
	}
	
	/* 校验临时日志目录 */
	if( !IsDir( psCmd->szLogTmp ) )
	{
		fprintf( stderr, "gen\texit\t无效的临时日志目录!\n" );
		return MY_FAIL;
	}
	
	/* 校验入口目录 */
	if( !IsDir( psCmd->szInDir ) )
	{
		fprintf( stderr, "gen\texit\t无效的入口文件存放目录!\n" );
		return MY_FAIL;
	}
	
	/* 校验备份目录 */
	if( !IsDir( psCmd->szBakDir ) )
	{
		fprintf( stderr, "gen\texit\t无效的入口文件备份目录!\n" );
		return MY_FAIL;
	}
	
	/* 校验临时备份目录 */
	if( !IsDir( psCmd->szTmpBak ) )
	{
		fprintf( stderr, "gen\texit\t无效的文件临时备份目录!\n" );
		return MY_FAIL;
	}
	
	/* 校验出口目录 */
	if( !IsDir( psCmd->szOutDir ) )
	{
		fprintf( stderr, "gen\texit\t无效的出口文件输出目录!\n" );
		return MY_FAIL;
	}
	
	/* 校验出口临时目录 */
	if( !IsDir( psCmd->szTmpDir ) )
	{
		fprintf( stderr, "gen\texit\t无效的出口临时文件存放目录!\n" );
		return MY_FAIL;
	}
	
	/* 获取数据库登陆用户名、口令 */
	if( MY_SUCCEED != GetLoginInfo( psCmd->szLoginFile, psCmd->szUserName, psCmd->szPassWord ) )
	{
		return MY_FAIL;
	}
	
	/* 初始化日志生成时间间隔 */
	if( 0 >= psCmd->nInterval )
	{
		psCmd->nInterval = 900;	
		psData->bSingleMode = MY_TRUE;
	}
	else
	{
		psCmd->nInterval = ( psCmd->nInterval + 60 - 1 ) / 60 * 60;
		psData->bSingleMode = MY_FALSE;
	}
	
	/* 检验处理日志与日志临时目录是否在同一个文件系统 */
	if( !IsSameFileSystem( psCmd->szProcLog, psCmd->szLogTmp ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "处理日志目录和日志临时日志目录不在同一个文件系统!\n\n" );
		return MY_FAIL;
	}
	
	/* 检验运行日志与日志临时目录是否在同一个文件系统 */
	if( !IsSameFileSystem( psCmd->szRunLog, psCmd->szLogTmp ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "运行日志目录和日志临时日志目录不在同一个文件系统!\n\n" );
		return MY_FAIL;
	}
	
	/* 检验入口与备份目录是否在同一个文件系统 */
	if( !IsSameFileSystem( psCmd->szInDir, psCmd->szBakDir ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件入口目录和入口文件备份目录不在同一个文件系统!\n\n" );
		return MY_FAIL;
	}
	
	/* 检验入口与临时备份目录是否在同一个文件系统 */
	if( !IsSameFileSystem( psCmd->szInDir, psCmd->szTmpBak ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件入口目录和入口文件临时备份目录不在同一个文件系统!\n\n" );
		return MY_FAIL;
	}
	
	/* 检验出口与出口临时目录是否在同一个文件系统 */
	if( !IsSameFileSystem( psCmd->szOutDir , psCmd->szTmpDir ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件出口目录和出口临时日志目录不在同一个文件系统!\n\n" );
		return MY_FAIL;
	}

	/* 获取详单格式描述信息文件 */
	if( MY_SUCCEED != GetIniInfo( psCmd->szCfgFile, "OPERINFO", psCmd->szOperFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单格式描述信息文件失败!\n" );
		return MY_FAIL;
	}

#ifdef	__INCREMENT_FILE__	
	/* 获取增量统计字段配置信息文件 */
	if( MY_SUCCEED != GetIniInfo( psCmd->szCfgFile, "FMTCTRL" , psCmd->szFmtCtrl ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取增量统计字段配置信息文件失败!\n" );
		return MY_FAIL;
	}
#endif
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetStoreCmd
description: 获取运行指令信息
Input: psData: 描述系统全局结构的结构体
       argc: 运行指令个数
       argv: 保存运行指令内容
Output:
Return: 0 MY_SUCCEED 1 MY_FAIL
others:
***********************************************************/

int GetStoreCmd( PSTORE_DATA psData, int argc, char * argv[] )
{
	PSTORE_CMD psCmd;
	extern char * optarg;
	int opt, pflag, rflag, mflag, iflag, bflag, kflag, oflag, tflag, hflag, lflag, sflag, cflag, fflag, vflag;

	psCmd = &(psData->sCmd);
	
	pflag = 0;
	rflag = 0;
	mflag = 0;
	iflag = 0;
	bflag = 0;
	kflag = 0;
	oflag = 0;
	tflag = 0;
	hflag = 0;
	lflag = 0;
	sflag = 0;
	cflag = 0;
	fflag = 0;
	vflag = 0;
	
	/* 获取运行程序名称 */
	memset( szBinName, 0, sizeof( szBinName ) );
	GetInDirName( szBinName, argv[0] );
	
	/* 获取运行命令 */
	while( -1 != ( opt = getopt( argc, argv, "P:p:R:r:M:m:I:i:B:b:K:k:O:o:T:t:H:h:L:l:S:s:C:c:F:f:V:v:" ) ) )
	{
		/* 分析运行命令 */
		switch( opt )
		{
			case 'P':
			case 'p':
				pflag = 1;
				strcpy( psCmd->szProcLog, optarg );
				break;
				
			case 'R':
			case 'r':
				rflag = 1;
				strcpy( psCmd->szRunLog, optarg );

				strcpy( szLogPath, optarg );
				strcpy( szLogPrefix, "wfs" );
				break;
			
			case 'M':
			case 'm':
				mflag = 1;
				strcpy( psCmd->szLogTmp, optarg );
				break;
				
			case 'I':
			case 'i':
				iflag = 1;
				strcpy( psCmd->szInDir, optarg );
				
				memset( szSrcDir, 0, sizeof( szSrcDir ) );
				GetInDirName( szSrcDir, psCmd->szInDir );
				break;
				
			case 'B':
			case 'b':
				bflag = 1;
				strcpy( psCmd->szBakDir, optarg );
				break;
				
			case 'K':
			case 'k':
				kflag = 1;
				strcpy( psCmd->szTmpBak, optarg );
				break;
				
			case 'O':
			case 'o':
				oflag = 1;
				strcpy( psCmd->szOutDir, optarg );
				break;
					
			case 'T':
			case 't':
				tflag = 1;
				strcpy( psCmd->szTmpDir, optarg );
				break;
			
			case 'H':
			case 'h':
				hflag = 1;
				strcpy( psCmd->szHostName, optarg );
				break;
				
			case 'L':
			case 'l':
				lflag = 1;
				strcpy( psCmd->szLoginFile, optarg );
				break;
				
			case 'S':
			case 's':
				sflag = 1;
				strcpy( psCmd->szServer, optarg );
				break;
				
			case 'C':
			case 'c':
				cflag = 1;
				strcpy( psCmd->szCtrlFlag, optarg );
				break;
			
			case 'F':
			case 'f':
				fflag = 1;
				strcpy( psCmd->szCfgFile, optarg );
				break;
					
			case 'V':
			case 'v':
				vflag = 1;
				psCmd->nInterval = atoi( optarg );
				break;
				
			default:
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "无效的当日详单存储系统运行参数 %c !\n", opt );
				break;
		}
	}
	
	/* 参数有效性检测 */
	if( !( pflag && rflag && mflag && iflag && bflag && kflag && oflag && tflag && hflag && lflag && sflag && cflag && fflag && vflag ) )
	{
		fprintf( stderr, "\n\n\t[usage]: %s -p Proclog  -r Runlog    -m logtMp -i Indir    -b Bakdir  -k tmpbaK  -o Outdir -t Tmpdir\n", szBinName );
		fprintf( stderr, "\t                 -h Hostname -l Loginfile -s Server -c Ctrlflag -f cfgFile -v interVal\n\n");
   		fprintf( stderr, "\t当日详单存储系统运行参数不足!\n\n" );
   		exit(0);
	}
	
	/* 检查运行指令合法性 */
	if( MY_SUCCEED != CheckStoreCmd( psData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "无效的详单存储系统运行参数!\n\n" );
		exit(0);
	}

	DebugOutStoreCmd( psCmd );
	
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
	int  nProcCtrlFlag ;

	memset( &sData, 0, sizeof( STORE_DATA ) );
	memset( &(sData.sCmd), 0, sizeof( STORE_CMD ) );

	/* 获取运行指令 */
	GetStoreCmd( &sData, argc, argv );
	
	strcpy( sData.szHostName, sData.sCmd.szHostName );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "启动当日详单文件存储系统!\n\n" );
	
	/* 连接数据库 */
	ConnectDataBase( (sData.sCmd).szUserName, (sData.sCmd).szPassWord, (sData.sCmd).szServer );
	
	/* 获取系统运行控制标志 */
	if( PROC_CTRL_RUN != GetProcCtrlFlag( sData.sCmd.szCtrlFlag ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "系统运行标志设置为禁止运行, 系统停止运行!\n\n" );
		DisConnectDataBase( );
		FreeMemoryData( &sData );
		
		exit(0);
	}
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取系统配置信息!\n\n" );
	
	/* 获取系统配置信息 */
	if( MY_SUCCEED != GetConfigInfo( &sData ) )
	{
		DisConnectDataBase( );
		FreeMemoryData( &sData );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "获取系统配置信息失败!\n\n" );
	}
	
	tmStartTime = time( NULL );
	tmStartTime = tmStartTime - tmStartTime % (sData.sCmd).nInterval + (sData.sCmd).nInterval;
	
	/* 获取当前批次 */
    GetCurrBatch( &sData );

	InitGroupHead( &sData );
	
	/* 恢复系统运行状态 */
	if( MY_SUCCEED != ResumeSystemEnv( &sData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "恢复系统运行状态失败!\n\n" );
		DisConnectDataBase( );
		FreeMemoryData( &sData );
		exit(0);
	}

	/* 打开当前批次的处理日志、错单日志 */
	if( MY_SUCCEED != OpenLogFile( &sData ) )
	{
		DisConnectDataBase( );
		FreeMemoryData( &sData );
		exit(0);
	}

	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "存储 [%s] 目录下的详单文件!\n\n", sData.sCmd.szInDir );
	
	/*while( PROC_CTRL_RUN == GetProcCtrlFlag( sData.sCmd.szCtrlFlag ) )*/
	for( ;; )
	{
		nProcCtrlFlag = GetProcCtrlFlag( sData.sCmd.szCtrlFlag ) ;
		if ( nProcCtrlFlag == PROC_CTRL_EXIT ) {
			break ;
		} else if ( nProcCtrlFlag == PROC_CTRL_RELOAD ) {
			FreeConfigInfo( &sData ) ;
			if ( ReloadConfigInfo( &sData ) != MY_SUCCEED ) {
				DisConnectDataBase( );
				FreeMemoryData( &sData );
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "获取系统配置信息失败!\n\n" );
				return -1 ;
			}
			UpdateSysProc( sData.sCmd.szCtrlFlag, PROC_CTRL_RUN ) ;
		}

		/* 处理入口目录下的文件 */

		if( MY_SUCCEED != StoreDetailFile( &sData ) )
		{
			DisConnectDataBase( );
			FreeMemoryData( &sData );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "存储详单文件 %s 失败!\n\n", sData.szFileName );
		}

		tmEndTime = time( NULL );
		tmEndTime = tmEndTime - tmEndTime % (sData.sCmd).nInterval + (sData.sCmd).nInterval;
		
		if( tmEndTime > tmStartTime )
 		{
 		
 		#ifdef	__INCREMENT_FILE__	
 			if( MY_SUCCEED != DumpIncData( &sData ) )
 			{
 				DisConnectDataBase( );
				FreeMemoryData( &sData );
				exit(0);
 			}
 		#endif
 			
  			tmStartTime = tmEndTime;

			/* 获取当前批次 
			GetCurrBatch( &sData );

			打开当前批次的处理日志、错单日志 
			if( MY_SUCCEED != OpenLogFile( &sData ) )
			{
				DisConnectDataBase( );
				FreeMemoryData( &sData );
				exit(0);
			}
			*/
			if( MY_SUCCEED != AlterLogFile( &sData ) )
			{                                         
				DisConnectDataBase( );                  
				FreeMemoryData( &sData );               
				exit(0);                                
			}                                         

		}
		
		sleep( 1 );
	
		/* break; */
	}
	
	/* 断开数据库连接 */
	DisConnectDataBase( );
	
	/* 释放系统申请的内存空间 */
	FreeMemoryData( &sData );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "当日详单文件存储系统运行完毕, 成功退出!\n\n" );
	
	return 0;
}
