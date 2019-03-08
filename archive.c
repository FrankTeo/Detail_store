

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd. 
file_name: archive.c   
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 归档、压缩往月详单存储文件
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
			modification: 修改内容
		 2. ... 
********************************************************************/


#include "cdef.h"
#include "zlib.h"


/********************************************************** 
function: DebugOutUserIndex
description: 打印用户详单索引信息
Input: psData: 描述系统全局结构的结构体
	   nUser: 用户编号
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int DebugOutUserIndex( PSTORE_DATA psData, int nUser )
{
	PBASE_INDEX pbIndex;
	PSERVICE_INDEX psIndex;
	PSERVICE_INDEX psiUser;
	
	pbIndex = &(psData->pbIndex[nUser]);
	
	/* 不存在往日详单, 直接返回 */
	if( NULL == pbIndex->psIndex )
	{
		return MY_SUCCEED;
	}
	
	psIndex = pbIndex->psIndex;
	
	printf(	"\n\t== User %04d Index Info ==\n\n", nUser );
	
	/* 遍历用户所有业务的往日详单 */
	while( NULL != psIndex )
	{
		printf( "SysType: %s, DetailLen: %ld\n", psIndex->szSysType, psIndex->ulOriginLen );
		
		psiUser = psIndex->psiUser;
		
		while( NULL != psiUser )
		{
			printf( "%02d %04d %s %6ld %6ld\n", psiUser->nDay, psiUser->nUserNo, psiUser->szSysType, psiUser->ulDetailOff, psiUser->ulDetailLen );
			
			psiUser = psiUser->psiUser;
		}
		
		psIndex = psIndex->psiNext;
	}

	printf(	"\n\t== User %04d Index Info ==\n\n", nUser );
	
	return MY_SUCCEED;
}

