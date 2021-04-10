/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_clk.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/

#include "CH56x_common.h"


/*******************************************************************************
* Function Name  : SystemInit
* Description    : 系统时钟初始化
* Input          : systemclck:系统时钟 Hz
* Return         : None
*******************************************************************************/
void SystemInit(uint32_t systemclck)
{	
	uint8_t sc;
	
	sc = systemclck/1000000;

    switch( sc )
    {
        case CLK_SOURCE_PLL_15MHz:
			R8_SAFE_ACCESS_SIG = 0x57; // enable safe access mode
			R8_SAFE_ACCESS_SIG = 0xa8;
			R8_CLK_PLL_DIV = 0x40 | 0x02;			
			R8_CLK_CFG_CTRL = 0x80 ;
			
            break;
        case CLK_SOURCE_PLL_30MHz:		
			R8_SAFE_ACCESS_SIG = 0x57; // enable safe access mode
			R8_SAFE_ACCESS_SIG = 0xa8;
			R8_CLK_PLL_DIV = 0x40;		
			R8_CLK_CFG_CTRL = 0x80 | RB_CLK_SEL_PLL;			
			
            break;
        case CLK_SOURCE_PLL_60MHz:
			R8_SAFE_ACCESS_SIG = 0x57; // enable safe access mode
			R8_SAFE_ACCESS_SIG = 0xa8;
			R8_CLK_PLL_DIV = 0x40 | 0x08;			
			R8_CLK_CFG_CTRL = 0x80 | RB_CLK_SEL_PLL;	
			
            break;
        case CLK_SOURCE_PLL_80MHz:
			R8_SAFE_ACCESS_SIG = 0x57; // enable safe access mode
			R8_SAFE_ACCESS_SIG = 0xa8;
			R8_CLK_PLL_DIV = 0x40 | 0x06;			
			R8_CLK_CFG_CTRL = 0x80 | RB_CLK_SEL_PLL;	
			
            break;
        case CLK_SOURCE_PLL_96MHz:
			R8_SAFE_ACCESS_SIG = 0x57; // enable safe access mode
			R8_SAFE_ACCESS_SIG = 0xa8;
			R8_CLK_PLL_DIV = 0x40 | 0x05;			
			R8_CLK_CFG_CTRL = 0x80 | RB_CLK_SEL_PLL;
			
            break;
        case CLK_SOURCE_PLL_120MHz:
			R8_SAFE_ACCESS_SIG = 0x57; // enable safe access mode
			R8_SAFE_ACCESS_SIG = 0xa8;
			R8_CLK_PLL_DIV = 0x40 | 0x04;			
			R8_CLK_CFG_CTRL = 0x80 | RB_CLK_SEL_PLL;
			
            break;
        default :
            break;		
    }	
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : GetSysClock
* Description    : 获取当前系统时钟
* Input          : None
* Return         : Hz
*******************************************************************************/
UINT32 GetSysClock( void )
{
	UINT8  rev;

	rev = R8_CLK_PLL_DIV & 0x0F;

	if(R8_CLK_CFG_CTRL & RB_CLK_SEL_PLL){             //系统时钟源来自 USB PHY 提供的480M
		if(rev == 0){
			return (30000000);
		}
		else{
			return (480000000/R8_CLK_PLL_DIV);
		}
	}
	else{                                             //系统时钟源来自外部晶振30M
		if(rev == 0){
			return (2000000);
		}
		else{
			return (30000000/R8_CLK_PLL_DIV);
		}
	}
}



