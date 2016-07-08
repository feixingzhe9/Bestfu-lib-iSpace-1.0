/***************************Copyright BestFu 2016-06-24*************************
文	件：    Group.c
说	明:	    iSpace分组处理
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    xjx
日	期：    2016-06-24
修  改:     暂无
*******************************************************************************/

#include "Group.h"
#include "SysHard.h"
#include "EEPROM.h"
#include "UserData.h"

#include "GroupData.h"

GroupManage_t groupManage;
GroupManage_t *pGroupManage = &groupManage;

GroupSceneManage_t groupSceneManage;
GroupSceneManage_t *pGroupSceneManage = &groupSceneManage;


/*******************************************************************************
函 数 名:  	Set_New_Group_Attr
功能说明:  	设置分组(非场景)
参    数:  	data[0]: 目标单元
			data[1]: 
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_New_Group_Attr(UnitPara_t *pData)
{
	u8 num = 0;
	u8 i = 0;
	u8 j = 0;
	u32 groupNum = 0;
	u8 unit = 0;
	u8 flag = 0;//
	u8 eepromWriteStartPos = pGroupManage->cnt;	//记录本次 “增加” 操作开始写入EEPROM的位置
	u8 eepromWriteCnt = 0;						//记录本次 “增加” 操作写入EEPROM的分组号的数量
	Group_t buf[GROUP_ONECE_SET_MAX] = {0};		//待写入EEPROM的数据(“增加”操作使用)
	
//	if(pGroupManage->cnt > 0)
//	{
//		eepromWriteStartPos = pGroupManage->cnt;
//	}
	
	if((pData->data[0] > UnitCnt_Get()) 												//单元号不正确
		|| (pData->len < 6) || (pData->len > (2+ (GROUP_ONECE_SET_MAX << 2) ) )			//参数过长 
		|| ((pData->data[1] != 1) && (pData->data[1] != 2) && (pData->data[1] != 3))	//参数执行选项值只有1，2，3
		|| ((pData->len - 2) % 4))//参数判断											//参数长度不正确
	{
		return PARA_MEANING_ERR;
	}
	
	num = (pData->len - 2) >> 2;
	unit = pData->data[0];
	
	if(1 == pData->data[1])//增加
	{
		if(pGroupManage->cnt + num > GROUP_NUM_MAX)//剩余空间判断
		{
			return DATA_SAVE_OVER;
		}
		for(i = 0; i < num; i++)
		{
			groupNum = *(u32*)&pData->data[2 + (i<<2)];
			if(0 == GroupNumExist(groupNum,unit))//该分组号不存在，则写入
			{
				flag = 1;
				pGroupManage->group[pGroupManage->cnt].groupNum = groupNum;
				pGroupManage->group[pGroupManage->cnt].unit = unit;			//写入内存
				pGroupManage->cnt++;
				
				buf[eepromWriteCnt].groupNum = groupNum;
				buf[eepromWriteCnt].unit = unit;
				eepromWriteCnt++;
			}
		}
		if(1 == flag)//说明有需要增加的分组号
		{
			WriteDataToEEPROMEx(NEW_GROUP_START_ADDR ,1,pGroupManage->cnt);
			WriteDataToEEPROM(NEW_GROUP_START_ADDR + 1 + (eepromWriteStartPos * ONE_GROUP_SPACE),\
										eepromWriteCnt * ONE_GROUP_SPACE, (u8*)buf);
		}	
	}
	else if(2 == pData->data[1])		//修改
	{
		if(pData->len != 10)		//修改操作的数据长度固定为10
		{
			return PARA_LEN_ERR;
		}
		
		groupNum = *(u32*)&pData->data[2];//旧的分组号
		i = GroupNumExist(groupNum,unit);
		if(0 == i)					//找不到旧的分组号
		{
			return PARA_MEANING_ERR;
		}
		else
		{
//			i -= 1;			
			pGroupManage->group[i - 1].groupNum = *(u32*)&pData->data[6];//赋值新的分组号
			pGroupManage->group[i - 1].unit = unit;
			
			WriteDataToEEPROM(NEW_GROUP_START_ADDR + 1 + (i - 1) * ONE_GROUP_SPACE, ONE_GROUP_SPACE,(u8*)&pGroupManage->group[i - 1]);
		}
	}
	else if(3 == pData->data[1])		//删除
	{
		if(0 == pGroupManage->cnt)
		{
			return COMPLETE;
		}
		for(j = 0; j < num; j++)
		{
			groupNum = *(u32*)&pData->data[2 + (j<<2)];
			DelOneGroupNum(groupNum,unit);
		}
		WriteDataToEEPROMEx(NEW_GROUP_START_ADDR, 1, pGroupManage->cnt);		
	}
	
	return COMPLETE;
}





/*******************************************************************************
函 数 名:  	Get_New_Group_Attr
功能说明:  	获取分组(非场景)信息
参    数:  	
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_New_Group_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	u8 unit = 0;
	u8 cnt = 0;
	u8 start = 0;		//
	u8 tail = 0;		//
	
//	u8 testData[50] = {0};
//	u8 i = 0;
	
	*rlen = 0;
	
	if((pData->data[0] > UnitCnt_Get()) 					//单元号不正确
		|| ((pData->data[1] != 1) && (pData->data[1] != 2))//参数执行选项只有1和2两个值
		|| (pData->len > 4)								//参数长度不正确
		)			
	
	{
		return PARA_MEANING_ERR;
	}
	unit = pData->data[0];
	
	if(1 == pData->data[1])//查询分组总数(不包括场景)
	{
		rpara[0] = 1;
		rpara[1] = GetUnitGroupNum(unit);
		*rlen = 2;		
	}
	else if(2 == pData->data[1])//查询分组值
	{
		if(pData->data[3] - pData->data[2] > 40)//查询范围过大
		{
			return PARA_MEANING_ERR;
		}
		start = pData->data[2];
		tail = pData->data[3];
		cnt = FindGroupInRange(unit,start,tail,(u32*)&rpara[2]);
		rpara[0] = 2;
		rpara[1] = cnt;
		*rlen = (cnt << 2) + 2;
	}
	
//	for(i = 0; i < *rlen; i++)
//	{
//		testData[i] = rpara[i];
//	}
	
	return COMPLETE;
}


/*******************************************************************************
函 数 名:  	Set_Group_Scene_Attr
功能说明:  	设置场景
参    数:  	unit:	需要查找的单元号
			start:	相对开始位置
			tail:	相对结束位置
			*data:	查找到的分组号存放
返 回 值:  	查询范围内有几个符合的分组号
*******************************************************************************/
MsgResult_t Set_Group_Scene_Attr(UnitPara_t *pData)
{

	u32 groupSceneNum;

#if (FLASH_ENCRYPTION_EN > 0u)
	u32 temp =0x00;	
	temp = DecryptionFlash();		//增加代码混淆 jay add 2015.12.07
	pData->len >>= temp;
#endif
	
	if (pData->len < 6) 			//参数长度错误
	{
		return PARA_LEN_ERR;
	}
#if (FLASH_ENCRYPTION_EN > 0u)	
	else if (((u8)(pData->data[0] + 1 + (u8)temp) > UnitCnt_Get())	//单元号不正确	（注意：清除指令为：0xFF, 所以加1后变成了零，符合条件）
			|| ((pData->data[1] != 1) && (pData->data[1] != 2))	//参数执行操作值只能是1和2	
			)
#else
	else if (((u8)(pData->data[0] + 1) > UnitCnt_Get())	//单元号不正确	（注意：清除指令为：0xFF, 所以加1后变成了零，符合条件）
			|| ((pData->data[1] != 1) && (pData->data[1] != 2))	//参数执行操作值只能是1和2	
			)	
#endif
	{
		return PARA_MEANING_ERR;
	}
	
	groupSceneNum = *(u32*)&pData->data[2];
	if (pData->data[1] == 1)	//增加一个场景
	{
		*(u8*)&pData->data[6] = pData->len - 7;		// 获取场景参数长度
#if (SCENCETIME_EN > 0u)			
		if((0x95 == pData->cmd)&&(pData->data[8] < SCENE_DATA_SPACE3))
		{
			return ((MsgResult_t)GroupSceneEEPROM_Set(groupSceneNum, pData->data[0] ,  (u8*)&pData->data[6]));
		}
		else if((0x96 == pData->cmd)&&(pData->data[8] < SCENE_DATA_SPACE3 + SCENE_TIME_DATA_SPACE3))
		{
			return ((MsgResult_t)GroupSceneEEPROM_SetEx(groupSceneNum, pData->data[0] ,  (u8*)&pData->data[6]));
		}
#else
		if (pData->data[8] < SCENE_DATA_SPACE3)	//空间长度合法
		{
			return ((MsgResult_t)GroupSceneEEPROM_Set(groupSceneNum, pData->data[0] ,  (u8*)&pData->data[6]));
		}
#endif			
		else
		{
			return PARA_LEN_ERR;
		}			

	
	}
	else if (pData->data[1] == 2) 			//删除场景
	{		
		if (CLEAR == pData->data[0])		//所有单元
		{
			GroupSceneEEPROM_Init();
		}
		else 
		{
			if(0 == groupSceneNum)//删除某一个单元
			{
				ClrUnitGroupScene(pData->data[0]);
			}
			else
			{
				DelOneGroupScene(groupSceneNum,pData->data[0]);
			}
					
		}
	}
	
	return COMPLETE;
}


