/*
 * unzap firmware
 *
 * (c) by Alexander Neumann <alexander@lochraster.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <stdint.h>
#include <avr/interrupt.h>
#include "../common/io.h"
#include "timer.h"

static volatile uint8_t internal_counter;

void timer_init(void)
{
#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
    /* initialize timer2, CTC at 10ms, prescaler 1024 */
    OCR2 = F_CPU/1024/100;
    TCCR2 = _BV(WGM21) | _BV(CS22) | _BV(CS21) | _BV(CS20);
    TIMSK |= _BV(OCIE2);
#elif defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
    /* initialize timer2, CTC at 10ms, prescaler 1024 */
    OCR2A = F_CPU/1024/100;
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
    TIMSK2 = _BV(OCIE2A);
#else
#error "unknown controller, unable to initialize timer2"
#endif
}

void timer_set(timer_t *t, uint8_t timeout)
{
    t->current = internal_counter;
    t->timeout = timeout;
}

bool timer_expired(timer_t *t)
{
    if (t->timeout == 0)
        return true;

    /* attention: this is not correct, if internal_counter is incremented by more than one
     * between two calls of timer_expired()! */
    if (t->current != internal_counter) {
        t->timeout--;
        t->current = internal_counter;
    }

    return false;
}

/* timer interrupt function */
#if defined(__AVR_ATmega8__)
ISR(TIMER2_COMP_vect, ISR_NOBLOCK) {
    internal_counter++;
}
#elif defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK) {
    internal_counter++;
}
#endif
