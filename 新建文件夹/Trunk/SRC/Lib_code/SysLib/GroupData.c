/***************************Copyright BestFu 2016-06-24*************************
文	件：    GroupData.c
说	明:	    iSpace分组数据处理
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    xjx
日	期：    2016-06-24
修  改:     暂无
*******************************************************************************/

#include "GroupData.h"
#include "Group.h"

#include "TimeHandle.h"
#include "EEPROM.h"
#include "UserData.h"


extern GroupSceneManage_t *pGroupSceneManage;

extern GroupManage_t *pGroupManage;

u8 GroupSceneNumExist(u32 groupSceneNum,u8 unit);
void ClearGroupSceneTimePara(const u8 label);
u8 ClrUnitGroupScene(u8 unit);
void GroupSceneEEPROM_Init(void);
#if (SCENCETIME_EN > 0u)
static void  GroupSceneTimeUseFlag_Clr(const u8 i);
#endif
static u8 DelOneGroupSceneFromRam(u8 i);


/*******************************************************************************
函 数 名:  	GroupSceneEEPROM_Init
功能说明:  	清除所有分组场景
参    数:  	
返 回 值:  	
*******************************************************************************/
void GroupSceneEEPROM_Init(void)
{	
	WriteDataToEEPROMEx(GROUP_SCENE_START_ADD,sizeof(GroupSceneManage_t),0);
	
	WriteDataToEEPROMEx(GROUP_SCENE_USE_FLAG_ADD,GROUP_SCENE_USE_SPACE,0);
#if (SCENCETIME_EN > 0u)	
	WriteDataToEEPROMEx(GROUP_SCENE_TIME_USE_FLAG_ADD,GROUP_SCENE_TIME_USE_SPACE,0);
#endif	
}


/*******************************************************************************
函 数 名:  	GroupSceneEEPROM_Init
功能说明:  	清除所有分组场景
参    数:  	
返 回 值:  	
*******************************************************************************/
void NewGroupEEPROM_Init(void)
{	
	
	WriteDataToEEPROMEx(NEW_GROUP_START_ADDR,sizeof(GroupManage_t),0);
}


/*******************************************************************************
函 数 名:  	NewGroupBufInit
功能说明:   分组(非场景)初始化
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void NewGroupBufInit(void)
{
#if CLR_GROUP_DATA
	WriteDataToEEPROMEx(NEW_GROUP_START_ADDR,sizeof(GroupManage_t),0);
#endif
	ReadDataFromEEPROM(NEW_GROUP_START_ADDR,1, &pGroupManage->cnt);
	ReadDataFromEEPROM(NEW_GROUP_START_ADDR + 1,pGroupManage->cnt * ONE_GROUP_SPACE, (u8*)pGroupManage->group);
}

/*******************************************************************************
函 数 名:  	GroupSceneBufInit
功能说明:   分组(场景)初始化
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void GroupSceneBufInit(void)
{
#if CLR_GROUP_DATA
	WriteDataToEEPROMEx(GROUP_SCENE_START_ADD,sizeof(GroupSceneManage_t),0);	
	WriteDataToEEPROMEx(GROUP_SCENE_USE_FLAG_ADD,GROUP_SCENE_USE_SPACE,0);
#if (SCENCETIME_EN > 0u)	
	WriteDataToEEPROMEx(GROUP_SCENE_TIME_USE_FLAG_ADD,GROUP_SCENE_TIME_USE_SPACE,0);
#endif
#endif
	ReadDataFromEEPROM(GROUP_SCENE_START_ADD,1, &pGroupSceneManage->cnt);
	ReadDataFromEEPROM(GROUP_SCENE_INDEX_START_ADD,pGroupSceneManage->cnt * ONE_GROUP_SCENE_INDEX_SPACE, \
							(u8*)&pGroupSceneManage->groupScene);
	
	ReadDataFromEEPROM(GROUP_SCENE_USE_FLAG_ADD,GROUP_SCENE_USE_SPACE, (u8*)&pGroupSceneManage->sceneUseFlag);
	
#if (SCENCETIME_EN > 0u)
	ReadDataFromEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD,GROUP_SCENE_TIME_USE_SPACE, (u8*)&pGroupSceneManage->sceneTimeUseFlag);
#endif	
		
}

/*******************************************************************************
函 数 名:  	GroupBufInit
功能说明:   分组初始化
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void GroupBufInit(void)
{
	NewGroupBufInit();	
	GroupSceneBufInit();
}


/*******************************************************************************
函 数 名:  	Get_New_Group_Attr
功能说明:  	设置场景
参    数:  	unit:	需要查找的单元号
			start:	相对开始位置
			tail:	相对结束位置
			*data:	查找到的分组号存放
返 回 值:  	查询范围内有几个符合的分组号
*******************************************************************************/
u8 FindGroupInRange(u8 unit, u8 start, u8 tail, u32 *data)
{
	u8 i = 0;
	u8 cnt = 0;
	u8 num = 0;
	
	for(i = 0; i < pGroupManage->cnt; i++)
	{
		if(pGroupManage->group[i].unit == unit)
		{
			cnt++;
			if(cnt <= tail)
			{
				if(cnt >= start)
				{
					data[num] = pGroupManage->group[i].groupNum;
					num++;
				}
			}
			else
			{
				break;
			}
		}
	}	
	return num;
}


/*******************************************************************************
函 数 名:  	Get_New_Group_Attr
功能说明:  	设置场景
参    数:  	unit:	需要查找的单元号
			
返 回 值:  	查询单元有几个分组号
*******************************************************************************/
u8 GetUnitGroupNum(u8 unit)
{
	u8 num = 0;
	u8 i = 0;
	
	for(i = 0; i < pGroupManage->cnt; i++)
	{
		if(pGroupManage->group[i].unit == unit)
		{
			num++;
		}
	}
	return num;
}


/*******************************************************************************
函 数 名:  	GetUnitGroupSceneNum
功能说明:  	查询单元分组(场景)数量
参    数:  	unit:	需要查找的单元号
			
返 回 值:  	查询单元有几个分组(场景)号
*******************************************************************************/
u8 GetUnitGroupSceneNum(u8 unit)
{
	u8 num = 0;
	u8 i = 0;
	
	for(i = 0; i < pGroupSceneManage->cnt; i++)
	{
		if(pGroupSceneManage->groupScene[i].unit == unit)
		{
			num++;
		}
	}
	return num;
}

