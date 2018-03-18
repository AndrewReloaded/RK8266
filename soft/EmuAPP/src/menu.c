#include "menu.h"

#include "ui.h"
#include "i8080.h"
#include "i8080_hal.h"
#include "reboot.h"
#include "ffs.h"
#include "files.h"
#include "tape.h"


static bool menu_load(void)
{
    uint8_t type;
    int16_t n;
    
again:
    ui_clear();
    ui_header("�����-86�� -->");
    ui_draw_list(
	"1.���������\n"
	"2.����\n"
	"3.�������\n"
	);
    switch (ui_select(5))
    {
	case 0:
	    // ���������
	    type=TYPE_PROG;
	    break;
	
	case 1:
	    // ����
	    type=TYPE_GAME;
	    break;
	
	case 2:
	    // �������
	    type=TYPE_UTIL;
	    break;
	
	default:
	    return false;
    }
    
select_again:
    n=ui_select_file(type);
    if (n < 0) goto again;
    
    // ������� ���� - ���������
    int16_t addr=load_file(n);
    if (addr >= 0)
    {
	// ��������� ����������� - ���������
	i8080_jump(addr);
	
	// ������������ � ��������
	return true;
    } else
    {
	// ������ �������� �����
	ui_clear();
	ui_header("�����-86�� -->");
	ui_draw_text(10, 10, "������ �������� ����� !");
	ui_sleep(1000);
	goto select_again;
    }
}


void menu(void)
{
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
    switch (ui_select(5))
    {
	case 0:
	    // ������� � �������
	    i8080_jump(0xF800);
	    break;
	
	case 1:
	    // ������ �����
	    ets_memset(i8080_hal_memory(), 0x00, 0x8000);
	    i8080_init();
	    i8080_jump(0xF800);
	    break;
	
	case 2:
	    // �������� ������
	    if (! menu_load()) goto again;
	    break;
	
	case 3:
	    // �������� � �����������
	    tape_load();
	    break;
	
	case 4:
	    // ������������� � ����� WiFi
	    reboot(0x55AA55AA);
	    break;
    }
}
