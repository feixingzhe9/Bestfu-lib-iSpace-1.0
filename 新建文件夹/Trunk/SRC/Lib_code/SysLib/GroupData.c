/***************************Copyright BestFu 2016-06-24*************************
��	����    GroupData.c
˵	��:	    iSpace�������ݴ���
��	�룺    Keil uVision4 V4.54.0.0
��	����    v1.0
��	д��    xjx
��	�ڣ�    2016-06-24
��  ��:     ����
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
�� �� ��:  	GroupSceneEEPROM_Init
����˵��:  	������з��鳡��
��    ��:  	
�� �� ֵ:  	
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
�� �� ��:  	GroupSceneEEPROM_Init
����˵��:  	������з��鳡��
��    ��:  	
�� �� ֵ:  	
*******************************************************************************/
void NewGroupEEPROM_Init(void)
{	
	
	WriteDataToEEPROMEx(NEW_GROUP_START_ADDR,sizeof(GroupManage_t),0);
}


/*******************************************************************************
�� �� ��:  	NewGroupBufInit
����˵��:   ����(�ǳ���)��ʼ��
��    ��:  	��
�� �� ֵ:  	��
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
�� �� ��:  	GroupSceneBufInit
����˵��:   ����(����)��ʼ��
��    ��:  	��
�� �� ֵ:  	��
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
�� �� ��:  	GroupBufInit
����˵��:   �����ʼ��
��    ��:  	��
�� �� ֵ:  	��
*******************************************************************************/
void GroupBufInit(void)
{
	NewGroupBufInit();	
	GroupSceneBufInit();
}


