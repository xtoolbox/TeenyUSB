/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_dvp.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/

#include "CH56x_common.h"


/*******************************************************************************
* Function Name  : DVP_INTCfg
* Description    : DVP中断配置
* Input          : s:  中断控制状态
					ENABLE  - 使能相应中断
					DISABLE - 关闭相应中断
				   i:  中断类型
					RB_DVP_IE_STP_FRM  - 帧结束中断
					RB_DVP_IE_FIFO_OV  - 接收FIFO溢出中断
					RB_DVP_IE_FRM_DONE - 帧结束中断
					RB_DVP_IE_ROW_DONE - 行结束中断
					RB_DVP_IE_STR_FRM  - 帧开始中断
* Return         : None
*******************************************************************************/
void DVP_INTCfg( UINT8 s,  UINT8 i )
{
    if(s){
    	R8_DVP_INT_EN |= i;
    }
    else{
    	R8_DVP_INT_EN &= ~i;
    }
}

/*******************************************************************************
* Function Name  : DVP_Mode
* Description    : DVP模式
* Input          : s:  数据位宽
					RB_DVP_D8_MOD  - 8位模式
					RB_DVP_D10_MOD - 10位模式
					RB_DVP_D12_MOD - 12位模式
				   i:  压缩数据模式
					Video_Mode - 使能视频模式
					JPEG_Mode  - 使能JPEG模式
* Return         : None
*******************************************************************************/
void DVP_Mode( UINT8 s,  DVP_Data_ModeTypeDef i)
{
	R8_DVP_CR0 &= ~RB_DVP_MSK_DAT_MOD;   //恢复默认模式8bit模式

    if(s){
    	R8_DVP_CR0 |= s;
    }
    else{
    	R8_DVP_CR0 &= ~(3<<4);
    }

    if(i){
    	R8_DVP_CR0 |= RB_DVP_JPEG;
    }
    else{
    	R8_DVP_CR0 &= ~RB_DVP_JPEG;
    }
}

/*******************************************************************************
* Function Name  : DVP_Cfg
* Description    : DVP配置
* Input          : s:  DMA使能控制
					DVP_DMA_Enable  - DMA使能
					DVP_DMA_Disable - DMA失能
				   i:  标志与FIFO清除控制
					DVP_FLAG_FIFO_RESET_Enable  - 复位标志与FIFO
                    DVP_FLAG_FIFO_RESET_Disable - 取消复位操作
				   j:  接收逻辑复位控制
					DVP_RX_RESET_Enable - 复位接收逻辑电路
					DVP_RX_RESET_Disable - 取消复位操作
* Return         : None
*******************************************************************************/
void DVP_Cfg( DVP_DMATypeDef s,  DVP_FLAG_FIFO_RESETTypeDef i, DVP_RX_RESETTypeDef j)
{
    switch( s )
    {
        case DVP_DMA_Enable:
        	R8_DVP_CR1 |= RB_DVP_DMA_EN;
            break;
        case DVP_DMA_Disable:
        	R8_DVP_CR1 &= ~RB_DVP_DMA_EN;
            break;
        default:
            break;
    }

    switch( i )
    {
        case DVP_RX_RESET_Enable:
        	R8_DVP_CR1 |= RB_DVP_ALL_CLR;
            break;
        case DVP_RX_RESET_Disable:
        	R8_DVP_CR1 &= ~RB_DVP_ALL_CLR;
            break;
        default:
            break;
    }

    switch( j )
    {
        case DVP_RX_RESET_Enable:
        	R8_DVP_CR1 |= RB_DVP_RCV_CLR;
            break;
        case DVP_RX_RESET_Disable:
        	R8_DVP_CR1 &= ~RB_DVP_RCV_CLR;
            break;
        default:
            break;
    }

}
