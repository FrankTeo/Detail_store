	

/********************************************************************
Copyright (C), 1995-2004, Si-Tech Information Technology Ltd.
file_name: cdef.h  
author: 张凡 version: （V1.00） date: （2004－08－20）
description: 当日详单存储系统公用变量、结构体定义
others:
history: 1.date: 修改时间 version: 版本（V1.00）author: 作者
                        modification: 修改内容
                 2. ...
********************************************************************/


#ifndef	_C_DEF_H
#define	_C_DEF_H

#define _ENCRYPT_
#include "common.h"


#define PROC_CTRL_RUN				0							/* 程序允许运行 */
#define PROC_CTRL_EXIT				1							/* 程序停止运行 */
#define PROC_CTRL_RELOAD			2							/* 重新载入配置 */

#define	MAX_MOBILE_HEAD				100000						/* 号段最大个数 */
#define	MAX_HEAD_USER				10000						/* 号段最大用户数 */
#define	MAX_SUBSCRIBER_LEN			25							/* 用户号码最大长度 */
#define MAX_HEAD_NUM				20

#define	STORE_MODE_MOBILE			0							/* 按手机号码存储 */
#define	STORE_MODE_FIXED			1							/* 按固定号码分业务取模存储 */
#define	STORE_MODE_MODULUS			2							/* 按号码不分业务取模存储 */

#define HEAD_FLAG_LEN					2								/* */

#define	STAT_MODE_TURN				0							/* 统计模式开启 */
#define	STAT_MODE_OFF				1							/* 统计模式关闭 */
#define	TRACE_FLAG_STORE			0							/* 记录追踪记录 */
#define	TRACE_FLAG_TRUNC			1							/* 截断文件 */

#define SYS_TYPE_LEN				8							/* 系统子代码长度 */
#define	SUBSCRIBER_OFFSET_LEN		4							/* 用户号码偏移 */
#define	FILE_NAME_LEN				20							/* 文件名长度 */
#define	INC_FIELD_NUM				23							/* 增量统计字段个数 */

#define ERROR_CODE_PREFIX			"Ew"						/* 前缀 */
#define ERROR_CODE_NORMAL			"fs000"						/* 正确记录 */
#define ERROR_CODE_CFGREC			"fs001"						/* 在配置文件汇中未找到匹配的记录 */
#define ERROR_CODE_RECLEN			"fs002"						/* 记录长度错误 */
#define ERROR_CODE_STOCTL			"fs003"						/* 未找到详单存储路径信息 */

#define ERROR_CODE_IDNULL			"fs005"						/* 用户号码为空 */

#define	REVERSE_FILE_FLAG			"f"							/*  */
/*#define	REVERSE_FILE_FLAG			".rd"						* 负话单标志 */

#define	DETAIL_SUBSCRIBER_FLAG		"S"							/* 详单数据区标志 */
#define	DETAIL_BLOCKFLAG_LEN		strlen( DETAIL_SUBSCRIBER_FLAG ) + SUBSCRIBER_OFFSET_LEN + strlen( CHAR_NEW_LINE );

#define	NOT_CHECK_FILEHEAD			"*"							/* 不校验文件头 */
#define	NOT_CHECK_SYSTYPE			"*"							/* 不校验系统代码 */
#define	DETAIL_RESERVE_CHAR			"  "						/* 详单存储记录保留字段 */

#define	DETAIL_RESERVE_LEN			strlen( DETAIL_RESERVE_CHAR )

#define	STORE_DIR_CURR				"curr"						/* 当日详单存储目录 */
#define	STORE_DIR_OLD				"old"						/* 规整后的当日详单存储目录 */
#define	STORE_DIR_DAY				"day"						/* 往日详单存储目录 */
#define	STORE_DIR_MONTH				"month"						/* 往月详单存储目录 */
#define	STORE_DIR_INDEX				"index"						/* 索引文件存储目录 */

#define	CURR_FILE_TAIL				".cur"						/* 当日详单存储文件扩展名 */
#define	DAY_FILE_TAIL				".day"						/* 往日详单存储文件扩展名 */
#define	OLD_FILE_TAIL				".old"						/* old文件扩展名 */
#define	MONTH_FILE_TAIL				".month"					/* 往月详单存储文件扩展名 */
#define	INDEX_FILE_TAIL				".index"					/* 索引文件扩展名 */

