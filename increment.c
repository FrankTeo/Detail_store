

/********************************************************************
copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
file_name: increment.c  
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 累计详单增量统计信息
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
            modification: 修改内容
         2. ...
********************************************************************/


#include "cdef.h"


/********************************************************** 
function: DebugOutGroupField
description: 一条输出增量信息
Input:	pgField: 一条详单增量信息
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutGroupField( PGROUP_FIELD pgField )
{
	printf( "\t-- Group Field -- \n" );
	printf( "PhoneNo      = %s\n" , pgField->szPhoneNo );
	printf( "CallType     = %s\n" , pgField->szCallType );
	printf( "FeeType      = %s\n" , pgField->szFeeType );
	printf( "RoamType     = %s\n" , pgField->szRoamType );
	printf( "ChatType     = %s\n" , pgField->szChatType );
	printf( "DialType     = %s\n" , pgField->szDialType );
	printf( "Infotype     = %s\n" , pgField->szInfoType );
	printf( "DistountType = %s\n" , pgField->szDiscountType );
	printf( "SameCity     = %s\n" , pgField->szSameCity );
	printf( "DataType     = %s\n" , pgField->szDataType );
	printf( "Cfee         = %lf\n", pgField->lfCfee );
	printf( "Lfee         = %lf\n", pgField->lfLfee );
	printf( "Ifee         = %lf\n", pgField->lfIfee );
	printf( "OldCfee      = %lf\n", pgField->lfOldCfee );
	printf( "OldLfee      = %lf\n", pgField->lfOldLfee );
	printf( "OldIfee      = %lf\n", pgField->lfOldIfee );
	printf( "CallTimes    = %d\n" , pgField->nCallTimes );
	printf( "Duration     = %d\n" , pgField->nDuration );
	printf( "DealTime     = %s\n" , pgField->szDealTime );
	printf( "Recv         = %s\n" , pgField->szRecv );
	printf( "\t-- Group Field -- \n\n" );
	
	return MY_SUCCEED;
}	

/********************************************************** 
function: DebugOutGroupList
description: 输出用户所有增量信息
Input:	pgList: 用户所有增量信息
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutGroupList( PGROUP_LIST pgList )
{
	while( NULL != pgList )
	{
		DebugOutGroupField( &(pgList->gField) );
		
		pgList = pgList->pglNext;
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: DebugOutPhoneList
description: 输出号段增量信息
Input:	psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int DebugOutPhoneList( PSTORE_DATA psData )
{
	PGROUP_HEAD pgHead; 
	PHEAD_LIST phList;
	PGROUP_LIST pgList;
	int nLoop1 = 0, nLoop2 = 0, nLoop3 = 0;
	
	for( nLoop3 = 0; nLoop3 < MAX_HEAD_NUM; nLoop3++ ) {
		pgHead = psData->pgHead[nLoop3];
                if( pgHead == NULL ) {
                        break;
                }       
		
		for( nLoop1 = 0; nLoop1 < MAX_MOBILE_HEAD; nLoop1++ )
		{
			phList = pgHead->phList[nLoop1];
			if( NULL != phList )
			{
				printf( "\t== Head %s ==\n\n", phList->szPhoneHead );
				for( nLoop2 = 0; nLoop2 < MAX_HEAD_USER; nLoop2++ )
				{
					pgList = phList->pgList[nLoop2];
		
					while( NULL != pgList )
					{
						DebugOutGroupField( &(pgList->gField) );
					
						pgList = pgList->pglNext;
					}
				}
				printf( "\t== Head %s ==\n\n", phList->szPhoneHead );
			}
		}
	}
	return MY_SUCCEED;
}

/********************************************************** 
function: GetFieldValue
description: 提取帐务接口字段信息
Input:	psData: 描述系统全局结构的结构体
		szField: 接口字段
		nFieldNo: 字段编号
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int GetFieldValue( PSTORE_DATA psData, char * szField, int nFieldNo )
{
	PFMT_CTRL pfCtrl;
	PFMT_INFO pfInfo;
	PFMT_REC pfRec;
	char szValue[128];
	int nLoop;
	
	pfCtrl = &(psData->fCtrl);
	pfInfo = pfCtrl->pfInfo + psData->poRecord->nOperNo;
	pfRec =  pfInfo->pfRec;

	/* DebugOutFmtInfo( pfInfo ); */	
	
	/* 匹配配置信息 */
	for( nLoop = 0; nLoop < pfInfo->nFieldCount; nLoop++, pfRec++ )
	{
		if( nFieldNo != pfRec->nOutNo )
		{
			continue;
		}
		
		/* 提取字段内容 */
		if( 0 != strcmp( pfRec->szValueNo, "*" ) )
		{
			strncpy( szValue, psData->szRecord + (pfInfo->pfRec + atoi(pfRec->szValueNo))->nStart, (pfInfo->pfRec + atoi(pfRec->szValueNo))->nLen );
			szValue[(pfInfo->pfRec + atoi(pfRec->szValueNo))->nLen] = 0;
		}
		else
		{
			strncpy( szValue, psData->szRecord + pfRec->nStart, pfRec->nLen );
			szValue[pfRec->nLen] = 0;
		}		

		TrimLeft( TrimRight( szValue ) );

		if( 0 == pfRec->nType )			/* 字符串 */
		{
			strncat( szField, szValue, strlen( szValue ) );
			continue;
		}
		else if( 1 == pfRec->nType )	/* 整数 */
		{
			sprintf( szField, "%ld", atol( szField ) + atol( szValue ) );
			continue;
		}
		else if( 2 == pfRec->nType )	/* 浮点数 */
		{
			sprintf( szField, "%lf", atof( szField ) + atof( szValue ) );
			continue;
		}
	}
	
	return MY_SUCCEED;
}