/*******************************************************************************
函 数 名:  	FindTheGroupScene
功能说明:  	查询单元内第几个分组(场景)号
参    数:  	unit:	需要查找的单元号
			
返 回 值:  	查询单元有几个分组(场景)号
*******************************************************************************/
u8 FindTheGroupScene(u8 unit,u8 num)
{
	u8 i = 0;
	u8 tmp = 0;
	
	for(i = 0; i < GROUP_SCENE_NUM_MAX; i++)
	{
		if(pGroupSceneManage->groupScene[i].unit == unit)
		{
			tmp++;
		}
		if(tmp == num)
		{
			return (i+1);
		}
	}
	return 0;
}
/*******************************************************************************
函 数 名:  	DelOneGroupSceneFromRam
功能说明:   从内存中删除特定分组场景
参    数:  	i:分组场景在内存中的顺序
返 回 值:  	
*******************************************************************************/
static u8 DelOneGroupSceneFromRam(u8 i)
{
	u8 label = 0;
#if (SCENCETIME_EN > 0u)
	u8 timeLabel = 0;
#endif
	if(0 == i)
	{
		return 0;
	}
	label = pGroupSceneManage->groupScene[i-1].addr;
	
#if (SCENCETIME_EN > 0u)	
	timeLabel = pGroupSceneManage->groupScene[i].sceneTimeSavePos;
#endif
	
	if(i == pGroupSceneManage->cnt)
	{
#if (SCENCETIME_EN > 0u)
		pGroupSceneManage->sceneTimeUseFlag[timeLabel/8] &= ~(1<<(timeLabel%8));
#endif
		pGroupSceneManage->sceneUseFlag[label/8] &= ~(1<<(label%8));
		pGroupSceneManage->cnt--;
		
//		pGroupSceneManage->groupScene[pGroupSceneManage->cnt].groupSceneNum = 0;
//		pGroupSceneManage->groupScene[pGroupSceneManage->cnt].unit = 0;
//		pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = 0;
		
	}
	else
	{
#if (SCENCETIME_EN > 0u)
		pGroupSceneManage->sceneTimeUseFlag[timeLabel/8] &= ~(1<<(timeLabel%8));
		pGroupSceneManage->groupScene[i - 1].sceneTimeSavePos = pGroupSceneManage->groupScene[pGroupSceneManage->cnt-1].sceneTimeSavePos;
#endif		
		pGroupSceneManage->sceneUseFlag[label/8] &= ~(1<<(label%8));
		
		pGroupSceneManage->groupScene[i - 1].addr = pGroupSceneManage->groupScene[pGroupSceneManage->cnt-1].addr;
		pGroupSceneManage->groupScene[i - 1].groupSceneNum = pGroupSceneManage->groupScene[pGroupSceneManage->cnt-1].groupSceneNum;
		
		pGroupSceneManage->groupScene[i - 1].unit = pGroupSceneManage->groupScene[pGroupSceneManage->cnt-1].unit;\
		pGroupSceneManage->cnt--;
		
//		pGroupSceneManage->groupScene[pGroupSceneManage->cnt].groupSceneNum = 0;
//		pGroupSceneManage->groupScene[pGroupSceneManage->cnt].unit = 0;
//		pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = 0;
	}
	return 1;
}
/*******************************************************************************
函 数 名:  	ClrUnitGroupScene
功能说明:  	清除所有单元分组场景
参    数:  	
返 回 值:  	
*******************************************************************************/
u8 ClrUnitGroupScene(u8 unit)
{
	u8 i = 0;
	const u8 writeToEepromNumOnce = 20;////写入EEPROM，一次写20*7=140个字节
	
	for(i = 0; i < pGroupSceneManage->cnt; i++)
	{
		if(unit == pGroupSceneManage->groupScene[i].unit)
		{
			DelOneGroupSceneFromRam(i+1);
			i--;
		}		
	}
	
	WriteDataToEEPROMEx(GROUP_SCENE_START_ADD , 1,pGroupSceneManage->cnt);

	WriteDataToEEPROM(GROUP_SCENE_USE_FLAG_ADD , GROUP_SCENE_USE_SPACE,(u8*)&pGroupSceneManage->sceneUseFlag);
#if (SCENCETIME_EN > 0u)	
	WriteDataToEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD , GROUP_SCENE_TIME_USE_SPACE,(u8*)&pGroupSceneManage->sceneTimeUseFlag);
#endif	
	for(i = 0; i < pGroupSceneManage->cnt/writeToEepromNumOnce; i++)
	{
		WriteDataToEEPROM(GROUP_SCENE_START_ADDR + 1 + i * ONE_GROUP_SCENE_INDEX_SPACE * writeToEepromNumOnce, \
					ONE_GROUP_SCENE_INDEX_SPACE * writeToEepromNumOnce,(u8*)&pGroupSceneManage->groupScene[i * writeToEepromNumOnce]);
	}
	if(pGroupSceneManage->cnt % writeToEepromNumOnce)
	{
		WriteDataToEEPROM(GROUP_SCENE_START_ADDR + 1 + i * ONE_GROUP_SCENE_INDEX_SPACE * writeToEepromNumOnce, \
					ONE_GROUP_SCENE_INDEX_SPACE * (pGroupSceneManage->cnt % writeToEepromNumOnce),(u8*)&pGroupSceneManage->groupScene[i * writeToEepromNumOnce]);
	}
	
	
	return 1;
}


