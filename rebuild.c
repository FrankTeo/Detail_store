

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name: rebuild.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 规整当日详单存储文件
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
			modification: 修改内容
		 2. ... 
********************************************************************/


#include "cdef.h"


/********************************************************** 
function: OpenDayFile
description: 打开往日详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int OpenDayFile( PSTORE_DATA psData )
{
	PSTORE_CONTROL psControl;
	char szFile[MAX_PATH_LEN + 1];
	
	psControl = psData->psCtrl;
	
	memset( szFile, 0, sizeof( szFile ) );
	
	sprintf( szFile, "%s/%s/%s/%s/%s%s%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_DAY, 
								psData->szFileName, DAY_FILE_TAIL, TMP_FILE_TAIL );
	
	/* 打开往日详单存储文件 */										
	psData->fpDetail = fopen( szFile, "w" );
	if( NULL == psData->fpDetail )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往日详单存储文件 %s 打开失败!\n", szFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	memset( szFile, 0, sizeof( szFile ) );
	
	sprintf( szFile, "%s/%s/%s/%s/%s%s%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_INDEX, 
								psData->szFileName, INDEX_FILE_TAIL, TMP_FILE_TAIL );
	
	/* 打开往日详单存储索引文件 */
	psData->fpIndex = fopen( szFile, "w" );
	if( NULL == psData->fpIndex )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往日详单索引文件 %s 打开失败!\n", szFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
									
	return MY_SUCCEED;	
}

/********************************************************** 
function: MoveTmpDayFile
description: 生成正式的输出文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int MoveTmpDayFile( PSTORE_DATA psData )
{
	PSTORE_CONTROL psControl;
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDestFile[MAX_PATH_LEN + 1];
	
	psControl = psData->psCtrl;
	
	memset( szSrcFile , 0, sizeof( szSrcFile ) );
	memset( szDestFile, 0, sizeof( szDestFile ) );
	
	sprintf( szSrcFile, "%s/%s/%s/%s/%s%s%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_DAY, 
								psData->szFileName, DAY_FILE_TAIL, TMP_FILE_TAIL );
	
	sprintf( szDestFile, "%s/%s/%s/%s/%s%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_DAY, 
								psData->szFileName, DAY_FILE_TAIL );
	
	/* 生成正式的往日详单存储文件 */
	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "生成正式往日详单存储文件失败!\n" );
		return MY_FAIL;
	}

	memset( szSrcFile , 0, sizeof( szSrcFile ) );
	memset( szDestFile, 0, sizeof( szDestFile ) );
	
	sprintf( szSrcFile, "%s/%s/%s/%s/%s%s%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_INDEX, 
								psData->szFileName, INDEX_FILE_TAIL, TMP_FILE_TAIL );
	
	sprintf( szDestFile, "%s/%s/%s/%s/%s%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_INDEX, 
								psData->szFileName, INDEX_FILE_TAIL );
	
	/* 生成正式的索引文件 */
	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "生成正式往日详单存储索引文件失败!\n" );
		return MY_FAIL;
	}
	
	memset( szSrcFile , 0, sizeof( szSrcFile ) );
	memset( szDestFile, 0, sizeof( szDestFile ) );
	
	sprintf( szSrcFile, "%s/%s/%s/%s/%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_CURR, 
								psData->szFileName );
	
	sprintf( szDestFile, "%s/%s/%s/%s/%s%s", 
								psControl->szRootDir, psControl->szBaseDir, 
								psControl->szGroupKey, STORE_DIR_OLD, 
								psData->szFileName, OLD_FILE_TAIL );
	
	/*return MY_SUCCEED;*/
	
	/* 备份当日详单存储文件 */
	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "备份当日详单存储文件失败!\n" );
		return MY_FAIL;
	}
								
	return MY_SUCCEED;	
}

