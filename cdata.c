

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name: cdata.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 数据操作功能函数
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
			modification: 修改内容
		 2. ... 
********************************************************************/


#include "cdef.h"


/********************************************************** 
function: DebugOutIndexControlRecord
description: 输出详单存储控制记录内容
Input:	picRecord: 详单存储控制记录
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutStoreControlRecord( PSTORE_CONTROL psControl )
{
	printf( "[%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s]\n", 
						psControl->szHostName,
						psControl->szRootDir,
						psControl->szBaseDir,
						psControl->szGroupKey,
						psControl->szAlterTime,
						psControl->szOldRootDir,
						psControl->szOldBaseDir,
						psControl->szStoreMode );
						
	return MY_SUCCEED;
}

/********************************************************** 
function: DebugOutIndexControlInfo
description: 输出详单存储控制信息
Input:	psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutStoreControlInfo( PSTORE_DATA psData )
{
	PSTORE_HEAD psHead ;
	PSTORE_CONTROL psControl; 
	int nLoop = 0, nLoop1 = 0;
	
	printf( "\n\t=== store control info ===\n\n" );
	
	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		psHead = psData->psHead[nLoop1];
		if( psHead == NULL ) {
			break ;
		}
		
		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			if( NULL != psHead->psControl[nLoop] )
			{
				DebugOutStoreControlRecord( psHead->psControl[nLoop] );
			}
		}
	}
	
	psControl = psData->pscFixed;

	while( NULL != psControl )
	{
		DebugOutStoreControlRecord( psControl );
		
		psControl = psControl->pscNext;
	}

	printf( "\n\t=== store control info ===\n\n" );
		
	return MY_SUCCEED;
}

/********************************************************** 
function: DebugOutOperInfo
description: 输出详单格式描述信息
Input:	poInfo: 保存详单格式描述信息的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutOperInfo( POPER_INFO poInfo )
{
	POPER_RECORD poRecord;
	int nLoop;
	
	poRecord = poInfo->poRecord;
	
	printf( "\n\t=== oper info ===\n\n" );
	
	for( nLoop = 0; nLoop < poInfo->nOperCount; nLoop++, poRecord++ )
	{
		printf( "[%s] [%s] [%s] [%2d] [%3d] [%3d] [%d] [%3d] [%d] [%3d] [%d] [%d] [%3d] [%4d] [%d] [%d]\n",
										poRecord->szFileHead, 
										poRecord->szSysType, 
										poRecord->szDataType, 
										poRecord->nOperNo,	
										poRecord->nRecLen,	
										poRecord->nUserLoc, 
										poRecord->nUserLen, 
										poRecord->nTimeLoc, 
										poRecord->nTimeLen,
										poRecord->nDealTimeLoc,
										poRecord->nDealTimeOff,
										poRecord->nDealTimeLen, 
										poRecord->nDetachFlag, 
										poRecord->nFeeUnit,
										poRecord->nStoreMode,
										poRecord->nStatMode );
	}
	
	printf( "\n\t=== oper info ===\n\n" );
		
	return MY_SUCCEED;
}

/********************************************************** 
function: DebugOutFmtInfo
description: 输出增量统计字段配置文件内容
Input:	pfInfo: 保存增量统计字段配置文件内容的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutFmtInfo( PFMT_INFO pfInfo )
{
	PFMT_REC pfRec;
	int nLoop;
	
	pfRec = pfInfo->pfRec;
	
	printf( "\n\t=== increment field ===\n\n" );
	
	for( nLoop = 0; nLoop < pfInfo->nFieldCount; nLoop++, pfRec++ )
	{
		printf( "[%-2d] [%-15s] [%3d] [%3d] [%d] [%2d] [%s]\n",
							pfRec->nFieldNo, 
							pfRec->szFieldName, 
							pfRec->nStart,
							pfRec->nLen,
							pfRec->nType,
							pfRec->nOutNo,
							pfRec->szValueNo );
	}
	
	printf( "\n\t=== increment field ===\n\n" );
	
	return MY_SUCCEED;
}

/********************************************************** 
function: DebugOutFmtCtrl
description: 输出增量统计字段配置信息
Input:	pfCtrl: 保存增量统计字段配置文件信息的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutFmtCtrl( PFMT_CTRL pfCtrl )
{
	PFMT_INFO pfInfo;
	int nLoop;
	
	pfInfo = pfCtrl->pfInfo;
	
	printf( "\n\t=== increment file ===\n\n" );
	
	for( nLoop = 0; nLoop < pfCtrl->nFmtCount; nLoop++, pfInfo++ )
	{
		printf( "%d %s\n", pfInfo->nFileNo,  pfInfo->szFmtFile );
		
		DebugOutFmtInfo( pfInfo );
	}
	printf( "\t== increment file ==\n" );
	
	return MY_SUCCEED;
}

/************************************************************
function: GetOperInfo
description: 获取详单格式描述信息
input: psData: 描述系统全局结构的结构体
	   szOperFile: 详单格式描述信息文件名
output:
return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetOperInfo( PSTORE_DATA psData, char * szOperFile )
{
	POPER_INFO poInfo;
	POPER_RECORD poRecord;
	FILE * fpOper;
	char szRecord[MAX_RECORD_LEN + 1];
	char * szPos;
	int nFieldNum;

	poInfo = &(psData->oInfo);
	
	poInfo->nOperCount = GetFileCount( szOperFile );
	if( 0 >= poInfo->nOperCount )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单格式描述信息文件 %s 为空!\n", szOperFile );
		return MY_FAIL;
	}

	poInfo->poRecord = ( POPER_RECORD ) malloc( poInfo->nOperCount * sizeof(OPER_REC) );
	if( NULL == poInfo->poRecord )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存详单格式描述信息的内存空间失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	memset( poInfo->poRecord, 0, poInfo->nOperCount * sizeof(OPER_REC) );

	fpOper = fopen( szOperFile, "r" );
	if( NULL == fpOper )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单信息文件打开失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	szPos = NULL;
	poInfo->nOperCount = 0;
	poRecord = poInfo->poRecord;

	memset( szRecord, 0, sizeof( szRecord ) );
		
	while( NULL != fgets( szRecord, sizeof( szRecord ), fpOper ) )
	{
		if( !IsRecordLine( szRecord ) )
		{
			continue;
		}
		
		nFieldNum = 0;

		memset( poRecord, 0, sizeof( OPER_REC ) );
		
		szPos = strtok( szRecord, FILE_FIELD_SEP );
		while( NULL != szPos )
		{
			switch( nFieldNum )
			{
				case 0:		/* 文件头 */
				
					strcpy( poRecord->szFileHead, szPos );
					break;
				
				case 1:		/* 系统类型 */
				
					strcpy( poRecord->szSysType, szPos );
					break;
					
				case 2:		/* 数据类型 */
					strcpy( poRecord->szDataType, szPos );
					break;
				
				case 3:		/* 增量统计字段配置编号 */
				
					poRecord->nOperNo = atoi( szPos );
					break;
					
				case 4:		/* 记录长度 */
				
					poRecord->nRecLen = atoi( szPos );
					break;
					
				case 5:		/* 用户字段偏移 */
				
					poRecord->nUserLoc = atoi( szPos );
					break;
					
				case 6:		/* 用户字段长度 */
				
					poRecord->nUserLen = atoi( szPos );
					
					if( 0 >= poRecord->nUserLen || poRecord->nUserLen > MAX_SUBSCRIBER_LEN )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "用户字段长度 %d 超出定义!\n", poRecord->nUserLen );
						fclose( fpOper );
						return MY_FAIL;
					}
					
					break;
					
				case 7:		/* 通话时间字段偏移 */
				
					poRecord->nTimeLoc = atoi( szPos );
					break;
					
				case 8:		/* 通话时间字段长度 */
				
					poRecord->nTimeLen = atoi( szPos );	
					
					if( 0 >= poRecord->nTimeLen || poRecord->nTimeLen > DATE_STD_LEN )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "时间字段长度 %s 超出定义!\n", szPos );
						fclose( fpOper );
						return MY_FAIL;
					}
					
					break;
				
				case 9:		/* 处理时间字段位置 */
				
					poRecord->nDealTimeLoc = atoi( szPos );
					break;
				
				case 10:		/* 处理时间字段偏移 */
				
					poRecord->nDealTimeOff = atoi( szPos );
					break;
						
				case 11:	/* 处理时间字段长度 */
				
					poRecord->nDealTimeLen = atoi( szPos );	
					
					if( 0 >= poRecord->nDealTimeLen || poRecord->nDealTimeLen > DATE_STD_LEN )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "处理时间字段长度 %s 超出定义!\n", szPos );
						fclose( fpOper );
						return MY_FAIL;
					}
					
					break;
						
				case 12:		/* 拆分标志 */
				
					poRecord->nDetachFlag = atoi( szPos );
					break;
				
				case 13:	/* 费用基本单位 */
				
					poRecord->nFeeUnit = atoi( szPos );
					break;
				
				case 14:	/* 存储方式 */
					
					poRecord->nStoreMode = atoi( szPos );
					break;
				
				case 15:	/* 增量数据统计方式 */
					
					poRecord->nStatMode = atoi( szPos );
					break;
					
				default:    /* 无效字段 */

					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "记录域字段数超出定义值!\n" );
					fclose( fpOper );
					return MY_FAIL;
			}
			
			nFieldNum++;
			szPos = strtok( NULL, FILE_FIELD_SEP );
		}
		
		if( 16 != nFieldNum )
		{
			fclose( fpOper );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "记录域字段数不足!\n" );
			return MY_FAIL;
		}
		
		poRecord++;		
		poInfo->nOperCount++;
		memset( szRecord, 0, sizeof( szRecord ) );
	}

	fclose( fpOper );
	
	/* DebugOutOperInfo( poInfo ); */

	return MY_SUCCEED;
}

