#ifndef DS3231_H
#define	DS3231_H

#include <xc.h> 
#include <stdint.h>
#include "I2C.h"

// Dirección del DS3231
#define DS3231_WRITE_ADDR 0xD0
#define DS3231_READ_ADDR  0xD1

// Funciones públicas
void DS3231_Init(void);
uint8_t DS3231_ReadSeconds(void);

#endif	/* DS3231_H */