/*******************************************************************************
函 数 名:  	DelOneGroupSceneFromRam
功能说明:   从内存中删除特定分组场景
参    数:  	i:分组场景在内存中的顺序
返 回 值:  	
*******************************************************************************/
static u8 DelOneNewGroupFromRam(u8 i)
{

	if(0 == i)
	{
		return 0;
	}

	if(i == pGroupManage->cnt)//要删除的在最后一个
	{		
		pGroupManage->cnt--;
		
//		pGroupManage->group[pGroupManage->cnt].groupNum = 0;
//		pGroupManage->group[pGroupManage->cnt].unit = 0;
		
	}
	else
	{
		pGroupManage->group[i - 1].groupNum = pGroupManage->group[pGroupManage->cnt-1].groupNum;
		
		pGroupManage->group[i - 1].unit = pGroupManage->group[pGroupManage->cnt-1].unit;
		pGroupManage->cnt--;
		
//		pGroupManage->group[pGroupManage->cnt].groupNum = 0;
//		pGroupManage->group[pGroupManage->cnt].unit = 0;

	}
	return 1;
}
/*******************************************************************************
函 数 名:  	ClrUnitGroupScene
功能说明:  	清除所有单元分组场景
参    数:  	
返 回 值:  	
*******************************************************************************/
u8 ClrUnitNewGroup(u8 unit)
{
	u8 i = 0;
	const u8 writeToEepromNumOnce = 20;////写入EEPROM，一次写20*5=个字节
	
	for(i = 0; i < pGroupManage->cnt; i++)
	{
		if(unit == pGroupManage->group[i].unit)
		{
			DelOneNewGroupFromRam(i+1);
			i--;
		}		
	}
	
	WriteDataToEEPROMEx(NEW_GROUP_START_ADDR , 1,pGroupManage->cnt);


	for(i = 0; i < pGroupManage->cnt/writeToEepromNumOnce; i++)
	{
		WriteDataToEEPROM(NEW_GROUP_START_ADDR + 1 + i * ONE_GROUP_SPACE * writeToEepromNumOnce, \
					ONE_GROUP_SPACE * writeToEepromNumOnce,(u8*)&pGroupManage->group[i * writeToEepromNumOnce]);
	}
	if(pGroupManage->cnt % writeToEepromNumOnce)
	{
		WriteDataToEEPROM(NEW_GROUP_START_ADDR + 1 + i * ONE_GROUP_SPACE * writeToEepromNumOnce, \
					ONE_GROUP_SPACE * (pGroupManage->cnt % writeToEepromNumOnce),(u8*)&pGroupManage->group[i * writeToEepromNumOnce]);
	}
	
	
	return 1;
}



#if (SCENCETIME_EN > 0u)
/*******************************************************************************
函 数 名：  ClearSceneTimePara
功能说明：  清除场景中的时间参数
参   数：   i:场景在内存中的顺序
返 回 值：  被SceneEEPROM_Set函数使用
*******************************************************************************/
void ClearGroupSceneTimePara(const u8 i)
{
	u8* pTimeLabel = &pGroupSceneManage->groupScene[i].sceneTimeSavePos;
	
	if(SCENE_DEFAULT_VALUE != *pTimeLabel)
	{
		if((*pTimeLabel <= SCENE_TIME_TOTAL_NUM)&&\
			(pGroupSceneManage->sceneTimeUseFlag[(*pTimeLabel - 1)/8]&(1<<((*pTimeLabel - 1)%8))))
		{
			GroupSceneTimeUseFlag_Clr(i);	
		}
		pGroupSceneManage->groupScene[i].sceneTimeSavePos = SCENE_DEFAULT_VALUE;
		WriteDataToEEPROMEx(GROUP_SCENE_INDEX_START_ADD + ONE_GROUP_SCENE_INDEX_SPACE*i + offsetof(GroupSceneIndex_t, sceneTimeSavePos),\
			sizeof(pGroupSceneManage->groupScene[i].sceneTimeSavePos), SCENE_DEFAULT_VALUE);
	}
}
#endif

/*******************************************************************************
函 数 名:  	DelOneGroupScene
功能说明:  	删除一个场景
参    数:  	i:场景内存的顺序
返 回 值:  	1:删除成功
			0:失败
*******************************************************************************/
u8 DelOneGroupScene(u32 groupSceneNum,u8 unit)
{
	u8 i = GroupSceneNumExist(groupSceneNum,unit);
	u8 label = 0;
#if (SCENCETIME_EN > 0u)
	u8 timeLabel = 0;
#endif
	
	if(i > 0)//存在
	{	
		label = pGroupSceneManage->groupScene[i-1].addr;
#if (SCENCETIME_EN > 0u)
		timeLabel = pGroupSceneManage->groupScene[i-1].sceneTimeSavePos;
#endif		
		if(i == pGroupSceneManage->cnt)//需要删除的分组号在最后面，直接删除
		{			
			pGroupSceneManage->cnt--;
			
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].groupSceneNum = 0;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].unit = 0;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = 0;

#if (SCENCETIME_EN > 0u)	
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].sceneTimeSavePos = 0;
#endif
						
		}
		else		//需要删除的分组号不在最后面，把最后面的分组号覆盖到要删除的位置
		{
			pGroupSceneManage->groupScene[i - 1].groupSceneNum = pGroupSceneManage->groupScene[pGroupSceneManage->cnt - 1].groupSceneNum;			
			pGroupSceneManage->groupScene[i - 1].unit = pGroupSceneManage->groupScene[pGroupSceneManage->cnt - 1].unit;//用最后一个分组号把要删除的分组号覆盖
			pGroupSceneManage->groupScene[i - 1].addr = pGroupSceneManage->groupScene[pGroupSceneManage->cnt - 1].addr;
#if (SCENCETIME_EN > 0u)			
			pGroupSceneManage->groupScene[i - 1].sceneTimeSavePos = pGroupSceneManage->groupScene[pGroupSceneManage->cnt - 1].sceneTimeSavePos;
#endif
			
			pGroupSceneManage->cnt--;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].groupSceneNum = 0;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].unit = 0;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = 0;

#if (SCENCETIME_EN > 0u)	
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].sceneTimeSavePos = 0;
#endif
			WriteDataToEEPROM(GROUP_SCENE_START_ADDR + 1 + (i - 1) * ONE_GROUP_SCENE_INDEX_SPACE, \
								ONE_GROUP_SCENE_INDEX_SPACE,(u8*)&pGroupSceneManage->groupScene[i - 1]);	
		}
		
		
		WriteDataToEEPROMEx(GROUP_SCENE_START_ADDR, 1, pGroupSceneManage->cnt);//总的计数
		
		pGroupSceneManage->sceneUseFlag[label/8] &= ~(1<<(label%8));
		
		WriteDataToEEPROMEx(GROUP_SCENE_USE_FLAG_ADD + (label / 8), \
								1,pGroupSceneManage->sceneUseFlag[label/8]);//存储地址映射表
