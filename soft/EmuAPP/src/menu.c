#include "menu.h"

#include "ui.h"
#include "i8080.h"
#include "i8080_hal.h"
#include "reboot.h"
#include "ffs.h"
#include "files.h"


#define MAX_FILES	80
static uint8_t load_type;
static uint8_t filelist[MAX_FILES];
static uint8_t n_files;


static PT_THREAD(menu_load_select(struct pt *pt))
{
    static struct pt sub;
    static uint32_t _sleep;
    uint16_t i;
    
    PT_BEGIN(pt);
	PT_YIELD(pt);
	
	// �������� ������� ������
	n_files=0;
	for (i=0; i<FAT_SIZE; i++)
	{
	    if (fat[i].type == load_type)
	    {
		filelist[n_files++]=i;
		if (n_files>=MAX_FILES) break;
	    }
	}
	
again:
	// ������
	ui_clear();
	ui_header("�����-86�� -->");
	if (n_files==0)
	{
	    ui_draw_text(10, 10, "��� ������ !");
	    PT_SLEEP(1000000);
	    PT_EXIT(pt);
	}
	
	ui_draw_text(10, 8, "�������� ���� ��� ��������:");
	for (i=0; i<n_files; i++)
	{
	    ui_draw_text(10+(i/20)*16, 10+(i%20), ffs_name(filelist[i]));
	}
	ui_select_n=0; ui_select_count=n_files;
	PT_SUB(ui_select);
	
	if (ui_select_n >= 0)
	{
	    // ������� ���� - ���������
	    int16_t addr=load_file(filelist[ui_select_n]);
	    if (addr >= 0)
	    {
		// ��������� ����������� - ���������
		i8080_jump(addr);
		
		// ������������ � ��������
		ui_select_n=-1;
		PT_EXIT(pt);
	    } else
	    {
		// ������ �������� �����
		ui_clear();
		ui_header("�����-86�� -->");
		ui_draw_text(10, 10, "������ �������� ����� !");
		PT_SLEEP(1000000);
		goto again;
	    }
	}
	
	// ������
	ui_select_n=0;	// ����� �� ������������ � ��������
    PT_END(pt);
}


static PT_THREAD(menu_load(struct pt *pt))
{
    static struct pt sub;
    
    PT_BEGIN(pt);
	PT_YIELD(pt);
again:
	ui_clear();
	ui_header("�����-86�� -->");
	ui_draw_list(
	    "1.���������\n"
	    "2.����\n"
	    "3.�������\n"
	    );
	ui_select_n=0; ui_select_count=3;
	PT_SUB(ui_select);
	if (ui_select_n==0)
	{
	    // ���������
	    load_type=TYPE_PROG;
	    PT_SUB(menu_load_select);
	} else
	if (ui_select_n==1)
	{
	    // ����
	    load_type=TYPE_GAME;
	    PT_SUB(menu_load_select);
	} else
	if (ui_select_n==2)
	{
	    // �������
	    load_type=TYPE_UTIL;
	    PT_SUB(menu_load_select);
	}
	
	if (ui_select_n < 0)
	{
	    // �����
	    PT_EXIT(pt);
	}
	
	goto again;
    PT_END(pt);
}


PT_THREAD(menu(struct pt *pt))
{
    static struct pt sub;
    
    PT_BEGIN(pt);
	PT_YIELD(pt);
again:
	ui_clear();
	ui_header("�����-86�� -->");
	ui_draw_list(
	    "1.������� � ������� (��� ������� ������)\n"
	    "2.������ �����\n"
	    "3.�������� ������\n"
	    "4.�������� � �����������\n"
	    "5.������������� � ����� WiFi\n"
	    );
	ui_draw_text(10, 20,
	    "�������� ����������:\n"
	    "�1-�4   - F1-F4\n"
	    "��      - Enter\n"
	    "��      - Enter �� ���.����������\n"
	    "��      - Backspace\n"
	    "��      - CTRL\n"
	    "��      - Shift\n"
	    "���/��� - Caps Lock\n"
	    "\\       - Home\n"
	    "���     - End/Delete\n"
	    "��2     - Alt\n"
	    );
	ui_select_n=0; ui_select_count=5;
	PT_SUB(ui_select);
	// ��� switch ������ ������ ��-�� pt
	if (ui_select_n==0)
	{
	    // ������� � �������
	    i8080_jump(0xF800);
	    PT_EXIT(pt);
	} else
	if (ui_select_n==1)
	{
	    // ������ �����
	    ets_memset(i8080_hal_memory(), 0x00, 0x8000);
	    i8080_init();
	    i8080_jump(0xF800);
	    PT_EXIT(pt);
	} else
	if (ui_select_n==2)
	{
	    // �������� ������
	    PT_SUB(menu_load);
	} else
	if (ui_select_n==3)
	{
	    // �������� � �����������
	} else
	if (ui_select_n==4)
	{
	    // ������������� � ����� WiFi
	    reboot(0x55AA55AA);
	}
	
	if (ui_select_n < 0)
	{
	    // �����
	    PT_EXIT(pt);
	}
	
	goto again;
    PT_END(pt);
}