/********************************************************** 
function: ExportDetailRecord
description: 输出规整后的详单记录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ExportDetailRecord( PSTORE_DATA psData )
{
	PSERVICE_DETAIL psDetail;
	PDETAIL_BLOCK pdBlock;
	PBASE_INDEX pbIndex;
	SERVICE_INDEX sIndex;

	int nLoop;
	
	/* 打开往日详单存储文件 */
	if( MY_SUCCEED != OpenDayFile( psData ) )
	{
		return MY_FAIL;
	}
	
	memset( psData->pbIndex, 0, MAX_HEAD_USER * sizeof( BASE_INDEX ) );

	psData->ulIndexOff  = MAX_HEAD_USER * sizeof( BASE_INDEX );
	psData->ulDetailOff = 0;
	
	/* 定位详单索引信息存储位置 */
	if( 0 != fseek( psData->fpIndex, MAX_HEAD_USER * sizeof( BASE_INDEX ), SEEK_SET ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位详单索引信息存储位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		/* 输出详单数据区标志 */
		if( 0 >= fprintf( psData->fpDetail, "%s%0*d\n", DETAIL_SUBSCRIBER_FLAG, SUBSCRIBER_OFFSET_LEN, nLoop ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出用户 %0*d 的详单数据区标志失败!\n", SUBSCRIBER_OFFSET_LEN, nLoop );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}
		
		psData->ulDetailOff += DETAIL_BLOCKFLAG_LEN;

		psDetail = psData->psDetail[nLoop];
		pbIndex  = &(psData->pbIndex[nLoop]);
	
		pbIndex->ulOffset  = psData->ulIndexOff;
		pbIndex->nUserNo   = nLoop;
		pbIndex->nDay      = atoi( psData->rCmd.szBuildDate + 6 );
		pbIndex->nService  = 0;
		
		if( NULL != psDetail )
		{
			while( NULL != psDetail )
			{
				memset( &sIndex, 0, sizeof( SERVICE_INDEX ) );
					
				strcpy( sIndex.szSysType, psDetail->szSysType );
				
				sIndex.ulDetailOff = psData->ulDetailOff; 
				sIndex.ulDetailLen = 0;
				sIndex.ulOriginLen = 0;
				sIndex.nUserNo     = nLoop;
				sIndex.nDay        = atoi( psData->rCmd.szBuildDate + 6 );
				
				pbIndex->nService++;
				psData->ulIndexOff += sizeof( SERVICE_INDEX );
				
				pdBlock = psDetail->pdBlock;
				
				while( NULL != pdBlock )
				{
					/* 输出往日详单存储记录 */
					if( strlen( pdBlock->szDetailBlock ) != fwrite( pdBlock->szDetailBlock, sizeof( char ), strlen( pdBlock->szDetailBlock ), psData->fpDetail ) )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出用户 %0*d 的详单数据区失败!\n", SUBSCRIBER_OFFSET_LEN, nLoop );
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
						return MY_FAIL;
					}
					
					psData->ulDetailOff += strlen( pdBlock->szDetailBlock );
					sIndex.ulDetailLen += strlen( pdBlock->szDetailBlock );
					
					pdBlock = pdBlock->pdbNext;
				}
				
				sIndex.ulOriginLen = sIndex.ulDetailLen;
				
				/* 输出详单索引信息 */
				if( 1 != fwrite( &sIndex, sizeof( SERVICE_INDEX ), 1, psData->fpIndex ) )
				{
					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单索引信息写失败!\n" );
					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
					return MY_FAIL;
				}
				
				psDetail = psDetail->psdNext;
			}
		}
	}
	
	/* 定位至索引文件头部 */
	if( 0 != fseek( psData->fpIndex, 0, SEEK_SET ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位详单索引文件头部位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 输出业务索引信息 */
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		if( 1 != fwrite( &(psData->pbIndex[nLoop]), sizeof( BASE_INDEX ), 1, psData->fpIndex ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "业务索引信息写失败!\n" );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}
	}
	
	if( NULL != psData->fpDetail )
	{
		fclose( psData->fpDetail );
		psData->fpDetail = NULL;
	}
	
	if( NULL != psData->fpIndex )
	{
		fclose( psData->fpIndex  );
		psData->fpIndex = NULL;
	}
	
	if( MY_SUCCEED != MoveTmpDayFile( psData ) )
	{
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: FreeDetailBlock
description: 释放详单存储数据块
Input: pdBlock: 详单存储数据块
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeDetailBlock( PDETAIL_BLOCK pdBlock )
{
	while( NULL != pdBlock->pdbNext )
	{
		FreeDetailBlock( pdBlock->pdbNext );
		pdBlock->pdbNext = NULL;
	}

	free( pdBlock );
	pdBlock = NULL;
	
	return MY_SUCCEED;
}

/********************************************************** 
function: FreeServiceDetail
description: 释放业务详单存储数据块
Input: pdBlock: 业务详单存储数据块
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeServiceDetail( PSERVICE_DETAIL psDetail )
{
	while( NULL != psDetail->psdNext )
	{
		FreeServiceDetail( psDetail->psdNext );
		psDetail->psdNext = NULL;
	}
	
	if( NULL != psDetail->pdBlock )
	{
		FreeDetailBlock( psDetail->pdBlock );
	}
	
	free( psDetail );
	psDetail = NULL;
	
	return MY_SUCCEED;
}

/********************************************************** 
function: FreeDetailMemory
description: 释放详单数据
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeDetailMemory( PSTORE_DATA psData )
{
	PSERVICE_DETAIL psDetail;
	int nLoop;
	
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		psDetail = psData->psDetail[nLoop];
		
		if( NULL != psDetail )
		{
			FreeServiceDetail( psDetail );
		}
	}

	return MY_SUCCEED;
}

/********************************************************** 
function: AttachDetailBlock
description: 将详单记录附加在详单数据区中
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int AttachDetailBlock( PSTORE_DATA psData, PSERVICE_DETAIL psDetail )
{
	PDETAIL_BLOCK pdBlock;
	
	/* 首次申请详单记录存储空间 */
	if( NULL == psDetail->pdBlock )
	{
		psDetail->pdBlock = (PDETAIL_BLOCK) malloc( sizeof( DETAIL_BLOCK ) );
		if( NULL == psDetail->pdBlock )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存用户详单记录的内存空间失败!\n" );
			return MY_FAIL;
		}
		
		pdBlock = psDetail->pdBlock;
		
		memset( pdBlock, 0, sizeof( DETAIL_BLOCK ) );
		
		pdBlock->pdbNext = NULL;
		pdBlock->lBlockOffset = 0;
		
		memset( pdBlock->szDetailBlock, 0, sizeof( pdBlock->szDetailBlock ) );
	}
	else
	{
		pdBlock = psDetail->pdBlock;
		
		while( NULL != pdBlock )
		{
			if( NULL == pdBlock->pdbNext )
			{
				break;
			}
			
			pdBlock = pdBlock->pdbNext;
		}
	}
	
	/* 申请新的详单记录存储空间 */
	if( DETAIL_BLOCK_SIZE < ( pdBlock->lBlockOffset + strlen( psData->szRecord ) ) )
	{
		pdBlock->pdbNext = (PDETAIL_BLOCK) malloc( sizeof( DETAIL_BLOCK ) );
		if( NULL == pdBlock->pdbNext )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存用户详单记录的内存空间失败!\n" );
			return MY_FAIL;
		}
		
		pdBlock = pdBlock->pdbNext;
		
		memset( pdBlock, 0, sizeof( DETAIL_BLOCK ) );
		
		pdBlock->pdbNext = NULL;
		pdBlock->lBlockOffset = 0;
		
		memset( pdBlock->szDetailBlock, 0, sizeof( pdBlock->szDetailBlock ) );
	}
	
	/* 附加详单记录 */
#ifdef __ENCRYPT__
	if ( MY_SUCCEED != EncryptUser( psData->poRecord, psData->szRecord ) ) {
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "加密用户字段失败!\n" );
		return MY_FAIL ;
	}
#endif
	strcat( pdBlock->szDetailBlock, psData->szRecord );
	
	pdBlock->lBlockOffset += strlen( psData->szRecord );
	
	return MY_SUCCEED;
}