/************************************************************
function: GetFmtInfo
description: 获取配置文件中的增量统计字段配置信息
input: pfInfo: 描述配置文件的结构体
output:
return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetFmtInfo( PFMT_INFO pfInfo )
{
	PFMT_REC pfRec;
	FILE * fpInfo;
	char szRecord[MAX_RECORD_LEN + 1];
	char * szPos;
	int nFieldNum;
	
	pfInfo->nFieldCount = GetFileCount( pfInfo->szFmtFile );
	if( 0 >= pfInfo->nFieldCount )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单格式描述信息文件 %s 为空!\n", pfInfo->szFmtFile );
		return MY_FAIL;
	}

	pfInfo->pfRec = ( PFMT_REC ) malloc( pfInfo->nFieldCount * sizeof(FMT_REC) );
	if( NULL == pfInfo->pfRec )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存详单格式描述信息文件的内存空间失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	memset( pfInfo->pfRec, 0, pfInfo->nFieldCount * sizeof(FMT_REC) );

	fpInfo = fopen( pfInfo->szFmtFile, "r" );
	if( NULL == fpInfo )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单格式描述信息文件打开失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		fclose( fpInfo );
		return MY_FAIL;
	}

	szPos = NULL;
	pfRec = pfInfo->pfRec;
	pfInfo->nFieldCount = 0;

	memset( szRecord, 0, sizeof( szRecord ) );
		
	while( NULL != fgets( szRecord, sizeof( szRecord ), fpInfo ) )
	{
		if( !IsRecordLine( szRecord ) )
		{
			continue;
		}
		
		nFieldNum = 0;
		memset( pfRec, 0, sizeof( FMT_REC ) );

		szPos = strtok( szRecord, FILE_FIELD_SEP );
		while( NULL != szPos )
		{
			switch( nFieldNum )
			{
				case 0:		
				
					pfRec->nFieldNo = atoi( szPos );
					break;
					
				case 1:
				
					strcpy( pfRec->szFieldName, szPos );
					break;
					
				case 2:
				
					pfRec->nStart = atoi( szPos );
					break;
					
				case 3:
				
					pfRec->nLen = atoi( szPos );
					break;	
					
				case 4:
				
					pfRec->nType = atoi( szPos );
					break;
					
				case 5:
				
					pfRec->nOutNo = atoi( szPos );
					
					if( 0 > pfRec->nOutNo || pfRec->nOutNo > INC_FIELD_NUM )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出字段编号 [%d] 超出定义范围!\n", pfRec->nOutNo );
						fclose( fpInfo );
						return MY_FAIL;
					}
					break;
					
				case 6:
				
					strcpy( pfRec->szValueNo, szPos );
					break;
					
				default:    /* 无效字段 */

					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "记录域字段数超出定义值!\n" );
					fclose( fpInfo );
					return MY_FAIL;
			}
			
			nFieldNum++;
			szPos = strtok( NULL, FILE_FIELD_SEP );
		}
		
		if( 7 != nFieldNum )
		{
			fclose( fpInfo );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "记录域字段数不足!\n", nFieldNum );
			fclose( fpInfo );
			return MY_FAIL;
		}
		
		pfRec++;		
		pfInfo->nFieldCount++;
		memset( szRecord, 0, sizeof( szRecord ) );
	}

	fclose( fpInfo );
	
	/* DebugOutFmtInfo( pfInfo ); */

	return MY_SUCCEED;
}

