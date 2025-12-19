/* The purpose of thie "bare metal" program is to figure out
 * how to get the H5 generating interrupts.
 * This began as h5_hello on 7-17-2020
 * This can write to the UART and also blink the LED.
 * Tom Trebisky  12-22-2016 7-17-2020
 *
 * Here is a play by play as experimenting procedes.
 *
 * 0 - the "power" LED does not respond or blink.
 *  This is not the focus of this exercise, but sorting
 *  this out may produce useful insights.
 *
 * --- As a first experiment, I tried configuring and then
 *  toggling ALL the bits in GPIO_L -- with no results.
 *  This was to check if the LED was miswired or misdocumented.
 * --- So I am busy looking over datasheets with the theory
 *  that the "R_GPIO" which along with a bunch of other "R_"
 *  peripherals does not have its clock gated on or some such.
 *  This is probably true, but I have yet to find a link between
 *  that and the interrupt problem.  The R_GPIO is at a base
 *  address of 0x01f02c00, while all the other GPIO devices
 *  have base addresses of 0x1c20xxxx.  The timer I use is also
 *  at 0x1c200c00, in the same section, which the block diagram
 *  shows as on the APB1 bus (The R gpio is on the APBS bus).
 *  It could be instructive to play with the WDOG, which is also
 *  in the "R_" section.
 *  There is an R_TIMER at 0x01f00800, but I have never tried to
 *  do anything with it.
 * --- What I am tempted to do is to look at U-Boot sources.
 *  I have no trouble accessing R_GPIO on the H3 boards, so maybe
 *  I can spot the difference in initialization.
 *
 * ----------------------------------------------------------------
 * ----------------------------------------------------------------
 *
 * Now for troubleshooting interrupts ...
 *
 * 1 - added vectors and tested that the table was active
 *  by generating an exception, which was fairly easy
 *  to get to work.
 * 2 - the next question is the GIC.  Do we use the same GIC
 *  as for the H3, or the v2.0 GIC (GIC400) that we have
 *  working for the armv8 Fire3 board.  It would seem that
 *  we want the GIC400 (which is GiCv2).
 * The other question is whether we have to power on the
 *  GIC and activate the clock for it,
 *  as we do for so many other peripherals.  Note that we
 *  inherit this setup from U-Boot for the uart and the
 *  GPIO section that drives the LED.
 * 3 - probably the next thing to try is to software interrupt
 *  capability of the GIC itself.
 */

#include "protos.h"

void int_init ( void );
void mk_fault ( void );

#define INT_unlock      asm volatile("msr DAIFClr, #3" : : : "cc")
#define INT_lock        asm volatile("msr DAIFSet, #3" : : : "cc")

/* We don't yet have printf, so these things are helpful
 */

#define PUTCHAR(x)      if ( buf <= end ) *buf++ = (x)

static const char hex_table[] = "0123456789ABCDEF";

// #define HEX(x)  ((x)<10 ? '0'+(x) : 'A'+(x)-10)
#define HEX(x)  hex_table[(x)]

static char *
shex2( char *buf, char *end, int val )
{
        PUTCHAR( HEX((val>>4)&0xf) );
        PUTCHAR( HEX(val&0xf) );
        return buf;
}

static char *
shex8( char *buf, char *end, int val )
{
        buf = shex2(buf,end,val>>24);
        buf = shex2(buf,end,val>>16);
        buf = shex2(buf,end,val>>8);
        return shex2(buf,end,val);
}

static void
show_hex ( int n )
{
		char buf[16];
		char *p;

		p = shex8 ( buf, &buf[16], n );
		*p++ = '\0';
		uart_puts ( buf );
}

static void
show_val ( char *msg, int val )
{
		uart_puts ( msg );
		show_hex ( val );
		uart_puts ( "\n" );
}

/* Here is the magic trick I learned from the RK3328
 */
/* This really belongs in start.S */
static void
el2_fixup ( void )
{
        unsigned long lval;

        /* XXX XXX -- grab FMO, IMO */
        asm volatile("mrs %0, hcr_el2" : "=r" (lval) : : "cc");
        lval |= 0x18;
        asm volatile("msr hcr_el2, %0" : : "r" (lval) : "cc");
}

static void
show_cpu ( void )
{
		unsigned int val;

        asm volatile("mrs %0, CurrentEL" : "=r" (val) : : "cc");
		uart_puts ( "Running in EL " );
		show_hex ( val >> 2 );
		uart_puts ( "\n" );

        asm volatile("mrs %0, hcr_el2" : "=r" (val) : : "cc");
		uart_puts ( "HCR " );
		show_hex ( val );
		uart_puts ( "\n" );
}

#define SGI_TEST	2

/* Count timer interrupts */
volatile int tcount;

