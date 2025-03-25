#include "Ports.h"

#define WREN  0x06
#define WRDI  0x04
#define RDSR  0x05
#define WRSR  0x01
#define READ  0x03
#define WRITE 0x02

#define ADDR_MSB_MASK   (unsigned short)0xFF00
#define ADDR_LSB_MASK   (unsigned short)0x00FF
#define MSB_ADDR_BYTE(addr)   ((unsigned char)((addr & ADDR_MSB_MASK)>>8))
#define LSB_ADDR_BYTE(addr)   ((unsigned char)(addr & ADDR_LSB_MASK))

void Init_fram();
inline unsigned char Read_fram(unsigned short adr);
inline void Write_fram(unsigned short adr, unsigned char data);