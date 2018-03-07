#include "tape.h"

#include "i8080_hal.h"
#include "ets.h"


static uint32_t prev_cycles=0;
static uint8_t bit=0, byte=0, bit_cnt=0;


static void tape_in_bit(uint8_t b)
{
    byte=(byte << 1) | bit;
    bit_cnt++;
    
    if (bit_cnt==8)
    {
	ets_printf("TAPE: 0x%02X\n", byte);
	byte=0;
	bit_cnt=0;
    }
}


#define SYNC_COUNT	16
void tape_in(void)
{
    static uint8_t start=0, c=0;
    static uint8_t sync=SYNC_COUNT;
    static uint16_t period=0;
    
    // �������� ���-�� ������ � �������� ������ ������
    uint32_t T=i8080_cycles - prev_cycles;
    prev_cycles=i8080_cycles;
    
    if (T > 5000)
    {
	// �������
	sync=SYNC_COUNT;
	start=0;
	return;
    }
    
    // �������������� �������
    if (sync > 0)
    {
	// �������������
	if ( (T < period/2) || (T > period+period/2) )
	{
	    // ������� ������
	    period=T;
	    sync=SYNC_COUNT;
	    start=0;
	    return;
	} else
	{
	    // ���������
	    period=(period+T)/2;
	    sync--;
	}
    } else
    {
	uint8_t d;
	
	// ���������� - �������� ��� ������� ������
	if ( (T < period/2) || (T > period*3) )
	{
	    // ������� ������
	    period=T;
	    sync=SYNC_COUNT;
	    start=0;
	    return;
	} else
	if (T >= period+period/2)
	{
	    // �������
	    d=1;
	    period=(period+T/2)/2;
	} else
	{
	    // ��������
	    d=0;
	    period=(period+T)/2;
	}
	
	// ������������ ������
	if (! start)
	{
	    // ���� ��������� ���
	    if (d)
	    {
		bit=1;
		start=1;
		c=1;
		//printf("MAG: bit %d (start)\n", bit);
		
		// ������ ������
		byte=1;
		bit_cnt=1;
	    }
	} else
	{
	    if (d)
	    {
		// ������� - ����� ��������
		bit=bit ^ 1;
		c=1;
		tape_in_bit(bit);
	    } else
	    {
		// ������ ������ �������� - ������ ����
		c^=1;
		if (c) tape_in_bit(bit);
	    }
	}
    }
}