/************************************************************
function: GetFmtCtrl
description: 获取增量统计字段配置信息
input: psData: 描述系统全局结构的结构体
	   szFmtCtrl: 增量统计字段配置文件
output:
return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetFmtCtrl( PSTORE_DATA psData, char * szFmtCtrl )
{
	PFMT_CTRL pfCtrl;
	PFMT_INFO pfInfo;
	FILE * fpCtrl;
	char szRecord[MAX_RECORD_LEN + 1];
	char * szPos;
	int nFieldNum;
	
	pfCtrl = &(psData->fCtrl);
	
	pfCtrl->nFmtCount = GetFileCount( szFmtCtrl );
	if( 0 >= pfCtrl->nFmtCount )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "增量配置文件 %s 为空!\n", szFmtCtrl );
		return MY_FAIL;
	}

	pfCtrl->pfInfo = ( PFMT_INFO ) malloc( pfCtrl->nFmtCount * sizeof(FMT_INFO) );
	if( NULL == pfCtrl->pfInfo )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存详单格式控制文件的内存空间失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	memset( pfCtrl->pfInfo, 0, pfCtrl->nFmtCount * sizeof(FMT_INFO) );

	fpCtrl = fopen( szFmtCtrl, "r" );
	if( NULL == fpCtrl )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单格式控制文件打开失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	szPos = NULL;
	pfInfo = pfCtrl->pfInfo;
	pfCtrl->nFmtCount = 0;

	memset( szRecord, 0, sizeof( szRecord ) );
		
	while( NULL != fgets( szRecord, sizeof( szRecord ), fpCtrl ) )
	{
		if( !IsRecordLine( szRecord ) )
		{
			continue;
		}
		
		nFieldNum = 0;
		
		memset( pfInfo, 0, sizeof( FMT_INFO ) );
		
		szPos = strtok( szRecord, FILE_FIELD_SEP );
		while( NULL != szPos )
		{
			switch( nFieldNum )
			{
				case 0:		/* 操作编号 */
				
					pfInfo->nFileNo = atoi(szPos );
					break;
					
				case 1:		/* 格式文件 */
				
					strcpy( pfInfo->szFmtFile, szPos );
					break;
				
				default:    /* 无效字段 */

					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "记录域字段数超出定义值!\n" );
					fclose( fpCtrl );
					return MY_FAIL;
			}
			
			nFieldNum++;
			szPos = strtok( NULL, FILE_FIELD_SEP );
		}
		
		if( 2 != nFieldNum )
		{
			fclose( fpCtrl );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "记录域字段数不足!\n", nFieldNum );
			return MY_FAIL;
		}
		
		if( MY_SUCCEED != GetFmtInfo( pfInfo ) )
		{
			fclose( fpCtrl );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取文件 %s 的增量统计字段配置信息失败!\n", pfInfo->szFmtFile );
			return MY_FAIL;
		}
		
		pfInfo++;		
		pfCtrl->nFmtCount++;
		memset( szRecord, 0, sizeof( szRecord ) );
	}

	fclose( fpCtrl );
	
	/* DebugOutFmtCtrl( pfCtrl ); */

	return MY_SUCCEED;
}

/**********************************************************
function: GetBaseOffset
description: 获取用户号码基址、偏移
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetBaseOffset( PSTORE_DATA psData )
{
	memset( psData->szBaseAddr, 0, sizeof( psData->szBaseAddr ) );
	memset( psData->szOffset, 0, sizeof( psData->szOffset ) );
	
	psData->nDetachNo = 0;
	
	if( SUBSCRIBER_OFFSET_LEN < strlen( psData->szSubscriber ) )
	{
		strncpy( psData->szBaseAddr, psData->szSubscriber, strlen( psData->szSubscriber ) - SUBSCRIBER_OFFSET_LEN );
		psData->szBaseAddr[strlen( psData->szSubscriber ) - SUBSCRIBER_OFFSET_LEN] = 0;
		
		strncpy( psData->szOffset, psData->szSubscriber + strlen( psData->szSubscriber ) - SUBSCRIBER_OFFSET_LEN, SUBSCRIBER_OFFSET_LEN );
		psData->szOffset[SUBSCRIBER_OFFSET_LEN] = 0;
	}
	else
	{
		strcpy( psData->szBaseAddr, psData->szSubscriber );
		strcpy( psData->szOffset, psData->szSubscriber );
	}
	
/*
	psData->nDetachNo = atoi(psData->szOffset) % psData->poRecord->nDetachFlag;
*/
	psData->nDetachNo = atoi(psData->szOffset);

	return MY_SUCCEED;
}

/**********************************************************
function: GetCurrBatch
description: 获取系统当前所处批次
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetCurrBatch( PSTORE_DATA psData )
{
    struct tm * tmTime;

    tmTime = localtime( &tmStartTime );

    memset( psData->szCurrBatch, 0, sizeof( psData->szCurrBatch ) );

    sprintf( psData->szCurrBatch, "%04d%02d%02d%02d%02d%02d",
                        tmTime->tm_year + 1900, tmTime->tm_mon + 1, tmTime->tm_mday,
                        tmTime->tm_hour, tmTime->tm_min,    tmTime->tm_sec );


    return MY_SUCCEED;
}

/**********************************************************
function: OpenLogFile
description: 打开系统当前批次的处理日志、错单日志
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int OpenLogFile( PSTORE_DATA psData )
{
	char szFilePath[MAX_PATH_LEN + 1];

	/* 关闭上次打开的, 打开当前的文件 
	if( -1 != psData->nProcHandle )
	{
		close( psData->nProcHandle );
		psData->nProcHandle = -1;
	}
	*/

	memset( szFilePath, 0, sizeof( szFilePath ) );

	sprintf( szFilePath, "%s/w%s.log", (psData->sCmd).szLogTmp, szSrcDir );

	psData->nProcHandle = open( szFilePath, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
	if( -1 == psData->nProcHandle )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "处理日志 %s 打开失败!\n", szFilePath );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	/* 关闭上次打开的, 打开当前的文件 */
	if( -1 != psData->nErrHandle )
	{
		close( psData->nErrHandle );
		psData->nErrHandle = -1;
	}

	memset( szFilePath, 0, sizeof( szFilePath ) );

	sprintf( szFilePath, "%s/w%s.err", (psData->sCmd).szLogTmp, szSrcDir );

	psData->nErrHandle = open( szFilePath, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
	if( -1 == psData->nErrHandle )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "错单日志 %s 打开失败!\n", szFilePath );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	return MY_SUCCEED;
}

