

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
file_name: sdetail.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 存储详单记录至当日详单存储文件
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
            modification: 修改内容
         2. ...
********************************************************************/


#include "cdef.h"


/**********************************************************
function: IsMatchOperRecord
description: 详单配置信息是否符合当前记录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsMatchOperRecord( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	
	poRecord = psData->poRecord;

	/* 比较文件头 */
	/*printf( "[%d][%s][%s][%2.2s][%s]\n", __LINE__,poRecord->szFileHead,psData->szFileName,psData->szRecord,poRecord->szSysType ) ;*/
	if( 0 != strcmp( poRecord->szFileHead, NOT_CHECK_FILEHEAD ) )
	{
		if( 0 != strncmp( psData->szFileName, poRecord->szFileHead, strlen( poRecord->szFileHead ) ) )
		{
			return MY_FALSE;
		}
	}
	
	/* 比较系统类型 */
	if( 0 != strcmp( poRecord->szSysType, NOT_CHECK_SYSTYPE ) )
	{
		if( 0 != strncmp( psData->szRecord, poRecord->szSysType, strlen( poRecord->szSysType ) ) )
		{
			return MY_FALSE;
		}
	}
	
	return MY_TRUE;
}

/**********************************************************
function: GetRecordType
description: 获取详单记录配置信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetRecordType( PSTORE_DATA psData )
{
	int nLoop;
	
	/* 匹配上次匹配的记录 */
	/*
	if( NULL != psData->poRecord )
	{
		if( IsMatchOperRecord( psData ) )
		{
			return MY_SUCCEED;
		}
	}
	*/
	
	/* 遍历详单格式描述信息文件 */
	for( nLoop = 0; nLoop < psData->oInfo.nOperCount; nLoop++ )
	{
		psData->poRecord = psData->oInfo.poRecord + nLoop;
		
		if( IsMatchOperRecord( psData ) )
		{
			return MY_SUCCEED;
		}
	}
	
	return MY_FAIL;
}

