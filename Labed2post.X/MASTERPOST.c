/* 
 * File:   lab.c
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
unsigned char voltaje; //variable para almacenar voltaje
char buffer[20]; //buffer para LCD


//variables para las fechas y tiempo
uint8_t selector = 0; //Variable para selector de modo
uint8_t bandera = 0; //Variable para antirrebotes
uint8_t segundo; //variable de segundos
uint8_t minuto; //variable de minutos
uint8_t hora; //variable de horas
uint8_t dow; //variable de día de semana
uint8_t dia; //variable de día
uint8_t mes; //variable de mes
uint8_t ano; //variable de año
uint8_t temporal; //variable temporal

//*****************************************************************************
// Definici?n de funciones para que se puedan colocar despu?s del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
void adcslave(void); //función del Slave_1 ADC
void select(void);
void Set_sec(uint8_t sec); //Función para setear segundos
void Set_min(uint8_t min); //Función para setear minutos
void Set_hour(uint8_t hour); //Función para setear horas
void Set_day_week(uint8_t day_week); //Función para setear día de semana
void Set_day(uint8_t day); //Función para setear día
void Set_month(uint8_t month); //Función para setear mes
void Set_year(uint8_t year); //Función para setear año
uint8_t Read(uint8_t address); //Función para setear minutos
void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h); //Función para obtener valores de tiempo
void Read_Fecha(uint8_t *d, uint8_t *mo, uint8_t *y); //Función para obtener valores de fecha
uint8_t Dec_to_Bcd(uint8_t dec_number); //Función para pasar de decimal a bcd
uint8_t Bcd_to_Dec(uint8_t bcd); //Función para pasar de bcd a decimal
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    Lcd_Init();
    Lcd_Clear(); //Limpiar LCD

    //valores inciales para el reloj 
    Lcd_Set_Cursor(1,1); //Ubica el cursor en la posición 1,1
    Lcd_Write_String("ADC:"); //Muestra el texto "S1:"
    Set_sec(0); //Inicializa los segundos a 0
    Set_min(0); //Inicializa los minutos a 0
    Set_hour(22); //Inicializa las horas a 12
    Set_day_week(3); //Determina el día de la semana
    Set_day(8); //Establece el día a 14
    Set_month(8); //Establece el mes a febrero
    Set_year(23); //Establece el año a 23
    
    while(1){
        
        //funcion del selector 
        select();

    }
}


//interrupciones

void __interrupt() isr(void){ //Interrupciones 
    if (INTCONbits.RBIF == 1){ //Chequear interrupciones del puerto B
        INTCONbits.RBIF = 0; //Limpiar bandera
        if (PORTBbits.RB3 == 0){ //Ver si se presiono botón RB3
            __delay_ms(2); //delay de 1 ms
            bandera = 1; //Bandera en 1 
        }
        if (PORTBbits.RB3 == 1 && bandera == 1){ //Esperar a que se suelte botón RBO
            if (selector == 0){ //Chequear el modo
                selector = 1; //Cambiar a 1
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 1){ //Chequear modo
                selector = 2; //Cambiar a modo a 2
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 2){ //Chequear modo
                selector = 3; //Cambiar modo a 3
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 3){ //Chequear modo
                selector = 4; //Cambiar a modo 4
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 4){ //Chequear modo
                selector = 5; //Cambiar modo a 5
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 5){ //Chequear modo
                selector = 6; //Cambiar modo a 6
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 6){ //Chequear modo
                selector = 0; //Cambiar a modo 0
                bandera = 0; //Limpiar bandera
            }
        }
        if (PORTBbits.RB2 == 0){ //Ver si se presionó RB2
            bandera = 2; //Bandera en 2
            __delay_ms(1); //delay de 1ms
        }
        if (PORTBbits.RB2 == 1 && bandera == 2){ //Ver si se dejó de presionar
            if (selector == 1){ //Chequear modo
                if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12){ //Chequear el mes
                    if (dia == 31){ //Si el día es 31 no sumar más
                        dia = 31; //Día igual a 31
                        Set_day(dia); //Setear día
                    }
                    else { //Sino sumar normalmente
                        dia++; //Sumar día
                        Set_day(dia); //Setear día
                    }
                }
                else if (mes == 4 || mes == 6 || mes == 9 || mes == 11){ //Chequear mes si es de 30 días
                    if (dia == 30){ //Si el día es 30 no sumar más
                        dia = 30; //Día en 30
                        Set_day(dia); //Setear día
                    }
                    else { //Sino sumar normalmente
                        dia++; //Sumar día normalmente
                        Set_day(dia); //Setear día
                    }
                }
                else if (mes == 2){ //Chequear si es febrero
                    if (dia == 28){ //Si el día es 28 no sumar más
                        dia = 28; //Día en 28
                        Set_day(dia); //Setear día
                    }
                    else { //Sino sumar normalmente
                        dia++; //Sumar día
                        Set_day(dia); //Setear día
                    }
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 2){ //Chequear modo
                if (mes == 12){ //Ver si el mes es 12
                    mes = 12; //No sumar más 
                    Set_month(mes); //Setear mes
                }
                else { //Sino sumar normalmente
                    mes++; //
                    Set_month(mes);   //Setear mes
                    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && (dia >> 30)){ //Chequear día 
                        Set_day(30); //Setear día 
                    }
                    else if (mes == 2 && dia >> 28){ //Chequear si es febrero y el día es mayor a 28
                        Set_day(28); //Setear día en 28
                    }
                    else {
                        Set_day(dia);
                    }
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 3){ //Chequear modo
                if (ano == 99){ //Ver si año es 99
                    ano = 99; //No sumar más y que se quede en 99
                    Set_year(ano); //Setear año
                }
                else { //Sino sumar normalmente
                    ano++; //Sumar año
                    Set_year(ano); //Setear año
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 4){ //Chequear modo
                if (hora == 23){ //Si hora es 23
                    hora = 23; //Hora en 23
                    Set_hour(hora); //Setear en hora
                }
                else { //Sino sumar normalmente
                    hora++; //Sumar hora
                    Set_hour(hora); //Setear hora
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 5){ //Chequear modo
                if (minuto == 59){ //Si el minuto es 59 no sumar más
                    minuto = 59; //Minuto en 59
                    Set_min(minuto); //Setear minuto
                }
                else { //Sino sumar normalmente
                    minuto++; //Sumar minuto
                    Set_min(minuto); //Setar minuto
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 6){ //Chequear modo
                if (segundo == 59){ //Chequer segundo si es 59
                    segundo = 59; //Segundo en 59
                    Set_sec(segundo); //Setear segundo
                }
                else { //Sino sumar normalmente
                    segundo++; //Sumar segundo
                    Set_sec(segundo); //Setear segundo
                }
                bandera = 0; //Limpiar bandera
            }
        }
        if (PORTBbits.RB1 == 0){ //Ver si se presiono al botón RB1
            bandera = 3; //Bandera en 3
            __delay_ms(1);
        }
        if (PORTBbits.RB1 == 1 && bandera == 3){ //Ver si se presióno el botón
            if (selector == 1){ //Chequear el modo
                if (dia == 1){ //Si día 1 no sumar más 
                    dia = 1; //Día en 1
                    Set_day(dia); //Setear día en 1
                }
                else { //Sino sumar normalmente
                    dia--; //Restar día
                    Set_day(dia); //Setear día
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 2){ //Chequear modo
                if (mes == 1){ //Si el mes es 1 no restar más
                    mes = 1; //Mes en 1
                    Set_month(mes); //Setear en 1
                }
                else { //Sino restar normalmente
                    mes--; //Restar mes
                    Set_month(mes); //Setear mes
                }
                bandera = 0;
            }
            else if (selector == 3){ //Chequear modo
                if (ano == 0){ //Ver si año 0 es 0 para no restar más
                    ano = 0; //Año en 0
                    Set_year(ano); //Setear año
                }
                else { //Sino restar normalmente
                    ano--; //Restar año
                    Set_year(ano); //Setear año
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 4){ //Chequear modo
                if (hora == 0){ //Si hora es 0 no restar más
                    hora = 0; //Hora en 0
                    Set_hour(hora); //Setear hora
                }
                else { //Sino restar normalmente
                    hora--; //Restar hora
                    Set_hour(hora); //Setear hora
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 5){ //Chequear bandera
                if (minuto == 0){ //Ver si minutos es 0 para no restar más
                    minuto = 0; //Minutos en 0
                    Set_min(minuto); //Setear minutos
                }
                else { //Sino no restar normalmente
                    minuto--; //Restar normalmente
                    Set_min(minuto); //Setear minutos
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 6){ //Chequear modo
                if (segundo == 0){ //Ver si segundo es 0
                    segundo = 0; //Segundo en 0
                    Set_sec(segundo); //Setear segundos
                }
                else { //Sino restar normalmente
                    segundo--; //Restar segundos
                    Set_sec(segundo); //Setear segundos
                }
                bandera = 0; //Limpiar bandera
            }
        }
    }
}
//*****************************************************************************
// Funci?n de Inicializaci?n
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    
    TRISB = 0b00001110; //TRISB como entradas y salidas
    TRISD = 0; //Puerto D como salida
    PORTB = 0; //Limpiar Puerto B 
    PORTC = 0; //Limpiar Puerto C
    PORTD = 0; //Limpiar Puerto D
    

    
    //------------CONFIGURACION DEL PORTB---------
    INTCONbits.GIE = 1; //Activar interrupciones globales
    INTCONbits.RBIF = 0; //Limpiar bandera de interrupción del puerto B
    INTCONbits.RBIE = 1; //Activar interrupción del puerto B
    
    OPTION_REGbits.nRBPU = 0; //Pull-ups activos
    IOCBbits.IOCB3 = 1; //Activar interrupt-onchange del pin B3
    IOCBbits.IOCB2 = 1; //Activar interrupt-onchange del pin B2
    IOCBbits.IOCB1 = 1; //Activar interrupt-onchange del pin B1
    
    WPUBbits.WPUB3 = 1; //Activar pull-up del pin B3
    WPUBbits.WPUB2 = 1; //Activar pull-up del pin B2
    WPUBbits.WPUB1 = 1; //Activar pull-up del pin B1 
    
    //INICIAR EL MASTER
    I2C_Master_Init(100000);        // Inicializar Comuncaci?n I2C

    
    //// --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b111; // 8 MHz
    OSCCONbits.SCS = 1; // Seleccionar oscilador interno
}



//lectura del adc del esclavo 
void adcslave(void){ //Función para obtener voltaje
    I2C_Master_Start(); //Iniciar I2C
    I2C_Master_Write(0x51); //Dirección del esclavo y mandar bit para que esclavo mande datos
    voltaje = I2C_Master_Read(0); //Guardar voltaje
    Lcd_Set_Cursor(2,1); //Cursor en 2,1
    sprintf(buffer, "%d ", voltaje); //Función para convertir variables en cadena de texto
    Lcd_Write_String(buffer); //Mostrar en LCD
    I2C_Master_Stop(); //Terminar i2c
    __delay_us(10); //delay
}

//funcion para el selector d modos
void select(void){
    if (selector == 0){ //Chequear el modo
            adcslave(); //Correr para obtener datos del voltaje de esclavo
            Read_Time(&segundo, &minuto, &hora); //Llamar función para obtener valores del tiempo
            Lcd_Set_Cursor(1,7); //Cursor en 1,7
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en la LCD

            Read_Fecha(&dia, &mes, &ano); //Llamar función para obtener valores de la fecha
            Lcd_Set_Cursor(2,7); //Cursor en 2,7
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en la LCD

            Lcd_Set_Cursor(2,5); //Cursor en 2,5
            Lcd_Write_String("R"); //Mostrar R de modo reloj
        }
        else if (selector == 1){ //Chequar modo
            Lcd_Set_Cursor(2,7); //Cursor en 2,7
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrarn en LCD
            Lcd_Set_Cursor(2,5); //Cursor en 2,5
            Lcd_Write_String("D "); //Mostrar D de modo día en LCD
        }
        else if (selector == 2){
            Lcd_Set_Cursor(2,7); //Cursor en 2,7
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar el LCD
            Lcd_Set_Cursor(2,5); //Cursor en 2,5
            Lcd_Write_String("M "); //Mostrar M de modo mes en LCD
        }
        else if (selector == 3){ //Chequear modo
            Lcd_Set_Cursor(2,7); //Cursor en 2,7
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(2,5); //Cursor en 2,5
            Lcd_Write_String("Y "); //Mostrar Y de modo year en LCD
        }
        else if (selector == 4){ //Chequear modo
            Lcd_Set_Cursor(1,7); //Cursor en 1,7
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(2,5); //Cursor en 2,5
            Lcd_Write_String("H "); //Mostrar H de modo hour en LCD
        }
        else if (selector == 5){ //Chequear modo
            Lcd_Set_Cursor(1,7); //Cursor en 1,7
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(2,5); //Cursor en 2,5
            Lcd_Write_String("m "); //Mostrar m de modo minuto en LCD
        }
        else if (selector == 6){ //Chequar modo
            Lcd_Set_Cursor(1,7); //Cursor en 1,7
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(2,5); //Cursor en 2,5
            Lcd_Write_String("S "); //Mostrar S de modo segundo
        }
    }