void
main ( void )
{
	int_init ();

	uart_init ();
	led_init ();

	// timer_init ( 10 );
	timer_init ( 2 );

	el2_fixup ();
	show_cpu ();

	gic_init ();
	intcon_ena ( SGI_TEST );

	// for Timer 0 (same as H3)
	intcon_ena ( 50 );

	uart_puts("\n" );
	uart_puts("Starting interrupt test!\n");

	INT_unlock;

	uart_puts("\n" );

	// Generate an address fault
	// works - 12-18-2025
	// mk_fault ();

	// works (with el2_fixup())- 12-18-2025
	uart_puts ( "Try a soft (SGI) interrupt\n" );
	uart_puts ( " should yield IRQ 2\n" );
	gic_soft_self ( SGI_TEST );

	// uart_puts(" .. Blinking\n");
	// blink ();

	uart_puts(" .. Spinning\n");
	uart_puts(" .. Should be blinking\n");

#ifdef notdef
	/* spin */
	for ( ;; )
	    ;
#endif

	for ( ;; ) {
		asm volatile ( "wfe" );
	}

}

/* On the Orange Pi, we can apparently read any address
 * without a fault, including zero.  But if the unaligned
 * fault bit is set, we get a fault reading at an address
 * like "1", which we use to verify that our vector table
 * is indeed the one in effect.
 */
void
mk_fault ( void )
{
	long *p;
	long test;

#ifdef notdef
	/* This works fine */
	uart_puts ( "zero\n" );
	p = (long *) 0x0;
	test = *p;
#endif

	uart_puts ( "one\n" );
	p = (long *) 0x1;
	test = *p;

#ifdef notdef
	uart_puts ( "two\n" );
	p = (long *) 0x2;
	test = *p;

	uart_puts ( "three\n" );
	p = (long *) 0x3;
	test = *p;
#endif
}

// ------------------------------------
// interrupt stuff follows

#define NUM_IREGS       34

typedef	unsigned long			reg_t;


struct int_regs {
        reg_t regs[NUM_IREGS];
};

/* We really only need one of these, but we provide
 * some elbow room in case weird things happen.
 */
static struct int_regs bogus_thread[8];

void *cur_thread;


// void do_irq ( void )
void irq_handler ( void )
{
	int irq;
	// uart_puts ( "IRQ !\n" );

	irq = intcon_irqwho ();

	// show_val ( "Handling IRQ: ", irq );

	// Check for timer 0
	if ( irq == 50 ) {
		// uart_puts ( "Timer 0 interrupt\n" );
		if ( tcount == 0 )
			uart_puts ( "first Timer 0 interrupt\n" );
		if ( tcount == 1 )
			uart_puts ( "second Timer 0 interrupt\n" );
		// if ( tcount == 1000 )
		// 		uart_puts ( "1000th Timer 0 interrupt\n" );
		tcount++;
		timer_handler ();
	} else {
		show_val ( "Unexpected interrupt, IRQ = ", irq );
	}


	intcon_irqack ( irq );
}

void
fault_handler ( unsigned exc )
{
	uart_puts ( "Fault !\n" );
	// printf ( "Fault ! (%d)\n", exc );
	uart_puts ( "spinning after fault\n" );
	for ( ;; )
	    ;
}

void
int_init ( void )
{
	cur_thread = bogus_thread;
}

/* This is the way the Fire3 handled things */

/* exc is offset of the exception
 * esr is the esr (exception syndrome register)
 * regs is the regs
 */
void
sync_handler ( unsigned exc, unsigned esr, unsigned long *regs )
{
    unsigned int class = esr >> 26;     /* 31:26 */
    unsigned int syndrome = esr & 0x1ffffff;    /* 24:0 */

	// show_val ( "Sync exc: ", exc );
	// show_val ( "Sync esr: ", esr );
	// show_val ( "Sync class: ", class );
	/* Timer interrupt gives:
		Sync exc: 00000280
		Sync esr: BEB83B2E
		Sync class: 0000002F
	*/

	// IRQ
    if ( exc == 0x280 ) {
        irq_handler ();
        return;
	}

	uart_puts ( "Synch exception, not interrupt\n" );
	show_val ( "Sync exc: ", exc );
	show_val ( "Sync esr: ", esr );
	show_val ( "Sync class: ", class );

	fault_handler ( exc );

#ifdef notdef
    // shutup = regs[0];
    switch( exc ) {
        case 0x280:     // IRQ
        // gic_clear ();
        handle_irq ();
        return;

        case 0x400:
        /* Synch from lower level */
        if( class == 0x17 ) {   // smc
            if( syndrome == 0 ) {
            // psciHandler(regs);
            return;
            }
        }
    }

    printf("\n--- PANIC! exception 0x%x on CPU%d EL %d, class=0x%x syndrome=0x%x\n",
            exc, GetCPUID(), GetCurrentSMode(), class, syndrome );

    boardReset();
#endif
}


/* THE END */