/*******************************************************************************
�� �� ��:  	Get_New_Group_Attr
����˵��:  	���ó���
��    ��:  	unit:	��Ҫ���ҵĵ�Ԫ��
			start:	��Կ�ʼλ��
			tail:	��Խ���λ��
			*data:	���ҵ��ķ���Ŵ��
�� �� ֵ:  	��ѯ��Χ���м������ϵķ����
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
�� �� ��:  	Get_New_Group_Attr
����˵��:  	���ó���
��    ��:  	unit:	��Ҫ���ҵĵ�Ԫ��
			
�� �� ֵ:  	��ѯ��Ԫ�м��������
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
�� �� ��:  	GetUnitGroupSceneNum
����˵��:  	��ѯ��Ԫ����(����)����
��    ��:  	unit:	��Ҫ���ҵĵ�Ԫ��
			
�� �� ֵ:  	��ѯ��Ԫ�м�������(����)��
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
�� �� ��:  	FindTheGroupScene
����˵��:  	��ѯ��Ԫ�ڵڼ�������(����)��
��    ��:  	unit:	��Ҫ���ҵĵ�Ԫ��
			
�� �� ֵ:  	��ѯ��Ԫ�м�������(����)��
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
�� �� ��:  	DelOneGroupSceneFromRam
����˵��:   ���ڴ���ɾ���ض����鳡��
��    ��:  	i:���鳡�����ڴ��е�˳��
�� �� ֵ:  	
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
�� �� ��:  	ClrUnitGroupScene
����˵��:  	������е�Ԫ���鳡��
��    ��:  	
�� �� ֵ:  	
*******************************************************************************/
u8 ClrUnitGroupScene(u8 unit)
{
	u8 i = 0;
	const u8 writeToEepromNumOnce = 20;////д��EEPROM��һ��д20*7=140���ֽ�
	
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
�� �� ��:  	DelOneGroupSceneFromRam
����˵��:   ���ڴ���ɾ���ض����鳡��
��    ��:  	i:���鳡�����ڴ��е�˳��
�� �� ֵ:  	
*******************************************************************************/
static u8 DelOneNewGroupFromRam(u8 i)
{

	if(0 == i)
	{
		return 0;
	}

	if(i == pGroupManage->cnt)//Ҫɾ���������һ��
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
�� �� ��:  	ClrUnitGroupScene
����˵��:  	������е�Ԫ���鳡��
��    ��:  	
�� �� ֵ:  	
*******************************************************************************/
u8 ClrUnitNewGroup(u8 unit)
{
	u8 i = 0;
	const u8 writeToEepromNumOnce = 20;////д��EEPROM��һ��д20*5=���ֽ�
	
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
�� �� ����  ClearSceneTimePara
����˵����  ��������е�ʱ�����
��   ����   i:�������ڴ��е�˳��
�� �� ֵ��  ��SceneEEPROM_Set����ʹ��
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
�� �� ��:  	DelOneGroupScene
����˵��:  	ɾ��һ������
��    ��:  	i:�����ڴ��˳��
�� �� ֵ:  	1:ɾ���ɹ�
			0:ʧ��
*******************************************************************************/
u8 DelOneGroupScene(u32 groupSceneNum,u8 unit)
{
	u8 i = GroupSceneNumExist(groupSceneNum,unit);
	u8 label = 0;
#if (SCENCETIME_EN > 0u)
	u8 timeLabel = 0;
#endif
	
	if(i > 0)//����
	{	
		label = pGroupSceneManage->groupScene[i-1].addr;
#if (SCENCETIME_EN > 0u)
		timeLabel = pGroupSceneManage->groupScene[i-1].sceneTimeSavePos;
#endif		
		if(i == pGroupSceneManage->cnt)//��Ҫɾ���ķ����������棬ֱ��ɾ��
		{			
			pGroupSceneManage->cnt--;
			
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].groupSceneNum = 0;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].unit = 0;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = 0;

#if (SCENCETIME_EN > 0u)	
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].sceneTimeSavePos = 0;
#endif
						
		}
		else		//��Ҫɾ���ķ���Ų�������棬�������ķ���Ÿ��ǵ�Ҫɾ����λ��
		{
			pGroupSceneManage->groupScene[i - 1].groupSceneNum = pGroupSceneManage->groupScene[pGroupSceneManage->cnt - 1].groupSceneNum;			
			pGroupSceneManage->groupScene[i - 1].unit = pGroupSceneManage->groupScene[pGroupSceneManage->cnt - 1].unit;//�����һ������Ű�Ҫɾ���ķ���Ÿ���
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
		
		
		WriteDataToEEPROMEx(GROUP_SCENE_START_ADDR, 1, pGroupSceneManage->cnt);//�ܵļ���
		
		pGroupSceneManage->sceneUseFlag[label/8] &= ~(1<<(label%8));
		
		WriteDataToEEPROMEx(GROUP_SCENE_USE_FLAG_ADD + (label / 8), \
								1,pGroupSceneManage->sceneUseFlag[label/8]);//�洢��ַӳ���
#if (SCENCETIME_EN > 0u)
		pGroupSceneManage->sceneTimeUseFlag[timeLabel/8] &= ~(1<<(timeLabel%8));
		WriteDataToEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD + (timeLabel / 8), \
								1,(u8*)&pGroupSceneManage->sceneUseFlag[timeLabel/8]);//ʱ��洢��ַӳ��
#endif
		
	}
	return 1;
}


/*******************************************************************************
�� �� ����  inline u32 Check_PositionRange(u8 position)
����˵����  ��鳡������ʼ�洢λ��
��   ����   position:����鳡������ʼ�洢λ�õı���
�� �� ֵ��  ���س�������ʼ�洢λ�õ�ֵ
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
�� �� ����  SceneEEPROMData_Save
����˵����  ����EEPROM�洢����
��   ����   addr:   �洢�յ�ַ
            data:   Ҫ�洢�ĳ�������
�� �� ֵ��  �洢��ţ�0��127��/0xFF(û�д洢�ռ�)
*******************************************************************************/
void GroupSceneEEPROMData_Save(u32 addr, u8 *data)
{
    WriteDataToEEPROM(addr, data[0] + 1, data); //��������
}

