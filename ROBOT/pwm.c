/*
void PWM_Duty(int Pwm1, int Pwm2)
{
    
    OC1RS = Pwm2; // Write new duty cycle
    OC2RS = Pwm1; // Write new duty cycle
}

void blink ()
{
    unsigned int t;
    //PWM_Duty(24000,24000);
    while(1)
    {
        LATAbits.LATA4 = 0;
        LATAbits.LATA0 = 1;
        LATBbits.LATB2 = 1;
        t = timer_start();
        while (!timer_timeout(t, 10000000));
        LATAbits.LATA4 = 1;
        LATAbits.LATA0 = 0;
        LATBbits.LATB2 = 0;
        t = timer_start();
        while (!timer_timeout(t, 10000000));
    }   
}

void start_PWM()
{
    RPB7Rbits.RPB7R = 0b0101;
    RPB8Rbits.RPB8R = 0b0101;
    OC1CON = 0x0000; // Turn off OC1 while doing setup.
    OC1R = 0x0000; // Initialize primary Compare Register
    OC1RS = 0x0000; // Initialize secondary Compare Register
    OC1CON = 0x0006; // Configure for PWM mode


    OC2CON = 0x0000; // Turn off OC3 while doing setup.
    OC2R = 0x0000; // Initialize primary Compare Register
    OC2RS = 0x0000; // Initialize secondary Compare Register
    OC2CON = 0x0006; // Configure for PWM mode

    PR2 = 2000; // Set period 24000

    T2CONSET = 0x8000; // Enable Timer2
    OC1CONSET = 0x8000; // Enable OC1
    OC2CONSET = 0x8000; // Enable OC3
}
 */