/**********************************************************
function: GetStoreInfo
description: 获取详单存储位置信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/
int GetStoreInfo( PSTORE_DATA psData )
{
	PSTORE_HEAD psHead ;
	PSTORE_CONTROL psControl; 
	char szHeadFlag[HEAD_FLAG_LEN+1];
	POPER_RECORD poRecord;
	int nLoop = 0;

	poRecord = psData->poRecord;

	if( STORE_MODE_MOBILE == poRecord->nStoreMode && IsMobileHead( psData->szSubscriber ) )
	{
		/* 校验手机号码长度 */
		/*
		if( MOBILE_NUMBER_LEN != strlen( psData->szSubscriber ) )
		{
			return MY_FAIL;
		}
		*/
		
		memset( szHeadFlag, 0, sizeof(szHeadFlag) ) ;
		strncpy(szHeadFlag, psData->szSubscriber, HEAD_FLAG_LEN ) ;
		szHeadFlag[HEAD_FLAG_LEN] = 0 ;
		
		for( nLoop = 0; nLoop < MAX_HEAD_NUM; nLoop++ ) {
			/*psHead = psData->psHead[nLoop] ;*/
			if( strcmp(psData->psHead[nLoop]->szHeadFlag, szHeadFlag ) == 0 ) {
				break ;
			}
		}

		if( NULL == psData->psHead[nLoop]->psControl[atoi(psData->szBaseAddr + 2 )] )
		{
			return MY_FAIL;
		}
		
		psData->psCtrl = psData->psHead[nLoop]->psControl[atoi(psData->szBaseAddr + 2 )];
	}
	else
	{
		psControl = psData->pscFixed;
		
		while( NULL != psControl )
		{
			if( atoi( psControl->szStoreMode ) == poRecord->nStoreMode )
			{
				if( STORE_MODE_MODULUS == poRecord->nStoreMode )
				{
					if( psControl->nBeginKey <= psData->nDetachNo 
					&&  psControl->nEndKey >= psData->nDetachNo )
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
				
			psControl = psControl->pscNext;
		}
		
		if( NULL == psControl )
		{
			return MY_FAIL;
		}

		psData->psCtrl = psControl;
	}

	return MY_SUCCEED;
}

/**********************************************************
function: GetRecordInfo
description: 获取详单记录信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetRecordInfo( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	
	/* 获取详单记录配置信息 */
	if( MY_SUCCEED != GetRecordType( psData ) )
	{
		/*strcpy( psData->szErrCode, ERROR_CODE_CFGREC );*/
		sprintf( psData->szErrCode, "%s%2.2s%s", ERROR_CODE_PREFIX, psData->szRecord, ERROR_CODE_CFGREC ) ;
		return MY_SUCCEED;
	}

	/* 校验详单记录长度 */
	if( strlen( psData->szRecord ) != psData->poRecord->nRecLen )
	{
		/*strcpy( psData->szErrCode, ERROR_CODE_RECLEN );*/
		sprintf( psData->szErrCode, "%s%2.2s%s", ERROR_CODE_PREFIX, psData->szRecord, ERROR_CODE_RECLEN ) ;
		return MY_SUCCEED;
	}
	poRecord = psData->poRecord;
	
	memset( psData->szSubscriber, 0, sizeof( psData->szSubscriber ) );
	memset( psData->szRecordTime, 0, sizeof( psData->szRecordTime ) );
	
	/* 提取用户号码 */
	strncpy( psData->szSubscriber, psData->szRecord + poRecord->nUserLoc, poRecord->nUserLen );
	psData->szSubscriber[poRecord->nUserLen] = 0;
	TrimLeft( TrimRight( psData->szSubscriber ) );
	
	/* 提取记录时间 */
	strncpy( psData->szRecordTime, psData->szRecord + poRecord->nTimeLoc, poRecord->nTimeLen );
	psData->szRecordTime[poRecord->nTimeLen] = 0;
	TrimLeft( TrimRight( psData->szRecordTime ) );
	
	/* 用户号码为空 */
	if( 0 == *( psData->szSubscriber ) )
	{
		/*strcpy( psData->szErrCode, ERROR_CODE_IDNULL );*/
		sprintf( psData->szErrCode, "%s%2.2s%s", ERROR_CODE_PREFIX, psData->szRecord, ERROR_CODE_IDNULL ) ;
		return MY_SUCCEED;
	}

	/* 获取用户号码基址、偏移 */
	GetBaseOffset( psData );
	
	/* 获取详单记录存储位置信息 */
	if( MY_SUCCEED != GetStoreInfo( psData ) )
	{
		/*strcpy( psData->szErrCode, ERROR_CODE_STOCTL );*/
		sprintf( psData->szErrCode, "%s%2.2s%s", ERROR_CODE_PREFIX, psData->szRecord, ERROR_CODE_STOCTL ) ;
		return MY_SUCCEED;
	}

	/* 打开详单存储文件 */
	if( MY_SUCCEED != OpenStoreFile( psData ) )
	{
		return MY_FAIL;
	}

#ifdef	__INCREMENT_FILE__	
	/* 累计增量统计信息 */
	if( MY_SUCCEED != AddupIncData( psData ) )
	{
		return MY_FAIL;
	}
#endif
	
	return MY_SUCCEED;
}