/********************************************************** 
function: InsertDetailRecord
description: 向详单数据区插入详单记录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int InsertDetailRecord( PSTORE_DATA psData )
{
	PSERVICE_DETAIL psDetail;
	POPER_RECORD poRecord;
	
	poRecord = psData->poRecord;
	
	/* 首次申请业务详单存储空间 */
	if( NULL == psData->psDetail[atoi(psData->szOffset)] )
	{
		psData->psDetail[atoi(psData->szOffset)] = (PSERVICE_DETAIL) malloc( sizeof( SERVICE_DETAIL ) );
		if( NULL == psData->psDetail[atoi(psData->szOffset)] )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存用户各业务详单记录的内存空间失败!\n" );
			return MY_FAIL;
		}
		
		psDetail = psData->psDetail[atoi(psData->szOffset)];
		
		memset( psDetail, 0, sizeof( SERVICE_DETAIL ) );
		
		psDetail->pdBlock = NULL;
		psDetail->psdNext = NULL;
		
		memset( psDetail->szSysType, 0, sizeof( psDetail->szSysType ) );
		
		strcpy( psDetail->szSysType, poRecord->szSysType );
	}
	else
	{
		psDetail = psData->psDetail[atoi(psData->szOffset)];
		
		while( NULL != psDetail )
		{
			/* 定位业务详单存储空间 */
			if( 0 == strcmp( psDetail->szSysType, poRecord->szSysType ) )
			{
				break;
			}
			
			/* 申请新的业务详单存储空间 */
			if( NULL == psDetail->psdNext )
			{
				psDetail->psdNext = (PSERVICE_DETAIL) malloc( sizeof( SERVICE_DETAIL ) );
				if( NULL == psDetail->psdNext )
				{
					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存用户各业务详单记录的内存空间失败!\n" );
					return MY_FAIL;
				}
		
				psDetail = psDetail->psdNext;
		
				memset( psDetail, 0, sizeof( SERVICE_DETAIL ) );
		
				psDetail->pdBlock = NULL;
				psDetail->psdNext = NULL;
				
				memset( psDetail->szSysType, 0, sizeof( psDetail->szSysType ) );
		
				strcpy( psDetail->szSysType, poRecord->szSysType );
		
				break;
			}
			else
			{
				psDetail = psDetail->psdNext;
			}
		}
	}
	
	/* 附加详单记录 */
	if( MY_SUCCEED != AttachDetailBlock( psData, psDetail ) )
	{
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetSystemType
description: 获取详单系统类型
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetSystemType( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	int nLoop;
	
	/* 遍历详单格式描述信息文件 */
	for( nLoop = 0; nLoop < psData->oInfo.nOperCount; nLoop++ )
	{
		poRecord = psData->oInfo.poRecord + nLoop;
		
		/* 比较系统类型 */
		if( 0 != strncmp( psData->szRecord + DETAIL_RESERVE_LEN, poRecord->szSysType, strlen( poRecord->szSysType ) ) )
		{
			continue;
		}
		else
		{
			psData->poRecord = poRecord;

			return MY_SUCCEED;
		}
	}
	
	return MY_FAIL;
}

/********************************************************** 
function: BuildDetailRecord
description: 规整详单存储记录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int BuildDetailRecord( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	long lTotalRecord;
	int nLoop;
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "开始规整详单文件 %s \n", psData->szFileName );
	
	lTotalRecord = 0;
	
	/* 判断当日详单存储文件是否存在 */
	if( !FileIsExist( psData->szStoreFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "FileName: %s, TotalRecrod: %ld \n\n", psData->szFileName, lTotalRecord );
		return MY_SUCCEED;
	}
	
	/* 打开当日详单存储文件 */
	psData->fpDetail = fopen( psData->szStoreFile, "r" );
	if( NULL == psData->fpDetail )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单存储文件 %s 打开失败!\n", psData->szStoreFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		psData->psDetail[nLoop] = NULL;
	}
	
	memset( psData->szRecord, 0, sizeof( psData->szRecord ) );
	
	/* 规整详单文件中的所有记录 */
	while( NULL != fgets( psData->szRecord, sizeof( psData->szRecord ), psData->fpDetail ) )
    {
    	lTotalRecord++;
    	
    	/* 获取详单系统类型 */
    	if( MY_SUCCEED != GetSystemType( psData ) )
    	{
    		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单记录 %s 的系统类型失败!\n", psData->szRecord );
    		return MY_FAIL;
    	}
    	
    	poRecord = psData->poRecord;
    	
    	memset( psData->szSubscriber, 0, sizeof( psData->szSubscriber ) );
	
		/* 提取用户号码 */
		/* 对用户字段解密 */
#ifdef __ENCRYPT__
		if( MY_SUCCEED != EncryptUser( psData->poRecord, psData->szRecord ) ) {
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "加密用户字段失败!\n" );
			return MY_FAIL ;
		}