#if (SCENCETIME_EN > 0u)
		pGroupSceneManage->sceneTimeUseFlag[timeLabel/8] &= ~(1<<(timeLabel%8));
		WriteDataToEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD + (timeLabel / 8), \
								1,(u8*)&pGroupSceneManage->sceneUseFlag[timeLabel/8]);//时间存储地址映射
#endif
		
	}
	return 1;
}


/*******************************************************************************
函 数 名：  inline u32 Check_PositionRange(u8 position)
功能说明：  检查场景的起始存储位置
参   数：   position:待检查场景的起始存储位置的变量
返 回 值：  返回场景的起始存储位置的值
*******************************************************************************/
u32 GroupSceneCheck_PositionRange(u8 position)
{
    if (position < SCENE_SPACE1_NUM)
    {
        return 1;
    }
    else if (position < SCENE_SPACE1_NUM + SCENE_SPACE2_NUM)
    {
        return (SCENE_SPACE1_NUM);
    }
    else
    {
        return (SCENE_SPACE1_NUM + SCENE_SPACE2_NUM);
    }
}


/*******************************************************************************
函 数 名：  SceneEEPROMData_Save
功能说明：  场景EEPROM存储数据
参   数：   addr:   存储空地址
            data:   要存储的场景数据
返 回 值：  存储编号（0～127）/0xFF(没有存储空间)
*******************************************************************************/
void GroupSceneEEPROMData_Save(u32 addr, u8 *data)
{
    WriteDataToEEPROM(addr, data[0] + 1, data); //保存数据
}

/*******************************************************************************
函 数 名：  SceneEEPROMSave_Addr
功能说明：  场景EEPROM存储地址
参   数：   seat:   存储编号
返 回 值：  存储地址
*******************************************************************************/
u32 GroupSceneEEPROMSave_Addr(u8 seat)
{
    u32 addr = GROUP_SCENE_DATA_START_ADD;

    if (seat < GROUP_SCENE_SPACE1_NUM)
    {
        addr += seat * GROUP_SCENE_DATA_SPACE1;
        goto RETURN;//位置处于存储空间1
    }

    addr += GROUP_SCENE_SPACE1_NUM * GROUP_SCENE_DATA_SPACE1;
    seat -= GROUP_SCENE_SPACE1_NUM;
    if (seat < GROUP_SCENE_SPACE2_NUM)
    {
        addr += seat * GROUP_SCENE_DATA_SPACE2;
        goto RETURN;//位置处于存储空间2
    }

    addr += GROUP_SCENE_SPACE2_NUM * GROUP_SCENE_DATA_SPACE2;
    seat -= GROUP_SCENE_SPACE2_NUM;
    if (seat < GROUP_SCENE_SPACE3_NUM)
    {
        addr += seat * GROUP_SCENE_DATA_SPACE3;
        goto RETURN;//位置处于存储空间3
    }

RETURN:
    return addr;
}


/*******************************************************************************
函 数 名：  SceneEEPROMSave_Seat
功能说明：  场景EEPROM存储位置
参   数：   seat:   存储长度
返 回 值：  存储位置
*******************************************************************************/
u32 GroupSceneEEPROMSave_Seat(const u8 len)
{
    if (len < GROUP_SCENE_DATA_SPACE1)    //长度处于第一存储空间
    {
        return  1;
    }
    else if (len < (GROUP_SCENE_DATA_SPACE2))     //长度处于第二存储空间
    {
        return GROUP_SCENE_SPACE1_NUM;
    }
    else if (len < (GROUP_SCENE_DATA_SPACE3))     //长度处于第三存储空间
    {
        return (GROUP_SCENE_SPACE1_NUM + GROUP_SCENE_SPACE2_NUM);
    }

    return GROUP_SCENE_MAX_NUM;   //长度越界
}


/*******************************************************************************
函 数 名：  SceneEEPROMSaveData_Seat
功能说明：  申请一个场景存储位置
参   数：   len:    目标存储数据长度
返 回 值：  number: 存储的位置值,MACRO_SCENE_ERR:表示没有申请到
*******************************************************************************/
u16 GroupSceneEEPROMData_Seat(const u8 len)
{
    u16 i;
	u16 xReturn = SCENE_ADDR_ERR;

	for(i = GroupSceneEEPROMSave_Seat(len); i < GROUP_SCENE_MAX_NUM; i++)
	{
		if (!(pGroupSceneManage->sceneUseFlag[i / 8] & (1 << (i % 8))))   //如果存储位置是空闲的
		{
			xReturn = i;
			break;
		}
	}
	return (xReturn);
}
///*******************************************************************************
//函 数 名：  static void  SceneUseFlag_Set(const u8 label)
//功能说明：  设置场景内存使用标记
//参   数：   label: 场景数据编号(0x00~0xff)
//返 回 值：  无
//*******************************************************************************/
//static void  GroupSceneUseFlag_Set(const u8 label)
//{
//	pGroupSceneManage->sceneUseFlag[label/8] |= 1<<(label%8);
//	WriteDataToEEPROM(GROUP_SCENE_USE_FLAG_ADD + (label)/8, 1, (u8*)&pGroupSceneManage->sceneUseFlag[(label)/8]);
//}

/*******************************************************************************
函 数 名:  	GroupNumExist
功能说明:  	判断分组号是否已经存在
参    数:  	groupNum:分组号
返 回 值:  	大于0:存在，
			0:不存在
*******************************************************************************/
u8 GroupNumExist(u32 groupNum,u8 unit)
{
	u8 i = 0;
	for(i = 0; i < pGroupManage->cnt; i++)
	{
		if((groupNum == pGroupManage->group[i].groupNum) && (unit == pGroupManage->group[i].unit))
		{
			return (i + 1);//
		}
	}
	
	return 0;
}

