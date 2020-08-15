#ifndef _SDCARD_H_
#define _SDCARD_H_ 1

#include <avr/eeprom.h>  // EEPROM handling
#include "diskio.h"      // Low level disk interface module
#include "pff.h"         // Petit FatFs - FAT file system module

EEMEM uint32_t EADDR;    // ����� ������ � �����

////////////////////////////////////////////////////////////////////////////////
// �������
////////////////////////////////////////////////////////////////////////////////

// ������ �� ����� ������ ����� ������
// note - ��������� �� ������������ ������
inline static void write_note(const uint8_t* note) {

    FATFS fs;             // ���������� ������� FATFS
    uint8_t buff[512];    // �����
    uint16_t br;          // ������� ����
    uint32_t addr;        // ����� ������ � �����

    // ��������� ����
    if (pf_mount(&fs) == FR_OK) {
    
        // ��������� ���� FILENAME, ������� � ����� �����
        if(pf_open(FILENAME) == FR_OK) {
        
            // ������������� ��������� �� ������� ������ 
            pf_lseek(0);
            
            uint8_t b = 0;

            // ������ ������ ����
            pf_read(&b, 1, &br);

            addr  = eeprom_read_dword(&EADDR);
            
            if ((addr >= MAXADDR) || (b == ' ') || !b) {
                addr = 0;
            }
        
            // ������������� ��������� �� ������ ������ 
            pf_lseek(addr & 0xFFFFFE00);
                
            if(pf_read(buff, sizeof(buff), &br) == FR_OK) {
                uint16_t locaddr = (uint16_t)(addr & 0x1FF);
        
                for (uint8_t i=0; i<NOTE_SIZE; i++)
                    buff[locaddr+i] = note[i];

                // ������������� ��������� �� ������ ������ 
                pf_lseek(addr & 0xFFFFFE00);                
                
                // � ����� ���������� ��� �� �����
                pf_write(buff, sizeof(buff), &br);

                // ������������ ������
                pf_write(0, 0, &br);

                // ����������� ����� � ��������� ��� � EEPROM
                addr += NOTE_SIZE;
                eeprom_write_dword(&EADDR, addr);
            }
        }
        
        // ����������� ����, ������� ������� ������� ���������
        pf_mount(NULL);
    }
    else {
        // �� ������� ������������ ����
    }
}

#endif  /* _SDCARD_H_ */