#define	INC_FILE_HEAD				"real."						/* 增量统计文件前缀 */
#define	INC_FIELD_SEP				"."							/* 增量统计文件各域分隔符 */
#define	INC_FILE_MLEN				( strlen( INC_FILE_HEAD ) + DATE_STD_LEN + strlen( INC_FIELD_SEP ) )

#define TLOG_FILE_LEN				22							/* 增量统计数据断点文件名长度 */
#define	TLOG_FILE_HEAD				"tmp."						/* 增量统计数据断点文件名头 */

#define	TBAK_LOG_LEN				25
#define	TBAK_LOG_HEAD				"tmpbak."					/* 临时备份日志文件文件名头 */

#define	BUILD_KEY_ALL				"ALL"						/* 规整所有号段的详单存储文件 */

#define	DETAIL_BLOCK_SIZE			4 * 1024					/* 详单数据块大小 */

typedef	struct _STORE_CMD
{
	char szProcLog[MAX_PATH_LEN + 1];							/* 处理日志目录 */
	char szRunLog[MAX_PATH_LEN + 1];							/* 运行日志目录 */
	char szLogTmp[MAX_PATH_LEN + 1];							/* 日志临时目录 */
	
	char szInDir[MAX_PATH_LEN + 1];								/* 入口目录 */
	char szBakDir[MAX_PATH_LEN + 1];							/* 备份目录 */
	char szTmpBak[MAX_PATH_LEN + 1];							/* 临时备份目录 */
	char szOutDir[MAX_PATH_LEN + 1];							/* 增量统计文件输出目录 */
	char szTmpDir[MAX_PATH_LEN + 1];							/* 出口临时目录 */
	
	char szHostName[32 + 1];									/* 主机标识 */
	char szLoginFile[MAX_PATH_LEN + 1];							/* 登陆口令文件 */
	char szServer[32];											/* 数据库服务名 */
	
	char szUserName[128 + 1];									/* 登陆用户名 */
	char szPassWord[128 + 1];									/* 登陆口令 */
	
	char szCfgFile[MAX_PATH_LEN + 1];							/* 配置文件 */
	char szCtrlFlag[128];										/* 控制标志 */
	int nInterval;												/* 批次的时间间隔 */
	
	char szOperFile[512];										/* 详单格式描述信息文件 */
	char szFmtCtrl[512];										/* 增量统计字段配置文件 */
	
} STORE_CMD;
typedef STORE_CMD * PSTORE_CMD;

typedef	struct	_REBUILD_CMD
{
	char szRunLog[MAX_PATH_LEN + 1];							/* 运行日志目录 */
	char szCfgFile[MAX_PATH_LEN + 1];							/* 配置文件 */
	
	char szBuildDate[14 + 1];									/* 规整日期 */
	char szBuildKey[128];										/* 规整号段 */

	char szHostName[32 + 1];									/* 主机标识 */
	char szLoginFile[MAX_PATH_LEN + 1];							/* 登陆口令文件 */
	char szServer[32];											/* 数据库服务名 */
	
	char szUserName[128 + 1];									/* 登陆用户名 */
	char szPassWord[128 + 1];									/* 登陆口令 */
	
	char szOperFile[512];										/* 详单格式描述信息文件 */
	char szFmtCtrl[512];										/* 增量统计字段配置文件 */
	
} REBUILD_CMD;
typedef REBUILD_CMD * PREBUILD_CMD;

typedef	struct	_ARCHIVE_CMD
{
	char szRunLog[MAX_PATH_LEN + 1];							/* 运行日志目录 */
	char szArchMonth[14 + 1];									/* 归档月份 */
	char szFileSystem[MAX_PATH_LEN + 1];						/* 归档文件系统 */
	
} ARCHIVE_CMD;
typedef ARCHIVE_CMD * PARCHIVE_CMD;

typedef	struct _STORE_CONTROL
{
	char szHostName[32 + 1];									/* 主机标识 */
	char szRootDir[64 + 1];										/* 根目录 */
	char szBaseDir[64 + 1];										/* 子目录 */
	
	char szGroupKey[16 + 1];									/* 分组键值 */
	
	char szKey[16 + 1];
	int nBeginKey;
	int nEndKey;
	
	char szAlterTime[14 + 1];									/* 变更时间 */
	char szOldRootDir[64 + 1];									/* 旧系统根目录 */
	char szOldBaseDir[64 + 1];									/* 旧系统子目录 */
	char szStoreMode[1 + 1];									/* 详单存储模式 */
	
	struct _STORE_CONTROL * pscNext;
	
} STORE_CONTROL;
typedef STORE_CONTROL * PSTORE_CONTROL;

