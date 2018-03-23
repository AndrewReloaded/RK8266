#include "fw_update.h"

#include "ets.h"
#include "crc16.h"


struct header
{
    char magic[8];
    uint16_t size4k;
    uint16_t reserved1;
    uint16_t reserved2;
    uint16_t crc;
};

static uint8_t hdrbuf[4096];
static struct header *hdr=(struct header*)hdrbuf;
static uint16_t *crcdata=(uint16_t*)(hdrbuf + sizeof(struct header));
static uint8_t buf[4096];


uint8_t check_fw_crc(uint32_t addr)
{
    int i;
    for (i=0; i<hdr->size4k; i++)
    {
	SPIRead(addr+i*4096, buf, 4096);
	if (CRC16(CRC16_INIT, buf, 4096) != crcdata[i])
	{
	    ets_printf("FW Update: CRC error for block %d\n", i);
	    return 0;
	}
    }
    return 1;
}


void fw_update(void)
{
    int i;
    
    // ������ � ��������� ���������
    SPIRead(0x80000, &hdrbuf, 4096);
    if ( (ets_memcmp(hdr->magic, "FWUPDATE", sizeof(hdr->magic))!=0) ||
	 (CRC16(CRC16_INIT, hdrbuf, sizeof(struct header)) != CRC16_OK) )
    {
	ets_printf("FW Update not found\n");
	return;
    }
    
    // �� ������ ������� ��������� ����������� ����� ������ �� �����
    ets_printf("FW Update: checking firmware...\n");
    if (! check_fw_crc(0x81000))
    {
	// �� ����� ���������
	return;
    }
    
    // �������� ������
    ets_printf("FW Update: flashing...\n");
    for (i=0; i<hdr->size4k; i++)
    {
	ets_printf("Sector %d\r", i);
	SPIRead(0x81000+i*4096, buf, 4096);
	SPIEraseSector(0x81+i);
	SPIWrite(0x01000+i*4096, buf, 4096);
    }
    ets_printf("Done                   \n");
    
    // ��������� ��������
    if (! check_fw_crc(0x01000))
    {
	// �� ����� ������� ��������
	ets_printf("FW Update: updated firmware ERROR ! - not erasing update\n");
	return;
    }
    
    // ����� ������� ��������� ����������
    SPIEraseSector(0x80);
    
    // ������
    ets_printf("FW Update: upgrade successfull !\n");
}
