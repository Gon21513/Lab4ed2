//Luis Pedro Gonzalez 21513

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

// --------------- Setup del ADC --------------- 
void adc_init(int channel);
int read_ADC();
void adc_change_channel(int channel);
int adc_get_channel();
int map_adc_volt(int value, int inputmin, int inputmax, int outmin, int outmax); // Funcion para mappear el adc a voltaje 

#endif	/* ADC__H */