/********************************************************** 
function: InitGroupField
description: 初始化详单增量信息
Input:	pgField: 详单增量信息
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int InitGroupField( PGROUP_FIELD pgField )
{
	memset( pgField, 0, sizeof( GROUP_FIELD ) );
	
	memset( pgField->szPhoneNo     , 0, sizeof( pgField->szPhoneNo ) );
	memset( pgField->szCallType    , 0, sizeof( pgField->szCallType ) );
	memset( pgField->szFeeType     , 0, sizeof( pgField->szFeeType ) );
	memset( pgField->szRoamType    , 0, sizeof( pgField->szRoamType ) );
	memset( pgField->szChatType    , 0, sizeof( pgField->szChatType ) );
	memset( pgField->szDialType    , 0, sizeof( pgField->szDialType ) );
	memset( pgField->szInfoType    , 0, sizeof( pgField->szInfoType ) );
	memset( pgField->szDiscountType, 0, sizeof( pgField->szDiscountType ) );
	memset( pgField->szSameCity    , 0, sizeof( pgField->szSameCity ) );
	memset( pgField->szDataType    , 0, sizeof( pgField->szDataType ) );

	pgField->lfCfee = 0;
	pgField->lfLfee = 0;
	pgField->lfIfee = 0;
	pgField->lfOldCfee = 0;
	pgField->lfOldLfee = 0;
	pgField->lfOldIfee = 0;
	
	pgField->nCallTimes = 0;
	pgField->nDuration = 0;
	
	memset( pgField->szDealTime, 0, sizeof( pgField->szDealTime) );
	memset( pgField->szSerial, 0, sizeof( pgField->szSerial) );
	memset( pgField->szTalkTime, 0, sizeof( pgField->szTalkTime ) );

	pgField->nPresentInfo = 0 ;

	memset( pgField->szRecv, 0, sizeof( pgField->szRecv ) );
	
	return MY_SUCCEED;
}

/********************************************************** 
function: GetGroupField
description: 提取详单中所有帐务接口字段信息
Input:	psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int GetGroupField( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	PGROUP_FIELD pgField;
	char szField[32];
	int nLoop;

	poRecord = psData->poRecord;
	pgField = &(psData->gField);
	
	InitGroupField( &(psData->gField) );
	
	/* 依次提取每个接口字段的内容 */
	for( nLoop = 0; nLoop < INC_FIELD_NUM; nLoop++ )
	{
		memset( szField, 0, sizeof( szField ) );
		GetFieldValue( psData, szField, nLoop );
		
		if( 0 == *( szField ) || 0 == strlen( szField ) )
		{
			strcpy( szField, "0" );
		}

		switch( nLoop )
		{
			case 0:		/* 用户号码 */
				strcpy( pgField->szPhoneNo, szField );
				break;
    	
			case 1:		/* 通话类型 */
				strcpy( pgField->szCallType, szField );
				break;
			
			case 2:		/* 费用类型 */
				strcpy( pgField->szFeeType, szField );
				break;
			
			case 3:		/* 漫游类型 */
				strcpy( pgField->szRoamType, szField );
				break;
			
			case 4:		/* 对端类型 */
				strcpy( pgField->szChatType, szField );
				break;
			
			case 5:		/* 拨打类型 */
				strcpy( pgField->szDialType, szField );
				break;
			
			case 6:		/* 信息台代码 */
				strcpy( pgField->szInfoType, szField );
				break;
				
			case 7:		/* 优惠代码 */
				strcpy( pgField->szDiscountType, szField );
				break;
			
			case 8:		/* 同城标志 */
				strcpy( pgField->szSameCity, szField );
				break;
				
			case 9:		/* 数据类型 */
				strcpy( pgField->szDataType, poRecord->szDataType );
				break;
				
			case 10:		/* 二次批价后的基本费用 */
				pgField->lfCfee = (double) ( atof( szField ) / poRecord->nFeeUnit );
				break;
				
			case 11:		/* 二次批价后的长途费用 */
				pgField->lfLfee = (double) ( atof( szField ) / poRecord->nFeeUnit );
				break;
			
			case 12:	/* 二次批价后的信息费用 */
				pgField->lfIfee = (double) ( atof( szField ) / poRecord->nFeeUnit );
				break;
				
			case 13:	/* 原始基本费用 */
				pgField->lfOldCfee = (double) ( atof( szField ) / poRecord->nFeeUnit );
				break;
			
			case 14:	/* 原始长途费用 */
				pgField->lfOldLfee = (double) ( atof( szField ) / poRecord->nFeeUnit );
				break;
				
			case 15:	/* 原始信息费用 */
				pgField->lfOldIfee = (double) ( atof( szField ) / poRecord->nFeeUnit );
				break;
				
			case 16:	/* 通话次数 */
				pgField->nCallTimes++;
				break;		
			
			case 17:	/* 通话时长 */
				pgField->nDuration = atol( szField );
				break;
				
			case 18:	/* 处理时间 */
				strcpy( pgField->szDealTime, psData->szCurrBatch );
				break;
				
			case 19:	/* 记录序列号 */
				strcpy( pgField->szSerial, szField );
				break;

			case 20:	/* 最大通话时间 */
				/*printf( "[%s][%s]\n", pgField->szTalkTime, szField );*/
				strcpy( pgField->szTalkTime, szField );
				break;

			case 21:	/* 单条记录的套餐赠送信息 */
				pgField->nPresentInfo = atoi( szField ) ;
				break ;

			case 22:	/* 保留字段 */
				strcpy( pgField->szRecv, szField );
				break;	
		}
	}

	/* DebugOutGroupField( pgField ); */

	return MY_SUCCEED;
}