/**********************************************************
function: OpenStoreFile
description: 打开详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int OpenStoreFile( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	PSTORE_CONTROL psControl;
	char szStoreFile[MAX_PATH_LEN + 1];
	char szStoreTime[14 + 1];
	
	poRecord = psData->poRecord;
	psControl = psData->psCtrl;
	
	memset( szStoreFile, 0, sizeof( szStoreFile ) );
	memset( szStoreTime, 0, sizeof( szStoreTime ) );
	
	strcpy( szStoreTime, psData->szCurrBatch );
	
	if( STORE_MODE_MODULUS == poRecord->nStoreMode )
	{
		if( 0 != strncmp( psData->szRecordTime, psData->szCurrBatch, 6 ) )
		{
			sprintf( szStoreTime, "%6.6s32000000", psData->szRecordTime );
		}
	}
	
	if( STORE_MODE_MODULUS == poRecord->nStoreMode )
	{
		sprintf( szStoreFile, "%s/%s/%6.6s/day%2.2s/%s%0*d.%8.8s", 
									psControl->szRootDir, 
									psControl->szBaseDir, 
									szStoreTime,
									szStoreTime + 6,
									psControl->szKey,
									SUBSCRIBER_OFFSET_LEN, 
									psData->nDetachNo, 
									szStoreTime );
	}
	else if( STORE_MODE_MOBILE == poRecord->nStoreMode && IsMobileHead( psData->szSubscriber ) ) 
	{
		sprintf( szStoreFile, "%s/%s/%s/%s/%s.%8.8s", 
										psControl->szRootDir, 
										psControl->szBaseDir, 
										psControl->szGroupKey, 
										STORE_DIR_CURR,
										psControl->szGroupKey,
										szStoreTime );
	}
	else
	{
		if( 0 == poRecord->nDetachFlag )
		{
			sprintf( szStoreFile, "%s/%s/%s/%s/%s.%s.%8.8s", 
										psControl->szRootDir, 
										psControl->szBaseDir, 
										psControl->szGroupKey, 
										STORE_DIR_CURR,
										psControl->szGroupKey,
										poRecord->szSysType, 
										szStoreTime );
		}
		else
		{
			sprintf( szStoreFile, "%s/%s/%s/%s/%s.%s%d.%8.8s", 
										psControl->szRootDir, 
										psControl->szBaseDir, 
										psControl->szGroupKey, 
										STORE_DIR_CURR,
										psControl->szGroupKey,
										poRecord->szSysType, 
										psData->nDetachNo, 
										szStoreTime );
		}
	}
	
	/* 比较上次存储文件 */
	if( 0 == strcmp( szStoreFile, psData->szStoreFile ) )
	{
		return MY_SUCCEED;
	}
	
	memset( psData->szStoreFile, 0, sizeof( psData->szStoreFile ) );
	strcpy( psData->szStoreFile, szStoreFile );
	
	/* 关闭上次打开的, 打开当前的文件 */
	if( -1 != psData->nStoreHandle )
	{
		close( psData->nStoreHandle );
		psData->nStoreHandle = -1;
	}

	psData->nStoreHandle = open( psData->szStoreFile, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
	if( -1 == psData->nStoreHandle )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单存储文件 %s 打开失败!\n", psData->szStoreFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	return MY_SUCCEED;
}

/**********************************************************
function: OpenIncFile
description: 打开增量统计文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int OpenIncFile( PSTORE_DATA psData, PHEAD_LIST phList )
{
	memset( psData->szIncFile, 0, sizeof( psData->szIncFile ) );
	
	sprintf( psData->szIncFile, "%s/%s%s.%s.tmp", psData->sCmd.szTmpDir, INC_FILE_HEAD, psData->szCurrBatch, phList->szPhoneHead );
	
	psData->fpInc = fopen( psData->szIncFile, "w" );
	if( NULL == psData->fpInc )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "增量统计文件 %s 打开失败!\n", psData->szIncFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	return MY_SUCCEED;
}

/********************************************************** 
function: WriteStroeFile
description: 写详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int WriteStroeFile( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	char szDetailInfo[MAX_RECORD_LEN + 1];
	
	/* 负话单不做存储 */
	if( 0 == strncmp( psData->szFileName, REVERSE_FILE_FLAG, strlen( REVERSE_FILE_FLAG ) ) )
	/*
	if( 0 == strncmp( psData->szFileName + strlen( psData->szFileName ) - strlen( REVERSE_FILE_FLAG ) , REVERSE_FILE_FLAG, strlen( REVERSE_FILE_FLAG ) ) )
	*/
	{
		psData->lAvailRecord++;
		return MY_SUCCEED;
	}
	
	poRecord = psData->poRecord;
	
	memset( szDetailInfo, 0, sizeof( szDetailInfo ) );
	
	/* 生成详单存储记录 */
	sprintf( szDetailInfo, "%s%*.*s%-*.*s\n", 
											DETAIL_RESERVE_CHAR, 
											strlen( psData->szRecord ) - 1, strlen( psData->szRecord ) - 1,
											psData->szRecord, 
											FILE_NAME_LEN, FILE_NAME_LEN, 
											psData->szFileName );
	
	/* 加密话单 */
#ifdef __ENCRYPT__
	if ( MY_SUCCEED != EncryptUser( psData->poRecord, szDetailInfo ) ) {
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "加密用户字段失败!\n" );
		return MY_FAIL ;
	}