typedef struct _STORE_HEAD
{
	PSTORE_CONTROL psControl[MAX_MOBILE_HEAD];
	
	char szHeadFlag[128 + 1];
	
} STORE_HEAD;
typedef STORE_HEAD * PSTORE_HEAD;

typedef	struct _OPER_REC
{
	char szFileHead[32 + 1];									/* 文件头 */
	char szSysType[SYS_TYPE_LEN + 1];							/* 系统代码 */
	char szDataType[16 + 1];									/* 数据类型 */
	
	int nOperNo;												/* 操作编号 */
	int nRecLen;												/* 记录长度 */
	int nUserLoc;												/* 用户字段位置 */
	int nUserLen;												/* 用户字段长度 */
	int nTimeLoc;												/* 通话时间字段位置 */
	int nTimeLen;												/* 通话时间字段长度 */
	int nDealTimeLoc;											/* 处理时间字段位置 */
	int nDealTimeOff;											/* 处理时间偏移 */
	int nDealTimeLen;											/* 处理时间字段长度 */
	int nDetachFlag;											/* 拆分标志 */
	int nFeeUnit;												/* 基本费用单位 */
	int nStoreMode;												/* 存储模式 */
	int nStatMode;												/* 增量数据统计方式 */
	
} OPER_REC;
typedef OPER_REC * POPER_RECORD;

typedef	struct _OPER_INFO
{
	POPER_RECORD poRecord;
	
	int nOperCount;												/* 记录数 */
	
} OPER_INFO;
typedef OPER_INFO * POPER_INFO;

typedef	struct _FMT_REC
{
	char szFieldName[32];										/* 字段名 */

	int nFieldNo;												/* 字段编号 */
	int nStart;													/* 字段开始位置 */
	int nLen;													/* 字段长度 */
	int nType;													/* 字段类型 */
	int nOutNo;													/* 输出编号 */
	char szValueNo[128];										/* 取值字段编号 */

} FMT_REC;
typedef FMT_REC * PFMT_REC;

typedef	struct _FMT_INFO
{
	PFMT_REC pfRec;
	
	char szFmtFile[512];										/* 格式文件 */
	
	int nFileNo;												/* 文件编号 */
	int nFieldCount;											/* 字段个数 */
	
} FMT_INFO;
typedef FMT_INFO * PFMT_INFO;

typedef	struct _FMT_CTRL
{
	PFMT_INFO pfInfo;
	
	int nFmtCount;												/* 记录数 */
	
} FMT_CTRL;
typedef FMT_CTRL * PFMT_CTRL;

typedef struct _GROUP_FIELD
{
	char szPhoneNo[MAX_SUBSCRIBER_LEN + 1];						/* 用户号码 */
	char szCallType[2 + 1];										/* 通话类型 */
	char szFeeType[2 + 1];										/* 费用类型 */
	char szRoamType[2 + 1];										/* 漫游类型 */
	char szChatType[10 + 1];									/* 对端类型 */
	char szDialType[6 + 1];										/* 拨打类型 */
	char szInfoType[6 + 1];										/* 信息台代码 */
	char szDiscountType[10 + 1];								/* 优惠代码 */
	char szSameCity[1 + 1];										/* 同城标志 */
	char szDataType[2 + 1];										/* 数据类型 */

	double lfCfee;   											/* 二次批价后的基本费用 */
	double lfLfee;												/* 二次批价后的长途费用 */
	double lfIfee;												/* 二次批价后的信息费用 */
	double lfOldCfee; 											/* 原始基本费用 */
	double lfOldLfee;											/* 原始长途费用 */
	double lfOldIfee;											/* 原始信息费用 */

	int nCallTimes;												/* 通话次数 */
	int nDuration;												/* 通话时长 */

	char szDealTime[14 + 1];									/* 处理时间 */
	char szSerial[8 + 1];										/* 记录序列号 */
	char szTalkTime[14 + 1];									/* 最大通话时间 */

	int  nPresentInfo ;											/* 单条记录的套餐赠送信息 */

	char szRecv[ 10 + 1];										/* 保留字段 */
	
} GROUP_FIELD;
typedef GROUP_FIELD * PGROUP_FIELD;

