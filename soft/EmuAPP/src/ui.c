#include "menu.h"

#include "ets.h"
#include "vg75.h"
#include "ps2.h"
#include "ps2_codes.h"
#include "pt.h"
#include "xprintf.h"
#include "zkg.h"
#include "i8080.h"
#include "i8080_hal.h"
#include "xlat.h"

#include "menu.h"


char ui_scr[38][78];


void ui_clear(void)
{
    ets_memset(ui_scr, 0x00, sizeof(ui_scr));
}


#define HEADER_Y	4
void ui_header(const char *s)
{
    static const uint8_t syms[]=
	{
	    0, 4, 16, 20, 2, 6, 18, 22,
	    1, 5, 17, 21, 3, 7, 19, 23
	};
    uint8_t l=ets_strlen(s);
    uint8_t x=(78-l*3)/2;	// ������ ������ ������������ ��� 3 �������
    uint8_t y;
    
    // ������ ������
    for (y=0; y<4; y++)
    {
	uint8_t i;
	for (i=0; i<l; i++)
	{
	    uint8_t c=xlat[(uint8_t)s[i]];
	    uint8_t b1=zkg[ ((y*2+0) << 7) + c];
	    uint8_t b2=zkg[ ((y*2+1) << 7) + c];
	    ui_scr[HEADER_Y+y][x+i*3+0]=syms[ ((b1 >> 2) & 0x0C) | ((b2 >> 4) & 0x03) ];
	    ui_scr[HEADER_Y+y][x+i*3+1]=syms[ (b1 & 0x0C) | ((b2 >> 2) & 0x03) ];
	    ui_scr[HEADER_Y+y][x+i*3+2]=syms[ ((b1 << 2) & 0x0C) | (b2  & 0x03) ];
	}
    }
}


#define LIST_Y	10
#define LIST_X	10
void ui_draw_list(const char *s)
{
    uint8_t y=LIST_Y;
    while (*s)
    {
	uint8_t x;
	for (x=LIST_X; (*s) && ((*s)!='\n'); x++)
	    ui_scr[y][x]=xlat[(uint8_t)(*s++)];
	if (*s) s++;	// ���������� '\n'
	y++;
    }
}


void ui_draw_text(uint8_t x, uint8_t y, const char *s)
{
    while (*s)
    {
	uint8_t xx;
	for (xx=x; (*s) && ((*s)!='\n'); xx++)
	    ui_scr[y][xx]=xlat[(uint8_t)(*s++)];
	if (*s) s++;	// ���������� '\n'
	y++;
    }
}


int8_t ui_select_n=0, ui_select_count=0;
PT_THREAD(ui_select(struct pt *pt))
{
    static uint8_t prev=0;
    
    PT_BEGIN(pt);
	if (ui_select_n < 0) ui_select_n=0;
	while (1)
	{
	    // ������� ���������� �������
	    ets_memcpy(&ui_scr[LIST_Y + (prev % 20)][LIST_X-4 + (prev / 20)*16], "    ", 4);
	    
	    // ������ ����� �������
	    ets_memcpy(&ui_scr[LIST_Y + (ui_select_n % 20)][LIST_X-4 + (ui_select_n / 20)*16], "--> ", 4);
	    
	    // ��������� ������� ������� ��� ���������� ��� �����������
	    prev=ui_select_n;
	    
	    // ������ �����
	    while (1)
	    {
		char c=ps2_sym(ps2_read());
		if (c==KEY_ESC)
		{
		    // ������
		    ui_select_n=-1;
		    PT_EXIT(pt);
		} else
		if (c==KEY_ENTER)
		{
		    // ����
		    PT_EXIT(pt);
		} else
		if ( (c==KEY_UP) && (ui_select_n > 0) )
		{
		    // �����
		    ui_select_n--;
		    break;
		} else
		if ( (c==KEY_DOWN) && (ui_select_n < ui_select_count-1) )
		{
		    // ����
		    ui_select_n++;
		    break;
		} else
		if ( (c==KEY_LEFT) && (ui_select_n > 0) )
		{
		    // �����
		    ui_select_n-=20;
		    if (ui_select_n < 0) ui_select_n=0;
		    break;
		} else
		if ( (c==KEY_RIGHT) && (ui_select_n < ui_select_count-1) )
		{
		    // ������
		    ui_select_n+=20;
		    if (ui_select_n >= ui_select_count) ui_select_n=ui_select_count-1;
		    break;
		} else
		if ( (c>='1') && (c<='9') )
		{
		    // ����� ������� ������ �� ������
		    uint8_t n=c-'1';
		    if (n < ui_select_count)
		    {
			ui_select_n=n;
			PT_EXIT(pt);
		    }
		}
		
		// ����
		PT_YIELD(pt);
	    }
	}
    PT_END(pt);
}


void ui_start(void)
{
    // ��������� ��������� ������
    struct screen save=screen;
    
    // ��������������� ����� ��� ����
    screen.screen_w=78;
    screen.screen_h=38;
    screen.char_h=8;
    screen.cursor_x=0;
    screen.cursor_y=0;
    screen.cursor_type=0;
    screen.vram=(uint8_t*)ui_scr;
    
    // ������� �����
    ui_clear();
    
    // ������ ������ ����
    struct pt pt;
    PT_INIT(&pt);
    
    // ���� ������ ����
    while (PT_SCHEDULE(menu(&pt)))
    {
	//vg75_periodic();
	ps2_periodic();
    }
    
    
    // ���������� ����� �� �����
    screen=save;
}