/********************************************************** 
function: GroupField
description: 累计详单增量统计信息
Input:	psData: 描述系统全局结构的结构体
		pglField: 用户增量统计累计信息
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int GroupField( PSTORE_DATA psData, PGROUP_FIELD pglField )
{
	PGROUP_FIELD pgField;
	
	pgField = &(psData->gField);

	pglField->lfCfee += pgField->lfCfee;
	pglField->lfLfee += pgField->lfLfee;
	pglField->lfIfee += pgField->lfIfee;
	
	pglField->lfOldCfee += pgField->lfOldCfee;
	pglField->lfOldLfee += pgField->lfOldLfee;
	pglField->lfOldIfee += pgField->lfOldIfee;
	
	pglField->nCallTimes += pgField->nCallTimes;
	pglField->nDuration  += pgField->nDuration;
	pglField->nPresentInfo  += pgField->nPresentInfo;

	if( 0 > strcmp( pglField->szTalkTime, pgField->szTalkTime ) )
	{
		memset( pglField->szTalkTime, 0, sizeof( pglField->szTalkTime ) );
		strcpy( pglField->szTalkTime, pgField->szTalkTime );
	}
		
	return MY_SUCCEED;
}

/**********************************************************
function: IsGroupField
description: 判断记录是否为分组记录
Input: psData: 描述系统全局结构的结构体
	   pglField: 用户增量统计累计信息
Output: 
Return: MY_TRUE: 是 MY_FALSE: 否
others:
***********************************************************/

