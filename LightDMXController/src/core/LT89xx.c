/*
 * LT89xx.c
 *
 * Created: 11.12.2020 23:42:16
 * Author: Franz Forster
 * 
 */ 

#include <asf.h>
#include "LT89XX.h"


#define MASK_READ_REGISTER		0x80
#define MASK_WRITE_REGISTER		0x00
#define MASK_REGISTER			0x7F

// Registers
#define REG_TX_RX_EN_CHANNEL	7	
#define REG_SYNCWORD0			36
#define REG_SYNCWORD1			37
#define REG_SYNCWORD2			38
#define REG_SYNCWORD3			39
#define REG_DATARATE			44
#define	REG_STATUS				48
#define REG_FIFO				50
#define REG_FIFO_CTRL			52

#define RX_EN_BIT				7
#define TX_EN_BIT				8

#define REG_STATUS_CRC_BIT		15


struct spi_module		spiMasterInstance;
struct spi_slave_inst	spiSlaveRef;

uint8_t channelNr = 76;


void		_Setup_Pins(void);
void		_Configure_SPI(void);
void		_Setup_PKT_IRQ(void);
void		_Write_Register(uint8_t reg, uint16_t data);
uint16_t	_Read_Register(uint8_t reg);


void LT89XX_Init(void)
{
	_Setup_Pins();
	_Setup_PKT_IRQ();
	_Configure_SPI();
	delay_ms(10);
	
	
	// Register initialisation:
	_Write_Register(0, 0x6fe0);
	_Write_Register(1, 0x5681);
	_Write_Register(2, 0x6617);
	_Write_Register(4, 0x9cc9);
	_Write_Register(5, 0x6637);
	_Write_Register(7, 0x004c);		// channel 76
	_Write_Register(8, 0x6c90);
	_Write_Register(9, 0x0000);		// power and gain (4800 on original)
	
	_Write_Register(10, 0x7ffd);
	_Write_Register(11, 0x0000);	// RSSI operates normaly
	_Write_Register(12, 0x0000);
	_Write_Register(13, 0x48bd);
	
	_Write_Register(22, 0x00ff);
	_Write_Register(23, 0x8005);
	_Write_Register(24, 0x0067);
	_Write_Register(25, 0x1659);
	_Write_Register(26, 0x19e0);
	_Write_Register(27, 0x1300);
	_Write_Register(28, 0x1800);
	
	_Write_Register(32, 0x5800); 
	_Write_Register(33, 0x3fc7);
	_Write_Register(34, 0x2000);
	_Write_Register(35, 0x0000);
	
	LT89XX_SetSyncWord(0x03805a5aa5a50380);
	
	_Write_Register(40, 0x4401);	// max. 3 error bits in syncword
	_Write_Register(41, 0xb400);	
	_Write_Register(42, 0xfdb0);
	_Write_Register(43, 0x000f);
	LT89XX_SetDataRate(LT89XX_250KBPS);
	_Write_Register(45, 0x0552);
	
}



void _Setup_Pins(void)
{
	ioport_set_pin_dir(LT89XX_PIN_RESET, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LT89XX_PIN_RESET, LOW);
	delay_ms(50);
	ioport_set_pin_level(LT89XX_PIN_RESET, HIGH);
	delay_ms(50);
}



void _Setup_PKT_IRQ(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = LT89XX_PKT_IRQ_PIN;
	config_extint_chan.gpio_pin_mux       = MUX_PA23A_EIC_EXTINT7;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_DOWN;
	config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(LT89XX_PKT_IRQ_EIC_LINE, &config_extint_chan);
}



void LT89XX_Register_PKT_Callback(const tRecvCallback cb)
{
	extint_register_callback(cb, LT89XX_PKT_IRQ_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(LT89XX_PKT_IRQ_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
}



void _Configure_SPI(void)
{
	struct spi_config configSpiMaster;
	struct spi_slave_inst_config configAttatchedSlave;
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&configAttatchedSlave);
	configAttatchedSlave.ss_pin = LT89XX_PIN_CS;
	spi_attach_slave(&spiSlaveRef, &configAttatchedSlave);
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&configSpiMaster);
	configSpiMaster.mux_setting = LT89XX_SERCOM_MUX_SETTING;
	configSpiMaster.pinmux_pad0 = LT89XX_SERCOM_PINMUX_PAD0;
	configSpiMaster.pinmux_pad1 = LT89XX_SERCOM_PINMUX_PAD1;
	configSpiMaster.pinmux_pad2 = LT89XX_SERCOM_PINMUX_PAD2;
	configSpiMaster.pinmux_pad3 = LT89XX_SERCOM_PINMUX_PAD3;
	// LT89XX uses CPOL=0 (sck low in idle) and CPHA=1 (sample on falling edge)
	configSpiMaster.transfer_mode = SPI_TRANSFER_MODE_1;
	configSpiMaster.mode_specific.master.baudrate = 500000; // higher than 500k not recommended due to timing limitations of LT89xx
	spi_init(&spiMasterInstance, LT89XX_SERCOM_SPI_MODULE, &configSpiMaster);
	spi_enable(&spiMasterInstance);
}