#endif
		strncpy( psData->szSubscriber, psData->szRecord + DETAIL_RESERVE_LEN + poRecord->nUserLoc, poRecord->nUserLen );
		psData->szSubscriber[poRecord->nUserLen] = 0;
		TrimLeft( TrimRight( psData->szSubscriber ) );
	
    	/* 获取用户号码基址、偏移 */
		GetBaseOffset( psData );
		
		/* 插入详单记录 */
		if( MY_SUCCEED != InsertDetailRecord( psData ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单记录插入失败!\n" );
			fclose( psData->fpDetail );
			return MY_FAIL;
		}
		
    	memset( psData->szRecord, 0, sizeof( psData->szRecord ) );
    }
    
    /* 关闭当日详单存储文件 */
    fclose( psData->fpDetail );
    
    /* 输出规整后的详单记录 */
    if( MY_SUCCEED != ExportDetailRecord( psData ) )
	{
		FreeDetailMemory( psData );
		return MY_FAIL;
	}
	
	/* 释放详单数据空间 */
	FreeDetailMemory( psData );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "FileName: %s, TotalRecrod: %ld \n\n", psData->szFileName, lTotalRecord );
	
	psData->lTotalRecord += lTotalRecord;
				
	return MY_SUCCEED;
}

/********************************************************** 
function: RebuildMobileFile
description: 规整移动号段的当日详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int RebuildMobileFile( PSTORE_DATA psData )
{
	PSTORE_HEAD psHead;
	PSTORE_CONTROL psControl ;
	int nLoop = 0, nLoop1 = 0;

	/* 遍历所有移动号段当日详单存储文件 */
	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		psHead = psData->psHead[nLoop1];
		if( psHead == NULL ) {
			break;
		}

		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			/*if( NULL != psData->psControl[nLoop] )*/
			if( psHead->psControl[nLoop] != NULL )
			{
				psControl = psHead->psControl[nLoop];
				/* 是否指定号段 */
				if( 0 != strcasecmp( psData->rCmd.szBuildKey, BUILD_KEY_ALL ) )
				{
					/*if( 0 != strcmp(psData->psControl[nLoop]->szGroupKey, psData->rCmd.szBuildKey))*/
					if( 0 != strcmp( psControl->szGroupKey, psData->rCmd.szBuildKey ) )
					{
						continue;
					}
				}
				
				/*psData->psCtrl = psData->psControl[nLoop];*/
				psData->psCtrl = psControl;
				psData->lTotalHead++;
				
				/* 生成当日详单存储文件名 */
				memset( psData->szFileName, 0, sizeof( psData->szFileName ) );
				memset( psData->szStoreFile, 0, sizeof( psData->szStoreFile ) );
			
				sprintf( psData->szFileName, "%s.%8.8s", psData->psCtrl->szGroupKey, psData->rCmd.szBuildDate );
				sprintf( psData->szStoreFile, "%s/%s/%s/%s/%s", psData->psCtrl->szRootDir, psData->psCtrl->szBaseDir, psData->psCtrl->szGroupKey, STORE_DIR_CURR, psData->szFileName );
				
				/* 规整指定的当日详单存储文件 */
				if( MY_SUCCEED != BuildDetailRecord( psData ) )
				{
					return MY_FAIL;
				}

			}
		}
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: RebuildFixedFile
description: 规整固定号段的当日详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int RebuildFixedFile( PSTORE_DATA psData )
{
	DIR * dirp;
	struct dirent * entry;
	char szFixedDir[MAX_PATH_LEN + 1];
	
	/* 存储位置是否定义 */
	if( NULL == psData->pscFixed )
	{
		return MY_SUCCEED;
	}
	
	/* 是否指定号段 */
	if( 0 != strcasecmp( psData->rCmd.szBuildKey, BUILD_KEY_ALL ) )
	{
		if( 0 != strcmp( psData->pscFixed->szGroupKey, psData->rCmd.szBuildKey ) )
		{
			return MY_SUCCEED;
		}
	}	
	
	psData->psCtrl = psData->pscFixed;

	memset( szFixedDir, 0, sizeof( szFixedDir ) );
	sprintf( szFixedDir, "%s/%s/%s/%s/", psData->psCtrl->szRootDir, psData->psCtrl->szBaseDir, psData->psCtrl->szGroupKey, STORE_DIR_CURR );
	
	/* 打开入口目录 */
	dirp = ( opendir( szFixedDir ) );
	if( NULL == dirp )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "固定号码详单存储目录 %s 打开失败!\n", szFixedDir );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	/* 处理入口目录下所有符合条件的文件 */
	while( NULL != ( entry = readdir( dirp ) ) )
	{
		if( 0 != strncmp( entry->d_name, psData->psCtrl->szGroupKey, strlen( psData->psCtrl->szGroupKey ) ) )
		{
			continue;
		}

		if( 0 != strcmp( psData->rCmd.szBuildDate, strchr( strchr( entry->d_name, '.' ) + 1, '.' ) + 1 ) )
		{
			continue;
		}
		
		psData->lTotalHead++;
		
		memset( psData->szFileName, 0, sizeof( psData->szFileName ) );
		memset( psData->szStoreFile, 0, sizeof( psData->szStoreFile ) );
	
		strcpy( psData->szFileName, entry->d_name );
		sprintf( psData->szStoreFile, "%s/%s", szFixedDir, psData->szFileName );
		
		/* 规整指定的当日详单存储文件 */
		if( MY_SUCCEED != BuildDetailRecord( psData ) )
		{
			closedir( dirp );
			return MY_FAIL;
		}
	}
		
	closedir( dirp );
			
	return MY_SUCCEED;
}

/********************************************************** 
function: RebuildDetailFile
description: 规整当日详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int RebuildDetailFile( PSTORE_DATA psData )
{
	int nLoop;
	
	psData->lTotalRecord = 0;
	psData->lTotalHead   = 0;
	
	memset( psData->szStartTime, 0, sizeof( psData->szStartTime ) );
	memset( psData->szEndTime  , 0, sizeof( psData->szEndTime ) );
	
	GetCurrTime( psData->szStartTime );
	
	/* 规整移动号段的当日详单存储文件 */
	if( MY_SUCCEED != RebuildMobileFile( psData ) )
	{
		return MY_FAIL;
	}
	
	/* 规整固定号段的当日详单存储文件 */
	if( MY_SUCCEED != RebuildFixedFile( psData ) )
	{
		return MY_FAIL;
	}
	
	GetCurrTime( psData->szEndTime );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "处理开始时间: %s\n"   , psData->szStartTime );
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "处理结束时间: %s\n\n" , psData->szEndTime );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "处理文件总计: %ld\n"  , psData->lTotalHead );
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "处理详单总计: %ld\n\n", psData->lTotalRecord );
	
	return MY_SUCCEED;
}