int IsGroupField( PSTORE_DATA psData, PGROUP_FIELD pglField )
{
	PGROUP_FIELD pgField;
	
	pgField = &(psData->gField);

	if( 0 != strcmp( pgField->szPhoneNo     , pglField->szPhoneNo ) 
	||  0 != strcmp( pgField->szCallType    , pglField->szCallType ) 
	||  0 != strcmp( pgField->szFeeType     , pglField->szFeeType ) 
	||  0 != strcmp( pgField->szRoamType    , pglField->szRoamType ) 
	||  0 != strcmp( pgField->szChatType    , pglField->szChatType ) 
	||  0 != strcmp( pgField->szDialType    , pglField->szDialType ) 
	||  0 != strcmp( pgField->szInfoType    , pglField->szInfoType )
	||  0 != strcmp( pgField->szDiscountType, pglField->szDiscountType ) 
	||  0 != strcmp( pgField->szSameCity    , pglField->szSameCity ) 
	||  0 != strcmp( pgField->szDataType    , pglField->szDataType ) 
	||  0 != strcmp( pgField->szDealTime    , pglField->szDealTime ) 
	)
	{
		return MY_FALSE;
	}
	
	return MY_TRUE;
}

/********************************************************** 
function: InitPhoneHeadList
description: 初始化号段增量信息
Input:	psData: 描述系统全局结构的结构体
		phList: 号段增量信息
Output:
Return: MY_SUCCEED 成功
others:
***********************************************************/

int InitPhoneHeadList( PSTORE_DATA psData, PHEAD_LIST phList )
{
	int nLoop;
	
	memset( phList, 0, sizeof( HEAD_LIST ) );

	strcpy( phList->szPhoneHead, psData->szBaseAddr );
	
	phList->lTotalRec = 0;
		
	phList->lfTotalCfee = (double) 0.00;
	phList->lfTotalLfee = (double) 0.00;
	phList->lfTotalIfee = (double) 0.00;
		
	phList->lfTotalOldCfee = (double) 0.00;
	phList->lfTotalOldLfee = (double) 0.00;
	phList->lfTotalOldIfee = (double) 0.00;
		
	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		phList->pgList[nLoop] = NULL;
	}
	
	phList->phlNext = NULL;
	
	return MY_SUCCEED;
}

/**********************************************************
function: AddupGroupList
description: 累计号段增量统计信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int AddupGroupList( PSTORE_DATA psData, PHEAD_LIST phList )
{
	PGROUP_LIST pgList;
	PGROUP_FIELD pgField;
	
  	pgField = &(psData->gField);
	
	phList->lTotalRec += pgField->nCallTimes;
	
	phList->lfTotalCfee += pgField->lfCfee;
	phList->lfTotalLfee += pgField->lfLfee;
	phList->lfTotalIfee += pgField->lfIfee;
	
	phList->lfTotalOldCfee += pgField->lfOldCfee;
	phList->lfTotalOldLfee += pgField->lfOldLfee;
	phList->lfTotalOldIfee += pgField->lfOldIfee;
	
	if( NULL == phList->pgList[atoi(psData->szOffset)] ) {
		/* 首次累计用户增量信息 */
		phList->pgList[atoi(psData->szOffset)] = ( PGROUP_LIST ) malloc( sizeof( GROUP_LIST ) );
		
		pgList = phList->pgList[atoi(psData->szOffset)];
		if( NULL == pgList ) {
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存移动用户增量的内存空间失败!\n" );
			return MY_FAIL;
		}
		
		memset( pgList, 0, sizeof( GROUP_LIST ) );
		pgList->pglNext = NULL;
		
		memcpy( &(pgList->gField), &(psData->gField), sizeof( GROUP_FIELD ) );
	
		return MY_SUCCEED;
	} else {
		/* 遍历用户增量统计分组信息 */
		pgList = phList->pgList[atoi(psData->szOffset)];
		
		while( NULL != pgList ) {
			if( IsGroupField( psData, &(pgList->gField) ) ) {
				/* 累计用户增量统计数据 */
				GroupField( psData, &(pgList->gField) );
			
				return MY_SUCCEED;
			}
			
			if( NULL == pgList->pglNext ) {
				break;
			} else {
				pgList = pgList->pglNext;
			}	
		}
	}
	
	/* 未找到匹配的用户增量分组 */
	pgList->pglNext = ( PGROUP_LIST ) malloc( sizeof( GROUP_LIST ) );

	if( NULL == pgList->pglNext ) {
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存移动用户增量的内存空间失败!\n" );
		return MY_FAIL;
	}
	
	pgList = pgList->pglNext;
	
	memset( pgList, 0, sizeof( GROUP_LIST ) );
	pgList->pglNext = NULL;
	
	/* 复制用户增量信息 */
	memcpy( &(pgList->gField), &(psData->gField), sizeof( GROUP_FIELD ) );


	return MY_SUCCEED;
}
	
