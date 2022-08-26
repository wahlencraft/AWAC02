/* This is a header declaring the functions defined by assembly rutines in
 * time.S. */

#ifndef TIME_H_
#define TIME_H_

/* Get a timestamp from timer 0/1 */
extern uint8_t read_timer0();
extern uint16_t read_timer1();

/* (Re)start counter 0/1 with a prescaler 1024 */
extern void start_counter0();
extern void start_counter1();

/* Calculate time in ms between two timestamps (from counter 0).
 *
 * Note: only accurate if prescaler is 1024.
 *
 * Can not handle a timestamp more than 255 counts ago ->
 * 2^16/(F_CPU/1,000,000*250) ms
 *
 * For F_CPU = 8 MHz that is 1 <= T <= 32 [ms]
 */
extern uint16_t timestamp_to_ms0(uint8_t timestamp0, uint8_t timestamp1);

/* Calculate time in ms between two timestamps (from counter 1).
 *
 * Note: only accurate if prescaler is 1024.
 *
 * Can not handle a timestamp more than 65535 counts ago ->
 * 2^24/(F_CPU/1,000,000*250) ms
 *
 * For F_CPU = 8 MHz that is 1 <= T <= 8388 [ms]
 */
extern uint16_t timestamp_to_ms1(uint16_t timestamp0, uint16_t timestamp1);

/* Use counter 0 timer A to perform a busy wait
 *
 * Counter 0 must be started first (with prescaler 1024)
 * The counter is not reset due to this call. So it can also be used for other
 * things.
 *
 * Note that the rage of allowed t depends on the system clock frequency
 * 1 <= t =< 255*1024/(F_CPU/1,000) [ms]
 * Also note that the system clock must be a multiple of 1,000
 *
 * For F_CPU = 8 MHz that is
 * 1 <= t <= 32 [ms]
 */
extern void busy_wait_ms0(uint8_t t);

/* Use counter 1 timer A to perform a busy wait
 *
 * Counter 1 must be started first (with prescaler 1024)
 * The counter is not reset due to this call. So it can also be used for other
 * things.
 *
 * Note that the rage of allowed t depends on the system clock frequency
 * 1 <= t =< 65535*1024/(F_CPU/1,000) [ms]
 * Also note that the system clock must be a multiple of 1,000
 *
 * For F_CPU = 8 MHz that is
 * 1 <= t <= 8,388 [ms]
 */
extern void busy_wait_ms1(uint16_t t);

/* Use counter 0 timer A to sleep
 *
 * Counter 0 must be started first (with prescaler 1024)
 * The counter is not reset due to this call. So it can also be used for other
 * things.
 *
 * Note that the rage of allowed t depends on the system clock frequency
 * 1 <= t =< 255*1024/(F_CPU/1,000) [ms]
 * Also note that the system clock must be a multiple of 1,000
 *
 * For F_CPU = 8 MHz that is
 * 1 <= t <= 32 [ms]
 */
extern void sleep_ms0(uint16_t t);

/* Set IRQ timer alarm 1 in t ms.
 *
 * Note that the rage of allowed t depends on the system clock frequency
 * 1 <= t =< 65535*1024/(F_CPU/1,000) [ms]
 * Also note that the system clock must be a multiple of 1,000
 *
 * For F_CPU = 8 MHz that is
 * 1 <= t <= 8,388 [ms]
 */
extern void set_timer1_irq_alarm_ms(uint16_t t);

/* Sleep until interrupt.
 *
 * This can be any type of interrupt. For example timers and external. */
extern void sleep_until_interrupt();

/* Use counter 1 timer A to sleep
 *
 * Counter 1 must be started first (with prescaler 1024)
 * The counter is not reset due to this call. So it can also be used for other
 * things.
 *
 * Note that the rage of allowed t depends on the system clock frequency
 * 1 <= t =< 65535*1024/(F_CPU/1,000) [ms]
 * Also note that the system clock must be a multiple of 1,000
 *
 * For F_CPU = 8 MHz that is
 * 1 <= t <= 8,388 [ms]
 */
extern void sleep_ms1(uint16_t t);

#endif // TIME_H_

