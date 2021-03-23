/*
 * LT89xx.c
 *
 * Created: 11.12.2020 23:42:16
 * Author: Franz Forster
 * 
 */ 


#ifndef LT89XX_H_
#define LT89XX_H_

#define VERSION_LT8910				// read bug


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