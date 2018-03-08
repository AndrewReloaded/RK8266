#include "menu.h"

#include "ui.h"
#include "i8080.h"
#include "i8080_hal.h"


PT_THREAD(menu(struct pt *pt))
{
    static struct pt sub;
    
    PT_BEGIN(pt);
	PT_YIELD(pt);
	ui_clear();
	ui_header("�����-86�� -->");
	ui_draw_list(
	    "1.������� � ������� (��� ������� ������)\n"
	    "2.������ �����\n"
	    );
	ui_select_n=0; ui_select_count=2;
	PT_SUB(ui_select);
	switch (ui_select_n)
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
	}
    PT_END(pt);
}