/**********************************************************
function: CheckDetailStoreFile
description: 检测断点记录所在详单存储文件内容
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int CheckDetailStoreFile( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;

	char szRecord[MAX_RECORD_LEN + 1];
	char szBreakRecord[MAX_RECORD_LEN + 1];
	long lFileSize;
	long lOffset;
	
	lFileSize = GetFileSize( psData->szStoreFile );
	
	if( lFileSize < strlen( psData->tInfo.szBreakRecord ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "断点详单记录尚未存储在详单存储文件中!\n" );
		
		psData->bResumeEnv = MY_FALSE;
		psData->bResumeInc = MY_FALSE;
		
		return MY_SUCCEED;
	}
	
	lOffset = strlen( psData->tInfo.szBreakRecord ) + FILE_NAME_LEN;

	if( -1 == lseek( psData->nStoreHandle, -lOffset, SEEK_END ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位断点详单记录存储位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			
		return MY_FAIL;
	}
	
	memset( szRecord, 0, sizeof( szRecord ) );
	memset( szBreakRecord, 0, sizeof( szBreakRecord ) );
	
	if( lOffset != read( psData->nStoreHandle, szRecord, lOffset ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "读取断点详单记录存储位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			
		return MY_FAIL;
	}
	
	poRecord = psData->poRecord;

	sprintf( szBreakRecord, "%*.*s%-*.*s\n", 
										strlen( psData->tInfo.szBreakRecord ) - 1, 
										strlen( psData->tInfo.szBreakRecord ) - 1, 
										psData->tInfo.szBreakRecord,
										FILE_NAME_LEN, FILE_NAME_LEN, psData->szFileName );
	memcpy( szBreakRecord + poRecord->nDealTimeLoc, psData->szCurrBatch + poRecord->nDealTimeOff, poRecord->nDealTimeLen );
	

	if( 0 == strcmp( szRecord, szBreakRecord ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "断点详单记录已存储在详单存储文件中!\n" );
	}
	else
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "断点详单记录尚未存储在详单存储文件中!\n" );
		psData->bResumeEnv = MY_FALSE;
		psData->bResumeInc = MY_FALSE;
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: StoreDetailRecord
description: 存储指定文件中的详单记录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int StoreDetailRecord( PSTORE_DATA psData )
{
	char szDetailFile[MAX_PATH_LEN + 1];

	/* 关闭上次打开的详单文件 */
	if( NULL != psData->fpDetail )
	{
		fclose( psData->fpDetail );
		psData->fpDetail = NULL;
	}
	
	memset( szDetailFile, 0, sizeof( szDetailFile ) );
	
	if( psData->bResumeInc )
	{
		sprintf( szDetailFile, "%s/%s.%s", psData->sCmd.szTmpBak, psData->szFileName, psData->szCurrBatch );
		
		if( !FileIsExist( szDetailFile ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "临时备份文件 %s 尚未移入临时备份目录下!\n", szDetailFile );
			sprintf( szDetailFile, "%s/%s", psData->sCmd.szInDir, psData->szFileName );
		}
	}
	else
	{
		sprintf( szDetailFile, "%s/%s", psData->sCmd.szInDir, psData->szFileName );
	}
	
	/* 打开详单文件 */
	psData->fpDetail = fopen( szDetailFile, "r" );
	if( NULL == psData->fpDetail )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "打开详单文件 %s 失败!\n", szDetailFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	psData->lTotalRecord = 0;
	psData->lAvailRecord = 0;
	psData->lErrorRecord = 0;
	
	memset( psData->szRecord, 0, sizeof( psData->szRecord ) );
	
	/* 遍历详单文件记录 */
	while( NULL != fgets( psData->szRecord, sizeof( psData->szRecord ), psData->fpDetail ) )
	{
		psData->lTotalRecord++;
		
		/* if( 2 == psData->nDealFile &&  3 == psData->lTotalRecord ) exit(0); */
		
		memset( psData->szErrCode, 0, sizeof( psData->szErrCode ) );
		strcpy( psData->szErrCode, ERROR_CODE_NORMAL );

		/* 获取详单记录信息 */
		if( MY_SUCCEED != GetRecordInfo( psData ) )
		{
			return MY_FAIL;
		}
		
		if( psData->bResumeEnv )
		{
			if( 0 == strcmp( psData->szFileName, psData->tInfo.szFileName ) )
			{
				if( 0 != strcmp( psData->szRecord, psData->tInfo.szBreakRecord ) )
				{
					continue;
				}
				else
				{
					if( MY_SUCCEED != CheckDetailStoreFile( psData ) )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "检测断点记录所在详单存储文件内容失败!\n" );
						return MY_FAIL;
					}
				
					if( psData->bResumeEnv )
					{
						psData->bResumeEnv = MY_FALSE;
						psData->bResumeInc = MY_FALSE;
						continue;
					}
				}
			}
		}
		
		if( psData->bResumeInc )
		{
			continue;
		}
		
		if( MY_SUCCEED != UpdateTraceFile( psData, TRACE_FLAG_STORE ) )
		{
			return MY_FAIL;
		}
		
		if( 0 != strstr( psData->szErrCode, ERROR_CODE_NORMAL ) )
		{
			/* 存储详单记录 */
			if( MY_SUCCEED != WriteStroeFile( psData ) )
			{
				return MY_FAIL;
			}
		}
		else
		{
			/* 写入错单文件 */
			if( MY_SUCCEED != WriteErrorFile( psData ) )
			{
				return MY_FAIL;
			}
		}
		
		memset( psData->szRecord, 0, sizeof( psData->szRecord ) );
	}
		
	return MY_SUCCEED;
}


