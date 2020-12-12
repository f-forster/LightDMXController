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


/** \name LED definitions
 *  @{ */
#define LED_RED_PIN             PIN_PA27
#define LED_GREEN_PIN           PIN_PA28
#define LED_ACTIVE				true
#define LED_INACTIVE            !LED_ACTIVE
/** @} */

/** \name Debug Pins definitions (also SERCOM3)
 *  @{ */
	 /*
#define DEBUG0_PIN				PIN_PA08
#define DEBUG1_PIN				PIN_PA09
#define DEBUG2_PIN				PIN_PA10
#define DEBUG3_PIN				PIN_PA11
*/

/** @} */


// DMX Pins

#define DMX_RX_PIN				PIN_PA17
#define DMX_TX_PIN				PIN_PA16
#define DMX_DIR_PIN				PIN_PA18

/** \name Debugger USART interface definitions
 * @{
 */
#define DBG_SERCOM_MODULE       SERCOM3
#define DBG_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define DBG_SERCOM_PINMUX_PAD0  PINMUX_PA22C_SERCOM3_PAD0
#define DBG_SERCOM_PINMUX_PAD1  PINMUX_PA23C_SERCOM3_PAD1
#define DBG_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define DBG_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define DBG_SERCOM_DMAC_ID_TX   SERCOM3_DMAC_ID_TX
#define DBG_SERCOM_DMAC_ID_RX   SERCOM3_DMAC_ID_RX
/** @} */


/** \name DMX/RDM USART interface definitions
 * @{
 */
#define DMX_SERCOM_MODULE       SERCOM1
#define DMX_SERCOM_MUX_SETTING  USART_RX_1_TX_0_XCK_1
#define DMX_SERCOM_PINMUX_PAD0  PINMUX_PA16C_SERCOM1_PAD0
#define DMX_SERCOM_PINMUX_PAD1  PINMUX_PA17C_SERCOM1_PAD1
#define DMX_SERCOM_PINMUX_PAD2  PINMUX_UNUSED
#define DMX_SERCOM_PINMUX_PAD3  PINMUX_UNUSED
#define DMX_SERCOM_DMAC_ID_TX   SERCOM1_DMAC_ID_TX
#define DMX_SERCOM_DMAC_ID_RX   SERCOM1_DMAC_ID_RX
/** @} */

























/** @} */

#ifdef __cplusplus
}
#endif

#endif // USER_BOARD_H