/*******************************************************************************
�� �� ����  SceneEEPROMSave_Addr
����˵����  ����EEPROM�洢��ַ
��   ����   seat:   �洢���
�� �� ֵ��  �洢��ַ
*******************************************************************************/
u32 GroupSceneEEPROMSave_Addr(u8 seat)
{
    u32 addr = GROUP_SCENE_DATA_START_ADD;

    if (seat < GROUP_SCENE_SPACE1_NUM)
    {
        addr += seat * GROUP_SCENE_DATA_SPACE1;
        goto RETURN;//λ�ô��ڴ洢�ռ�1
    }

    addr += GROUP_SCENE_SPACE1_NUM * GROUP_SCENE_DATA_SPACE1;
    seat -= GROUP_SCENE_SPACE1_NUM;
    if (seat < GROUP_SCENE_SPACE2_NUM)
    {
        addr += seat * GROUP_SCENE_DATA_SPACE2;
        goto RETURN;//λ�ô��ڴ洢�ռ�2
    }

    addr += GROUP_SCENE_SPACE2_NUM * GROUP_SCENE_DATA_SPACE2;
    seat -= GROUP_SCENE_SPACE2_NUM;
    if (seat < GROUP_SCENE_SPACE3_NUM)
    {
        addr += seat * GROUP_SCENE_DATA_SPACE3;
        goto RETURN;//λ�ô��ڴ洢�ռ�3
    }

RETURN:
    return addr;
}


/*******************************************************************************
�� �� ����  SceneEEPROMSave_Seat
����˵����  ����EEPROM�洢λ��
��   ����   seat:   �洢����
�� �� ֵ��  �洢λ��
*******************************************************************************/
u32 GroupSceneEEPROMSave_Seat(const u8 len)
{
    if (len < GROUP_SCENE_DATA_SPACE1)    //���ȴ��ڵ�һ�洢�ռ�
    {
        return  1;
    }
    else if (len < (GROUP_SCENE_DATA_SPACE2))     //���ȴ��ڵڶ��洢�ռ�
    {
        return GROUP_SCENE_SPACE1_NUM;
    }
    else if (len < (GROUP_SCENE_DATA_SPACE3))     //���ȴ��ڵ����洢�ռ�
    {
        return (GROUP_SCENE_SPACE1_NUM + GROUP_SCENE_SPACE2_NUM);
    }

    return GROUP_SCENE_MAX_NUM;   //����Խ��
}


/*******************************************************************************
�� �� ����  SceneEEPROMSaveData_Seat
����˵����  ����һ�������洢λ��
��   ����   len:    Ŀ��洢���ݳ���
�� �� ֵ��  number: �洢��λ��ֵ,MACRO_SCENE_ERR:��ʾû�����뵽
*******************************************************************************/
u16 GroupSceneEEPROMData_Seat(const u8 len)
{
    u16 i;
	u16 xReturn = SCENE_ADDR_ERR;

	for(i = GroupSceneEEPROMSave_Seat(len); i < GROUP_SCENE_MAX_NUM; i++)
	{
		if (!(pGroupSceneManage->sceneUseFlag[i / 8] & (1 << (i % 8))))   //����洢λ���ǿ��е�
		{
			xReturn = i;
			break;
		}
	}
	return (xReturn);
}
///*******************************************************************************
//�� �� ����  static void  SceneUseFlag_Set(const u8 label)
//����˵����  ���ó����ڴ�ʹ�ñ��
//��   ����   label: �������ݱ��(0x00~0xff)
//�� �� ֵ��  ��
//*******************************************************************************/
//static void  GroupSceneUseFlag_Set(const u8 label)
//{
//	pGroupSceneManage->sceneUseFlag[label/8] |= 1<<(label%8);
//	WriteDataToEEPROM(GROUP_SCENE_USE_FLAG_ADD + (label)/8, 1, (u8*)&pGroupSceneManage->sceneUseFlag[(label)/8]);
//}

