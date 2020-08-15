#ifndef _SDCARD_H_
#define _SDCARD_H_ 1

#include <avr/eeprom.h>  // EEPROM handling
#include "diskio.h"      // Low level disk interface module
#include "pff.h"         // Petit FatFs - FAT file system module

EEMEM uint32_t EADDR;    // адрес записи в файле

////////////////////////////////////////////////////////////////////////////////
// Функции
////////////////////////////////////////////////////////////////////////////////

// Запись на карту памяти одной строки
// note - указатель на записываемую строку
inline static void write_note(const uint8_t* note) {

    FATFS fs;             // объявление объекта FATFS
    uint8_t buff[512];    // буфер
    uint16_t br;          // счетчик байт
    uint32_t addr;        // адрес записи в файле

    // монтируем диск
    if (pf_mount(&fs) == FR_OK) {
    
        // открываем файл FILENAME, лежащий в корне диска
        if(pf_open(FILENAME) == FR_OK) {
        
            // устанавливаем указатель на нулевой сектор 
            pf_lseek(0);
            
            uint8_t b = 0;

            // читаем первый байт
            pf_read(&b, 1, &br);

            addr  = eeprom_read_dword(&EADDR);
            
            if ((addr >= MAXADDR) || (b == ' ') || !b) {
                addr = 0;
            }
        
            // устанавливаем указатель на нужный сектор 
            pf_lseek(addr & 0xFFFFFE00);
                
            if(pf_read(buff, sizeof(buff), &br) == FR_OK) {
                uint16_t locaddr = (uint16_t)(addr & 0x1FF);
        
                for (uint8_t i=0; i<NOTE_SIZE; i++)
                    buff[locaddr+i] = note[i];

                // устанавливаем указатель на нужный сектор 
                pf_lseek(addr & 0xFFFFFE00);                
                
                // а здесь записываем его на карту
                pf_write(buff, sizeof(buff), &br);

                // финализируем запись
                pf_write(0, 0, &br);

                // увеличиваем адрес и сохраняем его в EEPROM
                addr += NOTE_SIZE;
                eeprom_write_dword(&EADDR, addr);
            }
        }
        
        // демонтируем диск, передав функции нулевой указатель
        pf_mount(NULL);
    }
    else {
        // не удалось смонтировать диск
    }
}

#endif  /* _SDCARD_H_ */