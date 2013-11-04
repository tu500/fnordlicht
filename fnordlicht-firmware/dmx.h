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

#ifndef DMX_H
#define DMX_H

#include "globals.h"
#include "fifo.h"
#include "../common/io.h"

#if !CONFIG_DMX

#define dmx_init(...)

#else

 enum {IDLE, BREAK, CHANNELS};

 /* structs */
struct global_dmx_t {
    uint16_t curChannel;
    uint8_t recvDataBuf[3];	//RGB color channels
};

/* global variables */
extern volatile struct global_dmx_t global_dmx;

/* prototypes */
void dmx_init(void);

#endif

#endif
