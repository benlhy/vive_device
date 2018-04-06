#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include "../timer/timer.h"
#include <p32xxxx.h>

//VIVE
#define CYCLE 195000.0
#define RANGE 120.0

#define NOISEW 2500
#define SYNCW 190000

#define VIVE1 PORTBbits.RB2
#define VIVE2 PORTBbits.RB14
#define VIVE3 PORTAbits.RA1
#define VIVE4 PORTBbits.RB7

#define DEBUG1 LATAbits.LATA4
#define DEBUG2 LATBbits.LATB4
#define DEBUG3 LATBbits.LATB15
#define DEBUG4 LATBbits.LATB8

#define WAITING_X_FLASH 0
#define WAITING_X_PULSE 1
#define WAITING_Y_FLASH 2
#define WAITING_Y_PULSE 3
#define WAITING_E_FLASH 4
#define WAITING_S_FLASH 5
#define DONE 4
#define SENSOR_COUNT 4
#define SENSOR_CYCLE 6

#define Difference(a,b) a>b?a-b:b-a

#define bool unsigned char
#define true 1
#define false 0

bool dataready=false;
char buf[255];

// structure to store the sensor data
typedef struct {
  unsigned int x;
  unsigned int y;
} sensor_value;

sensor_value sensor_values[SENSOR_COUNT];
sensor_value old_sensor_values[SENSOR_COUNT];
bool sensor_ready[SENSOR_COUNT];
bool new_data = false;
unsigned int lf[4];
unsigned int sc[4];

unsigned int process_interrupt(int sensor_ix, unsigned int ts, unsigned int pts, unsigned int *state)
{
    unsigned int value = timer_diff(pts,ts);
    int i;
    if (value <NOISEW) return false; //noise
    
    if (value > SYNCW) //sync flash
    {
        if (WAITING_S_FLASH && new_data)
        {
            new_data=false;
            sprintf(buf, "{%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u}\n",
                    sensor_values[0].x*3/5,sensor_values[0].y*3/5,
                    sensor_values[1].x*3/5,sensor_values[1].y*3/5,
                    sensor_values[2].x*3/5,sensor_values[2].y*3/5,
                    sensor_values[3].x*3/5,sensor_values[3].y*3/5,
                    sensor_ready[0]+sensor_ready[1]*2+sensor_ready[2]*4+sensor_ready[3]*8,
                    sc[0],sc[1],sc[2],sc[3]);
            SerialTransmit(buf);
            sensor_ready[0]=sensor_ready[1]=sensor_ready[2]=sensor_ready[3]=false;
            for (i=0;i<SENSOR_COUNT;i++)
            {
                old_sensor_values[i]=sensor_values[i];
            }
        }
        *state = WAITING_X_PULSE;
        sc[sensor_ix]=0;
        lf[sensor_ix]=ts;
    }
    else
    {
        switch (*state)
        {
            case WAITING_X_PULSE:
            {
                *state = WAITING_Y_FLASH;
                sensor_values[sensor_ix].x = value;
                break;
            }
            case WAITING_Y_FLASH:
            {
                unsigned int tslf = timer_diff(lf[sensor_ix],ts);
                if (tslf > SYNCW) //sync flash
                {
                    lf[sensor_ix]=ts;
                    *state = WAITING_Y_PULSE;
                }else
                {
                    if (Difference(tslf,old_sensor_values[sensor_ix].x)<Difference(sensor_values[sensor_ix].x,old_sensor_values[sensor_ix].x))
                    {
                        sensor_values[sensor_ix].x=tslf;
                    }
                    sc[sensor_ix]++;
                }
                break;
            }
            case WAITING_Y_PULSE:
            {
                *state = WAITING_E_FLASH;
                sensor_values[sensor_ix].y = value;
                new_data=sensor_ready[sensor_ix]=true;
                break;
            }
            case WAITING_E_FLASH:
            {
                unsigned int tslf = timer_diff(lf[sensor_ix],ts);
                if (tslf > SYNCW) //sync flash
                {
                    lf[sensor_ix]=ts;
                    *state = WAITING_S_FLASH;
                }else
                {
                    if (Difference(tslf,old_sensor_values[sensor_ix].y)<Difference(sensor_values[sensor_ix].y,old_sensor_values[sensor_ix].y))
                    {
                        sensor_values[sensor_ix].y=tslf;
                    }
                    sc[sensor_ix]++;
                }
                break;
            }
        }
    }
    return true;
}

