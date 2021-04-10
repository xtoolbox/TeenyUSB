/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_timer.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/

#include "CH56x_common.h"


/*******************************************************************************
* Function Name  : TMR0_TimerInit
* Description    : Counting Function on TIM PeriPheral
* Input          : t: the Most End Value counting to
* Return         : None
*******************************************************************************/
void TMR0_TimerInit( UINT32 t )
{
    R32_TMR0_CNT_END = t;
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_CAP_COUNT;
}

/*******************************************************************************
* Function Name  : TMR1_TimerInit
* Description    : Counting Function on TIM PeriPheral
* Input          : t: the Most End Value counting to
* Return         : None
*******************************************************************************/
void TMR1_TimerInit( UINT32 t )
{
    R32_TMR1_CNT_END = t;
    R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR1_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_CAP_COUNT;
}

/*******************************************************************************
* Function Name  : TMR2_TimerInit
* Description    : Counting Function on TIM PeriPheral
* Input          : t: the Most End Value counting to
* Return         : None
*******************************************************************************/
void TMR2_TimerInit( UINT32 t )
{
    R32_TMR2_CNT_END = t;
    R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR2_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_CAP_COUNT;
}

/*******************************************************************************
* Function Name  : TMR0_EXTSignalCounterInit
* Description    : external signal count
* Input          : R32_TMR0_CNT_END
* Return         : None
*******************************************************************************/
void TMR0_EXTSignalCounterInit( UINT32 c )
{
    R32_TMR0_CNT_END = c;
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = RB_TMR_CAP_COUNT|RB_TMR_MODE_IN|RB_TMR_COUNT_EN;
}

/*******************************************************************************
* Function Name  : TMR1_EXTSignalCounterInit
* Description    : external signal count
* Input          : R32_TMR0_CNT_END
* Return         : None
*******************************************************************************/
void TMR1_EXTSignalCounterInit( UINT32 c )
{
    R32_TMR1_CNT_END = c;
    R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR1_CTRL_MOD = RB_TMR_CAP_COUNT|RB_TMR_MODE_IN|RB_TMR_COUNT_EN;
}

/*******************************************************************************
* Function Name  : TMR2_EXTSignalCounterInit
* Description    : external signal count
* Input          : R32_TMR0_CNT_END
* Return         : None
*******************************************************************************/
void TMR2_EXTSignalCounterInit( UINT32 c )
{
    R32_TMR2_CNT_END = c;
    R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR2_CTRL_MOD = RB_TMR_CAP_COUNT|RB_TMR_MODE_IN|RB_TMR_COUNT_EN;
}

/*******************************************************************************
* Function Name  : TMR0_PWMInit
* Description    : PWM Output Init
* Input          : PWM_PolarTypeDef  PWM_RepeatTsTypeDef
* Return         : None
*******************************************************************************/
void TMR0_PWMInit( PWM_PolarTypeDef pr, PWM_RepeatTsTypeDef ts )
{
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN
                        |RB_TMR_OUT_EN
                        |(pr<<4)
                        |(ts<<6);
}

/*******************************************************************************
* Function Name  : TMR1_PWMInit
* Description    : PWM Output Init
* Input          : PWM_PolarTypeDef  PWM_RepeatTsTypeDef
* Return         : None
*******************************************************************************/
void TMR1_PWMInit( PWM_PolarTypeDef pr, PWM_RepeatTsTypeDef ts )
{
    R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR1_CTRL_MOD = RB_TMR_COUNT_EN
                        |RB_TMR_OUT_EN
                        |(pr<<4)
                        |(ts<<6);
}

/*******************************************************************************
* Function Name  : TMR2_PWMInit
* Description    : PWM Output Init
* Input          : PWM_PolarTypeDef  PWM_RepeatTsTypeDef
* Return         : None
*******************************************************************************/
void TMR2_PWMInit( PWM_PolarTypeDef pr, PWM_RepeatTsTypeDef ts )
{
    R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR2_CTRL_MOD = RB_TMR_COUNT_EN
                        |RB_TMR_OUT_EN
                        |(pr<<4)
                        |(ts<<6);
}

/*******************************************************************************
* Function Name  : TMR0_CapInit
* Description    : TMR Capture
* Input          : CapModeTypeDef
* Return         : None
*******************************************************************************/
void TMR0_CapInit( CapModeTypeDef cap )
{
        R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
        R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN      \
                            |RB_TMR_MODE_IN     \
                            |(cap<<6);
}

/*******************************************************************************
* Function Name  : TMR1_CapInit
* Description    : TMR Capture
* Input          : CapModeTypeDef
* Return         : None
*******************************************************************************/
void TMR1_CapInit( CapModeTypeDef cap )
{
        R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR;
        R8_TMR1_CTRL_MOD = RB_TMR_COUNT_EN      \
                            |RB_TMR_MODE_IN     \
                            |(cap<<6);
}

/*******************************************************************************
* Function Name  : TMR2_CapInit
* Description    : TMR Capture
* Input          : CapModeTypeDef
* Return         : None
*******************************************************************************/
void TMR2_CapInit( CapModeTypeDef cap )
{
     R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR;
     R8_TMR2_CTRL_MOD = RB_TMR_COUNT_EN      \
                         |RB_TMR_MODE_IN     \
                         |(cap<<6);
}

/*******************************************************************************
* Function Name  : TMR1_DMACfd
* Description    : TMR DMA Configuration
* Input          : ENABLE/DISABLE
                   startAddr
                   endAddr
                   DMAModeTypeDef
* Return         : None
*******************************************************************************/
void TMR1_DMACfg( UINT8 s, UINT16 startAddr, UINT16 endAddr, DMAModeTypeDef m )
{
        if(s == DISABLE){
            R8_TMR1_CTRL_DMA = 0;
        }
        else{
            if(m)   R8_TMR1_CTRL_DMA = RB_TMR_DMA_LOOP|RB_TMR_DMA_ENABLE;
            else    R8_TMR1_CTRL_DMA = RB_TMR_DMA_ENABLE;
            R32_TMR1_DMA_BEG = startAddr;
            R32_TMR1_DMA_END = endAddr;
        }
}

/*******************************************************************************
* Function Name  : TMR2_DMACfd
* Description    : TMR DMA Configuration
* Input          : ENABLE/DISABLE
                   startAddr
                   endAddr
                   DMAModeTypeDef
* Return         : None
*******************************************************************************/
void TMR2_DMACfg( UINT8 s, UINT16 startAddr, UINT16 endAddr, DMAModeTypeDef m )
{
        if(s == DISABLE){
            R8_TMR2_CTRL_DMA = 0;
        }
        else{
            if(m)   R8_TMR2_CTRL_DMA = RB_TMR_DMA_LOOP|RB_TMR_DMA_ENABLE;
            else    R8_TMR2_CTRL_DMA = RB_TMR_DMA_ENABLE;
            R32_TMR2_DMA_BEG = startAddr;
            R32_TMR2_DMA_END = endAddr;
        }
}