/*******************************************************************************
函 数 名:  	GroupSceneNumExist
功能说明:  	判断分组号是否已经存在
参    数:  	groupNum:分组号
返 回 值:  	大于0:存在，
			0:不存在
*******************************************************************************/
u8 GroupSceneNumExist(u32 groupSceneNum,u8 unit)
{
	u8 i = 0;
	for(i = 0; i < pGroupSceneManage->cnt; i++)
	{
		if((groupSceneNum == pGroupSceneManage->groupScene[i].groupSceneNum) && (unit == pGroupSceneManage->groupScene[i].unit))
		{
			return (i + 1);//
		}
	}
	
	return 0;
}

/*******************************************************************************
函 数 名:  	FindGroupScene
功能说明:  	查找分组场景
参    数:  	group:分组号
			unit:单元号
			data:待写入数据的内容
返 回 值:  	大于0：存储地址偏移量	
			0:未找到
*******************************************************************************/
u8 GroupSceneEEPROM_Set(u32 groupNum, u8 unit, u8 *data)
{
	u8 i = GroupSceneNumExist(groupNum,unit);
	u16 label = 0;
	u16 labelTmp = 0;
	u8 labelClrFlag = 0;
	u8 flag = 0;
	if(0 != i)
	{
		label = pGroupSceneManage->groupScene[i-1].addr;
	}
	else
	{
		flag = 1;//说明没有找到该分组场景
	}
	
	labelTmp = label;	
	
	if(0 != label)
	{
#if (SCENCETIME_EN > 0u)
		ClearGroupSceneTimePara(i);
#endif
		
		if ((!(pGroupSceneManage->sceneUseFlag[((u8)label)/8] & (1<<(((u8)label)%8))))||\
			(GroupSceneCheck_PositionRange(((u8)label)) != GroupSceneEEPROMSave_Seat(data[0])))     //相同场景号的情况下,所需要的存储空间内存块不同,需要重新申请		
		{
			pGroupSceneManage->sceneUseFlag[((u8)label)/8] &= ~(1<<(((u8)label)%8));//长度不一致，清除之前的存储标识
			labelClrFlag = 1;
			label = 0;
		}	
	}
	
	 if(0 == label)   	//未找到该场景，场景还没有申请空间
    {
		if(SCENE_ADDR_ERR == (label = (GroupSceneEEPROMData_Seat(data[0]))))//申请一个存储空间
        {
			if(1 == labelClrFlag)
			{
				pGroupSceneManage->sceneUseFlag[((u8)labelTmp)/8] |= (1<<(((u8)labelTmp)%8));//恢复之前清除的存储标识
			}
			
            return EEPROM_RAND_ERR;
        }
		if(1 == flag)//该分组(场景)是新的
		{			
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].groupSceneNum = groupNum;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].unit = unit;			
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = ((u8)label);	
			
			pGroupSceneManage->cnt++;
			WriteDataToEEPROMEx(GROUP_SCENE_START_ADD,1, pGroupSceneManage->cnt);	
			
			i = pGroupSceneManage->cnt;
		}
//		else	//说明场景实际存储位置发生了改变
//		{
//			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = label;	
//		}
			
		pGroupSceneManage->sceneUseFlag[((u8)label)/8] |= (1<<(((u8)label)%8));
		
		pGroupSceneManage->groupScene[i-1].addr = (u8)label;
		
		WriteDataToEEPROM(GROUP_SCENE_INDEX_START_ADD + ONE_GROUP_SCENE_INDEX_SPACE * (i-1), \
							ONE_GROUP_SCENE_INDEX_SPACE, (u8*)&pGroupSceneManage->groupScene[i-1]);		
				
	}
	if((1 == flag) || (1 == labelClrFlag))
	{
		WriteDataToEEPROM(GROUP_SCENE_USE_FLAG_ADD ,GROUP_SCENE_USE_SPACE, (u8*)&pGroupSceneManage->sceneUseFlag);//
	}
	
	GroupSceneEEPROMData_Save(GroupSceneEEPROMSave_Addr(((u8)label)),data);
	
	return 1;
}

/*******************************************************************************
函 数 名:  	DelOneGroupNum
功能说明:  	删除一个分组号(非场景)
参    数:  	groupNum:分组号
			unit	:单元号
返 回 值:  	
			
*******************************************************************************/
u8 DelOneGroupNum(u32 groupNum, u8 unit)
{
	u8 i = 0;
	
	i = GroupNumExist(groupNum,unit);
	
	if(i > 0)//存在
	{

		if(i == pGroupManage->cnt)//需要删除的分组号在最后面，直接删除
		{
			pGroupManage->group[pGroupManage->cnt - 1].groupNum = 0;
			pGroupManage->group[pGroupManage->cnt - 1].unit = 0;
			
			pGroupManage->cnt--;					
		}
		else		//需要删除的分组号不在最后面，把最后面的分组号覆盖到要删除的位置
		{
			pGroupManage->group[i - 1].groupNum = pGroupManage->group[pGroupManage->cnt - 1].groupNum;
			pGroupManage->group[i - 1].unit = pGroupManage->group[pGroupManage->cnt - 1].unit;//用最后一个分组号把要删除的分组号覆盖
			pGroupManage->group[pGroupManage->cnt - 1].groupNum = 0;
			pGroupManage->group[pGroupManage->cnt - 1].unit = 0;
			
			pGroupManage->cnt--;
			
			WriteDataToEEPROM(NEW_GROUP_START_ADDR + 1 + (i - 1) * ONE_GROUP_SPACE, ONE_GROUP_SPACE,(u8*)&pGroupManage->group[i - 1]);
		}
		WriteDataToEEPROMEx(NEW_GROUP_START_ADDR, 1, pGroupManage->cnt);
	}
	return 1;
}


/*******************************************************************************
函 数 名：  GetCurGroupSceneSpace
功能说明：  获取场景标号对应的存储大小
参   数：   label:场景时间标号
返 回 值：  内存块的大小
*******************************************************************************/
static u8 GetCurGroupSceneSpace(const u8 label)
{
	if(label < SCENE_SPACE1_NUM)
	{
		return SCENE_DATA_SPACE1;
	}
	else if(label < SCENE_SPACE1_NUM + SCENE_SPACE2_NUM)
	{
		return SCENE_DATA_SPACE2;
	}
	else
	{
		return SCENE_DATA_SPACE3;
	}
}


