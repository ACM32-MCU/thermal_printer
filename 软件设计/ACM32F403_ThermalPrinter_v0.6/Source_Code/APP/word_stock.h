#include "HAL_SPI.h"
#include "IC_W25Qxx.h"

//************define error code********************
#define 	RET_OK				0x9000
#define 	RET_ERROR			0x9001
#define		RET_INS_ERR		0x6D00
#define		VER_PIN_ERR		0x6C00
#define		RET_P1P2_ERR	0x6A86
#define		RET_LC_ERR		0x6700
#define		RET_CLA_ERR		0x6F00
#define		RET_XOR_ERR		0x6E00

//************SPI Wire********************
#define TEST_1X_MODE    (0)
#define TEST_2X_MODE    (2)
#define TEST_4X_MODE    (3)


typedef struct{

	UINT8	NAD;
	UINT8	PCB;
	UINT8	LEN1;
	UINT8	LEN2;
	UINT8	CLA;
	UINT8	INS;
	UINT8	P1;
	UINT8	P2;
	UINT8	LC;	
	UINT8 ApduData[258];
	UINT8	RetLen;
	UINT8	SW1;
	UINT8	SW2;
	UINT8	XOR;
}T_APDU;


void SPI_Init_Nor(void);
void uart_test_flow(uint8_t *fu8_Data, uint32_t fu32_Size);
void Print_String(void);
void Get_GBK_16(uint8_t *code, uint8_t *dz_data);
UINT8 Get_GBK_24(uint8_t *code, uint8_t *dz_data);
void Get_GBK_char_16(uint8_t *code, uint8_t *dz_data);