/**********************************************************
function: AddupMobileData
description: 累计移动号码增量统计信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int AddupMobileData( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	PGROUP_HEAD pgHead ; 
	PHEAD_LIST phList;
	char szGroupHead[HEAD_FLAG_LEN+1] ;

	int nLoop, nLoop1, nLoop2, nLoop3 ;
	
	strncpy( szGroupHead, psData->szBaseAddr, HEAD_FLAG_LEN ) ;
	szGroupHead[HEAD_FLAG_LEN] = 0;

	for( nLoop = 0; nLoop < MAX_HEAD_NUM; nLoop++ ) {
		pgHead = psData->pgHead[nLoop];
		if ( pgHead == NULL ) {
			psData->pgHead[nLoop] = ( PGROUP_HEAD )malloc(sizeof(GROUP_HEAD)) ;
			pgHead = psData->pgHead[nLoop] ;
			if( pgHead == NULL ) {
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存详单存储位置信息的内存空间失败!\n" );
				return MY_FAIL ;
			}
			strncpy( pgHead->szGroupHead, szGroupHead, HEAD_FLAG_LEN ) ;
			pgHead->szGroupHead[HEAD_FLAG_LEN] = 0 ; 

			for( nLoop1 = 0; nLoop1 < MAX_MOBILE_HEAD; nLoop1++ ) {
				pgHead->phList[nLoop1] = NULL ;
			}
			break ;
		} else if ( strcmp( pgHead->szGroupHead, szGroupHead ) == 0 ) {
			break ;
		}
	}
	
	if( NULL == pgHead->phList[atoi(psData->szBaseAddr + 2)] )
	{
		/* 首次累计号段增量信息 */
		psData->nMobileHead++ ;

		pgHead->phList[atoi(psData->szBaseAddr + 2)] = ( PHEAD_LIST ) malloc( sizeof( HEAD_LIST ) );

		phList = pgHead->phList[atoi(psData->szBaseAddr + 2)];
		if( NULL == phList )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存移动号段增量的内存空间失败![%s]\n" ,strerror(errno));
			return MY_FAIL;
		}

		InitPhoneHeadList( psData, phList );
	}
	else 
	{
		/* 找到匹配的号段增量分组 */
		phList = pgHead->phList[atoi(psData->szBaseAddr + 2)]; 
	}
	
	/* 累计号段增量统计数据 */
	AddupGroupList( psData, phList );
	
	return MY_SUCCEED;
}

/**********************************************************
function: AddupFixedData
description: 累计固定号码增量统计信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int AddupFixedData( PSTORE_DATA psData )
{
	POPER_RECORD poRecord;
	PHEAD_LIST phList;
	
	if( NULL == psData->phlFixNumber )
	{
		/* 首次累计号段增量信息 */
		psData->nFixedHead++;

		psData->phlFixNumber = ( PHEAD_LIST ) malloc( sizeof( HEAD_LIST ) );

		phList = psData->phlFixNumber;
		if( NULL == phList )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存固定号段增量的内存空间失败!\n" );
			return MY_FAIL;
		}
		
		InitPhoneHeadList( psData, phList );
	}
	else
	{
		phList = psData->phlFixNumber;
		
		/* 遍历固定号段增量统计分组信息 */
		while( NULL != phList )
		{
			/* 找到匹配的号段增量分组 */
			if( 0 == strcmp( phList->szPhoneHead, psData->szBaseAddr ) )
			{
				break;
			}
			
			if( NULL == phList->phlNext )
			{
				/* 新增增量统计分组信息 */
				phList->phlNext = ( PHEAD_LIST ) malloc( sizeof( HEAD_LIST ) );

				phList = phList->phlNext;
				if( NULL == phList )
				{
					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "申请保存固定号段增量的内存空间失败!\n" );
					return MY_FAIL;
				}
		
				InitPhoneHeadList( psData, phList );
		
				break;
			}
			else
			{
				phList = phList->phlNext;
			}
		}
	}
	
	/* 累计号段增量统计数据 */
	AddupGroupList( psData, phList );
	
	return MY_SUCCEED;
}

