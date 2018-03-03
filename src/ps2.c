#include "ps2.h"

#include <osapi.h>
#include <gpio.h>
#include "gpio_lib.h"
#include "timer0.h"


#define DATA	13
#define CLK	12


#define RXQ_SIZE	16


static uint32_t prev_T=0;
static uint8_t rxq[RXQ_SIZE], rxq_head=0, rxq_tail=0;

static uint16_t tx=0, txbit=0;

volatile int n_ints=0;


static void RAMFUNC gpio_int(void *arg)
{
    static uint16_t rx=0, rxbit=1;
    
    uint32_t gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
    
    n_ints++;
    
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
    
    if (txbit)
    {
	if (txbit & (1<<9))
	{
	    // �������� 8 ��� ������ � 1 ��� �������� - ��������� � �����
	    tx=0;
	    txbit=0;
	    gpio_init_input_pu(DATA);
	} else
	{
	    // �� � ������ ��������
	    if (tx & txbit)
		gpio_on(DATA); else
		gpio_off(DATA);
	    txbit<<=1;
	}
    } else
    {
	// �� � ������ ������
	
	// ��������� ���
	if (gpio_in(DATA)) rx|=rxbit;
	rxbit<<=1;
	
	// ��������� �� ����� �����
	if (rxbit & (1<<11))
	{
	    // ������� 11 ���
	    if ( (!(rx & 0x001)) && (rx & 0x400) )	// �������� ������� ����� � ���� �����
	    {
		// ������� ��������� ���
		rx>>=1;
		rxq[rxq_head]=rx & 0xff;
		
		// ������� ��������
		rx^=rx >> 4;
		rx^=rx >> 2;
		rx^=rx >> 1;
		rx^=rx >> 8;
		
		if (! (rx & 1))
		{
		    // ��� ��������� !
		    rxq_head=(rxq_head+1) & (RXQ_SIZE-1);
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
    // ����������� DATA � CLK � GPIO
    gpio_init_input_pu(DATA);
    gpio_init_input_pu(CLK);
    
    // ����������� ���������� �� ������� ������ �� CLK
    gpio_pin_intr_state_set(GPIO_ID_PIN(CLK), GPIO_PIN_INTR_NEGEDGE);
    
    // ����������� ���������� �� GPIO
    ETS_GPIO_INTR_ATTACH(gpio_int, 0);
    ETS_GPIO_INTR_ENABLE();
}


void ps2_test(void)
{
    if (rxq_head != rxq_tail)
    {
	while (rxq_head != rxq_tail)
	{
	    os_printf("RX %02X\n", rxq[rxq_tail]);
	    rxq_tail=(rxq_tail+1) & (RXQ_SIZE-1);
	}
    } else
    {
	static uint8_t n=0;
	
	n++;
	if (n==10)
	{
	    os_printf("TX cmd\n");
	    
	    // CLK ����
	    gpio_off(CLK);
	    gpio_init_output(CLK);
	    os_delay_us(100);
	    
	    // DATA ���� (����� ���)
	    gpio_off(DATA);
	    gpio_init_output(DATA);
	    os_delay_us(200);
	    
	    // ������� ������
	    //tx=0x0F2;
	    tx=0x01ED;
	    txbit=1;
	    
	    // ��������� CLK
	    gpio_init_input_pu(CLK);
	} else
	if (n==15)
	{
	    static uint8_t leds=0;
	    
	    n=0;
	    
	    // CLK ����
	    gpio_off(CLK);
	    gpio_init_output(CLK);
	    os_delay_us(100);
	    
	    // DATA ���� (����� ���)
	    gpio_off(DATA);
	    gpio_init_output(DATA);
	    os_delay_us(200);
	    
	    // ������� ������
	    uint8_t p=leds;
	    p^=p >> 4;
	    p^=p >> 2;
	    p^=p >> 1;
	    tx=leds | ((p & 0x01) ? 0x000 : 0x100);
	    txbit=1;
	    os_printf("TX data 0x%03x\n", tx);
	    
	    leds=(leds+1) & 0x07;
	    
	    // ��������� CLK
	    gpio_init_input_pu(CLK);
	}
	os_printf("no data n_ints=%d\n", n_ints);
    }
}
