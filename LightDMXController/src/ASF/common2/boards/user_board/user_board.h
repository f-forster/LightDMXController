/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_common_boards
 * \defgroup user_board_group User board
 *
 * @{
 */

void system_board_init(void);

/** Name string macro */
#define BOARD_NAME                "USER_BOARD"


// leds
#define LED_RED_PIN             PIN_PA28
#define LED_GREEN_PIN           PIN_PA19
#define	LED_YELLOW_PIN			PIN_PA27
#define LED_ACTIVE				true
#define LED_INACTIVE            !LED_ACTIVE

#define BUTTON1_PIN				PIN_PA15

// unused pins
#define UNUSED_PIN_0			PIN_PA08
#define UNUSED_PIN_1			PIN_PA09
#define UNUSED_PIN_2			PIN_PA10
#define UNUSED_PIN_3			PIN_PA11
#define UNUSED_PIN_4			PIN_PA14

// USB interface
#define USB_DP_PIN				PIN_PA25
#define USB_DN_PIN				PIN_PA24

// DMX Pins
#define DMX_RX_PIN				PIN_PA17
#define DMX_TX_PIN				PIN_PA16
#define DMX_DIR_PIN				PIN_PA18

// Debugger USART interface definitions
#define DBG_SERCOM_MODULE       SERCOM3
#define DBG_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define DBG_SERCOM_PINMUX_PAD0  PINMUX_PA22C_SERCOM3_PAD0
#define DBG_SERCOM_PINMUX_PAD1  PINMUX_UNUSED						// PINMUX_PA23C_SERCOM3_PAD1
#define DBG_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define DBG_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define DBG_SERCOM_DMAC_ID_TX   SERCOM3_DMAC_ID_TX
#define DBG_SERCOM_DMAC_ID_RX   SERCOM3_DMAC_ID_RX


// DMX/RDM USART interface definitions
#define DMX_SERCOM_MODULE       SERCOM1
#define DMX_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define DMX_SERCOM_PINMUX_PAD0  PINMUX_PA16C_SERCOM1_PAD0
#define DMX_SERCOM_PINMUX_PAD1  PINMUX_PA17C_SERCOM1_PAD1
#define DMX_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define DMX_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define DMX_SERCOM_DMAC_ID_TX   SERCOM1_DMAC_ID_TX
#define DMX_SERCOM_DMAC_ID_RX   SERCOM1_DMAC_ID_RX

// LT89xx interface
#define LT89XX_PIN_CS				PIN_PA05
#define LT89XX_PIN_RESET			PIN_PA02

#define LT89XX_PKT_IRQ_PIN			PIN_PA03A_EIC_EXTINT3
#define LT89XX_PKT_IRQ_MUX			MUX_PA03A_EIC_EXTINT3
#define LT89XX_PKT_IRQ_EIC_LINE		3

#define LT89XX_SERCOM_MUX_SETTING	SPI_SIGNAL_MUX_SETTING_E 
									/* SPI Signal MUX settings: DS40001882G-page 462
									   -> SPI_SIGNAL_MUX_SETTING_E: 
									   DOPO set to 0x1 -> Data Out on PAD[2] (MOSI in Master Mode), SCK on PAD[3], SlaveSS on PAD[2}
									   DIPO set to 0x0 -> Data In on PAD[0] (MISO in Master Mode)   */
#define LT89XX_SERCOM_PINMUX_PAD0	PINMUX_PA04D_SERCOM0_PAD0
#define LT89XX_SERCOM_PINMUX_PAD1	PINMUX_UNUSED
#define LT89XX_SERCOM_PINMUX_PAD2	PINMUX_PA06D_SERCOM0_PAD2
#define LT89XX_SERCOM_PINMUX_PAD3	PINMUX_PA07D_SERCOM0_PAD3
#define LT89XX_SERCOM_SPI_MODULE	SERCOM0

#ifdef __cplusplus
}
#endif

#endif // USER_BOARD_H
