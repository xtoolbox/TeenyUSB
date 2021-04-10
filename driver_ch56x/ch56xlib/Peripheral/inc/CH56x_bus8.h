


#ifndef __CH56x_BUS8_H__
#define __CH56x_BUS8_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define ADDR_NONE	0x00
#define ADDR_6		0x04
#define ADDR_10		0x08
#define ADDR_15		0x0c

#define WIDTH_3		0x00
#define WIDTH_5		0x10
#define WIDTH_9		0x20
#define WIDTH_16	0x30

#define HOLD_2		0x00
#define HOLD_3		0x40

#define SETUP_2		0x00
#define SETUP_3		0x80


void BUS8_Init(UINT8 addroe, UINT8 width, UINT8 hold, UINT8 setup);


	 
#ifdef __cplusplus
}
#endif

#endif  // __CH56x_BUS8_H__	

