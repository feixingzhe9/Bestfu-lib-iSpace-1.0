/***************************Copyright BestFu 2014-05-14*************************
文	件：	Group.h
说	明：	iSpace分组数据操作头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	xjx
日	期：	2016-06-24
*******************************************************************************/

#ifndef __GROUPDATA_H
#define __GROUPDATA_H
#include "UnitCfg.h"

#define CLR_GROUP_DATA		0
#define SCENE_ADDR_ERR									((u16)(0xffff))
extern u32 GroupSceneEEPROM_SetEx(u32 groupSceneNum, u8 unit, u8 *data);
extern u8 GroupSceneEEPROM_Set(u32 groupNum, u8 unit, u8 *data);
extern u8 GroupNumExist(u32 groupNum,u8 unit);
extern u8 DelOneGroupNum(u32 groupNum, u8 unit);
extern void GroupSceneEEPROM_Init(void);
extern u8 ClrUnitGroupScene(u8 unit);
extern void GroupBufInit(void);
extern u32 SceneTimeEEPROMSave_Addr(u8 seat);
extern u8 GroupSceneEEPROM_Get(u8 i,u8 *data);
extern u8 GroupSceneEEPROM_GetEx(u8 i,  u8 *data);
extern u8 FindGroupInRange(u8 unit, u8 start, u8 tail, u32 *data);
extern u8 GetUnitGroupNum(u8 unit);
extern u8 GetUnitGroupSceneNum(u8 unit);
extern u8 FindTheGroupScene(u8 unit,u8 num);
extern u8 DelOneGroupScene(u32 groupSceneNum,u8 unit);
extern u8 GroupSceneNumExist(u32 groupSceneNum,u8 unit);
extern u8 ClrUnitNewGroup(u8 unit);
#endif