#endif

	/* 更新详单处理时间 */
	memcpy( szDetailInfo + poRecord->nDealTimeLoc + DETAIL_RESERVE_LEN, psData->szCurrBatch + poRecord->nDealTimeOff, poRecord->nDealTimeLen );
	
	if( strlen( szDetailInfo ) != write( psData->nStoreHandle, szDetailInfo, strlen( szDetailInfo ) ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "写详单存储文件失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	psData->lAvailRecord++;
	
	return MY_SUCCEED;
}

/********************************************************** 
function: EncryptUser
description: 对计费号码进行加密解密
Input: psData: 描述系统全局结构的结构体
	   szRecord: 详单记录
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/
int EncryptUser( POPER_RECORD poRecord , char * szRecord )
{
	char*	szUser;
	int 	nLoop ;
	char	szKey[MAX_RECORD_LEN + 1];

	szUser = szRecord + poRecord->nUserLoc + strlen( DETAIL_RESERVE_CHAR ) ;
	strcpy( szKey, "DFJLT" );

	for( nLoop = 0 ; nLoop < poRecord->nUserLen ; nLoop++ ) {
		szUser[nLoop] = szUser[nLoop] ^ szKey[nLoop%strlen(szKey)];
	}

	if( nLoop != poRecord->nUserLen ) {
		return MY_FAIL ;
	}

	return MY_SUCCEED;
}

/********************************************************** 
function: WriteProcLog
description: 写处理日志
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int WriteProcLog( PSTORE_DATA psData )
{
	char szLogInfo[MAX_RECORD_LEN + 1];
	char szSysType[SYS_TYPE_LEN + 1] ;
	
	memset( szLogInfo, 0, sizeof( szLogInfo ) );
	memset( szSysType, 0, sizeof( szSysType ) );

	strncpy( szSysType, psData->szFileName+1, 2 ) ;
	sprintf( szLogInfo, "%s:%s:%s%s:%s%s:T %ld:A %ld:E %ld\n", 
					psData->szFileName, szSysType,
					psData->szStartTime, psData->szStartMicroSec,
					psData->szEndTime, psData->szEndMicroSec,
					psData->lTotalRecord, psData->lAvailRecord, psData->lErrorRecord );
					
	if( strlen( szLogInfo ) != write( psData->nProcHandle, szLogInfo, strlen( szLogInfo ) ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "处理日志写失败\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: WriteErrorFile
description: 写错单文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int WriteErrorFile( PSTORE_DATA psData )
{
	char szErrorInfo[MAX_RECORD_LEN + 1];
	
	memset( szErrorInfo, 0, sizeof( szErrorInfo ) );
	
	sprintf( szErrorInfo, "%s:%s:%s", psData->szFileName, psData->szErrCode, psData->szRecord );
	
	if( strlen( szErrorInfo ) != write( psData->nErrHandle, szErrorInfo, strlen( szErrorInfo ) ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "写错单文件失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	psData->lErrorRecord++;
	
	return MY_SUCCEED;
}

/********************************************************** 
function: UpdateTraceFile
description: 更新系统处理追踪文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int UpdateTraceFile( PSTORE_DATA psData, int nFlag )
{
	char szTraceRecord[MAX_RECORD_LEN + 1];
	
	if( -1 == psData->nTraceHandle )
	{
		memset( psData->szTraceFile, 0, sizeof( psData->szTraceFile ) );
		
		sprintf( psData->szTraceFile, "%s/trace_file", psData->sCmd.szLogTmp );
		
		psData->nTraceHandle = open( psData->szTraceFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
		if( -1 == psData->nTraceHandle )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单存储追踪文件 %s 打开失败!\n", psData->szTraceFile );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}
	}
	
	switch( nFlag )
	{
		case TRACE_FLAG_STORE:
			
			if( -1 == lseek( psData->nTraceHandle, 0, SEEK_SET ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位追踪文件存储位置失败!\n" );
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
				return MY_FAIL;
			}
			
			memset( szTraceRecord, 0, sizeof( szTraceRecord ) );
			sprintf( szTraceRecord, "%s/%s/%s/\n", psData->szFileName, psData->szCurrBatch, psData->szRecord );
			
			if( strlen( szTraceRecord ) != write( psData->nTraceHandle, szTraceRecord, strlen( szTraceRecord ) ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "写详单存储追踪文件失败!\n" );
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
				return MY_FAIL;
			}
	
			break;
		
		case TRACE_FLAG_TRUNC:
		
			if( 0 != ftruncate( psData->nTraceHandle, 0 ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "清空详单存储追踪文件失败!\n" );
				return MY_FAIL;
			}
				
			break;
		
		default:
			
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "追踪标志 %d 无效!\n", nFlag );
			return MY_FAIL;
			
			break;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetTraceInfo
description: 获取追踪文件内容
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetTraceInfo( PSTORE_DATA psData, int nFlag )
{
	PTRACE_INFO ptInfo;
	char szTraceRecord[MAX_RECORD_LEN + 1];
	char * szPos;
	FILE * fpTrace;
	
	int nFieldNum;

	memset( psData->szTraceFile, 0, sizeof( psData->szTraceFile ) );
		
	sprintf( psData->szTraceFile, "%s/trace_file", psData->sCmd.szLogTmp );
	
	if( !FileIsExist( psData->szTraceFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "没有需要处理的断点追踪文件!\n\n" );
		
		psData->bResumeEnv = MY_FALSE;
		return MY_SUCCEED;
	}

	if( 0 == GetFileSize( psData->szTraceFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "断点追踪文件内容为空!\n\n" );
		
		psData->bResumeEnv = MY_FALSE;
		return MY_SUCCEED;
	}
	
	fpTrace = fopen( psData->szTraceFile, "r" );
	if( NULL == fpTrace )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "打开详单存储追踪文件 %s 失败!\n", psData->szTraceFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	memset( szTraceRecord, 0, sizeof( szTraceRecord ) );
	
	if( NULL == fgets( szTraceRecord, sizeof( szTraceRecord ), fpTrace ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "读取详单存储追踪记录失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		
		fclose( fpTrace );
		return MY_FAIL;
	}
	
	nFieldNum = 0;
	ptInfo = &(psData->tInfo);
	
	memset( ptInfo, 0, sizeof( TRACE_INFO ) );
	
	szPos = strtok( szTraceRecord, "/" );
	while( NULL != szPos )
	{
		switch( nFieldNum )
		{
			case 0:		/* 文件名 */
				
				strcpy( ptInfo->szFileName, szPos );
				break;
					
			case 1:		/* 断点批次 */
				
				strcpy( ptInfo->szBreakBatch, szPos );
				break;
			
			case 2:		/* 断点记录 */
				
				strcpy( ptInfo->szBreakRecord, szPos );
				break;
				
			default:    /* 其他 */

				break;
		}
		
		nFieldNum++;
		
		szPos = strtok( NULL, "/" );
	}
	
	if( 3 > nFieldNum )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "断点记录域字段数不足!\n" );
		fclose( fpTrace );
		return MY_FAIL;
	}
	
	fclose( fpTrace );
	
	/* printf( "%s\n%s\n%s\n", ptInfo->szFileName, ptInfo->szBreakBatch, ptInfo->szBreakRecord ); */
	
	return MY_SUCCEED;
}