/*******************************************************************************
函 数 名：  SceneEEPROM_Get
功能说明：  获取场景号标识是否有效
参   数：   unit:       单元位置
            area：      区域位置(0~4)
            num:        组号
            data[0]:    场景内容长度 data[0] = 0, 没有此场景
            data[1~n]:  场景内容
返 回 值：  标识结果 TRUE(有效)/FALSE(无效)
*******************************************************************************/
u8 GroupSceneEEPROM_Get(u8 i,u8 *data)
{
    u8 addr = 0;

    if(0 != i)
    {
		addr = pGroupSceneManage->groupScene[i].addr;
        ReadDataFromEEPROM(GroupSceneEEPROMSave_Addr(addr),GetCurGroupSceneSpace(addr), data);  //保存数据                        
    }
    else
    {
        *data = 0;  //返回场景长度为零
		return FALSE;
    }
	return TRUE;
}

#if (SCENCETIME_EN > 0u)


/*******************************************************************************
函 数 名：  static void  GroupSceneTimeUseFlag_Clr(const u8 label)
功能说明：  清除场景时间内存使用标记
参   数：   i 场景内存顺序
返 回 值：  无
*******************************************************************************/
static void  GroupSceneTimeUseFlag_Clr(const u8 i)
{
	pGroupSceneManage->sceneTimeUseFlag[i/8] &= ~(1<<(i%8));
	WriteDataToEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD + (i - 1)/8, 1, (u8*)&pGroupSceneManage->sceneTimeUseFlag[(i - 1)/8]);
}

///*******************************************************************************
//函 数 名：  static void  GroupSceneTimeUseFlag_Set(const u8 label)
//功能说明：  设置场景时间内存使用标记
//参   数：   label: 场景数据编号(0x01~56)
//返 回 值：  无
//*******************************************************************************/
//static void  GroupSceneTimeUseFlag_Set(const u8 label)
//{
//	pGroupSceneManage->sceneTimeUseFlag[label/8] |= 1<<(label%8);	
//	WriteDataToEEPROM(SCENE_TIME_USE_FLAG_ADD + (label - 1)/8, 1, (u8*)&pGroupSceneManage->sceneTimeUseFlag[(label - 1)/8]);
//}

/*******************************************************************************
函 数 名：  inline u32 CheckTime_PositionRange(u8 position)
功能说明：  检查场景的起始存储位置
参   数：   position:待检查场景的起始存储位置的变量
返 回 值：  返回场景的起始存储位置的值
*******************************************************************************/
__inline u32 GroupCheckTime_PositionRange(u8 position)
{
    if (position <= SCENE_TIME_SPACE1_NUM)
    {
        return 0;
    }
    else if (position <= SCENE_TIME_SPACE1_NUM + SCENE_TIME_SPACE2_NUM)
    {
        return (SCENE_TIME_SPACE1_NUM);
    }
    else if (position <= SCENE_TIME_TOTAL_NUM)
    {
        return (SCENE_TIME_SPACE1_NUM + SCENE_TIME_SPACE2_NUM);
    }
	else
	{
		return SCENE_TIME_TOTAL_NUM;
	}
}

/*******************************************************************************
函 数 名：  SceneTimeSave_Seat
功能说明：  场景EEPROMTime存储位置
参   数：   seat:   存储长度
返 回 值：  存储位置
*******************************************************************************/
static u32 GroupSceneTimeSave_Seat(const u8 len)
{
    if (len <= SCENE_TIME_DATA_SPACE1)    //长度处于第一存储空间
    {
        return  1;
    }
    else if (len <= (SCENE_TIME_DATA_SPACE2))     //长度处于第二存储空间
    {
        return SCENE_TIME_SPACE1_NUM;
    }
    else if (len <= (SCENE_TIME_DATA_SPACE3))     //长度处于第三存储空间
    {
        return (SCENE_TIME_SPACE1_NUM + SCENE_TIME_SPACE2_NUM);
    }

    return SCENE_TIME_TOTAL_NUM;   //长度越界
}

/*******************************************************************************
函 数 名：  SceneEEPROMTimeData_Seat
功能说明：  申请一个场景时间存储位置
参   数：   len:    目标存储数据长度
返 回 值：  number: 存储的位置值,MACRO_SCENE_ERR:表示没有申请到
*******************************************************************************/
static u8 SceneEEPROMTimeData_Seat(const u8 len)
{
    u16 i;
	u16 xReturn = SCENE_DEFAULT_VALUE;

	for(i = GroupSceneTimeSave_Seat(len); i < SCENE_TIME_TOTAL_NUM; i++)
	{
		if (!(pGroupSceneManage->sceneTimeUseFlag[i / 8] & (1 << (i % 8))))   //如果存储位置是空闲的
		{
			xReturn = i;
			break;
		}
	}
	return (xReturn);
}

/*******************************************************************************
函 数 名：  SceneTimeEEPROMSave_Addr
功能说明：  场景时间EEPROM存储地址
参   数：   seat:   存储编号
返 回 值：  存储地址
*******************************************************************************/
u32 SceneTimeEEPROMSave_Addr(u8 seat)
{
    u32 addr = SCENE_TIME_DATA_START_ADD;
	
	if(seat <= SCENE_TIME_SPACE1_NUM)
	{
		addr += (seat-1) * SCENE_TIME_DATA_SPACE1;
        goto RETURN;		//位置处于存储空间1
	}

    addr += SCENE_TIME_SPACE1_NUM * SCENE_DATA_SPACE1;
    seat -= SCENE_TIME_SPACE1_NUM;
    if (seat <= SCENE_SPACE2_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE2;
        goto RETURN;//位置处于存储空间2
    }

    addr += SCENE_TIME_SPACE2_NUM * SCENE_TIME_DATA_SPACE2;
    seat -= SCENE_TIME_SPACE2_NUM;
    if (seat <= SCENE_TIME_SPACE3_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE3;
        goto RETURN;//位置处于存储空间3
    }

RETURN:
    return addr;
}
#endif


#if (SCENCETIME_EN > 0u)

