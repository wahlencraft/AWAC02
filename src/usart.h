/* This is a header declaring the functions defined by assembly rutines in
 * usart.S. */

#ifndef USART_H_
#define USART_H_

extern void USART_init();
extern void USART_putchar(char c);
extern int USART_putchar_printf(char var, FILE *stream);

static FILE usart_stdout = FDEV_SETUP_STREAM(USART_putchar_printf, NULL, _FDEV_SETUP_WRITE);

#endif // USART_H_H