/**********************************************************
function: IsDetailFile
description: 判断字符串值是否为浮点数
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_TRUE: 是 MY_FALSE: 不是
others:
***********************************************************/

int IsDetailFile( PSTORE_DATA psData )
{
	POPER_INFO poInfo;
	POPER_RECORD poRecord;
	int nLoop;
	
	poInfo = &(psData->oInfo);
	poRecord = poInfo->poRecord;

	/* 比较文件头 */
	for( nLoop = 0; nLoop < poInfo->nOperCount; nLoop++, poRecord++ )
	{
		if( 0 == strncmp( psData->szFileName, poRecord->szFileHead, strlen( poRecord->szFileHead ) ) )
		{
			psData->nDealFile++;
			return MY_TRUE;
		}
	}

	return MY_FALSE;
}

/**********************************************************
function: StoreDetailFile
description: 将入口目录下的文件存储在当日详单存储文件中
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int StoreDetailFile( PSTORE_DATA psData )
{
	DIR * dirp;
	struct dirent * entry;
	int  nProcCtrlFlag = 0 ;

	
	/* 打开入口目录 */
	dirp = ( opendir( (psData->sCmd).szInDir ) );
	if( NULL == dirp )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "入口目录 %s 打开失败!\n", (psData->sCmd).szInDir );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	/* 处理入口目录下所有符合条件的文件 */
	while( NULL != ( entry = readdir( dirp ) ) )
	{
		/* 获取系统运行控制标志 */
        nProcCtrlFlag = GetProcCtrlFlag( psData->sCmd.szCtrlFlag ) ;
        if ( nProcCtrlFlag == PROC_CTRL_EXIT ) {
            break ;
        } else if ( nProcCtrlFlag == PROC_CTRL_RELOAD ) {
            FreeConfigInfo( psData ) ;
            if ( ReloadConfigInfo( psData ) != MY_SUCCEED ) {
                DisConnectDataBase( );
                FreeMemoryData( psData );
                MyLog( INFO_SOURCE_APP, INFO_LEVEL_EXIT, "获取系统配置信息失败!\n\n" );
                exit(0) ;
            }
						UpdateSysProc( psData->sCmd.szCtrlFlag, PROC_CTRL_RUN ) ;
        }

		tmEndTime = time( NULL );
 		tmEndTime = tmEndTime - tmEndTime % (psData->sCmd).nInterval + (psData->sCmd).nInterval;

 		if( tmEndTime > tmStartTime )
 		{
 			
 		#ifdef	__INCREMENT_FILE__	
 			if( MY_SUCCEED != DumpIncData( psData ) )
 			{
 				closedir( dirp );
 				return MY_FAIL;
 			}
 		#endif
 			
  			tmStartTime = tmEndTime;
			
			/* 获取当前批次 
			GetCurrBatch( psData );

			 打开当前批次的处理日志、错单日志 
			if( MY_SUCCEED != OpenLogFile( psData ) )
			{
				closedir( dirp );
				return MY_FAIL;
			}
			*/
			if( MY_SUCCEED != AlterLogFile( psData ) )
			{                                         
				closedir( dirp );
				return MY_FAIL;
			}                                         

		}
		
		memset( psData->szFileName, 0, sizeof( psData->szFileName ) );
		strcpy( psData->szFileName, entry->d_name );

		if( !IsDetailFile( psData ) )
		{
			continue;
		}
		
		/*printf( "Deal File %s\n", entry->d_name );*/
		
		memset( psData->szStartTime, 0, sizeof( psData->szStartTime ) );
		memset( psData->szEndTime, 0, sizeof( psData->szEndTime ) );
		
		/* 获取文件开始处理时间 */
		GetCurrTime( psData->szStartTime );
		GetCurrMicroSecond( psData->szStartMicroSec );
		
		if( MY_SUCCEED != StoreDetailRecord( psData ) )
		{
			closedir( dirp );
			return MY_FAIL;
		}
		
	#ifdef	__INCREMENT_FILE__	
		if( MY_SUCCEED != TmpBakDetailFile( psData ) )
		{
			closedir( dirp );
			return MY_FAIL;
		}
	#else
		if( MY_SUCCEED != BakSourceFile( psData ) )
		{
			closedir( dirp );
			return MY_FAIL;
		}
	#endif
	
		/* 获取文件结束处理时间 */
		GetCurrTime( psData->szEndTime );
		GetCurrMicroSecond( psData->szEndMicroSec );
		
		/* 写处理日志 */
		if( MY_SUCCEED != WriteProcLog( psData ) )
		{
			closedir( dirp );
			return MY_FAIL;
		}
		
		if( psData->bSingleMode )
		{
			
		#ifdef	__INCREMENT_FILE__	
			if( MY_SUCCEED != DumpIncData( psData ) )
 			{
 				closedir( dirp );
 				return MY_FAIL;
 			}
 		#endif
 			
		}
	}

	closedir( dirp );

	return MY_SUCCEED;
}

