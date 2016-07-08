/***************************Copyright BestFu 2016-06-24*************************
��	����    Group.c
˵	��:	    iSpace���鴦��
��	�룺    Keil uVision4 V4.54.0.0
��	����    v1.0
��	д��    xjx
��	�ڣ�    2016-06-24
��  ��:     ����
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
�� �� ��:  	Set_New_Group_Attr
����˵��:  	���÷���(�ǳ���)
��    ��:  	data[0]: Ŀ�굥Ԫ
			data[1]: 
�� �� ֵ:  	��Ϣִ�н��
*******************************************************************************/
MsgResult_t Set_New_Group_Attr(UnitPara_t *pData)
{
	u8 num = 0;
	u8 i = 0;
	u8 j = 0;
	u32 groupNum = 0;
	u8 unit = 0;
	u8 flag = 0;//
	u8 eepromWriteStartPos = pGroupManage->cnt;	//��¼���� �����ӡ� ������ʼд��EEPROM��λ��
	u8 eepromWriteCnt = 0;						//��¼���� �����ӡ� ����д��EEPROM�ķ���ŵ�����
	Group_t buf[GROUP_ONECE_SET_MAX] = {0};		//��д��EEPROM������(�����ӡ�����ʹ��)
	
//	if(pGroupManage->cnt > 0)
//	{
//		eepromWriteStartPos = pGroupManage->cnt;
//	}
	
	if((pData->data[0] > UnitCnt_Get()) 												//��Ԫ�Ų���ȷ
		|| (pData->len < 6) || (pData->len > (2+ (GROUP_ONECE_SET_MAX << 2) ) )			//�������� 
		|| ((pData->data[1] != 1) && (pData->data[1] != 2) && (pData->data[1] != 3))	//����ִ��ѡ��ֵֻ��1��2��3
		|| ((pData->len - 2) % 4))//�����ж�											//�������Ȳ���ȷ
	{
		return PARA_MEANING_ERR;
	}
	
	num = (pData->len - 2) >> 2;
	unit = pData->data[0];
	
	if(1 == pData->data[1])//����
	{
		if(pGroupManage->cnt + num > GROUP_NUM_MAX)//ʣ��ռ��ж�
		{
			return DATA_SAVE_OVER;
		}
		for(i = 0; i < num; i++)
		{
			groupNum = *(u32*)&pData->data[2 + (i<<2)];
			if(0 == GroupNumExist(groupNum,unit))//�÷���Ų����ڣ���д��
			{
				flag = 1;
				pGroupManage->group[pGroupManage->cnt].groupNum = groupNum;
				pGroupManage->group[pGroupManage->cnt].unit = unit;			//д���ڴ�
				pGroupManage->cnt++;
				
				buf[eepromWriteCnt].groupNum = groupNum;
				buf[eepromWriteCnt].unit = unit;
				eepromWriteCnt++;
			}
		}
		if(1 == flag)//˵������Ҫ���ӵķ����
		{
			WriteDataToEEPROMEx(NEW_GROUP_START_ADDR ,1,pGroupManage->cnt);
			WriteDataToEEPROM(NEW_GROUP_START_ADDR + 1 + (eepromWriteStartPos * ONE_GROUP_SPACE),\
										eepromWriteCnt * ONE_GROUP_SPACE, (u8*)buf);
		}	
	}
	else if(2 == pData->data[1])		//�޸�
	{
		if(pData->len != 10)		//�޸Ĳ��������ݳ��ȹ̶�Ϊ10
		{
			return PARA_LEN_ERR;
		}
		
		groupNum = *(u32*)&pData->data[2];//�ɵķ����
		i = GroupNumExist(groupNum,unit);
		if(0 == i)					//�Ҳ����ɵķ����
		{
			return PARA_MEANING_ERR;
		}
		else
		{
//			i -= 1;			
			pGroupManage->group[i - 1].groupNum = *(u32*)&pData->data[6];//��ֵ�µķ����
			pGroupManage->group[i - 1].unit = unit;
			
			WriteDataToEEPROM(NEW_GROUP_START_ADDR + 1 + (i - 1) * ONE_GROUP_SPACE, ONE_GROUP_SPACE,(u8*)&pGroupManage->group[i - 1]);
		}
	}
	else if(3 == pData->data[1])		//ɾ��
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
�� �� ��:  	Get_New_Group_Attr
����˵��:  	��ȡ����(�ǳ���)��Ϣ
��    ��:  	
�� �� ֵ:  	��Ϣִ�н��
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
	
	if((pData->data[0] > UnitCnt_Get()) 					//��Ԫ�Ų���ȷ
		|| ((pData->data[1] != 1) && (pData->data[1] != 2))//����ִ��ѡ��ֻ��1��2����ֵ
		|| (pData->len > 4)								//�������Ȳ���ȷ
		)			
	
	{
		return PARA_MEANING_ERR;
	}
	unit = pData->data[0];
	
	if(1 == pData->data[1])//��ѯ��������(����������)
	{
		rpara[0] = 1;
		rpara[1] = GetUnitGroupNum(unit);
		*rlen = 2;		
	}
	else if(2 == pData->data[1])//��ѯ����ֵ
	{
		if(pData->data[3] - pData->data[2] > 40)//��ѯ��Χ����
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
�� �� ��:  	Set_Group_Scene_Attr
����˵��:  	���ó���
��    ��:  	unit:	��Ҫ���ҵĵ�Ԫ��
			start:	��Կ�ʼλ��
			tail:	��Խ���λ��
			*data:	���ҵ��ķ���Ŵ��
�� �� ֵ:  	��ѯ��Χ���м������ϵķ����
*******************************************************************************/
MsgResult_t Set_Group_Scene_Attr(UnitPara_t *pData)
{

	u32 groupSceneNum;

#if (FLASH_ENCRYPTION_EN > 0u)
	u32 temp =0x00;	
	temp = DecryptionFlash();		//���Ӵ������ jay add 2015.12.07
	pData->len >>= temp;
#endif
	
	if (pData->len < 6) 			//�������ȴ���
	{
		return PARA_LEN_ERR;
	}
#if (FLASH_ENCRYPTION_EN > 0u)	
	else if (((u8)(pData->data[0] + 1 + (u8)temp) > UnitCnt_Get())	//��Ԫ�Ų���ȷ	��ע�⣺���ָ��Ϊ��0xFF, ���Լ�1�������㣬����������
			|| ((pData->data[1] != 1) && (pData->data[1] != 2))	//����ִ�в���ֵֻ����1��2	
			)
#else
	else if (((u8)(pData->data[0] + 1) > UnitCnt_Get())	//��Ԫ�Ų���ȷ	��ע�⣺���ָ��Ϊ��0xFF, ���Լ�1�������㣬����������
			|| ((pData->data[1] != 1) && (pData->data[1] != 2))	//����ִ�в���ֵֻ����1��2	
			)	
#endif
	{
		return PARA_MEANING_ERR;
	}
	
	groupSceneNum = *(u32*)&pData->data[2];
	if (pData->data[1] == 1)	//����һ������
	{
		*(u8*)&pData->data[6] = pData->len - 7;		// ��ȡ������������
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
		if (pData->data[8] < SCENE_DATA_SPACE3)	//�ռ䳤�ȺϷ�
		{
			return ((MsgResult_t)GroupSceneEEPROM_Set(groupSceneNum, pData->data[0] ,  (u8*)&pData->data[6]));
		}
#endif			
		else
		{
			return PARA_LEN_ERR;
		}			

	
	}
	else if (pData->data[1] == 2) 			//ɾ������
	{		
		if (CLEAR == pData->data[0])		//���е�Ԫ
		{
			GroupSceneEEPROM_Init();
		}
		else 
		{
			if(0 == groupSceneNum)//ɾ��ĳһ����Ԫ
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
�� �� ��:  	Get_Group_Scene_Attr
����˵��:  	��ȡ����(����)��Ϣ
��    ��:  	
�� �� ֵ:  	��Ϣִ�н��
*******************************************************************************/
MsgResult_t Get_Group_Scene_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	u8 unit = 0;
	u8 i = 0;
	
	u8 testData[50];
	
	unit = pData->data[0];
	
	

	if((unit > 24)								//��Ԫ�Ŵ���
		|| ((pData->data[1] != 1) && (pData->data[1] != 2))	//������ȡֵ����ֻ��Ϊ1��2
	)
	{
		return PARA_MEANING_ERR;
	}
	
	if(1 == pData->data[1])//��ѯ����(����)����
	{
		*rlen = 2;
		rpara[1] = GetUnitGroupSceneNum(unit);
	}
	else if(2 == pData->data[1])		//��ѯ����(����)ֵ�Ͳ���
	{
		i = FindTheGroupScene(unit,pData->data[2]);
		if(i > 0)			//�ҵ�
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