/********************************************************** 
function: OpenMonthFile
description: 打开往月归档、压缩文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int OpenMonthFile( PSTORE_DATA psData )
{
	char szMonthFile[MAX_PATH_LEN + 1];
	int nLoop;
	
	memset( szMonthFile, 0, sizeof( szMonthFile ) );
	
	sprintf( szMonthFile, "%s/%s/%s/%s.%s%s%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_MONTH, 
												psData->szStoreHead, psData->aCmd.szArchMonth, 
												MONTH_FILE_TAIL, TMP_FILE_TAIL );
	
	/* 打开往月详单存储文件 */
	psData->fpMonthDetail = fopen( szMonthFile, "w" );
	if( NULL == psData->fpMonthDetail )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往月归档详单文件 %s 打开失败!\n", szMonthFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	memset( szMonthFile, 0, sizeof( szMonthFile ) );
	
	sprintf( szMonthFile, "%s/%s/%s/%s.%s%s%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_INDEX, 
												psData->szStoreHead, psData->aCmd.szArchMonth, INDEX_FILE_TAIL,
												TMP_FILE_TAIL );
	
	/* 打开往月详单索引文件 */
	psData->fpMonthIndex = fopen( szMonthFile, "w" );
	if( NULL == psData->fpMonthIndex )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往月归档索引文件 %s 打开失败!\n", szMonthFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 定位详单索引信息存储位置 */
	if( 0 != fseek( psData->fpMonthIndex, MAX_HEAD_USER * sizeof( BASE_INDEX ), SEEK_SET ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位详单索引信息存储位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: OpenDetailFile
description: 打开往日详单存储文件
Input: psData: 描述系统全局结构的结构体
	   nDay: 指定打开哪一天的存储文件
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int OpenDetailFile( PSTORE_DATA psData, int nDay )
{
	memset( psData->szStoreFile, 0, sizeof( psData->szStoreFile ) );
	
	sprintf( psData->szStoreFile, "%s/%s/%s/%s.%s%02d%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_DAY, 
												psData->szStoreHead, psData->aCmd.szArchMonth, nDay, DAY_FILE_TAIL );
	
	/* 打开往日详单存储文件 */
	psData->fpDetail = fopen( psData->szStoreFile, "r" );
	if( NULL == psData->fpDetail )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往日详单存储文件 %s 打开失败!\n", psData->szStoreFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}

	return MY_SUCCEED;
}

/********************************************************** 
function: OpenDayIndex
description: 打开指定一天的索引文件
Input: psData: 描述系统全局结构的结构体
	   nDay: 指定哪一天的索引文件
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int OpenDayIndex( PSTORE_DATA psData, int nDay )
{
	memset( psData->szStoreFile, 0, sizeof( psData->szStoreFile ) );
	
	sprintf( psData->szStoreFile, "%s/%s/%s/%s.%s%02d%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_DAY, 
												psData->szStoreHead, psData->aCmd.szArchMonth, nDay, DAY_FILE_TAIL );
	
	/* 判断是否存在对应的往日详单存储文件 */
	if( !FileIsExist( psData->szStoreFile ) )
	{
		return MY_SUCCEED;
	}

	memset( psData->szIndexFile, 0, sizeof( psData->szIndexFile ) );
	
	sprintf( psData->szIndexFile, "%s/%s/%s/%s.%s%02d%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_INDEX, 
												psData->szStoreHead, psData->aCmd.szArchMonth, nDay, INDEX_FILE_TAIL );
	
	/* 判断索引文件是否存在 */
	if( !FileIsExist( psData->szIndexFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "索引文件 %s 不存在!\n", psData->szIndexFile );
		return MY_FAIL;
	}
	
	/* 打开往日详单索引文件 */
	psData->fpIndex = fopen( psData->szIndexFile, "r" );
	if( NULL == psData->fpIndex )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往日详单索引文件 %s 打开失败!\n", psData->szIndexFile );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetServiceIndex
description: 获取用户业务索引信息
Input: psData: 描述系统全局结构的结构体
	   pbIndex: 索引信息
	   nDay: 指定哪一天的索引文件
	   nUser: 用户编号
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetServiceIndex( PSTORE_DATA psData, PBASE_INDEX pbIndex, int nDay, int nUser )
{
	/* 定位业务索引信息存储位置 */
	if( 0 != fseek( psData->fpIndex, nUser * sizeof( BASE_INDEX ), SEEK_SET ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位业务索引信息存储位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 读取用户业务索引信息 */
	if( 1 != fread( pbIndex, sizeof( BASE_INDEX ), 1, psData->fpIndex ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "读取业务索引信息失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 校验用户编号 */
	if( nUser != pbIndex->nUserNo )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "索引信息用户编号 %d 与用户 %d 不符!\n", pbIndex->nUserNo, nUser  );
		return MY_FAIL;
	}
	
	/* 校验服务数目 */
	if( 0 > pbIndex->nService )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "业务数目 %d 错误!\n", pbIndex->nService  );
		return MY_FAIL;
	}
	
	/* 校验索引文件时间 */
	if( nDay != pbIndex->nDay )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "索引文件时间 %d 与归档时间 %d 不符!\n", pbIndex->nDay, nDay  );
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetDetailIndex
description: 获取用户详单索引信息
Input: psData: 描述系统全局结构的结构体
	   pbIndex: 业务索引信息
	   psIndex: 详单索引信息
	   nDay: 指定哪一天的索引文件
	   nUser: 用户编号
	   nService:  业务序号
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetDetailIndex( PSTORE_DATA psData, PBASE_INDEX pbIndex, PSERVICE_INDEX psIndex, int nDay, int nUser, int nService )
{
	/* 定位详单索引信息存储位置 */
	if( 0 != fseek( psData->fpIndex, pbIndex->ulOffset + nService * sizeof( SERVICE_INDEX ), SEEK_SET ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位详单索引信息存储位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
		
	/* 读取详单索引信息 */
	if( 1 != fread( psIndex, sizeof( SERVICE_INDEX ), 1, psData->fpIndex ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "读取详单索引信息失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 校验用户编号 */
	if( nUser != pbIndex->nUserNo )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "索引信息用户编号 %d 与用户 %d 不符!\n", pbIndex->nUserNo, nUser  );
		return MY_FAIL;
	}
	
	/* 校验索引文件时间 */
	if( nDay != pbIndex->nDay )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "索引文件时间 %d 与归档时间 %d 不符!\n", pbIndex->nDay, nDay  );
		return MY_FAIL;
	}
	
	/* printf( "%s %ld %ld %ld\n", psIndex->szSysType, psIndex->ulDetailOff, psIndex->ulOriginLen, psIndex->nUserNo ); */
			
	return MY_SUCCEED;
}
		
/********************************************************** 
function: GetUserIndexInfo
description: 获取指定用户的索引文件信息
Input: psData: 描述系统全局结构的结构体
	   nDay: 指定哪一天的索引文件
	   nUser: 用户编号
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetUserIndexInfo( PSTORE_DATA psData, int nDay, int nUser )
{
	PBASE_INDEX pbIndex;
	BASE_INDEX bIndex;
	PSERVICE_INDEX psIndex;
	PSERVICE_INDEX psiUser;
	SERVICE_INDEX sIndex;
	
	int nLoop;
	
	memset( &bIndex, 0, sizeof( BASE_INDEX ) );
	
	/* 获取用户业务索引信息 */
	if( MY_SUCCEED != GetServiceIndex( psData, &bIndex, nDay, nUser ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取业务索引信息失败!\n" );
		return MY_FAIL;
	}
	
	if( 0 == bIndex.nService )
	{
		return MY_SUCCEED;
	}
	
	/* printf( "User: %04d, BaseIndex: %ld %04d %d\n", nUser, bIndex.ulOffset, bIndex.nUserNo, bIndex.nService ); */
	
	pbIndex = &(psData->pbIndex[nUser]);

	/* 获取所有业务的详单索引信息 */
	for( nLoop = 0; nLoop < bIndex.nService; nLoop++ )
	{
		memset( &sIndex, 0, sizeof( SERVICE_INDEX ) );
		
		/* 获取用户详单索引信息 */
		if( MY_SUCCEED != GetDetailIndex( psData, &bIndex, &sIndex, nDay, nUser, nLoop ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取详单索引信息失败!\n" );
			return MY_FAIL;
		}
		
		if( NULL == pbIndex->psIndex )
		{
			pbIndex->nService++;

			pbIndex->psIndex = ( PSERVICE_INDEX ) malloc( sizeof( SERVICE_INDEX ) );
			if( NULL == pbIndex->psIndex )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存详单索引信息的内存空间失败!\n" );
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
				return MY_FAIL;
			}
			
			psIndex = pbIndex->psIndex;
			
			memset( psIndex, 0, sizeof( SERVICE_INDEX ) );
			memset( psIndex->szSysType, 0, sizeof( psIndex->szSysType ) );
			
			strcpy( psIndex->szSysType, sIndex.szSysType );
			
			psIndex->ulDetailLen = 0;
			psIndex->ulOriginLen = 0;
			psIndex->nUserNo     = nUser;
			psIndex->nDay        = atoi( psData->aCmd.szArchMonth + 4 );;
			
			psIndex->psiUser = NULL;
			psIndex->psiNext = NULL;
		}
		else
		{
			psIndex = pbIndex->psIndex;
			
			while( NULL != psIndex )
			{
				if( 0 == strcmp( psIndex->szSysType, sIndex.szSysType ) )
				{
					break;
				}
				
				if( NULL == psIndex->psiNext )
				{
					pbIndex->nService++;
					
					psIndex->psiNext = ( PSERVICE_INDEX ) malloc( sizeof( SERVICE_INDEX ) );
					if( NULL == psIndex->psiNext )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存详单索引信息的内存空间失败!\n" );
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
						return MY_FAIL;
					}
			
					psIndex = psIndex->psiNext;
			
					memset( psIndex, 0, sizeof( SERVICE_INDEX ) );
					memset( psIndex->szSysType, 0, sizeof( psIndex->szSysType ) );
			
					strcpy( psIndex->szSysType, sIndex.szSysType );
			
					psIndex->ulDetailLen = 0;
					psIndex->ulOriginLen = 0;
					psIndex->nUserNo     = nUser;
					psIndex->nDay        = atoi( psData->aCmd.szArchMonth + 4 );;
					
					psIndex->psiUser = NULL;
					psIndex->psiNext = NULL;
					
					break;					
				}
				
				psIndex = psIndex->psiNext;
			}
		}
		
		if( NULL == psIndex->psiUser )
		{
			psIndex->psiUser = ( PSERVICE_INDEX ) malloc( sizeof( SERVICE_INDEX ) );
			if( NULL == psIndex->psiUser )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存同一业务详单索引信息的内存空间失败!\n" );
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
				return MY_FAIL;
			}
		
			psiUser = psIndex->psiUser;
			
			memset( psiUser, 0, sizeof( SERVICE_INDEX ) );
			memcpy( psiUser, &sIndex, sizeof( SERVICE_INDEX ) );
			
			psIndex->ulOriginLen += psiUser->ulDetailLen;
		}
		else
		{
			psiUser = psIndex->psiUser;
			
			while( NULL != psiUser )
			{
				if( NULL == psiUser->psiUser )
				{
					break;
				}
				
				psiUser = psiUser->psiUser;
			}
			
			psiUser->psiUser = ( PSERVICE_INDEX ) malloc( sizeof( SERVICE_INDEX ) );
			if( NULL == psiUser->psiUser )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存同一业务详单索引信息的内存空间失败!\n" );
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
				return MY_FAIL;
			}
		
			psiUser = psiUser->psiUser;
			
			memset( psiUser, 0, sizeof( SERVICE_INDEX ) );
			memcpy( psiUser, &sIndex, sizeof( SERVICE_INDEX ) );
			
			psIndex->ulOriginLen += psiUser->ulDetailLen;
		}
	}
		
	return MY_SUCCEED;
}