void enable_all()
{
    IEC0bits.IC1IE = 1; // step 6: enable INT0 by setting IEC0<3>    
    IEC0bits.IC2IE = 1; // step 6: enable INT0 by setting IEC0<3>    
    IEC0bits.IC3IE = 1; // step 6: enable INT0 by setting IEC0<3>    
    IEC0bits.IC4IE = 1; // step 6: enable INT0 by setting IEC0<3>   
}

void __ISR(_INPUT_CAPTURE_1_VECTOR, IPL5SOFT) IC1ISR(void) 
{
    int unused = IC1BUF;
    unused = IC1BUF;
    unused = IC1BUF;
    unused = IC1BUF;
    static unsigned int ts = 0; 
    static unsigned int pts = 0; 
    static unsigned int state = WAITING_X_FLASH;
    bool v = VIVE1;
    DEBUG1 = v;
    if (!v)
    { 
        ts = _CP0_GET_COUNT();// first store time
        if (process_interrupt(0,ts, pts, &state))
        {
            pts = ts;        
        }
    }
    IFS0bits.IC1IF = 0; // step 5: clear the int flag
}

void __ISR(_INPUT_CAPTURE_2_VECTOR, IPL5SOFT) IC2ISR(void) 
{
    int unused = IC2BUF;
    unused = IC2BUF;
    unused = IC2BUF;
    unused = IC2BUF;
    static unsigned int ts = 0; 
    static unsigned int pts = 0; 
    static unsigned int state = WAITING_X_FLASH;
    bool v = VIVE2;
    DEBUG2 = v;
    if (!v)
    { 
        ts = _CP0_GET_COUNT();// first store time
        if (process_interrupt(1,ts, pts, &state))
        {
            pts = ts;        
        }
    }
    IFS0bits.IC2IF = 0; // step 5: clear the int flag
}

void __ISR(_INPUT_CAPTURE_3_VECTOR, IPL5SOFT) IC3ISR(void) 
{
    int unused = IC3BUF;
    unused = IC3BUF;
    unused = IC3BUF;
    unused = IC3BUF;
    static unsigned int ts = 0; 
    static unsigned int pts = 0; 
    static unsigned int state = WAITING_X_FLASH;
    bool v = VIVE3;
    DEBUG3 = v;
    if (!v)
    { 
        ts = _CP0_GET_COUNT();// first store time
        if (process_interrupt(2,ts, pts, &state))
        {
            pts = ts;        
        }
    }
    IFS0bits.IC3IF = 0; // step 5: clear the int flag
}

void __ISR(_INPUT_CAPTURE_4_VECTOR, IPL5SOFT) IC4ISR(void) 
{
    int unused = IC4BUF;
    unused = IC4BUF;
    unused = IC4BUF;
    unused = IC4BUF;
    static unsigned int ts = 0; 
    static unsigned int pts = 0; 
    static unsigned int state = WAITING_X_FLASH;
    bool v = VIVE4;
    DEBUG4 = v;
    if (!v)
    { 
        ts = _CP0_GET_COUNT();// first store time
        if (process_interrupt(3,ts, pts, &state))
        {
            pts = ts;        
        }
    }
    IFS0bits.IC4IF = 0; // step 5: clear the int flag
}

