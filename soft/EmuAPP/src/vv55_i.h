#ifndef VV55_I_H
#define VV55_I_H


#include "ets.h"


#ifdef __cplusplus
extern "C" {
#endif


// ������������
#define MOD_SS		0x20
#define MOD_US		0x40
#define MOD_RL		0x80


// ������ 0
#define RK_HOME		0x0001	// ������� �����-�����
#define RK_STR		0x0002	// ���
#define RK_AR2		0x0004	// ��2
#define RK_F1		0x0008	// �1
#define RK_F2		0x0010	// �2
#define RK_F3		0x0020	// �3
#define RK_F4		0x0040	// �4

// ������ 1
#define RK_TAB		0x0101	// ���
#define RK_PS		0x0102	// ��
#define RK_VK		0x0104	// ��
#define RK_ZB		0x0108	// ��
#define RK_LEFT		0x0110	// <-
#define RK_UP		0x0120	// �����
#define RK_RIGHT	0x0140	// ->
#define RK_DOWN		0x0180	// ����

// ������ 2
#define RK_0		0x0201	// 0
#define RK_1		0x0202	// 1
#define RK_2		0x0204	// 2
#define RK_3		0x0208	// 3
#define RK_4		0x0210	// 4
#define RK_5		0x0220	// 5
#define RK_6		0x0240	// 6
#define RK_7		0x0280	// 7

// ������ 3
#define RK_8		0x0301	// 8
#define RK_9		0x0302	// 9
#define RK_STAR		0x0304	// *
#define RK_SEMICOLON	0x0308	// ;
#define RK_COMMA	0x0310	// ,
#define RK_MINUS	0x0320	// -
#define RK_PERIOD	0x0340	// .
#define RK_SLASH	0x0380	// /

// ������ 4
#define RK_AT		0x0401	// @ �
#define RK_A		0x0402	// A �
#define RK_B		0x0404	// B �
#define RK_C		0x0408	// C �
#define RK_D		0x0410	// D �
#define RK_E		0x0420	// E �
#define RK_F		0x0440	// F �
#define RK_G		0x0480	// G �

// ������ 5
#define RK_H		0x0501	// H �
#define RK_I		0x0502	// I �
#define RK_J		0x0504	// J �
#define RK_K		0x0508	// K �
#define RK_L		0x0510	// L �
#define RK_M		0x0520	// M �
#define RK_N		0x0540	// N �
#define RK_O		0x0580	// O �

// ������ 6
#define RK_P		0x0601	// P �
#define RK_Q		0x0602	// Q �
#define RK_R		0x0604	// R �
#define RK_S		0x0608	// S �
#define RK_T		0x0610	// T �
#define RK_U		0x0620	// U �
#define RK_V		0x0640	// V �
#define RK_W		0x0680	// W �

// ������ 7
#define RK_X		0x0701	// X �
#define RK_Y		0x0702	// Y �
#define RK_Z		0x0704	// Z �
#define RK_L_BRACKET	0x0708	// [ �
#define RK_BACK_SLASH	0x0710	// \ �
#define RK_R_BRACKET	0x0720	// ] �
#define RK_CARET	0x0740	// ^ �
#define RK_SPACE	0x0780	// SPACE

// ������������
#define RK_SS		0x2000	// ��
#define RK_US		0x4000	// ��
#define RK_RL		0x8000	// ���/���


void kbd_init(void);
void kbd_press(uint16_t code);
void kbd_release(uint16_t code);
void kbd_releaseAll(uint16_t code);
bool kbd_rus(void);
bool kbd_ss(void);

void kbd_dump(void);


void vv55_i_W(uint8_t A, uint8_t value);
uint8_t vv55_i_R(uint8_t A);


#ifdef __cplusplus
};
#endif


#endif