/**********************************************************
function:  WriteTmpBakLog
description: 写临时备份文件日志� 
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int WriteTmpBakLog( PSTORE_DATA psData )
{
	char szTmpLog[MAX_PATH_LEN + 1];
	char szBuffer[MAX_RECORD_LEN + 1];
	int nHandle;
	
	memset( szTmpLog, 0, sizeof( szTmpLog ) );
	memset( szBuffer, 0, sizeof( szBuffer ) );
	
	sprintf( szTmpLog, "%s/%s%s.log", (psData->sCmd).szTmpBak, TBAK_LOG_HEAD, psData->szCurrBatch );
	sprintf( szBuffer, "%s\n", psData->szFileName );
	
	nHandle = open( szTmpLog, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
	if( -1 == nHandle )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时备份日志 %s 打开失败!\n", szTmpLog );
		return MY_FAIL;
	}
	
	if( strlen( szBuffer ) != write( nHandle, szBuffer, strlen( szBuffer ) ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "临时备份日志 %s 写失败!\n", szTmpLog );
		return MY_FAIL;
	}
	
	close( nHandle );
	
	return MY_SUCCEED;
}

/**********************************************************
function: TmpBakDetailFile
description: 将详单文件移入临时备份目录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int TmpBakDetailFile( PSTORE_DATA psData )
{
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDestFile[MAX_PATH_LEN + 1];

	if( MY_SUCCEED != WriteTmpBakLog( psData ) )
	{
		return MY_FAIL;
	}
	
	memset( szSrcFile, 0, sizeof( szSrcFile ) );
	memset( szDestFile, 0, sizeof( szDestFile ) );
			
	sprintf( szSrcFile , "%s/%s"   , psData->sCmd.szInDir , psData->szFileName );
	sprintf( szDestFile, "%s/%s.%s", psData->sCmd.szTmpBak, psData->szFileName, psData->szCurrBatch );
			
	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件 %s 移入临时备份目录失败!\n", szSrcFile );
		return MY_FAIL;
	}
	
	if( MY_SUCCEED != UpdateTraceFile( psData, TRACE_FLAG_TRUNC ) )
	{
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: BakDetailFile
description: 将详单文件移入备份目录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int BakDetailFile( PSTORE_DATA psData )
{
	FILE * fpLog;
	char szTmpLog[MAX_PATH_LEN + 1];
	char szFileName[MAX_PATH_LEN + 1];
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDestFile[MAX_PATH_LEN + 1];
	char bSucceed;
	
	if( 0 >= psData->nDealFile )
	{
		return MY_SUCCEED;
	}
	
	bSucceed = MY_FALSE;
	
	memset( szTmpLog, 0, sizeof( szTmpLog ) );
	sprintf( szTmpLog, "%s/%s%s.log", (psData->sCmd).szTmpBak, TBAK_LOG_HEAD, psData->szCurrBatch );
	
	fpLog = fopen( szTmpLog, "r" );
	if( NULL == szTmpLog )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "打开临时备份日志 %s 失败!\n", szTmpLog );
		return MY_FAIL;
	}
	
	memset( szFileName, 0, sizeof( szFileName ) );
	
	while( NULL != fgets( szFileName, sizeof( szFileName ), fpLog ) )
	{
		szFileName[strlen( szFileName ) - 1] = 0;

		memset( szSrcFile, 0, sizeof( szSrcFile ) );
		memset( szDestFile, 0, sizeof( szDestFile ) );
	
		sprintf( szSrcFile , "%s/%s.%s", (psData->sCmd).szTmpBak, szFileName, psData->szCurrBatch );	
		sprintf( szDestFile, "%s/%s.%s", (psData->sCmd).szBakDir, szFileName, psData->szCurrBatch );

		if( !FileIsExist( szSrcFile ) )
		{
			if( bSucceed )
			{
				/* 存在成功备份文件后, 文件列表中的文件不在临时备份目录 */
				
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件 [%s] 不在临时备份目录, 请确认文件位置!\n", szSrcFile );
				return MY_FAIL;
			}
			else
			{
				/* 如果在备份文件时系统中断, 回滚时临时备份目录文件与临时备份日志记录的文件列表不符 */
				
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件 [%s] 不在临时备份目录, 请确认已移至备份目录下!\n", szSrcFile );
				bSucceed = MY_FALSE;
			}
		}
		else
		{
			if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件 %s 移入备份目录失败!\n", szSrcFile );
				return MY_FAIL;
			}
			
			bSucceed = MY_TRUE;
		}

		memset( szFileName, 0, sizeof( szFileName ) );
	}
	
	fclose( fpLog );
	
	if( -1 == remove( szTmpLog ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "删除临时备份日志文件 %s 失败!\n", szTmpLog );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	memset( szTmpLog, 0, sizeof( szTmpLog ) );
			
	sprintf( szTmpLog, "%s/%s%s%s", (psData->sCmd).szTmpDir, TLOG_FILE_HEAD, psData->szCurrBatch, LOG_FILE_TAIL );
	
	/* ADD BY ZF 20050116 *
	if( psData->phList[0] != NULL || psData->phlFixNumber != NULL ) {
	* ADD END */
	if( FileIsExist( szTmpLog ) )
	{
		if( -1 == remove( szTmpLog ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "删除增量统计数据断点文件 %s 失败!\n", szTmpLog );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: BakSourceFile
description: 将入口文件移入备份目录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int BakSourceFile( PSTORE_DATA psData )
{
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDestFile[MAX_PATH_LEN + 1];
	
	memset( szSrcFile, 0, sizeof( szSrcFile ) );
	memset( szDestFile, 0, sizeof( szDestFile ) );
	
	sprintf( szSrcFile , "%s/%s", (psData->sCmd).szInDir, psData->szFileName );	
	sprintf( szDestFile, "%s/%s.%s", (psData->sCmd).szBakDir, psData->szFileName, psData->szCurrBatch );

	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "文件 %s 移入备份目录失败!\n", szSrcFile );
		return MY_FAIL;
	}
	
	if( MY_SUCCEED != UpdateTraceFile( psData, TRACE_FLAG_TRUNC ) )
	{
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: FreeGroupList
description: 释放用户增量统计数据
Input: pgList: 用户增量统计数据
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeGroupList( PGROUP_LIST pgList )
{
	if( NULL != pgList->pglNext )
	{
		FreeGroupList( pgList->pglNext );
	}

	free( pgList );
	pgList = NULL;
	
	return MY_SUCCEED;
}		

/**********************************************************
function: FreeFixedHeadList
description: 释放固定号码增量统计数据
Input: phList: 固定号码增量统计数据
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeFixedHeadList( PHEAD_LIST phList )
{
	int nLoop;
	
	while( NULL != phList->phlNext )
	{
		FreeFixedHeadList( phList->phlNext );
	}
	
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		if( NULL != phList->pgList[nLoop] )
		{
			FreeGroupList( phList->pgList[nLoop] );
		}
	}
	
	free( phList );
	phList = NULL;
	
	return MY_SUCCEED;
}

/**********************************************************
function: FreeHeadList
description: 释放增量统计数据
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeHeadList( PSTORE_DATA psData )
{
	PHEAD_LIST phList;
	PGROUP_LIST pgList;
	PGROUP_HEAD pgHead ;
	int nLoop, nLoop1, nLoop2;
	
	for( nLoop2 = 0; nLoop2 < MAX_HEAD_NUM; nLoop2++ ) {
		pgHead = psData->pgHead[nLoop2]; 
		if( pgHead == NULL ) {
			break ;
		}
		
		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			if( NULL != pgHead->phList[nLoop] )
			{
				for( nLoop1 = 0; nLoop1 < MAX_HEAD_USER; nLoop1++ )
				{
					pgList = pgHead->phList[nLoop]->pgList[nLoop1];
					if( NULL != pgList )
					{
						FreeGroupList( pgList );
/*					DebugOutGroupList( pgList );*/
					}
				}
	
				free( pgHead->phList[nLoop] );
				pgHead->phList[nLoop] = NULL;
			}
		}
	}
	
	if( NULL != psData->phlFixNumber )
	{
		FreeFixedHeadList( psData->phlFixNumber );
	}
	
	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		pgHead = psData->pgHead[nLoop1];
		if( pgHead == NULL ) {
			break ;
		}
	
		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			pgHead->phList[nLoop] = NULL;
		}
	}
	
	psData->phlFixNumber = NULL;
	
	psData->nMobileHead = 0;
	psData->nFixedHead  = 0;
	psData->nDealFile   = 0;

	psData->lAddupRecord = 0;
	psData->lNoaddRecord = 0;
				
	return MY_SUCCEED;
}