void init_vive()
{
    // initialize the sensor variables
    //V1.prevMic = 0;
    TRISAbits.TRISA4 = 0;  // A4 debug 
    TRISBbits.TRISB4 = 0;  // B4 debug 
    TRISBbits.TRISB15 = 0; // B15 debug 
    TRISBbits.TRISB8 = 0;  // B8 debug 

    TRISBbits.TRISB2  = 1; // connect the TS3633 ENV pin to B2
    TRISBbits.TRISB14 = 1; // connect the TS3633 ENV pin to B14
    TRISAbits.TRISA1  = 1; // connect the TS3633 ENV pin to A1
    TRISBbits.TRISB7  = 1; // connect the TS3633 ENV pin to B7

    INTCONbits.MVEC = 0x1;// enable multi-vector mode

    IC1Rbits.IC1R = 0b0100; // B2 is IC2 (input capture 3)
    IC1CONbits.ICM = 0b01; // detect falling edges
    IC1CONbits.ICI = 0; // interrupt on an edge
    IC1CONbits.ICTMR = 1; // store the value of timer2, but we're actually just using the interrupt and ignoring timer2
    IC1CONbits.FEDGE = 0; // first event is falling edge, doesn't really matter
    IC1CONbits.ON = 1;
    IPC1bits.IC1IP = 1; // step 4: interrupt priority 5
    IPC1bits.IC1IS = 0; // step 4: interrupt priority 1
    IFS0bits.IC1IF = 0; // step 5: clear the int flag
    IEC0bits.IC1IE = 1; // step 6: enable INT0 by setting IEC0<3>

 
    IC2Rbits.IC2R = 0b0001; // B14 is IC2 (input capture 3)
    IC2CONbits.ICM = 0b01; // detect falling edges
    IC2CONbits.ICI = 0; // interrupt on an edge
    IC2CONbits.ICTMR = 1; // store the value of timer2, but we're actually just using the interrupt and ignoring timer2
    IC2CONbits.FEDGE = 0; // first event is falling edge, doesn't really matter
    IC2CONbits.ON = 1;
    IPC2bits.IC2IP = 1; // step 4: interrupt priority 5
    IPC2bits.IC2IS = 0; // step 4: interrupt priority 1
    IFS0bits.IC2IF = 0; // step 5: clear the int flag
    IEC0bits.IC2IE = 1; // step 6: enable INT0 by setting IEC0<3>

    IC3Rbits.IC3R = 0b0000; // A1 is IC3 (input capture 3)
    IC3CONbits.ICM = 0b010; // detect falling edges
    IC3CONbits.ICI = 0; // interrupt on an edge
    IC3CONbits.ICTMR = 1; // store the value of timer2, but we're actually just using the interrupt and ignoring timer2
    IC3CONbits.FEDGE = 0; // first event is falling edge, doesn't really matter
    IC3CONbits.ON = 1;
    IPC3bits.IC3IP = 5; // step 4: interrupt priority 5
    IPC3bits.IC3IS = 1; // step 4: interrupt priority 1
    IFS0bits.IC3IF = 0; // step 5: clear the int flag
    IEC0bits.IC3IE = 1; // step 6: enable INT0 by setting IEC0<3>    

    IC4Rbits.IC4R = 0b0100; // B7 is IC4 (input capture 4)
    IC4CONbits.ICM = 0b010; // detect falling edges
    IC4CONbits.ICI = 0; // interrupt on an edge
    IC4CONbits.ICTMR = 1; // store the value of timer2, but we're actually just using the interrupt and ignoring timer2
    IC4CONbits.FEDGE = 0; // first event is falling edge, doesn't really matter
    IC4CONbits.ON = 1;
    IPC4bits.IC4IP = 5; // step 4: interrupt priority 5
    IPC4bits.IC4IS = 1; // step 4: interrupt priority 1
    IFS0bits.IC4IF = 0; // step 5: clear the int flag
    IEC0bits.IC4IE = 1; // step 6: enable INT0 by setting IEC0<3>    
}
    

/*
 
 
void __ISR(_INPUT_CAPTURE_2_VECTOR, IPL5SOFT) IC2ISR(void) 
{
    LATBbits.LATB4 = PORTBbits.RB14;
    unsigned int ts = 0, pts = 0, state = WAITING_X_FLASH;
    ts = _CP0_GET_COUNT();// first store time
    state = process_interrupt(1,ts, pts, state);
    pts = ts;
}

void __ISR(_INPUT_CAPTURE_3_VECTOR, IPL5SOFT) IC3ISR(void) 
{
    LATBbits.LATB15 = PORTAbits.RA1;
    unsigned int ts = 0, pts = 0, state = WAITING_X_FLASH;
    ts = _CP0_GET_COUNT();// first store time
    state = process_interrupt(2,ts, pts, state);
    pts = ts;
}

void __ISR(_INPUT_CAPTURE_4_VECTOR, IPL5SOFT) IC4ISR(void) 
{
    LATBbits.LATB8 = PORTBbits.RB7;
    unsigned int ts = 0, pts = 0, state = WAITING_X_FLASH;
    ts = _CP0_GET_COUNT();// first store time
    state = process_interrupt(3,ts, pts, state);
    pts = ts;
}

bool read_vive(sensor_value outvalues[SENSOR_COUNT])
{
    int i;
    bool r=true;
    for (i=0; i<SENSOR_COUNT; i++)
    {
        outvalues[i] = sensor_values[i];
        r = r * sensor_ready[i];
    }
    if (r) for (i=0; i<SENSOR_COUNT; i++) sensor_ready[i] = false;
    return r;
}


 */