void _Write_Register(uint8_t reg, uint16_t data)
{
	static uint8_t txData[3];
	txData[0] = MASK_WRITE_REGISTER | (MASK_REGISTER & reg);
	txData[1] = ((data >> 8) & 0xff);
	txData[2] = (data & 0xff);
	spi_select_slave(&spiMasterInstance, &spiSlaveRef, true);
	spi_write_buffer_wait(&spiMasterInstance, (const uint8_t*)&txData, sizeof(txData));
	spi_select_slave(&spiMasterInstance, &spiSlaveRef, false);
}


uint16_t _Read_Register(uint8_t reg)
{
	uint8_t rxBuffer[3], txBuffer[3];
	txBuffer[0] = MASK_READ_REGISTER | (MASK_REGISTER & reg);
	txBuffer[1] = txBuffer[2] = 0;
	spi_select_slave(&spiMasterInstance, &spiSlaveRef, true);
	spi_transceive_buffer_wait(&spiMasterInstance, txBuffer, rxBuffer, 3);
	spi_select_slave(&spiMasterInstance, &spiSlaveRef, false);
	return (rxBuffer[1] << 8 | rxBuffer[2]);
}


void LT89XX_SetDataRate(LT89XX_Datarate datarate)
{
	uint8_t dr = 0;
	if (datarate == LT89XX_1MBPS) {
		dr = 1;
	} else if (datarate == LT89XX_250KBPS) {
		dr = 4;
	} else if (datarate == LT89XX_125KBPS) {
		dr = 8;
	} else if (datarate == LT89XX_65_5KBPS) {
		dr = 10;
	} else {
		return;
	}
	_Write_Register(REG_DATARATE, ((uint16_t)dr<<8));
}



void LT89XX_SetChannel(uint8_t channel)
{
	channelNr = channel;
	_Write_Register(REG_TX_RX_EN_CHANNEL, channelNr & 0x7f);
}



void LT89XX_SetSyncWord(uint64_t syncword)
{
	_Write_Register(REG_SYNCWORD0, (syncword & 0xffff));
	_Write_Register(REG_SYNCWORD1, ((syncword>>16) & 0xffff));
	_Write_Register(REG_SYNCWORD2, ((syncword>>32) & 0xffff));
	_Write_Register(REG_SYNCWORD3, ((syncword>>48) & 0xffff));	
}



void LT89XX_StartListening(void)
{
	delay_us(10); // WTF
	_Write_Register(REG_TX_RX_EN_CHANNEL, channelNr & 0x7f);
	delay_us(10); // WTF
	_Write_Register(REG_FIFO_CTRL, 0x0080); // clear FIFO read pointer
	_Write_Register(REG_TX_RX_EN_CHANNEL, (channelNr & 0x7f) | (1<<RX_EN_BIT));
}



void LT89XX_StopListening(void)
{
	_Write_Register(REG_TX_RX_EN_CHANNEL, channelNr & 0x7f);
}


void LT89XX_Read(uint8_t* buffer, const uint8_t bufferSize, uint8_t* messageLength)
{
	uint16_t statusReg, packetSize, data;
	uint8_t position = 0;
	
	statusReg = _Read_Register(REG_STATUS);
	if (! (statusReg & REG_STATUS_CRC_BIT)) {
		// CRC ok
		
		data = _Read_Register(REG_FIFO);
		packetSize = data >> 8;
		if (packetSize > bufferSize + 1) {
			// rxBuffer to small
			return;
		} else {
			*messageLength = packetSize;
		}
		position = 0;
		buffer[position++] = (data & 0xff);
		while (position < packetSize) {
			data = _Read_Register(REG_FIFO);
			buffer[position++] = (data >> 8);
			buffer[position++] = (data & 0xff);
		}
		
	}
}


