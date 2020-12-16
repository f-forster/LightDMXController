/*
 * LT89xx.c
 *
 * Created: 11.12.2020 23:42:16
 * Author: Franz Forster
 * 
 */ 


#ifndef LT89XX_H_
#define LT89XX_H_

#define LT89XX_PIN_CS				PIN_PA10

#define LT89XX_PIN_RESET			PIN_PA19
#define LT89XX_PKT_IRQ_PIN			PIN_PA23A_EIC_EXTINT7
#define LT89XX_PKT_IRQ_MUX			MUX_PA23A_EIC_EXTINT7
#define LT89XX_PKT_IRQ_EIC_LINE		7

#define LT89XX_SERCOM_MUX_SETTING	SPI_SIGNAL_MUX_SETTING_D 
									/* SPI_SIGNAL_MUX_SETTING_D: 
									   DOPO set to 0x0  -> Data Out on PAD[0] (MOSI in Master Mode), SCK on PAD[1], SlaveSS on PAD[2}
									   DIPO set to 0x03 -> Data In on PAD[3] (MISO in Master Mode)   */
#define LT89XX_SERCOM_PINMUX_PAD0	PINMUX_PA08D_SERCOM2_PAD0
#define LT89XX_SERCOM_PINMUX_PAD1	PINMUX_PA09D_SERCOM2_PAD1
#define LT89XX_SERCOM_PINMUX_PAD2	PINMUX_UNUSED
#define LT89XX_SERCOM_PINMUX_PAD3	PINMUX_PA11D_SERCOM2_PAD3
#define LT89XX_SERCOM_SPI_MODULE	SERCOM2



typedef enum 
{
	LT89XX_1MBPS,
	LT89XX_250KBPS,
	LT89XX_125KBPS,
	LT89XX_65_5KBPS
	
} LT89XX_Datarate;

typedef void(* tRecvCallback)(void);

void LT89XX_Init(void);
void LT89XX_Register_PKT_Callback(const tRecvCallback cb);
void LT89XX_SetChannel(uint8_t channel);

void LT89XX_SetDataRate(LT89XX_Datarate datarate);
void LT89XX_SetSyncWord(uint64_t syncword);
void LT89XX_StartListening(void);

void LT89XX_StopListening(void);
void LT89XX_SetPowerAndGain(uint8_t txPower, uint8_t rxGain); // TBD
void LT89XX_Read(uint8_t* buffer, const uint8_t bufferSize, uint8_t* messageLength); 
void LT89XX_Write(void); // TBD




#endif /* LT89XX_H_ */