/********************************************************** 
function: GetMonthIndexInfo
description: 获取索引文件信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int GetMonthIndexInfo( PSTORE_DATA psData )
{
	PBASE_INDEX pbIndex;
	int nDay, nUser;
	int nLoop;
	
	/* 初始化所有用户的索引信息 */
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		pbIndex = &(psData->pbIndex[nLoop]);
		
		memset( pbIndex, 0, sizeof( BASE_INDEX ) );
		
		pbIndex->psIndex  = NULL;
		pbIndex->ulOffset = 0;
		pbIndex->nUserNo  = nLoop;
		pbIndex->nDay     = atoi( psData->aCmd.szArchMonth + 4 );
		pbIndex->nService = 0;
	}
	
	/* 获取归档月所有往日详单的索引信息 */	
	for( nDay = 1; nDay <= MAX_MONTH_DAY; nDay++ )
	{
		if( MY_SUCCEED != OpenDayIndex( psData, nDay ) )
		{
			return MY_FAIL;
		}
		
		if( NULL == psData->fpIndex )
		{
			continue;
		}
		
		psData->nDealFile++;		
		
		/* 获取所有用户的往日详单索引信息 */	
		for( nUser = 0; nUser < MAX_HEAD_USER; nUser++ )
		{
			if( MY_SUCCEED != GetUserIndexInfo( psData, nDay, nUser ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "获取索引文件 %s 中的索引信息失败!\n", psData->szIndexFile  );
				
				if( NULL != psData->fpIndex )
				{
					fclose( psData->fpIndex );
					psData->fpIndex = NULL;
				}
		
				return MY_FAIL;
			}
			
			/* DebugOutUserIndex( psData, nUser ); */
		}
		
		if( NULL != psData->fpIndex )
		{
			fclose( psData->fpIndex );
			psData->fpIndex = NULL;
		}
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: CompressServiceDetail
description: 压缩详单记录
Input: psData: 描述系统全局结构的结构体
	   psIndex: 详单索引信息
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int CompressServiceDetail( PSTORE_DATA psData, PSERVICE_INDEX psIndex )
{
	ulong ulOriginLen;
	int nErrorCode;
	
	psIndex->ulDetailLen = psIndex->ulOriginLen;
	
	/* 压缩归档详单数据区的详单数据 */
	nErrorCode = compress( ( Bytef * ) psData->szCompDetail, &(psIndex->ulDetailLen), ( const Bytef * ) psData->szArchDetail, psIndex->ulOriginLen );
    if( Z_OK != nErrorCode ) 
    {
    	MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "压缩归档详单数据失败, ErrorCode: %d!\n", nErrorCode );
		return MY_FAIL;
    }
/*    
    ulOriginLen = psIndex->ulOriginLen;
    
	nErrorCode = uncompress( ( Bytef * ) psData->szArchDetail, &(psIndex->ulOriginLen), ( const Bytef * ) psData->szCompDetail, psIndex->ulDetailLen );
    if( Z_OK != nErrorCode )
    {
        MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "解压缩往月归档详单数据失败, ErrorCode: %d!\n", nErrorCode );
        return MY_FAIL;
    }

	if( ulOriginLen != psIndex->ulOriginLen )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "归档详单数据解压缩后长度 %ld 与压缩前 %ld 不符!\n", psIndex->ulOriginLen, ulOriginLen );
		return MY_FAIL;
	}
	
	printf( "%s\n", psData->szArchDetail );
*/
	psIndex->ulDetailOff = psData->ulDetailOff;
	psData->ulDetailOff += psIndex->ulDetailLen;
	
	/* 输出归档、压缩详单记录 */
	if( psIndex->ulDetailLen != fwrite( psData->szCompDetail, sizeof( char ), psIndex->ulDetailLen, psData->fpMonthDetail ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出归档、压缩详单记录失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 输出归档、压缩详单索引信息 */
	if( 1 != fwrite( psIndex, sizeof( SERVICE_INDEX ), 1, psData->fpMonthIndex ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出归档、压缩详单索引信息失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/*
	printf( "ServiceIndnex: %04d %02d %s %6ld %6ld %6ld\n", 
													psIndex->nUserNo, psIndex->nDay, psIndex->szSysType, 
													psIndex->ulDetailOff, psIndex->ulDetailLen, psIndex->ulOriginLen );
	*/

	return MY_SUCCEED;
}

/********************************************************** 
function: ArchiveServiceDetail
description: 归档指定用户的详单记录
Input: psData: 描述系统全局结构的结构体
	   psiUser: 详单索引信息
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ArchiveServiceDetail( PSTORE_DATA psData, PSERVICE_INDEX psIndex )
{
	PSERVICE_INDEX psiUser;
	
	psiUser = psIndex->psiUser;
	
	while( NULL != psiUser )
	{
		/* printf( "%02d %04d %s %6ld %6ld\n", psiUser->nDay, psiUser->nUserNo, psiUser->szSysType, psiUser->ulDetailOff, psiUser->ulDetailLen ); */
		
		if( MY_SUCCEED != OpenDetailFile( psData, psiUser->nDay ) )
		{
			return MY_FAIL;
		}
		
		/* 定位详单数据存储位置 */
		if( 0 != fseek( psData->fpDetail, psiUser->ulDetailOff, SEEK_SET ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位详单数据存储位置失败!\n" );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			
			if( NULL != psData->fpDetail )
			{
				fclose( psData->fpDetail );
				psData->fpDetail = NULL;
			}
		
			return MY_FAIL;
		}
		
		/* 长度校验 */
		if( psIndex->ulOriginLen < psData->ulBuffOffset + psiUser->ulDetailLen )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "归档详单数据区溢出( %ld - %ld )!\n", 
																				psIndex->ulOriginLen, psData->ulBuffOffset + psiUser->ulDetailLen );
			return MY_FAIL;
		}
		
		/* 读取详单数据 */
		if( psiUser->ulDetailLen != fread( psData->szArchDetail + psData->ulBuffOffset, sizeof( char ), psiUser->ulDetailLen, psData->fpDetail ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "读取详单数据失败!\n" );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			
			if( NULL != psData->fpDetail )
			{
				fclose( psData->fpDetail );
				psData->fpDetail = NULL;
			}
			
			return MY_FAIL;
		}	
		
		psData->ulBuffOffset += psiUser->ulDetailLen;
		
		/* printf( "%s\n", psData->szArchDetail ); */
		
		if( NULL != psData->fpDetail )
		{
			fclose( psData->fpDetail );
			psData->fpDetail = NULL;
		}
		
		psiUser = psiUser->psiUser;
	}

	return MY_SUCCEED;
}
	
/********************************************************** 
function: ArchiveUserDetail
description: 归档、压缩指定用户的详单记录
Input: psData: 描述系统全局结构的结构体
	   nUser: 用户编号
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ArchiveUserDetail( PSTORE_DATA psData, int nUser )
{
	PBASE_INDEX pbIndex;
	PSERVICE_INDEX psIndex;
	PSERVICE_INDEX psiUser;
	
	pbIndex = &(psData->pbIndex[nUser]);
	
	if( NULL == pbIndex->psIndex )
	{
		return MY_SUCCEED;
	}
	
	pbIndex->ulOffset = psData->ulIndexOff;
	
	psIndex = pbIndex->psIndex;
	
	while( NULL != psIndex )
	{
		if( NULL == psIndex->psiUser )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "用户 %04d 的详单索引信息为空!\n", nUser );
			return MY_FAIL;
		}
		
		/* 申请详单归档数据区 */
		psData->szArchDetail = ( char * ) malloc( psIndex->ulOriginLen + 1 );
		if( NULL == psData->szArchDetail )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存归档详单记录的内存空间失败!\n" );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}

		memset( psData->szArchDetail, 0, psIndex->ulOriginLen + 1 );
		
		/* 申请详单压缩数据区 */
		psData->szCompDetail = ( char * ) malloc( psIndex->ulOriginLen + 1 );
		if( NULL == psData->szCompDetail )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存压缩详单记录的内存空间失败!\n" );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}
		
		memset( psData->szCompDetail, 0, psIndex->ulOriginLen + 1 );
		
		psData->ulBuffOffset = 0;
		
		/* 归档用户详单 */
		if( MY_SUCCEED != ArchiveServiceDetail( psData, psIndex ) )
		{
			return MY_FAIL;
		}
		
		/* 压缩用户归档详单 */	
		if( MY_SUCCEED != CompressServiceDetail( psData, psIndex ) )
		{
			return MY_FAIL;
		}
		
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
		
		psIndex = psIndex->psiNext;
	}
	
	psData->ulIndexOff += pbIndex->nService * sizeof( SERVICE_INDEX );
	
	/* printf( "%04d %02d %6ld %2d\n", pbIndex->nUserNo, pbIndex->nDay, pbIndex->ulOffset, pbIndex->nService ); */

	return MY_SUCCEED;
}

