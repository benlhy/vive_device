#include "ROBOT.h"

int main(int argc, char** argv) {
     // do your TRIS and LAT commands here
    ANSELA = 0; // A Analogic off
    ANSELB = 0; // B Analogic off
    
    __builtin_disable_interrupts();
    
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
   
    DDPCONbits.JTAGEN = 0;   

    init_vive(); 
    __builtin_enable_interrupts();

    uart_init();
    
    while (0)
    {
        SerialTransmit("test\n");
    }
   
    while(1){;}
    return 0;
}