/**********************************************************
function: CloseSystemFile
description: 关闭系统运行打开的文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int CloseSystemFile( PSTORE_DATA psData )
{
	if( -1 != psData->nStoreHandle )
	{
		close( psData->nStoreHandle );
		psData->nStoreHandle = -1;
	}
	
	if( -1 != psData->nProcHandle )
	{
		close( psData->nProcHandle );
		psData->nProcHandle = -1;
	}
	
	if( -1 != psData->nErrHandle )
	{
		close( psData->nErrHandle );
		psData->nErrHandle = -1;
	}
	
	if( -1 != psData->nTraceHandle )
	{
		close( psData->nTraceHandle );
		psData->nTraceHandle = -1;
	}
	
	if( NULL != psData->fpDetail )
	{
		fclose( psData->fpDetail );
		psData->fpDetail = NULL;
	}
	
	if( NULL != psData->fpInc )
	{
		fclose( psData->fpInc );
		psData->fpInc = NULL;
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: FreeFixedControl
description: 释放系统运行申请的内存空间
Input: psControl: 详单存储位置信息
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeFixedControl( PSTORE_CONTROL psControl )
{
	if( NULL != psControl->pscNext )
	{
		FreeFixedControl( psControl->pscNext );
	}
	
	free( psControl );
	psControl = NULL;
	
	return MY_SUCCEED;
}

/**********************************************************
function: FreeMemoryData
description: 释放系统运行申请的内存空间
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeMemoryData( PSTORE_DATA psData )
{
	PSTORE_CONTROL psControl;
	PSTORE_HEAD psHead ;
	PFMT_CTRL pfCtrl;
	PFMT_INFO pfInfo;
	int nLoop = 0, nLoop1 = 0;
	
	CloseSystemFile( psData );
	
	pfCtrl = &(psData->fCtrl);
	pfInfo = pfCtrl->pfInfo;
	
	for( nLoop = 0; nLoop < pfCtrl->nFmtCount; nLoop++, pfInfo++ )
	{
		if( NULL != pfInfo->pfRec )
		{
			free( pfInfo->pfRec );
			pfInfo->pfRec = NULL;
		}
	}
	
	if( NULL != pfCtrl->pfInfo )
	{
		free( pfCtrl->pfInfo );
		pfCtrl->pfInfo = NULL;
	}

	if( NULL != (psData->oInfo).poRecord )
	{
		free( (psData->oInfo).poRecord );
		(psData->oInfo).poRecord = NULL;
	}

	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		psHead = psData->psHead[nLoop1];
		if( psHead == NULL ) {
			break ;
		}
	
		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			if( NULL != psHead->psControl[nLoop] )
			{
				free( psHead->psControl[nLoop] );
				psHead->psControl[nLoop] = NULL;
			}
		}
	}
	
	if( NULL != psData->pscFixed )
	{
		FreeFixedControl( psData->pscFixed );
		psData->pscFixed = NULL;
	}
	
	FreeHeadList( psData );
	
	return MY_SUCCEED;
}

/**********************************************************
function: FreeConfigInfo
description: 释放配置文件内存
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/
int FreeConfigInfo( PSTORE_DATA psData )
{
	PSTORE_HEAD psHead ;
	PSTORE_CONTROL psControl;
	PFMT_CTRL pfCtrl;
	PFMT_INFO pfInfo;
	int nLoop = 0, nLoop1 = 0;
	
	pfCtrl = &(psData->fCtrl);
	pfInfo = pfCtrl->pfInfo;
	
	for( nLoop = 0; nLoop < pfCtrl->nFmtCount; nLoop++, pfInfo++ )
	{
		if( NULL != pfInfo->pfRec )
		{
			free( pfInfo->pfRec );
			pfInfo->pfRec = NULL;
		}
	}
	
	if( NULL != pfCtrl->pfInfo )
	{
		free( pfCtrl->pfInfo );
		pfCtrl->pfInfo = NULL;
	}

	if( NULL != (psData->oInfo).poRecord )
	{
		free( (psData->oInfo).poRecord );
		(psData->oInfo).poRecord = NULL;
	}

	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		/*psHead = psData->psHead + nLoop1 ;*/
		psHead = psData->psHead[nLoop1];
		if( psHead == NULL ) {
			break ;
		}
		
		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			if( NULL != psHead->psControl[nLoop] )
			{
				free( psHead->psControl[nLoop] );
				/*psData->psHead[nLoop]->psControl[nLoop] = NULL;*/
			}
		}
	}
	
	if( NULL != psData->pscFixed )
	{
		FreeFixedControl( psData->pscFixed );
		psData->pscFixed = NULL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetConfigInfo
description: 获取系统运行配置信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetConfigInfo( PSTORE_DATA psData )
{
	int nLoop = 0, nLoop1 = 0;
	PGROUP_HEAD pgHead ;
	
	/* 初始化系统变量 */
	
	psData->pscFixed = NULL;
	psData->poRecord = NULL;
	psData->psCtrl   = NULL;

	psData->fpDetail = NULL;
	psData->fpInc    = NULL;
		
	psData->nStoreHandle = -1;
	psData->nProcHandle  = -1;
	psData->nErrHandle   = -1;
	psData->nTraceHandle = -1;
	
	psData->nMobileHead = 0;
	psData->nFixedHead  = 0;
	psData->nDealFile   = 0;
	
	psData->lAddupRecord = 0;
	psData->lNoaddRecord = 0;
	
	memset( psData->szCurrBatch, 0, sizeof( psData->szCurrBatch ) );
	
	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		pgHead = psData->pgHead[nLoop1];
		if( pgHead == NULL ) {
			break ;
		}
		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			pgHead->phList[nLoop] = NULL;
		}
	}
	
	psData->phlFixNumber = NULL;
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取详单存储路径信息!\n" );
	
	/* 获取详单存储路径信息 */
	if( MY_SUCCEED != GetStoreControlInfo( psData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单存储路径信息失败!\n" );
		return MY_FAIL;
	}
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取详单格式描述信息!\n" );
	
	/* 获取详单格式描述信息 */
	if( MY_SUCCEED != GetOperInfo( psData, psData->sCmd.szOperFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单格式描述信息失败!\n" );
		return MY_FAIL;
	}

#ifdef	__INCREMENT_FILE__	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取增量统计字段配置信息!\n\n" );
	
	/* 获取增量统计字段配置信息 */
	if( MY_SUCCEED != GetFmtCtrl( psData, psData->sCmd.szFmtCtrl ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取增量统计字段配置信息失败!\n" );
		return MY_FAIL;
	}
#endif

	return MY_SUCCEED;
}

/********************************************************** 
function: ReloadConfigInfo
description: 重新获取系统运行配置信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ReloadConfigInfo( PSTORE_DATA psData )
{
	int nLoop;
	
	/* 初始化系统变量 */
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取详单存储路径信息!\n" );
	
	/* 获取详单存储路径信息 */
	if( MY_SUCCEED != GetStoreControlInfo( psData ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单存储路径信息失败!\n" );
		return MY_FAIL;
	}
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取详单格式描述信息!\n" );
	
	/* 获取详单格式描述信息 */
	if( MY_SUCCEED != GetOperInfo( psData, psData->sCmd.szOperFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单格式描述信息失败!\n" );
		return MY_FAIL;
	}

#ifdef	__INCREMENT_FILE__	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "获取增量统计字段配置信息!\n\n" );
	
	/* 获取增量统计字段配置信息 */
	if( MY_SUCCEED != GetFmtCtrl( psData, psData->sCmd.szFmtCtrl ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取增量统计字段配置信息失败!\n" );
		return MY_FAIL;
	}
#endif

	return MY_SUCCEED;
}

/********************************************************** 
function: AlterLogFile
description: 变更系统当前批次的处理日志、错单日志
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int AlterLogFile( PSTORE_DATA psData )
{
	if( MY_SUCCEED != CloseLogFile( psData ) )
	{
		return MY_FAIL;
	}

	if( MY_SUCCEED != MoveLogFile( psData ) )
	{
		return MY_FAIL;
	}
	
	/* 获取当前批次 */
	GetCurrBatch( psData );

	if( MY_SUCCEED != OpenLogFile( psData ) )
	{
		return MY_FAIL;
	}
										
	return MY_SUCCEED;
}

/********************************************************** 
function: CloseLogFile
description: 关闭系统当前批次的处理日志、错单日志
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int CloseLogFile( PSTORE_DATA psData )
{
	if( -1 != psData->nProcHandle ) { 
		close( psData->nProcHandle );
		psData->nProcHandle = -1; 
	} 
	
	if( -1 != psData->nErrHandle ) { 
		close( psData->nErrHandle ); 
		psData->nErrHandle = -1; 
	} 
	return MY_SUCCEED; 
}

/********************************************************** 
function: MoveLogFile
description: 移动系统当前批次的处理日志、错单日志
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int MoveLogFile( PSTORE_DATA psData )
{
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDstFile[MAX_PATH_LEN + 1];
			
	memset( szSrcFile, 0, sizeof( szSrcFile ) ); 
	memset( szDstFile, 0, sizeof( szDstFile ) ); 

	sprintf( szSrcFile, "%s/w%s.log", (psData->sCmd).szLogTmp, szSrcDir ); 
	/*sprintf( szDstFile, "%s/log%2.2s/w%s%8.8s.log", (psData->sCmd).szProcLog, psData->szCurrBatch + 6, szSrcDir, psData->szCurrBatch + 4 ) ;*/
	sprintf( szDstFile, "%s/log%2.2s/w%8.8s.%s.log", (psData->sCmd).szProcLog, psData->szCurrBatch + 6, psData->szCurrBatch + 4, szSrcDir ) ;
	if( MY_SUCCEED != MoveFile( szSrcFile, szDstFile ) ) { 
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "移动临时日志文件 %s 失败\n", szSrcFile ); 
		return MY_FAIL; 
	} 
	
	memset( szSrcFile, 0, sizeof( szSrcFile ) ); 
	memset( szDstFile, 0, sizeof( szDstFile ) ); 
	
	sprintf( szSrcFile, "%s/w%s.err", (psData->sCmd).szLogTmp, szSrcDir ); 
	/*
	sprintf( szDstFile, "%s/log%2.2s/w%s%8.8s.err", (psData->sCmd).szProcLog, psData->szCurrBatch + 6, szSrcDir, psData->szCurrBatch + 4 ) ;
	*/
	sprintf( szDstFile, "%s/log%2.2s/w%8.8s.%s.err", (psData->sCmd).szProcLog, psData->szCurrBatch + 6, psData->szCurrBatch + 4, szSrcDir ) ;
	
	if( MY_SUCCEED != MoveFile( szSrcFile, szDstFile ) ) { 
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "移动临时错单文件 %s 失败\n", szSrcFile ); 
		return MY_FAIL; 
	} 
	
	return MY_SUCCEED;
}

/********************************************************** 
function: InitStoreHead
description:
Input: psData:
Output:
Return: MY_SUCCEED: 成功
others:
***********************************************************/

int InitStoreHead( PSTORE_DATA psData )
{
        int nLoop ;
        
        for( nLoop = 0; nLoop < MAX_HEAD_NUM; nLoop++ ) {
                psData->psHead[nLoop] = NULL ;
        }

        return MY_SUCCEED ;
}

/**********************************************************
function: InitStoreHead
description:
Input: psData:
Output:
Return: MY_SUCCEED: 3I9&
others:
***********************************************************/

int InitGroupHead( PSTORE_DATA psData )
{
        int nLoop ;
        
        for( nLoop = 0; nLoop < MAX_HEAD_NUM; nLoop++ ) {
                psData->pgHead[nLoop] = NULL ;
        }

        return MY_SUCCEED ;
}
