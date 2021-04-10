


#ifndef __CH56x_HSPI_H__
#define __CH56x_HSPI_H__

#ifdef __cplusplus
 extern "C" {
#endif


/**
  * @brief  HSPI Mode
  */
typedef enum
{
	DOWN_Mode = 0,
	UP_Mode,
}HSPI_ModeTypeDef;



void HSPI_Mode( UINT8 s,  HSPI_ModeTypeDef i);
void HSPI_INTCfg( UINT8 s,  UINT8 i );


	 
#ifdef __cplusplus
}
#endif

#endif  // __CH56x_HSPI_H__	