int IsIncTmpFile( char * szFileName, char * szBatch )
{
	char szFileBatch[DATE_STD_LEN + 1];
	char szHead[128 + 1];
	
	if( 0 != strcmp( szFileName + strlen( szFileName ) - strlen( TMP_FILE_TAIL ), TMP_FILE_TAIL ) )
	{
		return MY_FALSE;
	}
	
	if( INC_FILE_MLEN >= strlen( szFileName ) )
	{
		return MY_FALSE;
	}
	
	if( 0 != strncmp( szFileName, INC_FILE_HEAD, strlen( INC_FILE_HEAD ) ) )
	{
		return MY_FALSE;
	}
	
	memset( szFileBatch, 0, sizeof( szFileBatch ) );
	memset( szHead, 0, sizeof( szHead ) );
	
	strncpy( szFileBatch, szFileName + strlen( INC_FILE_HEAD ), DATE_STD_LEN );
	szFileBatch[DATE_STD_LEN];

	if( !IsDate( szFileBatch ) )
	{
		return MY_FALSE;
	}
	
	if( 0 != strncmp( szFileBatch, szBatch, strlen( szBatch ) ) )
	{
		return MY_FALSE;
	}

	strncpy( szHead, szFileName + strlen( INC_FILE_HEAD ) + DATE_STD_LEN + strlen( INC_FIELD_SEP ), 
							strlen( szFileName ) - strlen( INC_FILE_HEAD ) - DATE_STD_LEN - strlen( INC_FIELD_SEP ) - strlen( TMP_FILE_TAIL ) );
	szHead[strlen( szFileName ) - strlen( INC_FILE_HEAD ) - DATE_STD_LEN - strlen( INC_FIELD_SEP ) - strlen( TMP_FILE_TAIL )] = 0;
	
	if( !IsNum( szHead ) )
	{
		return MY_FALSE;
	}
	
	return MY_TRUE;
}

int MoveTmpIncFile( PSTORE_DATA psData )
{
	DIR * dirp;
	struct dirent * entry;
	char szPath[MAX_PATH_LEN + 1];
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDestFile[MAX_PATH_LEN + 1];
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "将临时目录下 %s 的增量统计文件移入出口目录!\n", psData->szBatchTime );
			
	dirp = ( opendir( (psData->sCmd).szTmpDir ) );
	if( NULL == dirp )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时目录 %s 打开失败!\n", (psData->sCmd).szTmpDir );
		return MY_FAIL;
	}
	
	while( NULL != ( entry = readdir( dirp ) ) )
	{
		if( IsIncTmpFile( entry->d_name, psData->szBatchTime ) )
		{
			memset( szSrcFile , 0, sizeof( szSrcFile ) );
			memset( szDestFile, 0, sizeof( szDestFile ) );
			
			sprintf( szSrcFile , "%s/%s", (psData->sCmd).szTmpDir, entry->d_name );
			sprintf( szDestFile, "%s/%s", (psData->sCmd).szOutDir, entry->d_name );
			szDestFile[strlen(szDestFile) - strlen( TMP_FILE_TAIL )] = 0;
			
			if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出正式增量统计文件失败!\n" );
				return MY_FAIL;
			}
		}
	}
	
	closedir( dirp );

	return MY_SUCCEED;
}