typedef struct _GROUP_LIST
{
	GROUP_FIELD gField;

	struct _GROUP_LIST * pglNext;
	
} GROUP_LIST;
typedef	GROUP_LIST * PGROUP_LIST;

typedef struct _HEAD_LIST
{
	PGROUP_LIST pgList[MAX_HEAD_USER];

	char szPhoneHead[MAX_SUBSCRIBER_LEN + 1];

	long lTotalRec;												/* 详单条数 */
		
	double lfTotalCfee;											/* 基本费总数 */
	double lfTotalLfee;											/* 长途费总数 */
	double lfTotalIfee;											/* 信息费总数 */
	double lfTotalOldCfee;										/* 原始基本费总数 */
	double lfTotalOldLfee;										/* 原始长途费总数 */
	double lfTotalOldIfee;										/* 原始信息费总数 */
	
	struct _HEAD_LIST * phlNext;
} HEAD_LIST;
typedef HEAD_LIST * PHEAD_LIST;

typedef struct _GROUP_HEAD
{
	PHEAD_LIST phList[MAX_MOBILE_HEAD];
	
	char szGroupHead[128+1] ;
	
	struct _GROUP_HEAD * pghNext;
} GROUP_HEAD;
typedef GROUP_HEAD * PGROUP_HEAD ;

typedef	struct _DETAIL_BLOCK
{
	char szDetailBlock[DETAIL_BLOCK_SIZE + 1];
	long lBlockOffset;
	
	struct _DETAIL_BLOCK * pdbNext;
	
} DETAIL_BLOCK;
typedef	DETAIL_BLOCK * PDETAIL_BLOCK;

typedef struct _SERVICE_DETAIL
{
	PDETAIL_BLOCK pdBlock;
	char szSysType[SYS_TYPE_LEN + 1];							/* 系统代码 */
	
	struct _SERVICE_DETAIL * psdNext;
	
} SERVICE_DETAIL;
typedef	SERVICE_DETAIL * PSERVICE_DETAIL;

typedef	struct _SERVICE_INDEX
{
	char szSysType[SYS_TYPE_LEN + 1];							/* 系统代码 */
	
	ulong ulDetailOff;
	ulong ulDetailLen;
	ulong ulOriginLen;
	
	int nUserNo;
	int nDay;
	
	struct _SERVICE_INDEX * psiUser;
	struct _SERVICE_INDEX * psiNext;
	
} SERVICE_INDEX;
typedef	SERVICE_INDEX * PSERVICE_INDEX;

typedef	struct _BASE_INDEX
{
	PSERVICE_INDEX psIndex;
	
	ulong ulOffset;
	
	int nUserNo;
	int nDay;
	int nService;
	
} BASE_INDEX;
typedef BASE_INDEX * PBASE_INDEX;

typedef	struct _FILE_LIST
{
	char szFile[MAX_PATH_LEN + 1];
	
	struct _FILE_LIST * pflNext;
} FILE_LIST;
typedef FILE_LIST * PFILE_LIST;

typedef	struct _TRACE_INFO
{
	char szFileName[MAX_RECORD_LEN + 1];
	char szBreakBatch[14 + 1];
	char szBreakRecord[MAX_RECORD_LEN + 1];
} TRACE_INFO;
typedef TRACE_INFO * PTRACE_INFO;

