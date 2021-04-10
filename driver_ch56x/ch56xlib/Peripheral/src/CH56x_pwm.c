/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_pwm.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/

#include "CH56x_common.h"


/*******************************************************************************
* Function Name  : PWMX_CycleCfg
* Description    : PWM0-PWM3基准时钟配置
* Input          : cyc:
					refer to PWMX_CycleTypeDef
* Return         : None
*******************************************************************************/
void PWMX_CycleCfg( PWMX_CycleTypeDef cyc )
{
    switch( cyc )
    {
        case PWMX_Cycle_256:
        	R8_PWM_CTRL_CFG &= ~RB_PWM_CYCLE_SEL;    //PWM 配置控制寄存器,时钟周期选择
            break;

        case PWMX_Cycle_255:
        	R8_PWM_CTRL_CFG |= RB_PWM_CYCLE_SEL;
            break;

        default :
            break;
    }
}

/*******************************************************************************
* Function Name  : PWMX_ACTOUT
* Description    : PWM0-PWM3通道输出波形配置
* Input          : ch:	select channel of pwm
					refer to channel of PWM define
				   da:	effective pulse width
				   pr:  select wave polar
					refer to PWMX_PolarTypeDef
				   s :  control pwmx function
					ENABLE  - 输出PWM
					DISABLE - 关闭PWM
* Return         : None
*******************************************************************************/
void PWMX_ACTOUT( UINT8 ch, UINT8 da, PWMX_PolarTypeDef pr, UINT8 s)
{
    UINT8 i;

    if(s == DISABLE)	R8_PWM_CTRL_MOD &= ~(ch);                        //判断PWM是否进行输出使能
    else
    {

    	(pr)?(R8_PWM_CTRL_MOD|=(ch<<4)):(R8_PWM_CTRL_MOD&=~(ch<<4));     //PWM输出极性控制       1：默认高电平，低有效；0：默认低电平，高有效。
        for(i=0; i<4; i++)
        {
            if((ch>>i)&1)		*((PUINT8V)((&R8_PWM0_DATA)+i)) = da;
        }
        R8_PWM_CTRL_MOD |= (ch);
    }
}





