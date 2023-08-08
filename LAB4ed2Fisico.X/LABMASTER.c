/* 
 * File:   prelab3.c
 * Author: Luis Pedro Gonzalez 21513
 *
 * Created on 06 de agosto de 2023, 04:09 PM
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
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>
#include "I2C.h"
#include <xc.h>
#include "DS3231.h"
#include "LCD.h"
//*****************************************************************************
// Definici?n de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000

uint8_t seconds; //segundos del rtc
char sec[3];  // Cadena para almacenar los segundos en formato de texto

//*****************************************************************************
// Definici?n de funciones para que se puedan colocar despu?s del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    Lcd_Init();

    //confgurar posicion del cursor
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Seg:");
    
    while(1){
//        I2C_Master_Start();
//        I2C_Master_Write(0x50);
//        I2C_Master_Write(PORTB);
//        I2C_Master_Stop();
//        __delay_ms(200);
        seconds = DS3231_ReadSeconds();
        Lcd_Set_Cursor(2,1);  // Ajusta el cursor al inicio del LCD
        sprintf(sec, "%02u", seconds);  // Convertir los segundos en formato de dos dígitos
        Lcd_Write_String(sec);  // Muestra los segundos
        __delay_ms(50);  // Espera un segundo antes de actualizar

        
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        PORTA = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(50);
        //PORTB++; 
    }
    return;
}
//*****************************************************************************
// Funci?n de Inicializaci?n
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    TRISB = 0;
    TRISD = 0;
    
    PORTA = 0;
    //PORTC = 0;
    PORTB = 0;
    PORTD = 0;
    
    I2C_Master_Init(100000);        // Inicializar Comuncaci?n I2C

    //// --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b111; // 8 MHz
    OSCCONbits.SCS = 1; // Seleccionar oscilador interno
}