int MoveTmpBakFile( PSTORE_DATA psData )
{
	FILE * fpLog;
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDestFile[MAX_PATH_LEN + 1];
	char szFileName[MAX_PATH_LEN + 1];

	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "将临时备份目录下 %s 的文件移入备份目录!\n", psData->szBatchTime );
	
	fpLog = fopen( psData->szTmpBakLog, "r" );
	if( NULL == fpLog )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "打开临时备份日志文件 %s 失败!\n", psData->szTmpBakLog );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	memset( psData->szFileName, 0, sizeof( psData->szFileName ) );
	
	while( NULL != fgets( szFileName, sizeof( szFileName ), fpLog ) )
	{
		szFileName[strlen( szFileName ) - 1] = 0;

		memset( szSrcFile , 0, sizeof( szSrcFile ) );
		memset( szDestFile, 0, sizeof( szDestFile ) );
	
		sprintf( szSrcFile , "%s/%s.%s", (psData->sCmd).szTmpBak, szFileName, psData->szBatchTime );	
		sprintf( szDestFile, "%s/%s.%s", (psData->sCmd).szBakDir, szFileName, psData->szBatchTime );

		if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件 %s 移入备份目录失败!\n", szSrcFile );
			return MY_FAIL;
		}
		
		memset( szFileName, 0, sizeof( szFileName ) );
	}
	
	fclose( fpLog );
	
	return MY_SUCCEED;
}

int CleanLogFile( PSTORE_DATA psData )
{
	char szPath[MAX_PATH_LEN + 1];
	
	memset( szPath, 0, sizeof( szPath ) );
	sprintf( szPath, "%s/%s%s%s", (psData->sCmd).szTmpBak, TBAK_LOG_HEAD, psData->szBatchTime, LOG_FILE_TAIL );

	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "删除临时备份日志文件 %s !\n", szPath );
		
	if( -1 == remove( szPath ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "删除临时备份日志文件 %s 失败!\n", szPath );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	memset( szPath, 0, sizeof( szPath ) );
	sprintf( szPath, "%s/%s%s%s", (psData->sCmd).szTmpDir, TLOG_FILE_HEAD, psData->szBatchTime, LOG_FILE_TAIL );

	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "删除增量统计数据断点文件 %s !\n", szPath );
	
	if( -1 == remove( szPath ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "删除增量统计数据断点文件 %s 失败!\n", szPath );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
}

int GetTmpBakLog( PSTORE_DATA psData )
{
	memset( psData->szTmpBakLog, 0, sizeof( psData->szTmpBakLog ) );

	sprintf( psData->szTmpBakLog, "%s/%s%s%s", (psData->sCmd).szTmpBak, TBAK_LOG_HEAD, psData->szBatchTime, LOG_FILE_TAIL );
	
	if( !FileIsExist( psData->szTmpBakLog ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时备份日志文件 %s 不存在!\n", psData->szTmpBakLog );
		return MY_FAIL;
	}
	
	if( 0 >= GetFileCount( psData->szTmpBakLog ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时备份日志文件 %s 为空!\n", psData->szTmpBakLog );
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

int MoveTmpFile( PSTORE_DATA psData )
{
	DIR * dirp;
	struct dirent * entry;
	char bHasTmpFile;
	
	bHasTmpFile = MY_FALSE;
	
	dirp = ( opendir( (psData->sCmd).szTmpDir ) );
	if( NULL == dirp )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时目录 %s 打开失败!\n", (psData->sCmd).szTmpDir );
		return MY_FAIL;
	}
	
	while( NULL != ( entry = readdir( dirp ) ) )
	{
		if( TLOG_FILE_LEN == strlen( entry->d_name )
		&& 0 == strncmp( entry->d_name, TLOG_FILE_HEAD, strlen( TLOG_FILE_HEAD ) )
		&& 0 == strcmp( entry->d_name + strlen( entry->d_name ) - strlen( LOG_FILE_TAIL ), LOG_FILE_TAIL )
		)
		{
			if( bHasTmpFile )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时目录下存在多个增量统计数据断点文件!\n" );
				return MY_FAIL;
			}
			
			memset( psData->szBatchTime, 0, sizeof( psData->szBatchTime ) );
			memset( psData->szTmpIncLog, 0, sizeof( psData->szTmpIncLog ) );
			
			strncpy( psData->szBatchTime, entry->d_name + strlen( TLOG_FILE_HEAD ), TLOG_FILE_LEN - strlen( TLOG_FILE_HEAD ) - strlen( LOG_FILE_TAIL ) );
			strcpy( psData->szTmpIncLog, entry->d_name );
			
			bHasTmpFile = MY_TRUE;
		}
	}
	
	closedir( dirp );
	
	if( bHasTmpFile )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "处理增量统计数据断点文件 %s !\n", psData->szTmpIncLog );
		
		if( MY_SUCCEED != GetTmpBakLog( psData ) )
		{
			return MY_FAIL;
		}
		
		if( MY_SUCCEED != MoveTmpIncFile( psData ) )
		{
			return MY_FAIL;
		}
		
		if( MY_SUCCEED != MoveTmpBakFile( psData ) )
		{
			return MY_FAIL;
		}
		
		if( MY_SUCCEED != CleanLogFile( psData ) )
		{
			return MY_FAIL;
		}
		
		psData->bResumeEnv = MY_TRUE;
	}
	else
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "没有需要恢复的增量统计数据断点文件!\n\n" );
	}
	
	return MY_SUCCEED;;
}


