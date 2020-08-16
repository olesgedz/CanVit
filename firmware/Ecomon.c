/*
    Система экологического мониторинга 0.1
    Разработчик: инженер Бояршинова В.Д. (НИО-8307)
    e-mail: tory2008.97@mail.ru
    Разработано: VIII 2020 г.

    MCU: ATmega328 freq.: 8 MHz
    FUSE: HI 0xD9, LO:0xE2
    Internal Calibrated RC Oscillator 8 МГц без делителя    
    
    Описание: комплекс метеозондирования для оценки окружающей среды и выявления техногенных факторов.
    Режим работы: система выполняет мониторинг температуры, влажности, давления, частиц СО, 
		  PM2.5 (пыль и пыльца), GPS параметров (дата и время, широта, долгота, скорость и 
                  направление движения), производит запись полученных данных на SD карту,
                  а также их передачу по радиоканалу.
     
    Примеры записей: 

   № ;  t  ;  P  ;RH; CO ;PM2.5;  date  ;  time  ;   Latitude ;   Longitude ; speed;course;V;\n
00000; 00,0;000,0;00;0000;000,0;00.00.00;00:00:00;00°00.0000,N;000°00.0000,E;000.00;000.00;A__
00001;-10,5;   !!;10;0030;035,8;13.08.19;13:15:20;22°32.1843,N;137°01.1905,E;020.15;180.28;A__
00002;   ++;541,5;!!;  ++;   ++;13.08.19;13:16:20;  °       !!;050°01.1906,E;130.89;360.00;A__
00003;   !!;   ++;++;1850;500,0;26.10.19;08:45:00;22°32.1745,N;114°01.1920,E;    ++;015.26;G__
00004;!!!,!;!!!,!;!!;!!!!;!!!,!;!!.!!.!!;!!:!!:!!;!!°!!.!!!!,!;!!!°!!.!!!!,!;!!!.!!;!!!.!!;A__

     6     6     6  3    5     6        9        9          13           14      7      7 1 2 = 92
*/


//TODO:
// 1. Разобраться с модулями + вынести SPI
// 3. Переписать измерение влажности и отладить в железе
// 4. Чисто прикола ради в пример вбить координаты изместных мест (Кремль, Эйфелева башня и т.д.)

////////////////////////////////////////////////////////////////////////////////
// Заголовочные файлы
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>       // AVR device-specific IO definitions
#include <avr/pgmspace.h> // Program Space Utilities

#include "config.h"       // Настройки программы, собранные в одном месте
#include "dht.h"
#include "sdcard.h"
#include "i2c.h"
#include "bmp180.h"

////////////////////////////////////////////////////////////////////////////////
// Глобальные переменные
////////////////////////////////////////////////////////////////////////////////

PROGMEM const uint8_t EMPTY_NOTE[NOTE_SIZE] = EMPTY_NOTE_TEMPLATE;

// счетчик номера записи
uint16_t notes_counter = 0;

////////////////////////////////////////////////////////////////////////////////
// Инициализация SPI
////////////////////////////////////////////////////////////////////////////////

inline static void init_SPI(void) {
    SPI_DDRX  |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
    SPI_PORTX |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS)|(1<<SPI_MISO)|(1<<2);
   
    // разрешение SPI, старший бит вперед, мастер, режим 0
    SPCR = (1<<SPE)|(1<<MSTR);
    SPSR = (1<<SPI2X);    
}

////////////////////////////////////////////////////////////////////////////////
// Точка входа
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    init_SPI();
    DHT_INIT();

    Bmp180CalibrationData calibrationData;
    uint8_t result = bmp180ReadCalibrationData(&calibrationData);
    while(1) {
        _delay_ms(2000);
        notes_counter++;
    
        uint8_t note[NOTE_SIZE];
        
        for (uint8_t i=0; i<NOTE_SIZE; i++)
            note[i] = pgm_read_byte(EMPTY_NOTE+i);
        
        // Номер записи
        for (uint16_t i=0, divider = 10000, temp = notes_counter; i<5; i++, temp %= divider, divider /= 10)
            note[i] = temp/divider+'0';
        
        //Rh & t  
        DHT11_getData(note+9, note+18);
        if (result == BMP180_OK)
        {
            Bmp180Data bmp180Data;
            result = bmp180ReadData(BMP180_OSS_STANDARD, &bmp180Data, &calibrationData);
            if (result == BMP180_OK)
            {
                float temperature = bmp180Data.temperatureC;
              //  *(note + 9) = (uint8_t) temperature;
                //int t = (int)(temperature * 10);
                
               // note[14] = (temperature - t) * 10 + ;
                long pressure  = bmp180Data.pressurePa;
                int p = (int)pressure;
                note[14] = (pressure % 10) + '0';
                pressure /= 10;
                note[13] = (pressure % 10) + '0';
                pressure /= 10;
                note[12] = (pressure % 10) + '0';
                pressure /= 10;
                note[11] = (pressure % 10) + '0';
            }
        }
       

        // Запись на SD карту
        write_note(note);

     
    }
}