/*******************************************************************************
函 数 名：  static void SceneParamSeparate(u8 *timedata, u8 *orgdata)
功能说明：  场景数据分离函数
参   数：   timedata：时间数据存储指针,其中timedata[0]表哦是其长度
			orgdata：时间数据存储指针,orgdata[0]表示是其长度
返 回 值：  无
*******************************************************************************/
static void GroupSceneParamSeparate(u8* timedata, u8* orgdata)
{
	const u8 len = orgdata[0];
	u8  pos = 0,pos1 = 0;
	u8* orgaddr;
	
	while( len > pos + pos1)
	{
		orgaddr = &orgdata[pos + 1] + ((UnitPara_t *)&orgdata[pos + 1])->len + offsetof(UnitPara_t,data);
		memcpy(timedata + pos1 + 1, orgaddr, sizeof(DlyTimeData_t));
		pos += ((UnitPara_t *)&orgdata[pos + 1])->len + offsetof(UnitPara_t,data);
		pos1 += sizeof(DlyTimeData_t);
		memmove(orgaddr, orgaddr + sizeof(DlyTimeData_t), len - pos - pos1);
	}
	timedata[0] = pos1;
	orgdata[0] = pos;
}



/*******************************************************************************
函 数 名：  SceneTimeEEPROMSave_Addr
功能说明：  场景时间EEPROM存储地址
参   数：   seat:   存储编号
返 回 值：  存储地址
*******************************************************************************/
static u32 GroupSceneTimeEEPROMSave_Addr(u8 seat)
{
    u32 addr = SCENE_TIME_DATA_START_ADD;
	
	if(seat <= SCENE_TIME_SPACE1_NUM)
	{
		addr += (seat-1) * SCENE_TIME_DATA_SPACE1;
        goto RETURN;		//位置处于存储空间1
	}

    addr += SCENE_TIME_SPACE1_NUM * SCENE_DATA_SPACE1;
    seat -= SCENE_TIME_SPACE1_NUM;
    if (seat <= SCENE_SPACE2_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE2;
        goto RETURN;//位置处于存储空间2
    }

    addr += SCENE_TIME_SPACE2_NUM * SCENE_TIME_DATA_SPACE2;
    seat -= SCENE_TIME_SPACE2_NUM;
    if (seat <= SCENE_TIME_SPACE3_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE3;
        goto RETURN;//位置处于存储空间3
    }

RETURN:
    return addr;
}

/*******************************************************************************
函 数 名：  u32 SceneEEPROM_SetEx(u8 unit, u8 area, u8 num, u8 *data)
功能说明：  写入一个场景的扩展接口,支持延时顺序执行
参   数：   unit:       单元位置
            area：      区域位置(0~4)
            num:        情景号
            cnt:        属性命令数量
            data[0~n]:  场景时间数据内容
返 回 值：  存储结果
*******************************************************************************/
u32 GroupSceneEEPROM_SetEx(u32 groupSceneNum, u8 unit, u8 *data)
{
	
	u8 	data_tmp[SCENE_TIME_DATA_SPACE3 + 1];
	u8 i = GroupSceneNumExist(groupSceneNum,unit);
	u8 label = 0;
	u8 timePos = SCENE_DEFAULT_VALUE;
	u8 flag = 0;//用来标识场景是否存在
	u8 timeChangeFlag = 0;
	u8 changeFlag = 0;
	u8 labelTmp = 0;
	u8 timePosTmp = 0;
	u8 status = 0;
//	u8 cntFlag = 0;//总数变化标识
	
	if(i != 0)
	{
		label = pGroupSceneManage->groupScene[i-1].addr;
		timePos = pGroupSceneManage->groupScene[i-1].sceneTimeSavePos;
		labelTmp = label;
		timePosTmp = timePos;
	}
	else//场景不存在
	{
		pGroupSceneManage->cnt++;
		flag = 1;
		
		i = pGroupSceneManage->cnt;
	}
	
	GroupSceneParamSeparate((u8*)data_tmp, (u8*)data);		//分离数据
	if(TRUE == CheckAllTimeParaIsOK((DlyTimeData_t*)&data_tmp[1], data_tmp[0]/sizeof(DlyTimeData_t)))
	{
		if(label != 0)
		{
			if(SCENE_DEFAULT_VALUE != (timePos = pGroupSceneManage->groupScene[i-1].sceneTimeSavePos))
			{
				if((timePos > SCENE_TIME_TOTAL_NUM)||\
					(!(pGroupSceneManage->sceneTimeUseFlag[timePos/8]&(1<<(timePos%8)))))
				{
					timePos = SCENE_DEFAULT_VALUE;
				}
				else if(GroupCheckTime_PositionRange(timePos) != GroupSceneTimeSave_Seat(data_tmp[0]))
				{
//					GroupSceneTimeUseFlag_Clr(timePos);//????
					timeChangeFlag = 1;
					pGroupSceneManage->sceneTimeUseFlag[timePos/8] &= ~(1<<(timePos%8));
					timePos = SCENE_DEFAULT_VALUE;
				}
			}
			if((!(pGroupSceneManage->sceneUseFlag[label/8]&(1<<(label%8))))||\
				(GroupSceneCheck_PositionRange(label) != GroupSceneEEPROMSave_Seat(data[0])))
			{
//				DelOneGroupScene(i);				//清除之前存储标识,不删除时间参数
				changeFlag = 1;
				pGroupSceneManage->sceneUseFlag[label/8] |= 1<<(label%8);
				label = 0;
			}
		}
			
		if(SCENE_DEFAULT_VALUE == timePos)
		{
			if(SCENE_DEFAULT_VALUE == (timePos = (SceneEEPROMTimeData_Seat(data_tmp[0]))))//申请一个存储空间
			{
				if(1 == flag)
				{
					pGroupSceneManage->cnt--;//申请空间失败，恢复场景总数
				}
				if(1 == timeChangeFlag)
				{
					pGroupSceneManage->sceneTimeUseFlag[timePosTmp/8] |= 1<<(timePosTmp%8);//申请空间失败，恢复时间存储标识
				}
				return EEPROM_RAND_ERR;
			}
			status |= (1 << 0);
		}
		
		if(0 == label)   	//未找到该场景，场景还没有申请空间
		{
			if(0 == (label = (GroupSceneEEPROMData_Seat(data[0]))))//申请一个存储空间
			{
				if(1 == flag)
				{
					pGroupSceneManage->cnt--;//申请空间失败，恢复场景总数
				}
				if(1 == changeFlag)
				{
					pGroupSceneManage->sceneUseFlag[labelTmp/8] |= 1<<(labelTmp%8);//申请空间失败，恢复场景存储标识
				}
				return EEPROM_RAND_ERR;
			}
			status |= (1 << 1); 
		}
		
		switch(status)
		{
		case 3:
		case 2:
//			GroupSceneUseFlag_Set(label);
			pGroupSceneManage->sceneUseFlag[label/8] |= 1<<(label%8);
			pGroupSceneManage->groupScene[i-1].addr = label;//////
			pGroupSceneManage->groupScene[i-1].groupSceneNum = groupSceneNum;
			pGroupSceneManage->groupScene[i-1].unit = unit;
		case 1:
			if((1 == status)||(3 == status))
			{
//				GroupSceneTimeUseFlag_Set(timePos);
				pGroupSceneManage->sceneTimeUseFlag[timePos/8] |= 1<<(timePos%8);	
				pGroupSceneManage->groupScene[i-1].sceneTimeSavePos = timePos;//////
				pGroupSceneManage->groupScene[i-1].groupSceneNum = groupSceneNum;
				pGroupSceneManage->groupScene[i-1].unit = unit;
			}
//			SET_SCENE_INDEX_VAL(unit,area,num,label,timePos);
			WriteDataToEEPROM(GROUP_SCENE_INDEX_START_ADD + ONE_GROUP_SCENE_INDEX_SPACE * (i - 1), \
								ONE_GROUP_SCENE_INDEX_SPACE, (u8*)&pGroupSceneManage->groupScene[i-1]);
		break;
		default:
			break;		
		}
		
		if(1 == flag)
		{
			WriteDataToEEPROMEx(GROUP_SCENE_START_ADD , 1, pGroupSceneManage->cnt);
		}
		if((1 == changeFlag) || (1 == flag))
		{
			WriteDataToEEPROM(GROUP_SCENE_USE_FLAG_ADD,GROUP_SCENE_USE_SPACE,(u8*)&pGroupSceneManage->sceneUseFlag);//写场景存储标识
		}
		
		if((1 == timeChangeFlag) || (1 == flag))
		{
			WriteDataToEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD,GROUP_SCENE_TIME_USE_SPACE,\
							(u8*)&pGroupSceneManage->sceneTimeUseFlag);//写时间场景存储标识
		}
		
		
		WriteDataToEEPROM(GroupSceneTimeEEPROMSave_Addr(timePos), data_tmp[0], &data_tmp[1]); //写时间场景内容
		GroupSceneEEPROMData_Save(GroupSceneEEPROMSave_Addr(label), data);	//写场景内容
		return COMPLETE;
	}
	return TIME_PARA_ERR;	
}