/**********************************************************
function: ResumeBreakState
description: 恢复系统断点环境
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ResumeBreakState( PSTORE_DATA psData )
{
	char bHasTmpBakLog;
	
	char szTmpLogBak[MAX_PATH_LEN + 1];
	char szRecord[MAX_RECORD_LEN + 1];

	
#ifdef	__INCREMENT_FILE__
	FILE * fpTmpBakLog;	
	DIR * dirp;
	struct dirent * entry;
#endif

	bHasTmpBakLog = MY_FALSE;

#ifdef	__INCREMENT_FILE__		
	/* 是否存在临时备份日志文件 */
	dirp = ( opendir( (psData->sCmd).szTmpBak ) );
	if( NULL == dirp )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时备份目录 %s 打开失败!\n", (psData->sCmd).szTmpBak );
		return MY_FAIL;
	}
	
	while( NULL != ( entry = readdir( dirp ) ) )
	{
		if( TBAK_LOG_LEN == strlen( entry->d_name )
		&& 0 == strncmp( entry->d_name, TBAK_LOG_HEAD, strlen( TBAK_LOG_HEAD ) )
		&& 0 == strcmp( entry->d_name + strlen( entry->d_name ) - strlen( LOG_FILE_TAIL ), LOG_FILE_TAIL )
		)
		{
			if( bHasTmpBakLog )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时备份目录下存在多个临时备份日志文件!\n" );
				return MY_FAIL;
			}
			
			memset( psData->szCurrBatch, 0, sizeof( psData->szCurrBatch ) );
			
			strncpy( psData->szCurrBatch, entry->d_name + strlen( TBAK_LOG_HEAD ), DATE_STD_LEN );
			psData->szCurrBatch[DATE_STD_LEN] = 0;
			
			bHasTmpBakLog = MY_TRUE;
		}
	}
	
	closedir( dirp );
#endif
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取追踪文件断点记录内容!\n" );
	
	/* 检查系统处理追踪文件 */
	if( MY_SUCCEED != GetTraceInfo( psData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取追踪文件内容失败!\n" );
		return MY_FAIL;
	}

#ifdef	__INCREMENT_FILE__	
	if( bHasTmpBakLog )
	{
		if( psData->bResumeEnv )
		{
			if( 0 != strcmp( psData->szCurrBatch, psData->tInfo.szBreakBatch ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时备份文件批次 %s 与断点文件批次 %s 不一致!\n", psData->szCurrBatch, psData->tInfo.szBreakBatch );
				
				return MY_FAIL;
			}
		}
		
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "恢复临时备份文件的增量统计信息!\n" );
	
		psData->bResumeInc = MY_TRUE;
	
		memset( szTmpLogBak, 0, sizeof( szTmpLogBak ) );
	
		sprintf( szTmpLogBak, "%s/%s%s%s", (psData->sCmd).szTmpBak, TBAK_LOG_HEAD, psData->szCurrBatch, LOG_FILE_TAIL );
	
		fpTmpBakLog = fopen( szTmpLogBak, "r" );
		if( NULL == fpTmpBakLog )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "打开临时备份日志文件 %s 失败!\n", szTmpLogBak );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}
	
		memset( szRecord, 0, sizeof( szRecord ) );
	
		while( NULL != fgets( szRecord, sizeof( szRecord ), fpTmpBakLog ) )
		{
			psData->nDealFile++;
			
			szRecord[strlen(szRecord) - 1] = 0;
		
			memset( psData->szFileName, 0, sizeof( psData->szFileName ) );
			
			strcpy( psData->szFileName, szRecord );
			
			if( 0 == strcmp( psData->szFileName, psData->tInfo.szFileName ) )
			{
				psData->bResumeEnv = MY_FALSE;
			}
	
			if( MY_SUCCEED != StoreDetailRecord( psData ) )
			{
				fclose( fpTmpBakLog );
				return MY_FAIL;
			}
				
			memset( szRecord, 0, sizeof( szRecord ) );
		}

		fclose( fpTmpBakLog );
	}
	else
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "没有需要恢复的临时备份文件!\n\n" );
	}