typedef	struct _STORE_DATA
{
	STORE_CMD sCmd;												/* 当日详单存储运行指令 */
	REBUILD_CMD rCmd;											/* 往日详单规整运行指令 */
	ARCHIVE_CMD aCmd;											/* 往月详单归档运行指令 */
	PSTORE_HEAD psHead[MAX_HEAD_NUM];							/*  */
	/* PSTORE_CONTROL psControl[MAX_MOBILE_HEAD];					* 详单存储路径控制信息 */
	PSTORE_CONTROL pscFixed;									/* 固定号码详单存储路径 */
	OPER_INFO oInfo;											/* 详单格式描述信息 */
	FMT_CTRL fCtrl;												/* 增量统计字段配置信息 */

	POPER_RECORD poRecord;										/* 当前处理记录的详单格式描述信息 */
	PSTORE_CONTROL psCtrl;										/* 当前处理记录存储位置信息 */	
	
	/* PHEAD_LIST phList[MAX_MOBILE_HEAD];							* 移动号码增量统计数据 */
	PGROUP_HEAD pgHead[MAX_HEAD_NUM];				/* 移动号码增量统计数据 */
	PHEAD_LIST phlFixNumber;									/* 固定号码增量统计数据 */
	PFILE_LIST pfList;											/* 存储文件列表 */
	
	GROUP_FIELD gField;											/* 当前记录增量信息 */
	
	PSERVICE_DETAIL psDetail[MAX_HEAD_USER];					/* 详单数据 */
	BASE_INDEX pbIndex[MAX_HEAD_USER];							/* 索引数据 */
	
	TRACE_INFO tInfo;											/* 断点信息 */
	
	char szHostName[32 + 1];									/* 主机标识 */
	char szFileName[128];										/* 处理文件名 */
	char szStartTime[14 + 1];									/* 文件开始处理时间 */
	char szStartMicroSec[6 + 1];								/* 文件开始处理时间微秒 */
	char szEndTime[14 + 1];										/* 文件结束处理时间 */
	char szEndMicroSec[6 + 1];									/* 文件结束处理时间微秒 */
	char szCurrBatch[14 + 1];									/* 当前系统所处批次 */
	char szBatchTime[14 + 1];									/* 系统中断时的批次 */
	
	char szRecord[2048];										/* 当前处理详单记录 */
	char szErrCode[16 + 1];										/* 错误代码 */
	
	char szStoreDir[MAX_PATH_LEN + 1];							/* 详单存储目录 */
	char szStoreFile[MAX_PATH_LEN + 1];							/* 详单存储文件 */
	char szIncFile[MAX_PATH_LEN + 1];							/* 增量统计文件 */
	char szTraceFile[MAX_PATH_LEN + 1];							/* 存储追踪文件 */
	char szIndexFile[MAX_PATH_LEN + 1];							/* 索引文件 */
	
	char szTmpBakLog[MAX_PATH_LEN + 1];							/* 临时备份文件日志 */
	char szTmpIncLog[MAX_PATH_LEN + 1];							/* 临时增量文件日志 */
	
	char szStoreKey[128];										/* 归档键值 */
	char szStoreHead[128];										/* 归档文件头 */
	char szSubscriber[MAX_SUBSCRIBER_LEN + 1];					/* 用户号码 */
	char szRecordTime[DATE_STD_LEN + 1];						/* 详单记录时间 */
	
	char szBaseAddr[MAX_SUBSCRIBER_LEN + 1];					/* 用户号码基址 */
	char szOffset[SUBSCRIBER_OFFSET_LEN + 1];					/* 用户号码偏移 */
	
	char * szArchDetail;										/* 归档详单数据区 */
	char * szCompDetail;										/* 压缩详单数据区 */
	
	char bSingleMode;											/* 单文件处理模式 */
	char bResumeEnv;											/* 恢复标志 */
	char bResumeInc;											/* 恢复增量统计信息标志 */
	
	FILE * fpDetail;											/* 详单文件指针 */
	FILE * fpInc;												/* 增量统计文件指针 */
	FILE * fpIndex;												/* 索引文件指针 */
	FILE * fpMonthDetail;										/* 往月详单文件指针 */
	FILE * fpMonthIndex;										/* 往月索引文件指针 */
	
	long lTotalRecord;											/* 总记录数 */
	long lAvailRecord;											/* 有效记录数 */
	long lErrorRecord;											/* 错误记录数 */
	long lAddupRecord;											/* 统计记录数 */
	long lNoaddRecord;											/* 为统计记录数 */
	
	long lTotalHead;											/* 处理的总号段数 */
	
	ulong ulIndexOff;											/* 索引信息偏移 */
	ulong ulDetailOff;											/* 详单信息偏移 */
	ulong ulBuffOffset;											/* 归档详单数据区详单偏移 */
	
	int nStoreHandle;											/* 详单存储文件句柄 */
	int nProcHandle;											/* 处理日志句柄 */
	int nErrHandle;												/* 错单日志句柄 */
	int nTraceHandle;											/* 追踪文件句柄 */
	
	int nDealFile;												/* 一个批次处理的文件数 */
	int nMobileHead;											/* 移动号段个数 */
	int nFixedHead;												/* 固定号段个数 */
	
	int nDetachNo;												/* 拆分标志 */
	int nService;												/* 用户业务个数 */
	int nRetCode;												/* 返回值 */
	
} STORE_DATA;
typedef	STORE_DATA * PSTORE_DATA;

#endif
