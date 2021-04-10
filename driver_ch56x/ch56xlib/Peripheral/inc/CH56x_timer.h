


#ifndef __CH56x_TIMER_H__
#define __CH56x_TIMER_H__

#ifdef __cplusplus
 extern "C" {
#endif
	

/**
  * @brief  脉冲宽度调制有效输出字数
  */
typedef enum
{
	PWM_Times_1 = 0,					// PWM 有效输出重复1次数
	PWM_Times_4 = 1,						// PWM 有效输出重复4次数
	PWM_Times_8 = 2,						// PWM 有效输出重复8次数
	PWM_Times_16 = 3,						// PWM 有效输出重复16次数
}PWM_RepeatTsTypeDef;


/**
  * @brief  输入捕获边沿方式
  */
typedef enum
{
	CAP_NULL = 0,						// 不捕捉
	Edge_To_Edge = 1,						// 任意边沿之间
	FallEdge_To_FallEdge = 2,				// 下降沿到下降沿
	RiseEdge_To_RiseEdge = 3,				// 上升沿到上升沿
}CapModeTypeDef;


/**
  * @brief  直接访问存储器循环模式
  */
typedef enum
{
	Mode_Single = 0,				// 单次模式
	Mode_LOOP = 1,						// 循环模式
}DMAModeTypeDef;


/**
  * @brief  PWM波输出极性
  */
typedef enum
{
	high_on_low = 0,						// 默认低电平，高电平有效
	low_on_high = 1,						// 默认高电平，低电平有效
}PWM_PolarTypeDef;


/****************** TMR0 */
// 定时和计数
void TMR0_TimerInit( UINT32 t );									/* 定时功能初始化 */
void TMR0_EXTSignalCounterInit( UINT32 c );							/* 外部信号计数功能初始化 */
#define  TMR0_GetCurrentCount()		R32_TMR0_COUNT	 				/* 获取当前计数值，67108864 */

// 脉冲宽度调制功能
#define TMR0_PWMCycleCfg( cyc )	    (R32_TMR0_CNT_END=cyc)			/* PWM0 通道输出波形周期配置, 最大67108864 */
void TMR0_PWMInit( PWM_PolarTypeDef pr, PWM_RepeatTsTypeDef ts );	/* PWM 输出初始化 */
#define TMR0_PWMActDataWidth( d )   (R32_TMR0_FIFO = d)			/* PWM0 有效数据脉宽, 最大67108864 */

// 捕捉脉宽
#define TMR0_CAPTimeoutCfg( cyc )   (R32_TMR0_CNT_END=cyc)			/* CAP0 捕捉电平超时配置, 最大33554432 */
void TMR0_CapInit( CapModeTypeDef cap );							/* 外部信号捕捉功能初始化 */
#define TMR0_CAPGetData()			R32_TMR0_FIFO					/* 获取脉冲数据 */
#define TMR0_CAPDataCounter()		R8_TMR0_FIFO_COUNT				/* 获取当前已捕获数据个数 */

#define TMR0_Disable()				(R8_TMR0_CTRL_MOD &= ~RB_TMR_COUNT_EN)		/* 关闭 TMR0 */
// refer to TMR0 interrupt bit define
#define	TMR0_ITCfg(s,f)				((s)?(R8_TMR0_INTER_EN|=f):(R8_TMR0_INTER_EN&=~f))		/* TMR0 相应中断位开启与关闭 */
// refer to TMR0 interrupt bit define
#define TMR0_ClearITFlag(f)         (R8_TMR0_INT_FLAG = f)			/* 清除中断标志 */
#define TMR0_GetITFlag(f)           (R8_TMR0_INT_FLAG&f)			/* 查询中断标志状态 */


/****************** TMR1 */
// 定时和计数
void TMR1_TimerInit( UINT32 t );									/* 定时功能初始化 */
void TMR1_EXTSignalCounterInit( UINT32 c );							/* 外部信号计数功能初始化 */
#define  TMR1_GetCurrentCount()		R32_TMR1_COUNT	 				/* 获取当前计数值，最大67108864 */

// 脉冲宽度调制功能
#define TMR1_PWMCycleCfg( cyc )	    (R32_TMR1_CNT_END=cyc)			/* PWM1 通道输出波形周期配置, 最大67108864 */
void TMR1_PWMInit( PWM_PolarTypeDef pr, PWM_RepeatTsTypeDef ts );	/* PWM1 输出初始化 */
#define TMR1_PWMActDataWidth( d )   (R32_TMR1_FIFO = d)			/* PWM1 有效数据脉宽, 最大67108864 */

// 捕捉脉宽
#define TMR1_CAPTimeoutCfg( cyc )   (R32_TMR1_CNT_END=cyc)			/* CAP1 捕捉电平超时配置, 最大33554432 */
void TMR1_CapInit( CapModeTypeDef cap );							/* 外部信号捕捉功能初始化 */
#define TMR1_CAPGetData()			R32_TMR1_FIFO					/* 获取脉冲数据 */
#define TMR1_CAPDataCounter()		R8_TMR1_FIFO_COUNT				/* 获取当前已捕获数据个数 */

void TMR1_DMACfg( UINT8 s, UINT16 startAddr, UINT16 endAddr, DMAModeTypeDef m );    /* DMA配置  */

#define TMR1_Disable()				(R8_TMR1_CTRL_MOD &= ~RB_TMR_COUNT_EN)		/* 关闭 TMR1 */
// refer to TMR1 interrupt bit define
#define	TMR1_ITCfg(s,f)				((s)?(R8_TMR1_INTER_EN|=f):(R8_TMR1_INTER_EN&=~f))		/* TMR1 相应中断位开启与关闭 */
// refer to TMR1 interrupt bit define
#define TMR1_ClearITFlag(f)         (R8_TMR1_INT_FLAG = f)			/* 清除中断标志 */
#define TMR1_GetITFlag(f)           (R8_TMR1_INT_FLAG&f)			/* 查询中断标志状态 */


/****************** TMR2 */
// 定时和计数
void TMR2_TimerInit( UINT32 t );									/* 定时功能初始化 */
void TMR2_EXTSignalCounterInit( UINT32 c );							/* 外部信号计数功能初始化 */
#define  TMR2_GetCurrentCount()		R32_TMR2_COUNT	 				/* 获取当前计数值，最大67108864 */

// 脉冲宽度调制功能
#define TMR2_PWMCycleCfg( cyc )	    (R32_TMR2_CNT_END=cyc)			/* PWM2 通道输出波形周期配置, 最大67108864 */
void TMR2_PWMInit( PWM_PolarTypeDef pr, PWM_RepeatTsTypeDef ts );	/* PWM2 输出初始化 */
#define TMR2_PWMActDataWidth( d )   (R32_TMR2_FIFO = d)			/* PWM2 有效数据脉宽, 最大67108864 */

// 捕捉脉宽
#define TMR2_CAPTimeoutCfg( cyc )   (R32_TMR2_CNT_END=cyc)			/* CAP2 捕捉电平超时配置, 最大33554432 */
void TMR2_CapInit( CapModeTypeDef cap );							/* 外部信号捕捉功能初始化 */
#define TMR2_CAPGetData()			R32_TMR2_FIFO					/* 获取脉冲数据 */
#define TMR2_CAPDataCounter()		R8_TMR2_FIFO_COUNT				/* 获取当前已捕获数据个数 */

void TMR2_DMACfg( UINT8 s, UINT16 startAddr, UINT16 endAddr, DMAModeTypeDef m );    /* DMA配置  */

#define TMR2_Disable()				(R8_TMR2_CTRL_MOD &= ~RB_TMR_COUNT_EN)		/* 关闭 TMR2 */
// refer to TMR2 interrupt bit define
#define	TMR2_ITCfg(s,f)				((s)?(R8_TMR2_INTER_EN|=f):(R8_TMR2_INTER_EN&=~f))		/* TMR2 相应中断位开启与关闭 */
// refer to TMR2 interrupt bit define
#define TMR2_ClearITFlag(f)         (R8_TMR2_INT_FLAG = f)			/* 清除中断标志 */
#define TMR2_GetITFlag(f)           (R8_TMR2_INT_FLAG & f)			/* 查询中断标志状态 */



#ifdef __cplusplus
 }
#endif

#endif // __CH56x_TIMER_H__





