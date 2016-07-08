/***************************Copyright BestFu 2014-05-14*************************
��	����	Group.h
˵	����	iSpace�������ͷ�ļ�
��	�룺	Keil uVision4 V4.54.0.0
��	����	v1.0
��	д��	xjx
��	�ڣ�	2016-06-24
*******************************************************************************/
#ifndef __GROUP_H
#define __GROUP_H


#include "UnitCfg.h"

#define GROUP_NUM_MAX					150								//����������(��������������)


#define GROUP_ONECE_SET_MAX				40								//һ�����÷�����������

#define NEW_GROUP_START_ADDR				(0x200)							//����(�ǳ���)�׵�ַ
#define ONE_GROUP_SPACE					(sizeof(Group_t))
	
#define NEW_GROUP_SPACE					(150*5)	
#define GROUP_SCENE_NUM_MAX				255								//��󳡾���������
#define GROUP_SCENE_START_ADDR			(0x500)							//���鳡����Ϣ�洢�׵�ַ
#define ONE_GROUP_SCENE_INDEX_SPACE		(sizeof(GroupSceneIndex_t))		//һ�����鳡��������ռ�ռ�
	
#define GROUP_SCENE_USE_SPACE				(0x20)											//�����ڴ��ʾռ�ÿռ��С

#define GROUP_SCENE_INDEX_START_ADD		(GROUP_SCENE_START_ADD + 1)								//�豸�����������ݴ洢��ַ	

#define GROUP_SCENE_START_ADD				(0x500)//(GROUP_START_ADD + NEW_GROUP_SPACE)					//������ʼ��ַ(��ʼ��ַ0x600)

#define GROUP_SCENE_INDEX_SPACE			(0x700)											//�豸���������ռ�洢��С(֧��3����չ)

#define GROUP_SCENE_USE_FLAG_ADD			(GROUP_SCENE_START_ADD + GROUP_SCENE_INDEX_SPACE)			//�����ڴ�ʹ�ñ�ǵ�ַ(��ʼ��ַ0xC00)

#define GROUP_SCENE_TIME_USE_FLAG_ADD		(GROUP_SCENE_USE_FLAG_ADD + GROUP_SCENE_USE_SPACE)			//����ʱ���ڴ�ʹ�ñ�ǵ�ַ	

#define GROUP_SCENE_USE_FLAG_TOTAL_SPACE	(0x80)											//�����ڴ�ʹ�ñ����ռ�ÿռ��С

#define GROUP_SCENE_TIME_USE_SPACE		(GROUP_SCENE_TIME_TOTAL_NUM/8)						//����ʱ���ڴ��ʾռ�ÿռ��С
#define GROUP_SCENE_TIME_TOTAL_NUM		(GROUP_SCENE_TIME_SPACE1_NUM +\
									 GROUP_SCENE_TIME_SPACE2_NUM +\
									 GROUP_SCENE_TIME_SPACE3_NUM)							//����ʱ�䳡�����������

#define GROUP_SCENE_TIME_DATA_SPACE1      (10)											//����ʱ��ռ�1�Ĵ�С
#define GROUP_SCENE_TIME_DATA_SPACE2      (35)											//����ʱ��ռ�2�Ĵ�С
#define GROUP_SCENE_TIME_DATA_SPACE3      (50)											//����ʱ��ռ�3�Ĵ�С

#define GROUP_SCENE_TIME_SPACE1_NUM		(32)											//������ʱ�ռ�1������
#define GROUP_SCENE_TIME_SPACE2_NUM		(16)											//������ʱ�ռ�2������
#define GROUP_SCENE_TIME_SPACE3_NUM		(8)												//������ʱ�ռ�3������

/*�������ݴ洢��ַ*/
#define GROUP_SCENE_SPACE1_NUM			(128)											//�����ռ�1������
#define GROUP_SCENE_SPACE2_NUM			(64)											//�����ռ�2������
#define GROUP_SCENE_SPACE3_NUM			(64)											//�����ռ�3������

#define GROUP_SCENE_MAX_NUM				(GROUP_SCENE_SPACE1_NUM +\
									 GROUP_SCENE_SPACE2_NUM +\
									 GROUP_SCENE_SPACE3_NUM)								//��󳡾���
#define GROUP_SCENE_DATA_START_ADD		(GROUP_SCENE_USE_FLAG_ADD+GROUP_SCENE_USE_FLAG_TOTAL_SPACE)	//�������ݴ洢λ�õ���ʼ��ַ(��ʼ��ַ0xC80)
#define GROUP_SCENE_DATA_SPACE			(0x1000)										//�������ݴ洢�ռ�,4K
#define GROUP_SCENE_DATA_SPACE1			(8)												//�������ݿռ�1��С
#define GROUP_SCENE_DATA_SPACE2			(16)											//�������ݿռ�2��С
#define GROUP_SCENE_DATA_SPACE3			(32)											//�������ݿռ�3��С


#pragma pack(1)     				//��1�ֽڶ���
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
	u8 sceneTimeSavePos;		//�����洢λ��,1~56
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

