/*-----------------------------------------------------------------------
/  PFF - Low level disk interface modlue include file    (C)ChaN, 2009
/-----------------------------------------------------------------------*/

#ifndef _DISKIO

#include <avr/io.h>
#include <util/delay.h>

/*_______________ макросы ____________________*/

/*передать байт данных по SPI*/
#define SPI_WriteByte_m(data)  do{ SPDR = data; while(!(SPSR & (1<<SPIF))); } while(0)

/*прочитать байт данных по SPI*/
#define SPI_ReadByte_m(data)  do{ SPDR = 0xff; while(!(SPSR & (1<<SPIF))); data = SPDR;} while(0)

/* ______________ встраиваемые функции _____________*/

/*получить байт данных по SPI*/
inline static uint8_t SPI_ReadByte_i(void) {
   SPDR = 0xff;
   while(!(SPSR & (1<<SPIF)));
   
   return SPDR;   
}


/* CS = L */
#define SELECT()	do{SPI_PORTX &= ~(1<<(SPI_SS)); }while(0)
/* CS = H */
#define DESELECT()	do{SPI_PORTX |= (1<<(SPI_SS)); }while(0)
/* CS status (true:CS == L) */
#define MMC_SEL	    (!(SPI_PORTX & (1<<(SPI_SS))))


/* Status of Disk Functions */
typedef uint8_t DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (uint8_t*, uint32_t, uint16_t, uint16_t);
DRESULT disk_writep (const uint8_t*, uint32_t);

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */

/* Card type flags (CardType) */
#define CT_MMC				0x01	/* MMC ver 3 */
#define CT_SD1				0x02	/* SD ver 1 */
#define CT_SD2				0x04	/* SD ver 2 */
#define CT_SDC				(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK			0x08	/* Block addressing */

#define _DISKIO
#endif
