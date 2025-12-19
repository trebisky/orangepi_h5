/* protos.h */

void blink ( void );

void uart_init ( void );
void uart_gpio_init ( void );
void uart_puts ( char * );

void gic_init ( void );
void intcon_ena ( int );
void gic_soft_self ( int );
int intcon_irqwho ( void );
void intcon_irqack ( int );

void led_init ( void );
void led_toggle ( void );

void timer_init ( int );
void timer_handler ( void );

// THE END