#endif
	
	/* 处理追踪文件断点记录 */
	if( psData->bResumeEnv )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "回滚文件 %s 的断点数据!\n\n", psData->tInfo.szFileName );
	
		psData->nDealFile++;
			
		memset( psData->szFileName, 0, sizeof( psData->szFileName ) );
		memset( psData->szCurrBatch, 0, sizeof( psData->szCurrBatch ) );
		
		strcpy( psData->szFileName, psData->tInfo.szFileName );
		strcpy( psData->szCurrBatch, psData->tInfo.szBreakBatch );
		
		/* 获取文件开始处理时间 */
		GetCurrTime( psData->szStartTime );
		GetCurrMicroSecond( psData->szStartMicroSec );
		
		if( MY_SUCCEED != StoreDetailRecord( psData ) )
		{
			/*fclose( fpTmpBakLog );*/
			return MY_FAIL;
		}
		
	#ifdef	__INCREMENT_FILE__		
		if( MY_SUCCEED != TmpBakDetailFile( psData ) )
		{
			return MY_FAIL;
		}
	#else
		if( MY_SUCCEED != BakSourceFile( psData ) )
		{
			/*closedir( dirp );*/
			return MY_FAIL;
		}
	#endif
			
		/* 获取文件结束处理时间 */
		GetCurrTime( psData->szEndTime );
		GetCurrMicroSecond( psData->szEndMicroSec );
		
		/*
		if( MY_SUCCEED != OpenLogFile( psData ) )
		{
			return MY_FAIL;
		}
		*/
		
		/* 写处理日志 */
		if( MY_SUCCEED != WriteProcLog( psData ) )
		{
			return MY_FAIL;
		}
		
		if( psData->bSingleMode )
		{
			
		#ifdef	__INCREMENT_FILE__	
			if( MY_SUCCEED != DumpIncData( psData ) )
 			{
 				return MY_FAIL;
 			}
 		#endif
 			
		}
	}

	tmStartTime = GetSecondTime( psData->szCurrBatch );

	psData->bResumeInc = MY_FALSE;

	return MY_SUCCEED;
}

/**********************************************************
function: ResumeSystemEnv
description: 恢复系统运行环境至上次中止时的状态
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ResumeSystemEnv( PSTORE_DATA psData )
{
	psData->bResumeEnv = MY_TRUE;
	psData->bResumeInc = MY_TRUE;
	
#ifdef	__INCREMENT_FILE__	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "检测增量统计数据断点文件!\n" );
	
	/* 将临时目录下未移出的文件移入出口目录, 同时将临时备份目录下的文件移入备份目录, 不需要恢复 */
	if( MY_SUCCEED != MoveTmpFile( psData ) )
	{
		return MY_FAIL;
	}
#endif
	
	/* 不需要恢复, 直接返回 */
	if( !(psData->bResumeEnv) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "成功恢复系统运行环境!\n\n" );
		return MY_SUCCEED;
	}
	
	/* 在内存中恢复增量统计信息、处理断点文件 */
	if( MY_SUCCEED != ResumeBreakState( psData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "恢复系统运行环境失败!\n" );
		return MY_FAIL;
	}
	
	psData->bResumeEnv = MY_FALSE;
	psData->bResumeInc = MY_FALSE;
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "成功恢复系统运行环境!\n\n" );
	
	return MY_SUCCEED;
}
