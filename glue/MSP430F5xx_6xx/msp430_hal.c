
static void msp430_hal_ports_init(void)
{
    // Turn everything off, device drivers enable what is needed.
    // All configured for digital I/O
#ifdef P1SEL
    P1SEL = 0;
#endif
#ifdef P2SEL
    P2SEL = 0;
#endif
#ifdef P3SEL
    P3SEL = 0;
#endif
#ifdef P4SEL
    P4SEL = 0;
#endif
#ifdef P5SEL
    P5SEL = 0;
#endif
#ifdef P6SEL
    P6SEL = 0;
#endif

    // make all gpios inputs
#ifdef P1DIR
    P1DIR = 0;
    P1OUT = 0;
#endif
#ifdef P2DIR
    P2DIR = 0;
    P2OUT = 0;
#endif
#ifdef P3DIR
    P3DIR = 0;
    P3OUT = 0;
#endif
#ifdef P4DIR
    P4DIR = 0;
    P4OUT = 0;
#endif

#ifdef P5DIR
    P5DIR = 0;
    P5OUT = 0;
#endif

#ifdef P6DIR
    P6DIR = 0;
    P6OUT = 0;
#endif

#ifdef P7DIR
    P7DIR = 0;
    P7OUT = 0;
#endif

#ifdef P8DIR
    P8DIR = 0;
    P8OUT = 0;
#endif
}

void msp430_hal_init(void)
{
    msp430_hal_ports_init();
}
