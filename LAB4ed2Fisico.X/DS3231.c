//Luis Pedro Gonzalez 

#include "DS3231.h"

// Funci�n para convertir BCD a binario
uint8_t BCDtoBinary(uint8_t val) {
    return (val & 0x0F) + ((val >> 4) * 10);
}

// Inicializa DS3231
void DS3231_Init(void) {
    // Inicializar el I2C en modo maestro
    I2C_Master_Init(100000);  // 100 kHz es un valor com�n para I2C
    // Si requieres alguna configuraci�n espec�fica para el DS3231, realiza aqu�
}

uint8_t DS3231_ReadSeconds(void) {
    uint8_t secondsBCD;
    
    I2C_Master_Start(); // Comienza la comunicaci�n I2C
    I2C_Master_Write(DS3231_WRITE_ADDR); // Direcci�n + Write
    I2C_Master_Write(0x00); // Direcci�n del registro de segundos- direccion de los minutos en datsheet
    I2C_Master_RepeatedStart(); // Reinicia la comunicaci�n para cambiar a modo de lectura
    I2C_Master_Write(DS3231_READ_ADDR);  // Escribe la direcci�n del DS3231 en modo lectura
    secondsBCD = I2C_Master_Read(0); // Leer segundos
    I2C_Master_Stop();
    
    return BCDtoBinary(secondsBCD); // Convertir a binario y retornar
}