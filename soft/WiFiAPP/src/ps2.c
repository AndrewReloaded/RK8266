#include "ps2.h"

#include <osapi.h>
#include <gpio.h>
#include "gpio_lib.h"
#include "timer0.h"
#include "board.h"

//http://www.avrfreaks.net/sites/default/files/PS2%20Keyboard.pdf


#define RXQ_SIZE	16


static uint32_t prev_T=0;
static uint16_t rxq[RXQ_SIZE];
static uint8_t rxq_head=0, rxq_tail=0;


static void RAMFUNC gpio_int(void *arg)
{
    static uint16_t rx=0, rxbit=1;
    static bool was_E0=0, was_E1=0, was_F0=0;
    
    uint32_t gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
    
    // �������� ����� �� �������� ����, ���� ��� ������� ������� - ������� ������� ������
    uint32_t T=getCycleCount();
    uint32_t dT=T-prev_T;
    if (dT < 8000) return;	// ���� ����� ������ 50��� (�� ��������� 60-100���), �� ��� ������ (���������� ��)
    if (dT > 19200)	// 120��� �������
    {
	// ���������� ��������
	rx=0;
	rxbit=1;
    }
    prev_T=T;
    
    {
	// �� � ������ ������
	
	// ��������� ���
	if (gpio_in(PS2_DATA)) rx|=rxbit;
	rxbit<<=1;
	
	// ��������� �� ����� �����
	if (rxbit & (1<<11))
	{
	    // ������� 11 ���
	    if ( (!(rx & 0x001)) && (rx & 0x400) )	// �������� ������� ����� � ���� �����
	    {
		// ������� ��������� ���
		rx>>=1;
		
		// �������� ���
		uint8_t code=rx & 0xff;
		
		// ������� ��������
		rx^=rx >> 4;
		rx^=rx >> 2;
		rx^=rx >> 1;
		rx^=rx >> 8;
		
		if (! (rx & 1))
		{
		    // ��� ��������� !
		    if (code==0xE0) was_E0=1; else
		    if (code==0xE1) was_E1=1; else
		    if (code==0xF0) was_F0=1; else
		    {
			uint16_t code16=code;
			
			// ����������� ������
			if (was_E0) code16|=0x0100; else
			if (was_E1) code16|=0x0200;
			
			// �������
			if (was_F0) code16|=0x8000;
			
			// ������ � �����
			rxq[rxq_head]=code16;
			rxq_head=(rxq_head + 1) & (RXQ_SIZE-1);
			
			// ���������� �����
			was_E0=was_E1=was_F0=0;
		    }
		}
	    }
	    
	    // ���������� ��������
	    rx=0;
	    rxbit=1;
	}
    }
}


void ps2_init(void)
{
    // ����������� PS2_DATA � PS2_CLK � GPIO
    gpio_init_input_pu(PS2_DATA);
    gpio_init_input_pu(PS2_CLK);
    
    // ����������� ���������� �� ������� ������ �� PS2_CLK
    gpio_pin_intr_state_set(GPIO_ID_PIN(PS2_CLK), GPIO_PIN_INTR_NEGEDGE);
    
    // ����������� ���������� �� GPIO
    ETS_GPIO_INTR_ATTACH(gpio_int, 0);
    ETS_GPIO_INTR_ENABLE();
}


uint16_t ps2_read(void)
{
    if (rxq_head == rxq_tail) return 0;
    uint16_t d=rxq[rxq_tail];
    rxq_tail=(rxq_tail + 1) & (RXQ_SIZE-1);
    return d;
}