/*******************************************************************************
�� �� ��:  	GroupNumExist
����˵��:  	�жϷ�����Ƿ��Ѿ�����
��    ��:  	groupNum:�����
�� �� ֵ:  	����0:���ڣ�
			0:������
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
�� �� ��:  	GroupSceneNumExist
����˵��:  	�жϷ�����Ƿ��Ѿ�����
��    ��:  	groupNum:�����
�� �� ֵ:  	����0:���ڣ�
			0:������
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
�� �� ��:  	FindGroupScene
����˵��:  	���ҷ��鳡��
��    ��:  	group:�����
			unit:��Ԫ��
			data:��д�����ݵ�����
�� �� ֵ:  	����0���洢��ַƫ����	
			0:δ�ҵ�
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
		flag = 1;//˵��û���ҵ��÷��鳡��
	}
	
	labelTmp = label;	
	
	if(0 != label)
	{
#if (SCENCETIME_EN > 0u)
		ClearGroupSceneTimePara(i);
#endif
		
		if ((!(pGroupSceneManage->sceneUseFlag[((u8)label)/8] & (1<<(((u8)label)%8))))||\
			(GroupSceneCheck_PositionRange(((u8)label)) != GroupSceneEEPROMSave_Seat(data[0])))     //��ͬ�����ŵ������,����Ҫ�Ĵ洢�ռ��ڴ�鲻ͬ,��Ҫ��������		
		{
			pGroupSceneManage->sceneUseFlag[((u8)label)/8] &= ~(1<<(((u8)label)%8));//���Ȳ�һ�£����֮ǰ�Ĵ洢��ʶ
			labelClrFlag = 1;
			label = 0;
		}	
	}
	
	 if(0 == label)   	//δ�ҵ��ó�����������û������ռ�
    {
		if(SCENE_ADDR_ERR == (label = (GroupSceneEEPROMData_Seat(data[0]))))//����һ���洢�ռ�
        {
			if(1 == labelClrFlag)
			{
				pGroupSceneManage->sceneUseFlag[((u8)labelTmp)/8] |= (1<<(((u8)labelTmp)%8));//�ָ�֮ǰ����Ĵ洢��ʶ
			}
			
            return EEPROM_RAND_ERR;
        }
		if(1 == flag)//�÷���(����)���µ�
		{			
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].groupSceneNum = groupNum;
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].unit = unit;			
			pGroupSceneManage->groupScene[pGroupSceneManage->cnt].addr = ((u8)label);	
			
			pGroupSceneManage->cnt++;
			WriteDataToEEPROMEx(GROUP_SCENE_START_ADD,1, pGroupSceneManage->cnt);	
			
			i = pGroupSceneManage->cnt;
		}
//		else	//˵������ʵ�ʴ洢λ�÷����˸ı�
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
�� �� ��:  	DelOneGroupNum
����˵��:  	ɾ��һ�������(�ǳ���)
��    ��:  	groupNum:�����
			unit	:��Ԫ��
�� �� ֵ:  	
			
*******************************************************************************/
u8 DelOneGroupNum(u32 groupNum, u8 unit)
{
	u8 i = 0;
	
	i = GroupNumExist(groupNum,unit);
	
	if(i > 0)//����
	{

		if(i == pGroupManage->cnt)//��Ҫɾ���ķ����������棬ֱ��ɾ��
		{
			pGroupManage->group[pGroupManage->cnt - 1].groupNum = 0;
			pGroupManage->group[pGroupManage->cnt - 1].unit = 0;
			
			pGroupManage->cnt--;					
		}
		else		//��Ҫɾ���ķ���Ų�������棬�������ķ���Ÿ��ǵ�Ҫɾ����λ��
		{
			pGroupManage->group[i - 1].groupNum = pGroupManage->group[pGroupManage->cnt - 1].groupNum;
			pGroupManage->group[i - 1].unit = pGroupManage->group[pGroupManage->cnt - 1].unit;//�����һ������Ű�Ҫɾ���ķ���Ÿ���
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
�� �� ����  GetCurGroupSceneSpace
����˵����  ��ȡ������Ŷ�Ӧ�Ĵ洢��С
��   ����   label:����ʱ����
�� �� ֵ��  �ڴ��Ĵ�С
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
�� �� ����  SceneEEPROM_Get
����˵����  ��ȡ�����ű�ʶ�Ƿ���Ч
��   ����   unit:       ��Ԫλ��
            area��      ����λ��(0~4)
            num:        ���
            data[0]:    �������ݳ��� data[0] = 0, û�д˳���
            data[1~n]:  ��������
�� �� ֵ��  ��ʶ��� TRUE(��Ч)/FALSE(��Ч)
*******************************************************************************/
u8 GroupSceneEEPROM_Get(u8 i,u8 *data)
{
    u8 addr = 0;

    if(0 != i)
    {
		addr = pGroupSceneManage->groupScene[i].addr;
        ReadDataFromEEPROM(GroupSceneEEPROMSave_Addr(addr),GetCurGroupSceneSpace(addr), data);  //��������                        
    }
    else
    {
        *data = 0;  //���س�������Ϊ��
		return FALSE;
    }
	return TRUE;
}

#if (SCENCETIME_EN > 0u)


/*******************************************************************************
�� �� ����  static void  GroupSceneTimeUseFlag_Clr(const u8 label)
����˵����  �������ʱ���ڴ�ʹ�ñ��
��   ����   i �����ڴ�˳��
�� �� ֵ��  ��
*******************************************************************************/
static void  GroupSceneTimeUseFlag_Clr(const u8 i)
{
	pGroupSceneManage->sceneTimeUseFlag[i/8] &= ~(1<<(i%8));
	WriteDataToEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD + (i - 1)/8, 1, (u8*)&pGroupSceneManage->sceneTimeUseFlag[(i - 1)/8]);
}

///*******************************************************************************
//�� �� ����  static void  GroupSceneTimeUseFlag_Set(const u8 label)
//����˵����  ���ó���ʱ���ڴ�ʹ�ñ��
//��   ����   label: �������ݱ��(0x01~56)
//�� �� ֵ��  ��
//*******************************************************************************/
//static void  GroupSceneTimeUseFlag_Set(const u8 label)
//{
//	pGroupSceneManage->sceneTimeUseFlag[label/8] |= 1<<(label%8);	
//	WriteDataToEEPROM(SCENE_TIME_USE_FLAG_ADD + (label - 1)/8, 1, (u8*)&pGroupSceneManage->sceneTimeUseFlag[(label - 1)/8]);
//}

/*******************************************************************************
�� �� ����  inline u32 CheckTime_PositionRange(u8 position)
����˵����  ��鳡������ʼ�洢λ��
��   ����   position:����鳡������ʼ�洢λ�õı���
�� �� ֵ��  ���س�������ʼ�洢λ�õ�ֵ
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
�� �� ����  SceneTimeSave_Seat
����˵����  ����EEPROMTime�洢λ��
��   ����   seat:   �洢����
�� �� ֵ��  �洢λ��
*******************************************************************************/
static u32 GroupSceneTimeSave_Seat(const u8 len)
{
    if (len <= SCENE_TIME_DATA_SPACE1)    //���ȴ��ڵ�һ�洢�ռ�
    {
        return  1;
    }
    else if (len <= (SCENE_TIME_DATA_SPACE2))     //���ȴ��ڵڶ��洢�ռ�
    {
        return SCENE_TIME_SPACE1_NUM;
    }
    else if (len <= (SCENE_TIME_DATA_SPACE3))     //���ȴ��ڵ����洢�ռ�
    {
        return (SCENE_TIME_SPACE1_NUM + SCENE_TIME_SPACE2_NUM);
    }

    return SCENE_TIME_TOTAL_NUM;   //����Խ��
}

/*******************************************************************************
�� �� ����  SceneEEPROMTimeData_Seat
����˵����  ����һ������ʱ��洢λ��
��   ����   len:    Ŀ��洢���ݳ���
�� �� ֵ��  number: �洢��λ��ֵ,MACRO_SCENE_ERR:��ʾû�����뵽
*******************************************************************************/
static u8 SceneEEPROMTimeData_Seat(const u8 len)
{
    u16 i;
	u16 xReturn = SCENE_DEFAULT_VALUE;

	for(i = GroupSceneTimeSave_Seat(len); i < SCENE_TIME_TOTAL_NUM; i++)
	{
		if (!(pGroupSceneManage->sceneTimeUseFlag[i / 8] & (1 << (i % 8))))   //����洢λ���ǿ��е�
		{
			xReturn = i;
			break;
		}
	}
	return (xReturn);
}

/*******************************************************************************
�� �� ����  SceneTimeEEPROMSave_Addr
����˵����  ����ʱ��EEPROM�洢��ַ
��   ����   seat:   �洢���
�� �� ֵ��  �洢��ַ
*******************************************************************************/
u32 SceneTimeEEPROMSave_Addr(u8 seat)
{
    u32 addr = SCENE_TIME_DATA_START_ADD;
	
	if(seat <= SCENE_TIME_SPACE1_NUM)
	{
		addr += (seat-1) * SCENE_TIME_DATA_SPACE1;
        goto RETURN;		//λ�ô��ڴ洢�ռ�1
	}

    addr += SCENE_TIME_SPACE1_NUM * SCENE_DATA_SPACE1;
    seat -= SCENE_TIME_SPACE1_NUM;
    if (seat <= SCENE_SPACE2_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE2;
        goto RETURN;//λ�ô��ڴ洢�ռ�2
    }

    addr += SCENE_TIME_SPACE2_NUM * SCENE_TIME_DATA_SPACE2;
    seat -= SCENE_TIME_SPACE2_NUM;
    if (seat <= SCENE_TIME_SPACE3_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE3;
        goto RETURN;//λ�ô��ڴ洢�ռ�3
    }

RETURN:
    return addr;
}
#endif


#if (SCENCETIME_EN > 0u)

/*******************************************************************************
�� �� ����  static void SceneParamSeparate(u8 *timedata, u8 *orgdata)
����˵����  �������ݷ��뺯��
��   ����   timedata��ʱ�����ݴ洢ָ��,����timedata[0]��Ŷ���䳤��
			orgdata��ʱ�����ݴ洢ָ��,orgdata[0]��ʾ���䳤��
�� �� ֵ��  ��
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
�� �� ����  SceneTimeEEPROMSave_Addr
����˵����  ����ʱ��EEPROM�洢��ַ
��   ����   seat:   �洢���
�� �� ֵ��  �洢��ַ
*******************************************************************************/
static u32 GroupSceneTimeEEPROMSave_Addr(u8 seat)
{
    u32 addr = SCENE_TIME_DATA_START_ADD;
	
	if(seat <= SCENE_TIME_SPACE1_NUM)
	{
		addr += (seat-1) * SCENE_TIME_DATA_SPACE1;
        goto RETURN;		//λ�ô��ڴ洢�ռ�1
	}

    addr += SCENE_TIME_SPACE1_NUM * SCENE_DATA_SPACE1;
    seat -= SCENE_TIME_SPACE1_NUM;
    if (seat <= SCENE_SPACE2_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE2;
        goto RETURN;//λ�ô��ڴ洢�ռ�2
    }

    addr += SCENE_TIME_SPACE2_NUM * SCENE_TIME_DATA_SPACE2;
    seat -= SCENE_TIME_SPACE2_NUM;
    if (seat <= SCENE_TIME_SPACE3_NUM)
    {
        addr += (seat - 1) * SCENE_TIME_DATA_SPACE3;
        goto RETURN;//λ�ô��ڴ洢�ռ�3
    }

RETURN:
    return addr;
}

/*******************************************************************************
�� �� ����  u32 SceneEEPROM_SetEx(u8 unit, u8 area, u8 num, u8 *data)
����˵����  д��һ����������չ�ӿ�,֧����ʱ˳��ִ��
��   ����   unit:       ��Ԫλ��
            area��      ����λ��(0~4)
            num:        �龰��
            cnt:        ������������
            data[0~n]:  ����ʱ����������
�� �� ֵ��  �洢���
*******************************************************************************/
u32 GroupSceneEEPROM_SetEx(u32 groupSceneNum, u8 unit, u8 *data)
{
	
	u8 	data_tmp[SCENE_TIME_DATA_SPACE3 + 1];
	u8 i = GroupSceneNumExist(groupSceneNum,unit);
	u8 label = 0;
	u8 timePos = SCENE_DEFAULT_VALUE;
	u8 flag = 0;//������ʶ�����Ƿ����
	u8 timeChangeFlag = 0;
	u8 changeFlag = 0;
	u8 labelTmp = 0;
	u8 timePosTmp = 0;
	u8 status = 0;
//	u8 cntFlag = 0;//�����仯��ʶ
	
	if(i != 0)
	{
		label = pGroupSceneManage->groupScene[i-1].addr;
		timePos = pGroupSceneManage->groupScene[i-1].sceneTimeSavePos;
		labelTmp = label;
		timePosTmp = timePos;
	}
	else//����������
	{
		pGroupSceneManage->cnt++;
		flag = 1;
		
		i = pGroupSceneManage->cnt;
	}
	
	GroupSceneParamSeparate((u8*)data_tmp, (u8*)data);		//��������
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
//				DelOneGroupScene(i);				//���֮ǰ�洢��ʶ,��ɾ��ʱ�����
				changeFlag = 1;
				pGroupSceneManage->sceneUseFlag[label/8] |= 1<<(label%8);
				label = 0;
			}
		}
			
		if(SCENE_DEFAULT_VALUE == timePos)
		{
			if(SCENE_DEFAULT_VALUE == (timePos = (SceneEEPROMTimeData_Seat(data_tmp[0]))))//����һ���洢�ռ�
			{
				if(1 == flag)
				{
					pGroupSceneManage->cnt--;//����ռ�ʧ�ܣ��ָ���������
				}
				if(1 == timeChangeFlag)
				{
					pGroupSceneManage->sceneTimeUseFlag[timePosTmp/8] |= 1<<(timePosTmp%8);//����ռ�ʧ�ܣ��ָ�ʱ��洢��ʶ
				}
				return EEPROM_RAND_ERR;
			}
			status |= (1 << 0);
		}
		
		if(0 == label)   	//δ�ҵ��ó�����������û������ռ�
		{
			if(0 == (label = (GroupSceneEEPROMData_Seat(data[0]))))//����һ���洢�ռ�
			{
				if(1 == flag)
				{
					pGroupSceneManage->cnt--;//����ռ�ʧ�ܣ��ָ���������
				}
				if(1 == changeFlag)
				{
					pGroupSceneManage->sceneUseFlag[labelTmp/8] |= 1<<(labelTmp%8);//����ռ�ʧ�ܣ��ָ������洢��ʶ
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
			WriteDataToEEPROM(GROUP_SCENE_USE_FLAG_ADD,GROUP_SCENE_USE_SPACE,(u8*)&pGroupSceneManage->sceneUseFlag);//д�����洢��ʶ
		}
		
		if((1 == timeChangeFlag) || (1 == flag))
		{
			WriteDataToEEPROM(GROUP_SCENE_TIME_USE_FLAG_ADD,GROUP_SCENE_TIME_USE_SPACE,\
							(u8*)&pGroupSceneManage->sceneTimeUseFlag);//дʱ�䳡���洢��ʶ
		}
		
		
		WriteDataToEEPROM(GroupSceneTimeEEPROMSave_Addr(timePos), data_tmp[0], &data_tmp[1]); //дʱ�䳡������
		GroupSceneEEPROMData_Save(GroupSceneEEPROMSave_Addr(label), data);	//д��������
		return COMPLETE;
	}
	return TIME_PARA_ERR;	
}


/*******************************************************************************
�� �� ����  GetCurSceneTimeSpace
����˵����  ��ȡ����ʱ���Ŷ�Ӧ�Ĵ洢��С
��   ����   label:����ʱ����
�� �� ֵ��  �ڴ��Ĵ�С
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
�� �� ����  static void SceneParamCombine(u8 *timedata, u8 *orgdata)
����˵����  �������ݺϲ�����
��   ����   timedata��ʱ�����ݴ洢ָ��
			orgdata��ʱ�����ݴ洢ָ��,orgdata[0]��ʾ����
�� �� ֵ��  ��
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
�� �� ����  SceneEEPROM_GetEx
����˵����  ��ȡ�����ű�ʶ�Ƿ���Ч
��   ����   unit:       ��Ԫλ��
            area��     ����λ��(0~4)
            num:        ���
            data[0]:    �������ݳ��� data[0] = 0, û�д˳���
            data[1~n]:  ��������
�� �� ֵ��  ��ʶ��� TRUE(��Ч)/FALSE(��Ч)
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
		ReadDataFromEEPROM(GroupSceneEEPROMSave_Addr(label),GetCurGroupSceneSpace(label), data);  //��������   
		if((SCENE_DEFAULT_VALUE != (sceneTimeLabel = pGroupSceneManage->groupScene[i].sceneTimeSavePos))&&\
			(sceneTimeLabel <= SCENE_TIME_TOTAL_NUM))
		{
			ReadDataFromEEPROM(SceneTimeEEPROMSave_Addr(sceneTimeLabel), GetCurGroupSceneTimeSpace(sceneTimeLabel), data_tmp); //��ȡʱ������
			GroupSceneParamCombine(data_tmp, data);
			return TRUE;
		}
	}
    else
    {
        *data = 0;  //���س�������Ϊ��
    }
    return FALSE;
}

#endif


/**************************Copyright BestFu 2014-05-14*************************/