/*******************************************************************************
函 数 名:  	Get_Group_Scene_Attr
功能说明:  	获取分组(场景)信息
参    数:  	
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_Group_Scene_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	u8 unit = 0;
	u8 i = 0;
	
	u8 testData[50];
	
	unit = pData->data[0];
	
	

	if((unit > 24)								//单元号错误
		|| ((pData->data[1] != 1) && (pData->data[1] != 2))	//操作符取值错误：只能为1或2
	)
	{
		return PARA_MEANING_ERR;
	}
	
	if(1 == pData->data[1])//查询分组(场景)总数
	{
		*rlen = 2;
		rpara[1] = GetUnitGroupSceneNum(unit);
	}
	else if(2 == pData->data[1])		//查询分组(场景)值和参数
	{
		i = FindTheGroupScene(unit,pData->data[2]);
		if(i > 0)			//找到
		{
			rpara[0] = 2;
			rpara[1] = 0;
			*(u32*)&rpara[2] = pGroupSceneManage->groupScene[i].groupSceneNum;
			
			
#if (SCENCETIME_EN > 0u)			
			GroupSceneEEPROM_GetEx(i,&rpara[6]);
#else
			GroupSceneEEPROM_Get(i,&rpara[6]);
#endif	
			*rlen = rpara[6] + 7;
		}
	}
	
	for(i = 0; i < *rlen; i++)
	{
		testData[i] = rpara[i];
	}
	return COMPLETE;
}

/**************************Copyright BestFu 2014-05-14*************************/