/**********************************************************
function: AddupIncData
description: 累计增量统计信息
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int AddupIncData( PSTORE_DATA psData )
{
	if( STAT_MODE_TURN != psData->poRecord->nStatMode )
	{
		psData->lNoaddRecord++;
		return MY_SUCCEED;
	}
	
	psData->lAddupRecord++;
	
	/* 提取详单增量信息 */
	GetGroupField( psData );

	if( IsMobileHead( psData->szSubscriber) && MOBILE_HEAD_LEN == strlen( psData->szBaseAddr ) )
	{
		/* 累计移动号码增量统计信息 */
		if( MY_SUCCEED != AddupMobileData( psData ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "累计移动号码增量数据失败!\n" );
			return MY_FAIL;
		}
	}
	else
	{
		/* 累计固定号码增量统计信息 */
		if( MY_SUCCEED != AddupFixedData( psData ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "累计固定号码增量数据失败!\n" );
			return MY_FAIL;
		}
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: DumpUserIncData
description: 输出用户增量统计数据
Input: psData: 描述系统全局结构的结构体
	   pgList: 用户增量统计数据
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int DumpUserIncData( PSTORE_DATA psData, PGROUP_LIST pgList )
{
	PGROUP_FIELD pgField;

	pgField = &(pgList->gField);

#ifndef	__DUMP_BIN_DATA__
	if( 0 >= fprintf( psData->fpInc, "%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%lf/%lf/%lf/%lf/%lf/%lf/%d/%d/%s/%s/%s/%d/%s/\n",
									pgField->szPhoneNo, pgField->szCallType, pgField->szFeeType,
									pgField->szRoamType, pgField->szChatType, pgField->szDialType,
									pgField->szInfoType, pgField->szDiscountType, 
									pgField->szSameCity, pgField->szDataType,
					
									pgField->lfCfee, pgField->lfLfee, pgField->lfIfee,
									pgField->lfOldCfee, pgField->lfOldLfee, pgField->lfOldIfee,
					
									pgField->nCallTimes, pgField->nDuration, 
									pgField->szDealTime, 
									pgField->szSerial, pgField->szTalkTime,
									pgField->nPresentInfo,
									pgField->szRecv ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出用户 %s 的增量统计数据失败!\n", pgField->szPhoneNo );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
#else
	if( 1 != fwrite( pgField, sizeof( GROUP_FIELD ), 1, piData->fpInc ) )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出用户 %s 的增量统计数据失败!\n", pgField->szPhoneNo );
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
		return MY_FAIL;
	}
#endif
	
	return MY_SUCCEED;
}

/**********************************************************
function: DumpHeadIncData
description: 输出号段增量统计数据
Input: psData: 描述系统全局结构的结构体
	   pgList: 用户增量统计数据
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int DumpHeadIncData( PSTORE_DATA psData, PHEAD_LIST phList )
{
	PGROUP_LIST pgList;
	int nLoop = 0;	

	if( MY_SUCCEED != OpenIncFile( psData, phList ) )
	{
		return MY_FAIL;
	}

	for( nLoop = 0; nLoop < MAX_HEAD_USER; nLoop++ )
	{
		pgList = phList->pgList[nLoop];

		while( NULL != pgList )
		{
			if( MY_SUCCEED != DumpUserIncData( psData, pgList ) )
			{
				MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "输出号段 %s 的增量统计数据失败!\n", phList->szPhoneHead );
				return MY_FAIL;
			}
				
			pgList = pgList->pglNext;
		}
	}
	
	if( NULL != psData->fpInc )
	{
		fclose( psData->fpInc );
		psData->fpInc = NULL;
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: CreateIncBreakFile
description: 生成增量统计数据断点文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int CreateIncBreakFile( PSTORE_DATA psData )
{
	FILE * fpBreak;
	char szBreakFile[MAX_PATH_LEN + 1];
	
	memset( szBreakFile, 0, sizeof( szBreakFile ) );
	sprintf( szBreakFile, "%s/%s%s%s", psData->sCmd.szTmpDir, TLOG_FILE_HEAD, psData->szCurrBatch, LOG_FILE_TAIL );
	
	fpBreak = fopen( szBreakFile, "a" );
	if( NULL == fpBreak )
	{
		MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "增量统计数据断点文件 %s 打开失败!\n", szBreakFile );
		return MY_FAIL;
	}
	
	fclose( fpBreak );
	
	return MY_SUCCEED;
}

/**********************************************************
function: MoveIncFile
description: 移动指定的增量统计文件
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int MoveIncFile( PSTORE_DATA psData, PHEAD_LIST phList )
{
	char szSrcFile[512];
	char szDestFile[512];
	
	memset( szSrcFile, 0, sizeof( szSrcFile ) );
	memset( szDestFile, 0, sizeof( szDestFile ) );
			
	sprintf( szSrcFile , "%s/%s%s.%s.tmp", psData->sCmd.szTmpDir, INC_FILE_HEAD, psData->szCurrBatch, phList->szPhoneHead );
	sprintf( szDestFile, "%s/%s%s.%s"    , psData->sCmd.szOutDir, INC_FILE_HEAD, psData->szCurrBatch, phList->szPhoneHead );
			
	if( MY_SUCCEED != MoveFile( szSrcFile, szDestFile ) )
	{
		return MY_FAIL;
	}
	
	return MY_SUCCEED;
}


/**********************************************************
function: MoveIncData
description: 将增量统计文件移入正式目录
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int MoveIncData( PSTORE_DATA psData )
{
	PGROUP_HEAD pgHead ;
	PHEAD_LIST phList;

	int nLoop = 0, nLoop1 = 0 ;

	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		/*pgHead = psData->pgHead[nLoop1] ;*/
		
		if( psData->pgHead[nLoop1] == NULL ) { 
			break ;
		}

		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			phList = psData->pgHead[nLoop1]->phList[nLoop];
			
			if( NULL != phList )
			{
				if( MY_SUCCEED != MoveIncFile( psData, phList ) )
				{
					MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "移动移动号段 %s 的增量统计文件失败!\n", phList->szPhoneHead );
					return MY_FAIL;
				}
			}
		}
	}
	
	phList = psData->phlFixNumber;
	
	while( phList != NULL )
	{
		if( MY_SUCCEED != MoveIncFile( psData, phList ) )
		{
			MyLog( INFO_SOURCE_APP, INFO_LEVEL_WARN, "移动固定号段 %s 的增量统计文件失败!\n", phList->szPhoneHead );
			return MY_FAIL;
		}
			
		phList = phList->phlNext;
	}
	
	return MY_SUCCEED;
}

