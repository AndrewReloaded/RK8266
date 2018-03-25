#include "tv.h"

#include "timer0.h"
#include "gpio_lib.h"
#include "i2s.h"
#include "board.h"


// ������������ �������� ��� I2S
uint8_t tv_empty_line[64];
static uint8_t tv_sync_4_28__4_28[64], tv_sync_4_28__28_4[64];
static uint8_t tv_sync_28_4__4_28[64], tv_sync_28_4__28_4[64];


static const uint8_t* TV_vsync1[] =
{
    tv_sync_4_28__4_28,	// 623
    tv_sync_4_28__4_28,	// 624
    tv_sync_4_28__4_28,	// 625
    tv_sync_28_4__28_4,	// 1
    tv_sync_28_4__28_4,	// 2
    tv_sync_28_4__4_28,	// 3
    tv_sync_4_28__4_28,	// 4
    tv_sync_4_28__4_28,	// 5
    0,			// 6 - ������ ������, ��� ������������ �� 304 ����� � ���� (�.�. �� ���� �� ���������)
    // 7..310=304 ������ - ������ ����
};

static const uint8_t* TV_vsync2[] =
{
    tv_sync_4_28__4_28,	// 311
    tv_sync_4_28__4_28,	// 312
    tv_sync_4_28__28_4,	// 313
    tv_sync_28_4__28_4,	// 314
    tv_sync_28_4__28_4,	// 315
    tv_sync_4_28__4_28,	// 316
    tv_sync_4_28__4_28,	// 317
    0,			// 318
    // 319..622=304 ������ - ������ ����
};

static const uint8_t** TV_fields[]={TV_vsync1, TV_vsync2};


static void t0_int(void)
{
    // ��������� ������
    gpio_off(TV_SYNC);
    timer0_write(getCycleCount()+100*160);
}


extern void tv_data_field(void);
extern uint8_t* tv_data_line(void);


static const uint8_t* tv_i2s_cb(void)
{
    static uint8_t field=0;
    static const uint8_t* *sync=TV_vsync1;
    static uint16_t line=0;
#define T_DELAY_N	7
    static uint32_t Tdelay[T_DELAY_N];	// ����� �������� ������������� (�.�. I2S ����� �������� ��� ������)
    static uint8_t Tdelay_n=0;
    
    // �������� �������������
    gpio_on(TV_SYNC);
    
    // ����������� ������ �� ���������� ������
    Tdelay_n=(Tdelay_n+1) % T_DELAY_N;
    uint32_t *T=&Tdelay[Tdelay_n];
    timer0_write(getCycleCount()+(*T));
    
    if (line)
    {
	// ���� �������
	line--;
	(*T)=8*160;	// 2+4+2 ��� - �������� ������
	return tv_data_line();
    } else
    {
	// ���� �������������
	if (*sync)
	{
	    (*T)=68*160;	// 2+64+2 ��� - �������� ������
	    return *sync++;
	} else
	{
	    // ����� ������������� - ���� ������ ������ ����� ��������
	    line=304;
	    tv_data_field();
	    
	    // ������ ����
	    field^=1;
	    sync=TV_fields[field];
	    
	    (*T)=8*160;	// 2+4+2 ��� - �������� ������
	    return tv_empty_line;
	}
    }
}


void tv_init(void)
{
    // ������ ���� SYNC
    gpio_init_output(TV_SYNC);
    
    // ��������� ��� �������� (������� ���� 3-2-1-0)
    
    // ������ ������ - ������ ������ 4���
    ets_memset(tv_empty_line, 0x00, 64);
    ets_memset(tv_empty_line+0, 0xff, 4);
    
    // ������� 4-28, 4-28 ���
    ets_memset(tv_sync_4_28__4_28, 0x00, 64);
    ets_memset(tv_sync_4_28__4_28+0, 0xff, 4);
    ets_memset(tv_sync_4_28__4_28+32, 0xff, 4);
    
    // ������� 4-28, 28-4 ���
    ets_memset(tv_sync_4_28__28_4, 0x00, 32);
    ets_memset(tv_sync_4_28__28_4+0, 0xff, 4);
    
    ets_memset(tv_sync_4_28__28_4+32, 0xff, 32);
    ets_memset(tv_sync_4_28__28_4+64-4, 0x00, 4);

    // ������� 28-4, 4-28 ���
    ets_memset(tv_sync_28_4__4_28, 0xff, 32);
    ets_memset(tv_sync_28_4__4_28+32-4, 0x00, 4);
    
    ets_memset(tv_sync_28_4__4_28+32, 0x00, 32);
    ets_memset(tv_sync_28_4__4_28+64-4, 0xff, 4);

    // ������� 28-4, 28-4 ���
    ets_memset(tv_sync_28_4__28_4, 0xff, 32);
    ets_memset(tv_sync_28_4__28_4+32-4, 0x00, 4);
    ets_memset(tv_sync_28_4__28_4+64-4, 0x00, 4);
}


void tv_start(void)
{
    // ����������� ���������� �� �������
    timer0_isr_init();
    timer0_attachInterrupt(t0_int);
    
    // ��������� I2S
    i2s_init(tv_i2s_cb, 64);
    i2s_start();
}
