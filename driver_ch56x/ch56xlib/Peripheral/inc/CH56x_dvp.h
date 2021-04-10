


#ifndef __CH56x_DVP_H__
#define __CH56x_DVP_H__

#ifdef __cplusplus
 extern "C" {
#endif


/**
  * @brief  DVP Data Mode
  */
typedef enum
{
	Video_Mode = 0,
	JPEG_Mode,
}DVP_Data_ModeTypeDef;


/**
  * @brief  DVP DMA 
  */
typedef enum
{
	DVP_DMA_Disable = 0,
	DVP_DMA_Enable,
}DVP_DMATypeDef;

/**
  * @brief  DVP FLAG and FIFO Reset
  */
typedef enum
{
	DVP_FLAG_FIFO_RESET_Disable = 0,
	DVP_FLAG_FIFO_RESET_Enable,
}DVP_FLAG_FIFO_RESETTypeDef;

/**
  * @brief  DVP RX Reset
  */
typedef enum
{
	DVP_RX_RESET_Disable = 0,
	DVP_RX_RESET_Enable,
}DVP_RX_RESETTypeDef;



void DVP_INTCfg( UINT8 s,  UINT8 i );
void DVP_Mode( UINT8 s,  DVP_Data_ModeTypeDef i);
void DVP_Cfg( DVP_DMATypeDef s,  DVP_FLAG_FIFO_RESETTypeDef i, DVP_RX_RESETTypeDef j);

	 
	 
#ifdef __cplusplus
}
#endif

#endif  // __CH56x_DVP_H__	