/**********************************************************
function: DumpIncData
description: 输出增量统计数据
Input: psData: 描述系统全局结构的结构体
Output:
Return: MY_SUCCEED: 成功 MY_FAIL: 失败
others:
***********************************************************/

int DumpIncData( PSTORE_DATA psData )
{
	PHEAD_LIST phList;
	PGROUP_HEAD pgHead ;
	int nLoop = 0, nLoop1 = 0, nLoop2 = 0;	
	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "输出 %s 的增量统计数据!\n", psData->szCurrBatch );
	
	for( nLoop1 = 0; nLoop1 < MAX_HEAD_NUM; nLoop1++ ) {
		pgHead = psData->pgHead[nLoop1];
		if( pgHead == NULL ) {
			break ;
		}
		
		for( nLoop = 0; nLoop < MAX_MOBILE_HEAD; nLoop++ )
		{
			phList = pgHead->phList[nLoop];
			
			if( NULL != phList )
			{
				if( MY_SUCCEED != DumpHeadIncData( psData, phList ) )
				{
					return MY_FAIL;
				}
			}
		}
	}
	
	phList = psData->phlFixNumber;
	
	while( phList != NULL )
	{
		if( MY_SUCCEED != DumpHeadIncData( psData, phList ) )
		{
			return MY_FAIL;
		}
			
		phList = phList->phlNext;
	}

	/* 生成增量统计数据断点文件 */
	if( 0 < ( psData->nMobileHead + psData->nFixedHead ) )
	{
		if( MY_SUCCEED != CreateIncBreakFile( psData ) )
		{
			return MY_FAIL;
		}		
	}

	if( MY_SUCCEED != MoveIncData( psData ) )
	{
		return MY_FAIL;
	}
	
	if( MY_SUCCEED != BakDetailFile( psData ) )
	{
		return MY_FAIL;
	}

	
	MyLog( INFO_SOURCE_APP, INFO_LEVEL_GEN, "共处理 %d 个文件, 生成 MOBILE: %d, FIXED: %d !\n\n", psData->nDealFile, psData->nMobileHead, psData->nFixedHead );
	
	FreeHeadList( psData );
	
	return MY_SUCCEED;
}
