


#ifndef __CH56x_PWM_H__
#define __CH56x_PWM_H__

#ifdef __cplusplus
 extern "C" {
#endif


/**
  * @brief  channel of PWM define
  */
#define	CH_PWM0 	0x01				// PWM0 通道
#define	CH_PWM1		0x02				// PWM1 通道
#define	CH_PWM2		0x04				// PWM2  通道
#define	CH_PWM3		0x08				// PWM3 通道


/**
  * @brief  channel of PWM define
  */
typedef enum
{
	High_Level = 0,					     //默认低电平，高电平有效
	Low_Level,							 //默认高电平，低电平有效
}PWMX_PolarTypeDef;


/**
  * @brief  Configuration PWM0_3 Cycle size
  */
typedef enum
{
	PWMX_Cycle_256 = 0,					                               //256个PWMX周期
	PWMX_Cycle_255,						                               //255个PWMX周期

}PWMX_CycleTypeDef;


#define  PWMX_CLKCfg( d )  (R8_PWM_CLOCK_DIV=d)	                       //PWM基准时钟配置
void PWMX_CycleCfg( PWMX_CycleTypeDef cyc );		                   //PWM输出波形周期配置

#define PWM0_ActDataWidth( d )		(R8_PWM0_DATA = d)		            //PWM0 有效数据脉宽
#define PWM1_ActDataWidth( d )		(R8_PWM1_DATA = d)					//PWM1 有效数据脉宽
#define PWM2_ActDataWidth( d )		(R8_PWM2_DATA = d)					//PWM2 有效数据脉宽
#define PWM3_ActDataWidth( d )		(R8_PWM3_DATA = d)					//PWM3 有效数据脉宽

//占空比=数据有效宽度/波形周期
void PWMX_ACTOUT( UINT8 ch, UINT8 da, PWMX_PolarTypeDef pr, UINT8 s);	//PWM0-3输出波形配置


		 
#ifdef __cplusplus
}
#endif

#endif  // __CH56x_PWM_H__	

