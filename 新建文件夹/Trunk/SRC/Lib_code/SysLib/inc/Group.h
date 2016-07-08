/***************************Copyright BestFu 2014-05-14*************************
文	件：	Group.h
说	明：	iSpace分组操作头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	xjx
日	期：	2016-06-24
*******************************************************************************/
#ifndef __GROUP_H
#define __GROUP_H


#include "UnitCfg.h"

#define GROUP_NUM_MAX					150								//最大分组数量(不包括场景分组)


#define GROUP_ONECE_SET_MAX				40								//一次设置分组的最大数量

#define NEW_GROUP_START_ADDR				(0x200)							//分组(非场景)首地址
#define ONE_GROUP_SPACE					(sizeof(Group_t))
	
#define NEW_GROUP_SPACE					(150*5)	
#define GROUP_SCENE_NUM_MAX				255								//最大场景分组数量
#define GROUP_SCENE_START_ADDR			(0x500)							//分组场景信息存储首地址
#define ONE_GROUP_SCENE_INDEX_SPACE		(sizeof(GroupSceneIndex_t))		//一个分组场景索引所占空间
	
#define GROUP_SCENE_USE_SPACE				(0x20)											//场景内存表示占用空间大小

#define GROUP_SCENE_INDEX_START_ADD		(GROUP_SCENE_START_ADD + 1)								//设备场景索引数据存储地址	

#define GROUP_SCENE_START_ADD				(0x500)//(GROUP_START_ADD + NEW_GROUP_SPACE)					//场景起始地址(起始地址0x600)

#define GROUP_SCENE_INDEX_SPACE			(0x700)											//设备场景索引空间存储大小(支持3个扩展)

#define GROUP_SCENE_USE_FLAG_ADD			(GROUP_SCENE_START_ADD + GROUP_SCENE_INDEX_SPACE)			//场景内存使用标记地址(起始地址0xC00)

#define GROUP_SCENE_TIME_USE_FLAG_ADD		(GROUP_SCENE_USE_FLAG_ADD + GROUP_SCENE_USE_SPACE)			//场景时间内存使用标记地址	

#define GROUP_SCENE_USE_FLAG_TOTAL_SPACE	(0x80)											//场景内存使用标记总占用空间大小

#define GROUP_SCENE_TIME_USE_SPACE		(GROUP_SCENE_TIME_TOTAL_NUM/8)						//场景时间内存表示占用空间大小
#define GROUP_SCENE_TIME_TOTAL_NUM		(GROUP_SCENE_TIME_SPACE1_NUM +\
									 GROUP_SCENE_TIME_SPACE2_NUM +\
									 GROUP_SCENE_TIME_SPACE3_NUM)							//场景时间场景数量最大数

#define GROUP_SCENE_TIME_DATA_SPACE1      (10)											//场景时间空间1的大小
#define GROUP_SCENE_TIME_DATA_SPACE2      (35)											//场景时间空间2的大小
#define GROUP_SCENE_TIME_DATA_SPACE3      (50)											//场景时间空间3的大小

#define GROUP_SCENE_TIME_SPACE1_NUM		(32)											//场景延时空间1的数量
#define GROUP_SCENE_TIME_SPACE2_NUM		(16)											//场景延时空间2的数量
#define GROUP_SCENE_TIME_SPACE3_NUM		(8)												//场景延时空间3的数量

/*场景数据存储地址*/
#define GROUP_SCENE_SPACE1_NUM			(128)											//场景空间1的总数
#define GROUP_SCENE_SPACE2_NUM			(64)											//场景空间2的总数
#define GROUP_SCENE_SPACE3_NUM			(64)											//场景空间3的总数

#define GROUP_SCENE_MAX_NUM				(GROUP_SCENE_SPACE1_NUM +\
									 GROUP_SCENE_SPACE2_NUM +\
									 GROUP_SCENE_SPACE3_NUM)								//最大场景数
#define GROUP_SCENE_DATA_START_ADD		(GROUP_SCENE_USE_FLAG_ADD+GROUP_SCENE_USE_FLAG_TOTAL_SPACE)	//场景数据存储位置的起始地址(起始地址0xC80)
#define GROUP_SCENE_DATA_SPACE			(0x1000)										//场景数据存储空间,4K
#define GROUP_SCENE_DATA_SPACE1			(8)												//场景数据空间1大小
#define GROUP_SCENE_DATA_SPACE2			(16)											//场景数据空间2大小
#define GROUP_SCENE_DATA_SPACE3			(32)											//场景数据空间3大小


#pragma pack(1)     				//按1字节对齐
typedef struct
{
	u8  unit;	//
	u32 groupNum;
}Group_t;

typedef struct
{
	u8 cnt;
	Group_t group[GROUP_NUM_MAX];
}GroupManage_t;

typedef struct
{
	u8  unit;
	u8  addr;
	u32 groupSceneNum;
#if (SCENCETIME_EN > 0u)	
	u8 sceneTimeSavePos;		//场景存储位置,1~56
#endif
}GroupSceneIndex_t;



typedef struct
{
	u8 cnt;
	u8 sceneUseFlag[GROUP_SCENE_USE_SPACE];
	GroupSceneIndex_t groupScene[GROUP_SCENE_NUM_MAX];
#if (SCENCETIME_EN > 0u)
	u8 sceneTimeUseFlag[SCENE_TIME_USE_SPACE];
#endif
}GroupSceneManage_t;
#pragma pack()


extern MsgResult_t Get_Group_Scene_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);
extern MsgResult_t Set_Group_Scene_Attr(UnitPara_t *pData);
extern MsgResult_t Get_New_Group_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);
extern MsgResult_t Set_New_Group_Attr(UnitPara_t *pData);


#endif
/**************************Copyright BestFu 2014-05-14*************************/

