/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_hspi.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/

#include "CH56x_common.h"


/*******************************************************************************
* Function Name  : HSPI_Mode
* Description    : DVP模式
* Input          : s:  数据位宽
					RB_HPIF_DAT8_MOD  - 8位模式
					RB_HPIF_DAT16_MOD - 16位模式
					RB_HPIF_DAT32_MOD - 32位模式
				   i:  工作模式
					UP_Mode   - 使能上端模式
					DOWN_Mode - 使能下端模式
* Return         : None
*******************************************************************************/
void HSPI_Mode( UINT8 s,  HSPI_ModeTypeDef i)
{
	R8_HSPI_CFG &= ~RB_HSPI_MSK_SIZE;   //恢复默认模式8bit模式

    if(s){
    	R8_HSPI_CFG |= s;
    }
    else{
    	R8_HSPI_CFG &= ~RB_HSPI_MSK_SIZE;
    }

    if(i){
    	R8_HSPI_CFG |= RB_HSPI_MODE;
    }
    else{
    	R8_HSPI_CFG &= ~RB_HSPI_MODE;
    }
}

/*******************************************************************************
* Function Name  : HSPI_INTCfg
* Description    : HSPI中断配置
* Input          : s:  中断控制状态
					ENABLE  - 使能相应中断
					DISABLE - 关闭相应中断
				   i:  中断类型
					RB_HSPI_IE_T_DONE  - 突发序列发送完成中断
					RB_HSPI_IE_R_DONE  - 接收FIFO溢出中断
					RB_HSPI_IE_FIFO_OV - 单包接收完成中断
					RB_HSPI_IE_B_DONE  - 担保发送完成中断
* Return         : None
*******************************************************************************/
void HSPI_INTCfg( UINT8 s,  UINT8 i )
{
    if(s){
    	R8_HSPI_INT_EN |= i;
    }
    else{
    	R8_HSPI_INT_EN &= ~i;
    }
}

