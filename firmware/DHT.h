#ifndef _DHT_H_
#define _DHT_H_ 1

#include <avr/io.h>
#include <util/delay.h>

////////////////////////////////////////////////////////////////////////////////
// Макросы
////////////////////////////////////////////////////////////////////////////////

// Макросы для определения DDRx, PORTx, PINx
#define DDR_DHT    __CONCAT(DDR,  DHT_PORTNAME)
#define PORT_DHT   __CONCAT(PORT, DHT_PORTNAME)
#define PIN_DHT    __CONCAT(PIN,  DHT_PORTNAME)

// Инициализация датчика: подтяжка к питанию внутренним резистором
#define DHT_INIT() do { PORT_DHT |= (1<<DHT_WIRE); } while (0)

// Посылка в датчик запросного импульса для получения в ответ измерений
#define DHT_START() do {         \
	DDR_DHT |= (1<<DHT_WIRE);     \
    PORT_DHT &=~ (1<<DHT_WIRE);   \
	_delay_ms(20);	             \
    PORT_DHT |= (1<<DHT_WIRE);    \
	DDR_DHT &=~ (1<<DHT_WIRE);    \
} while (0)


////////////////////////////////////////////////////////////////////////////////
// Функции
////////////////////////////////////////////////////////////////////////////////

/*
inline static uint8_t readBit(void)
{
    uint8_t counter = 0;
    uint8_t counterLo = 0;

    while(PIN_DHT&(1<<DHT_WIRE)) {
        counter++;
        if (counter == 255)
            break;
    }

    while((PIN_DHT&(1<<DHT_WIRE))==0) {
        counterLo++;
        if (counterLo == 255)
            break;
    }

    return counter;
}

#define _COUNT 80

volatile static uint8_t tx_buffer[6] = {0};

// tx_buffer[4] - checksum
// tx_buffer[3] - Temp_Lo
// tx_buffer[2] - Temp_Hi
// tx_buffer[1] - RH_Lo
// tx_buffer[0] - RH_Hi
inline static void measureDHT(void) {
    // now pull it low for ~20 milliseconds
    DHT_START();
    _delay_us(40);
    
    readBit();
    readBit();
    
    // read in timings
    uint8_t i;
    for (i=0; i < 5; i++) {
        uint8_t b = 0;

        tx_buffer[i] = 0;

        uint8_t j;
        for(j=0; j<8; j++)
        {
            uint8_t counter = readBit();
            b <<= 1;
            if (counter > _COUNT)
                b |= 1;
        }

        tx_buffer[i] = b;
    }
}
*/

#define DHT11_OK		0xFF
#define DHT11_ERROR		0x00

uint8_t DHT11_readByte(void) {
	uint8_t DHT11_counter = 0;
	uint8_t DHT11_data = 0x00;	
	DDR_DHT &=~ (1<<DHT_WIRE);
	for(int DHT11_i = 7; DHT11_i >= 0; DHT11_i--) {
		DHT11_counter = 0;
		while(!(PIN_DHT & (1<<DHT_WIRE)) && (DHT11_counter < 10)) {
			_delay_us(10);
			DHT11_counter++;
		}
		DHT11_counter = 0;
		while((PIN_DHT & (1<<DHT_WIRE)) && (DHT11_counter < 15)) {
			_delay_us(10);
			DHT11_counter++;
		}
		if(DHT11_counter > 5) {
			DHT11_data += (1<<DHT11_i);
		}
	}
	return DHT11_data;
}

inline uint8_t DHT11_getData(int8_t* temperature, uint8_t* humidity) {

	//Start signal
    //DHT_START();
   
	//_delay_us(60);
	//if(PIN_DHT & (1<<DHT_WIRE)) {
	//	return DHT11_ERROR;
	//}
    
	//while(!(PIN_DHT & (1<<DHT_WIRE)));
	//while(PIN_DHT & (1<<DHT_WIRE));
	
	//Data bytes
	uint8_t DHT11_RH_integral = DHT11_readByte();
	DHT11_readByte();
	uint8_t DHT11_T_integral  = DHT11_readByte();
	DHT11_readByte();
	DHT11_readByte();
	
    DHT11_RH_integral = 44;
    
	*humidity     = DHT11_RH_integral/10+'0';
    *(humidity+1) = DHT11_RH_integral%10+'0';
    
    DHT11_T_integral = 22;
    
	*temperature = DHT11_T_integral/10+'0';
    *(temperature+1) = DHT11_T_integral%10+'0';

		
	return DHT11_OK;
}


#endif  /* _DHT_H_ */