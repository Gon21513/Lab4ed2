//slave

/*
 * File:   labed2slave.c
 * Author: Luis Pedro Gonzalez 21513
 *
 * Created on 30 de julio de 2023, 04:50 PM
 */
//*****************************************************************************
// Palabra de configuraci?n
//*****************************************************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definici?n e importaci?n de librer?as
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "ADC.h"
#include <stdio.h>
#include <pic16f887.h>
#include<pic.h>
#include "I2C.h"
//*****************************************************************************
// Definici?n de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
uint8_t dato;
uint8_t numadc; //variable para el valor del adc
int ADC; //valor del adc 
//*****************************************************************************
// Definici?n de funciones para que se puedan colocar despu?s del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);

//*****************************************************************************
// C?digo de Interrupci?n 
//*****************************************************************************
void __interrupt() isr(void){
    
    
//    // ---- INTERRUPCION DEL ADC --------
    if (PIR1bits.ADIF == 1){ // Chequear bandera del conversor ADC
        
        numadc = read_ADC();
        //numadc = ADC; // pasar valor del adc a num1
        PIR1bits.ADIF = 0; // Borrar el indicador del conversor ADC

    }

   if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        
        //recepcion de datos
//        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
//            //__delay_us(7);
//            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
//            //__delay_us(2);
//            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupci?n recepci?n/transmisi?n SSP
//            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
//            while(!SSPSTATbits.BF);     // Esperar a que la recepci?n se complete
//            //PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepci?n
//            __delay_us(250);
//            
//        }
        
        //envio de datos
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = numadc;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    adc_init(0);//CONFIGURAR EL CANAL 0 DEL adc 

    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
        
                //verifica la conversion adc
        if (ADCON0bits.GO == 0){
            ADCON0bits.GO = 1;
            __delay_ms(50);
        }
      // __delay_ms(500);
    }
    return;
}
//*****************************************************************************
// Funci?n de Inicializaci?n
//*****************************************************************************
void setup(void){
    //ANSEL = 0; // Solo AN0 es analógico, el resto son digitales
    ANSELH = 0;
    
    //TRISA = 1;
    ANSELbits.ANS0 = 1;
    TRISAbits.TRISA0 = 1;
    
    TRISB = 0;
    TRISD = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    //PORTC = 0;
    
    I2C_Slave_Init(0x50);   

    
    //// --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b111; // 8 MHz
    OSCCONbits.SCS = 1; // Seleccionar oscilador interno
    
    //INTCONbits.GIE = 1;         // Habilitamos interrupciones
    //INTCONbits.GIE = 1;
    //INTCONbits.PEIE = 1;
    
    
}