/********************************************************** 
function: MoveTmpMonthFile
description: 生成正式的输出文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int MoveTmpMonthFile( PSTORE_DATA psData )
{
	char szSrcFile[MAX_PATH_LEN + 1];
	char szDestFile[MAX_PATH_LEN + 1];

	memset( szSrcFile , 0, sizeof( szSrcFile ) );
	memset( szDestFile, 0, sizeof( szDestFile ) );
	
	sprintf( szSrcFile, "%s/%s/%s/%s.%s%s%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_MONTH, 
												psData->szStoreHead, psData->aCmd.szArchMonth, 
												MONTH_FILE_TAIL, TMP_FILE_TAIL );
	
	sprintf( szDestFile, "%s/%s/%s/%s.%s%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_MONTH, 
												psData->szStoreHead, psData->aCmd.szArchMonth, MONTH_FILE_TAIL );

	/* 生成正式的往月详单存储文件 */
	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "生成正式往月详单存储文件失败!\n" );
		return MY_FAIL;
	}
	
	sprintf( szSrcFile, "%s/%s/%s/%s.%s%s%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_INDEX, 
												psData->szStoreHead, psData->aCmd.szArchMonth, INDEX_FILE_TAIL,
												TMP_FILE_TAIL );
	
	sprintf( szDestFile, "%s/%s/%s/%s.%s%s", 
												psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_INDEX, 
												psData->szStoreHead, psData->aCmd.szArchMonth, INDEX_FILE_TAIL );
	
	/* 生成正式的往月详单索引文件 */
	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "备份当日详单索引文件失败!\n" );
		return MY_FAIL;
	}
								
	return MY_SUCCEED;	
}