/*******************************************************************************
函 数 名：  GetCurSceneTimeSpace
功能说明：  获取场景时间标号对应的存储大小
参   数：   label:场景时间标号
返 回 值：  内存块的大小
*******************************************************************************/
static u8 GetCurGroupSceneTimeSpace(const u8 label)
{
	if(label <= SCENE_TIME_SPACE1_NUM)
	{
		return SCENE_TIME_DATA_SPACE1;
	}
	else if(label <= SCENE_TIME_SPACE1_NUM + SCENE_TIME_SPACE2_NUM)
	{
		return SCENE_TIME_DATA_SPACE2;
	}
	else
	{
		return SCENE_TIME_DATA_SPACE3;
	}
}

/*******************************************************************************
函 数 名：  static void SceneParamCombine(u8 *timedata, u8 *orgdata)
功能说明：  场景数据合并函数
参   数：   timedata：时间数据存储指针
			orgdata：时间数据存储指针,orgdata[0]表示长度
返 回 值：  无
*******************************************************************************/
static void GroupSceneParamCombine(u8* timedata, u8* orgdata)
{
	const u8 len = orgdata[0];
	u8  pos = 0,pos1 = 0;
	u8* orgaddr;
	
	while( len > pos)
	{
		orgaddr = &orgdata[pos + pos1 + 1] + ((UnitPara_t *)&orgdata[pos + pos1 + 1])->len + offsetof(UnitPara_t,data);
		pos += ((UnitPara_t *)&orgdata[pos + pos1 + 1])->len + offsetof(UnitPara_t,data);
		memmove(orgaddr + sizeof(DlyTimeData_t), orgaddr, len - pos);
		memcpy(orgaddr, timedata + pos1, sizeof(DlyTimeData_t));	
		pos1 += sizeof(DlyTimeData_t);
	}
	orgdata[0] = pos + pos1;
}
/*******************************************************************************
函 数 名：  SceneEEPROM_GetEx
功能说明：  获取场景号标识是否有效
参   数：   unit:       单元位置
            area：     区域位置(0~4)
            num:        组号
            data[0]:    场景内容长度 data[0] = 0, 没有此场景
            data[1~n]:  场景内容
返 回 值：  标识结果 TRUE(有效)/FALSE(无效)
*******************************************************************************/
u8 GroupSceneEEPROM_GetEx(u8 i,  u8 *data)
{
    u8 	data_tmp[SCENE_TIME_DATA_SPACE3];
	u8 label = 0;
	u8 sceneTimeLabel;
//	if(i > 0)
	{
		label = pGroupSceneManage->groupScene[i].addr;
	}
	if(0 != label)
	{
		ReadDataFromEEPROM(GroupSceneEEPROMSave_Addr(label),GetCurGroupSceneSpace(label), data);  //保存数据   
		if((SCENE_DEFAULT_VALUE != (sceneTimeLabel = pGroupSceneManage->groupScene[i].sceneTimeSavePos))&&\
			(sceneTimeLabel <= SCENE_TIME_TOTAL_NUM))
		{
			ReadDataFromEEPROM(SceneTimeEEPROMSave_Addr(sceneTimeLabel), GetCurGroupSceneTimeSpace(sceneTimeLabel), data_tmp); //获取时间数据
			GroupSceneParamCombine(data_tmp, data);
			return TRUE;
		}
	}
    else
    {
        *data = 0;  //返回场景长度为零
    }
    return FALSE;
}

#endif


/**************************Copyright BestFu 2014-05-14*************************/
