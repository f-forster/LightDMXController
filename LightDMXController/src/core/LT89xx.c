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


struct spi_module		spiMasterInstance;
struct spi_slave_inst	spiSlaveRef;



void _Setup_Pins(void);
void _Write_Register(uint8_t reg, uint16_t data);
void _Configure_SPI(void);

void LT89XX_Init(void)
{
	_Setup_Pins();
	_Configure_SPI();
	
	
	// Register initialisation:
	_Write_Register(35, 0x55aa);
	_Write_Register(35, 0x55aa);
}



void _Setup_Pins(void)
{
	ioport_set_pin_dir(LT89XX_PIN_RESET, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LT89XX_PIN_PKT_IRQ, IOPORT_DIR_INPUT);
	
	ioport_set_pin_level(LT89XX_PIN_RESET, LOW);
	delay_ms(50);
	ioport_set_pin_level(LT89XX_PIN_RESET, HIGH);
	delay_ms(50);
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
	spi_select_slave(&spiMasterInstance, &spiSlaveRef, true);
	spi_write(&spiMasterInstance, MASK_READ_REGISTER | (MASK_REGISTER & reg));
	spi_write_buffer_wait(&spiMasterInstance, (const uint8_t*)&data, sizeof(data));
	spi_select_slave(&spiMasterInstance, &spiSlaveRef, false);
}