/********************************************************** 
function: FreeUserIndex
description: 生成正式的输出文件
Input: psiUser: 用户详单信息
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeUserIndex( PSERVICE_INDEX psiUser )
{
	while( NULL != psiUser->psiUser )
	{
		FreeUserIndex( psiUser->psiUser );
		psiUser->psiUser = NULL;
	}
	
	free( psiUser );
	psiUser = NULL;
	
	return MY_SUCCEED;
}

/********************************************************** 
function: FreeServiceIndex
description: 释放业务详单信息内存空间
Input: psIndex 业务详单信息
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeServiceIndex( PSERVICE_INDEX psIndex )
{
	while( NULL != psIndex->psiNext )
	{
		FreeServiceIndex( psIndex->psiNext );
		psIndex->psiNext = NULL;
	}
	
	FreeUserIndex( psIndex->psiUser );
	psIndex->psiUser = NULL;
	
	free( psIndex );
	psIndex = NULL;
	
	return MY_SUCCEED;
}

/********************************************************** 
function: FreeFileList
description: 释放文件列表内存空间
Input: pfList 文件列表信息
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeFileList( PFILE_LIST pfList )
{
	while( NULL != pfList->pflNext )
	{
		FreeFileList( pfList->pflNext );
		pfList->pflNext = NULL;
	}
	
	free( pfList );
	pfList = NULL;

	return MY_SUCCEED;
}

/********************************************************** 
function: FreeMonthMemory
description: 释放往月详单索引信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int FreeMonthMemory( PSTORE_DATA psData )
{
	PBASE_INDEX pbIndex;
	int nLoop;
	
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		pbIndex = &(psData->pbIndex[nLoop]);
		
		if( NULL != pbIndex->psIndex )
		{
			FreeServiceIndex( pbIndex->psIndex );
			pbIndex->psIndex = NULL;
		}
	}
	
	if( NULL != psData->fpMonthDetail )
	{
		fclose( psData->fpMonthDetail );
		psData->fpMonthDetail = NULL;
	}
	
	if( NULL != psData->fpMonthIndex )
	{
		fclose( psData->fpMonthIndex );
		psData->fpMonthIndex = NULL;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: ArchiveDetailFile
description: 归档、压缩往月详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ArchiveDetailFile( PSTORE_DATA psData )
{
	PBASE_INDEX pbIndex;
	
	int nLoop;
	
	psData->nDealFile = 0;
	psData->szArchDetail = NULL;
	psData->szCompDetail = NULL;
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "归档 %s 的详单文件\n", psData->szStoreKey );
	
	/* 获取用户归档月的所有往日详单索引信息 */
	if( MY_SUCCEED != GetMonthIndexInfo( psData ) )
	{
		return MY_FAIL;
	}

	if( 0 >= psData->nDealFile )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "不存在 %s 的 %s 的详单文件\n\n", psData->szStoreKey, psData->aCmd.szArchMonth );
		return MY_SUCCEED;
	}
	
	/* 打开往月详单存储文件、往月详单索引文件 */
	if( MY_SUCCEED != OpenMonthFile( psData ) )
	{
		return MY_FAIL;
	}
	
	psData->ulIndexOff  = MAX_HEAD_USER * sizeof( BASE_INDEX );
	psData->ulDetailOff = 0;
	
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		if( MY_SUCCEED != ArchiveUserDetail( psData, nLoop ) )
		{
			return MY_FAIL;
		}
	}
	
	/* 定位至索引文件头部 */
	if( 0 != fseek( psData->fpMonthIndex, 0, SEEK_SET ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "定位往月详单索引文件头部位置失败!\n" );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 输出业务索引信息 */
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		if( 1 != fwrite( &(psData->pbIndex[nLoop]), sizeof( BASE_INDEX ), 1, psData->fpMonthIndex ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "往月详单业务索引信息写失败!\n" );
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
			return MY_FAIL;
		}
	}
	
	/* 输出正式文件 */
	if( MY_SUCCEED != MoveTmpMonthFile( psData ) )
	{
		return MY_FAIL;
	}
	
	FreeMonthMemory( psData );
		
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "成功归档 %s 的 %d 个详单文件\n\n", psData->szStoreKey, psData->nDealFile );
	
	return MY_SUCCEED;
}

