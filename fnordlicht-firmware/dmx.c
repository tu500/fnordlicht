/* vim:ts=4 sts=4 et tw=80
 *
 *         fnordlicht firmware
 *             DMX Support
 *
 *    for additional information please
 *    see http://lochraster.org/fnordlichtmini
 *
 * (c) by Andreas Galauner <andreas@galauner.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

#if CONFIG_DMX

#include "../common/io.h"
#include <avr/interrupt.h>

#include "../common/common.h"
#include "dmx.h"
#include "pwm.h"
#include "uart.h"

/* define uart mode (8N2) */
#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
/* in atmega8, we need a special switching bit
 * for addressing UCSRC */
#define UART_UCSRC _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1) | _BV(USBS)

#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
/* in atmega88, this isn't needed any more */
#define UART_UCSRC _BV(_UCSZ0_UART0) | _BV(_UCSZ1_UART0) | _BV(_USBS_UART0)
#endif

/* global variables */
volatile struct global_dmx_t global_dmx;

/** init the hardware uart */
void dmx_init(void)
{
    //DMX uses 250kBaud
    #define BAUD 250000
    #include <util/setbaud.h>

    /* set baud rate */
    _UBRRH_UART0 = UBRRH_VALUE;
    _UBRRL_UART0 = UBRRL_VALUE;

    #if USE_2X
    _UCSRA_UART0 |= (1 << _U2X_UART0);
    #endif

    /* set mode */
    _UCSRC_UART0 = UART_UCSRC;

    /* receiver and receiver complete interrupt */
    _UCSRB_UART0 = _BV(_RXEN_UART0) | _BV(_RXCIE_UART0);

    /* initialize DMX receiver state machine state */
    global_dmx.curChannel = 0;
    global_dmx.recvDataBuf[0] = 0;
    global_dmx.recvDataBuf[1] = 0;
    global_dmx.recvDataBuf[2] = 0;

    global_pwm.target.rgb.red = 0;
    global_pwm.target.rgb.green = 255;
}

/** interrupts*/

/** uart receive interrupt */
ISR(_SIG_UART_RECV_UART0)
{
    static unsigned int dmx_channel_rx_count = 0;
    static unsigned char dmx_valid = 0;
    unsigned char tmp = 0;

    tmp =  _UDR_UART0;

    if(_UCSRA_UART0&(1<<FE))
    {
        dmx_channel_rx_count = 0;
        if(tmp == 0)
        {
            dmx_valid = 1;
            dmx_channel_rx_count++;
        }
        else
        {
            dmx_valid = 0;
        }
        return;
    }

    if(dmx_valid)
    {
        if(dmx_channel_rx_count == CONFIG_DMX_ADDRESS) global_dmx.recvDataBuf[0] = tmp;
        if(dmx_channel_rx_count == CONFIG_DMX_ADDRESS+1) global_dmx.recvDataBuf[1] = tmp;
        if(dmx_channel_rx_count == CONFIG_DMX_ADDRESS+2)
        {
            global_dmx.recvDataBuf[2] = tmp;
            struct rgb_color_t color;
            color.red = global_dmx.recvDataBuf[0];
            color.green = global_dmx.recvDataBuf[1];
            color.blue = global_dmx.recvDataBuf[2];
            
            pwm_fade_rgb(&color, 255, 0);
        }

        if(dmx_channel_rx_count < 514)
        {
            dmx_channel_rx_count++;
        }
        return;
    }
}

#endif