/********************************************************** 
function: ArchiveFixedFile
description: 归档、压缩固定号段的往月详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ArchiveFixedFile( PSTORE_DATA psData )
{
	PFILE_LIST pfList;
	DIR * dirp;
	struct dirent * entry;
	char szFixedDir[MAX_PATH_LEN + 1];
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "归档 %s 的详单文件\n", psData->szStoreKey );
	
	psData->pfList = NULL;
	
	memset( szFixedDir, 0, sizeof( szFixedDir ) );
	sprintf( szFixedDir, "%s/%s/%s", psData->aCmd.szFileSystem, psData->szStoreKey, STORE_DIR_DAY );
	
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
		if( 0 != strncmp( entry->d_name, psData->szStoreKey, strlen( psData->szStoreKey ) ) )
		{
			continue;
		}
		
		if( strlen( entry->d_name ) <= ( strlen( psData->szStoreKey ) + 1 + 8 + strlen( DAY_FILE_TAIL ) + 1 ) )
		{
			continue;
		}

		if( 0 != strcmp( entry->d_name + strlen( entry->d_name ) - strlen( DAY_FILE_TAIL ), DAY_FILE_TAIL ) )
		{
			continue;
		}
		
		memset( psData->szStoreHead, 0, sizeof( psData->szStoreHead ) );
		strcpy( psData->szStoreHead, entry->d_name );
		psData->szStoreHead[strlen( entry->d_name ) - 8 - strlen( DAY_FILE_TAIL ) - 1] = 0;
		
		if( NULL == psData->pfList )
		{
			psData->pfList = ( PFILE_LIST ) malloc( sizeof( FILE_LIST ) );
			if( NULL == psData->pfList )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存往日详单存储文件列表的内存空间失败!\n" );
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
				return MY_FAIL;
			}
			
			pfList = psData->pfList;
			
			memset( pfList, 0, sizeof( FILE_LIST ) );
			memset( pfList->szFile, 0, sizeof( pfList->szFile ) );
			
			strcpy( pfList->szFile, psData->szStoreHead );
		}
		else
		{
			pfList = psData->pfList;
			
			while( NULL != pfList )
			{
				if( 0 == strcmp( pfList->szFile, psData->szStoreHead ) )
				{
					break;
				}
				
				if( NULL == pfList->pflNext )
				{
					pfList->pflNext = ( PFILE_LIST ) malloc( sizeof( FILE_LIST ) );
					if( NULL == pfList->pflNext )
					{
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存往日详单存储文件列表的内存空间失败!\n" );
						MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
						return MY_FAIL;
					}
			
					pfList = pfList->pflNext;
			
					memset( pfList, 0, sizeof( FILE_LIST ) );
					memset( pfList->szFile, 0, sizeof( pfList->szFile ) );
			
					strcpy( pfList->szFile, psData->szStoreHead );
				}
			
				pfList = pfList->pflNext;
			}
		}
	}
		
	closedir( dirp );
	
	pfList = psData->pfList;
	
	while( NULL != pfList )
	{
   		memset( psData->szStoreHead, 0, sizeof( psData->szStoreHead ) );
    		
   		strcpy( psData->szStoreHead, pfList->szFile );
    		
   		if( MY_SUCCEED != ArchiveDetailFile( psData ) )
   		{
   			return MY_FAIL;
   		}
		
		pfList = pfList->pflNext;
	}
	
	if( NULL != psData->pfList )
	{
		FreeFileList( psData->pfList );
		psData->pfList = NULL;
	}
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "成功归档 %s 的详单文件\n\n", psData->szStoreKey );
	
	return MY_SUCCEED;
}

/********************************************************** 
function: ArchiveMonthDetail
description: 归档、压缩往月详单存储文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int ArchiveMonthDetail( PSTORE_DATA psData )
{
	DIR * dirSystem;
	struct dirent * entrySystem;
	
	memset( psData->szStartTime, 0, sizeof( psData->szStartTime ) );
	memset( psData->szEndTime  , 0, sizeof( psData->szEndTime ) );
	
	GetCurrTime( psData->szStartTime );
	
	/* 打开文件系统 */
	dirSystem = ( opendir( psData->aCmd.szFileSystem ) );
	if( NULL == dirSystem )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "详单存储文件系统 %s 打开失败!\n", psData->aCmd.szFileSystem );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
	
	/* 遍历文件系统下的所有存储目录 */
	while( NULL != ( entrySystem = readdir( dirSystem ) ) )
	{
		memset( psData->szStoreDir, 0, sizeof( psData->szStoreDir ) );
		sprintf( psData->szStoreDir, "%s/%s", psData->aCmd.szFileSystem, entrySystem->d_name );
		
    	if( !IsDir( psData->szStoreDir ) )
    	{
    		continue;
    	}
    	
    	if( MOBILE_HEAD_LEN != strlen( entrySystem->d_name ) && 0 != strcmp( entrySystem->d_name, "fixed" ) )
    	{
    		continue;
    	}
    	
    	if( IsMobileHead( entrySystem->d_name ) && MOBILE_HEAD_LEN == strlen( entrySystem->d_name ) )
    	{
    		memset( psData->szStoreKey, 0, sizeof( psData->szStoreKey ) );
    		memset( psData->szStoreHead, 0, sizeof( psData->szStoreHead ) );
    		
    		strcpy( psData->szStoreKey, entrySystem->d_name );
    		strcpy( psData->szStoreHead, entrySystem->d_name );
    		
    		if( MY_SUCCEED != ArchiveDetailFile( psData ) )
    		{
    			return MY_FAIL;
    		}
    	}
    	else if( 0 == strcmp( entrySystem->d_name, "fixed" ) )
    	{
    		memset( psData->szStoreKey, 0, sizeof( psData->szStoreKey ) );
    		strcpy( psData->szStoreKey, entrySystem->d_name );
    		
    		if( MY_SUCCEED != ArchiveFixedFile( psData ) )
    		{
    			return MY_FAIL;
    		}
    	}
    	else
    	{
    		continue;
    	}
    }
	
	closedir( dirSystem );
	
	GetCurrTime( psData->szEndTime );
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "归档开始时间: %s\n"   , psData->szStartTime );
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "归档结束时间: %s\n\n" , psData->szEndTime );
	
	return MY_SUCCEED;
}
