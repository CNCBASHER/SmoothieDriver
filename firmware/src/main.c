/*        This file is part of Smoothie (http://smoothieware.org/). Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>. */

#ifdef __USE_CMSIS
#include "LPC122x.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include "io.h"

//#include "i2c.h"
#include "uart.h"
#include "small_printf.h"

#define printf(...) func_printf_nofloat (putchar, __VA_ARGS__)

#define LED_PORT 0
#define LED_PIN 7

#define UART_CHAN 0
#define UART_BAUD 115200

#define ENC_STEPPING 4

#define PWM_CYCLE 256
#define PWM_RAMP_STEPS 16
#define PWM_MIN_SPEED 4

#if (UART_CHAN == 0)
extern volatile uint32_t UARTCount0;
extern volatile uint8_t UARTBuffer0[BUFSIZE];
#define LPC_UART LPC_UART0
#else
extern volatile uint32_t UARTCount1;
extern volatile uint8_t UARTBuffer1[BUFSIZE];
#define LPC_UART LPC_UART1
#endif

//extern unsigned int __data_section_table;

static int putchar (char c)
{
	/* THRE status, contain valid data */
	while ( !(LPC_UART->LSR & LSR_THRE) );
	LPC_UART->THR = c;
	return 0;
}

volatile int g_en0;
volatile int g_pos0;
volatile int g_target0;
volatile int g_en1;
volatile int g_pos1;
volatile int g_target1;

Pin_t ccu_en0;
Pin_t ccu_dir0;
Pin_t ccu_step0;

Pin_t mot0_en;
Pin_t mot0_mode;
Pin_t mot0_brake;
Pin_t mot0_phase;

Pin_t enc0_a;
Pin_t enc0_b;

Pin_t ccu_en1;
Pin_t ccu_dir1;
Pin_t ccu_step1;

Pin_t mot1_en;
Pin_t mot1_mode;
Pin_t mot1_brake;
Pin_t mot1_phase;

Pin_t enc1_a;
Pin_t enc1_b;

static inline void int_ccu_en0() {
	if(LPC_GPIO[ccu_en0.port]->PIN & ccu_en0.mask) {
		g_pos0 = 0;
		g_target0 = 0;
		Pin_Off(mot0_brake);
		Pin_Off(mot0_mode);
		Pin_Off(mot0_en);
		LPC_GPIO[ccu_step0.port]->IE |= ccu_step0.mask;
		if(ENC_STEPPING >= 2) {
			LPC_GPIO[enc0_a.port]->IBE |= enc0_a.mask;
			if(ENC_STEPPING == 4) {
				LPC_GPIO[enc0_b.port]->IBE |= enc0_b.mask;
				LPC_GPIO[enc0_b.port]->IE |= enc0_b.mask;
			}
		}
		LPC_GPIO[enc0_a.port]->IE |= enc0_a.mask;
		printf("ccu_en0 rise\r\n");
		g_en0 = 1;
	} else {
		Pin_Off(mot0_mode);
		Pin_On(mot0_en);
		Pin_On(mot0_brake);
		LPC_GPIO[ccu_step0.port]->IE &= ~ccu_step0.mask;
		LPC_GPIO[enc0_a.port]->IE &= ~enc0_a.mask;
		LPC_GPIO[enc0_b.port]->IE &= ~enc0_b.mask;
		printf("ccu_en0 fall\r\n");
		g_en0 = 0;
	}
	LPC_GPIO[ccu_en0.port]->IC |= ccu_en0.mask;
}

static inline void int_ccu_step0() {
	if(LPC_GPIO[ccu_dir0.port]->PIN & ccu_dir0.mask) {
		g_target0++;
//		printf("ccu_step0 forward %d\r\n", g_target0);
	} else {
		g_target0--;
//		printf("ccu_step0 backward %d\r\n", g_target0);
	}
	LPC_GPIO[ccu_step0.port]->IC |= ccu_step0.mask;
}

static inline void int_enc0_a() {
	if((LPC_GPIO[enc0_b.port]->PIN & enc0_b.mask) ^ (LPC_GPIO[enc0_a.port]->PIN & enc0_a.mask)) {
		g_pos0++;
//		printf("enc0 forward %d\r\n", g_pos0);
	} else {
		g_pos0--;
//		printf("enc0 backward %d\r\n", g_pos0);
	}
	LPC_GPIO[enc0_a.port]->IC |= enc0_a.mask;
}

static inline void int_enc0_b() {
	if((LPC_GPIO[enc0_a.port]->PIN & enc0_a.mask) ^ (LPC_GPIO[enc0_b.port]->PIN & enc0_b.mask)) {
		g_pos0--;
//		printf("enc0 forward %d\r\n", g_pos0);
	} else {
		g_pos0++;
//		printf("enc0 backward %d\r\n", g_pos0);
	}
	LPC_GPIO[enc0_b.port]->IC |= enc0_b.mask;
}

static inline void int_ccu_en1() {
	if(LPC_GPIO[ccu_en1.port]->PIN & ccu_en1.mask) {
		g_pos1 = 0;
		g_target1 = 0;
		Pin_Off(mot1_brake);
		Pin_Off(mot1_mode);
		Pin_Off(mot1_en);
		LPC_GPIO[ccu_step1.port]->IE |= ccu_step1.mask;
		if(ENC_STEPPING >= 2) {
			LPC_GPIO[enc1_a.port]->IBE |= enc1_a.mask;
			if(ENC_STEPPING == 4) {
				LPC_GPIO[enc1_b.port]->IBE |= enc1_b.mask;
				LPC_GPIO[enc1_b.port]->IE |= enc1_b.mask;
			}
		}
		LPC_GPIO[enc1_a.port]->IE |= enc1_a.mask;
		printf("ccu_en1 rise\r\n");
		g_en1 = 1;
	} else {
/*
		Pin_Off(mot1_mode);
		Pin_On(mot1_en);
		Pin_On(mot1_brake);
		LPC_GPIO[ccu_step1.port]->IE &= ~ccu_step1.mask;
		LPC_GPIO[enc1_a.port]->IE &= ~enc1_a.mask;
		LPC_GPIO[enc1_b.port]->IE &= ~enc1_b.mask;
		printf("ccu_en1 fall\r\n");
		g_en1 = 0;
*/
	}
	LPC_GPIO[ccu_en1.port]->IC |= ccu_en1.mask;
}

static inline void int_ccu_step1() {
	if(LPC_GPIO[ccu_dir1.port]->PIN & ccu_dir1.mask) {
		g_target1++;
//		printf("ccu_step1 forward %d\r\n", g_target1);
	} else {
		g_target1--;
//		printf("ccu_step1 backward %d\r\n", g_target1);
	}
	LPC_GPIO[ccu_step1.port]->IC |= ccu_step1.mask;
}

static inline void int_enc1_a() {
	if(!(LPC_GPIO[enc1_b.port]->PIN & enc1_b.mask) != !(LPC_GPIO[enc1_a.port]->PIN & enc1_a.mask)) {
		g_pos1++;
//		if(LPC_GPIO[enc1_a.port]->PIN & enc1_a.mask) printf("rise "); else printf("fall ");
//		printf("enc1_a forward %d\r\n", g_pos1);
	} else {
		g_pos1--;
//		if(LPC_GPIO[enc1_a.port]->PIN & enc1_a.mask) printf("rise "); else printf("fall ");
//		printf("enc1_a backward %d\r\n", g_pos1);
	}
	LPC_GPIO[enc1_a.port]->IC |= enc1_a.mask;
}

static inline void int_enc1_b() {
	if(!(LPC_GPIO[enc1_a.port]->PIN & enc1_a.mask) != !(LPC_GPIO[enc1_b.port]->PIN & enc1_b.mask)) {
		g_pos1--;
//		if(LPC_GPIO[enc1_b.port]->PIN & enc1_b.mask) printf("rise "); else printf("fall ");
//		printf("enc1_b backward %d\r\n", g_pos1);
	} else {
		g_pos1++;
//		if(LPC_GPIO[enc1_b.port]->PIN & enc1_b.mask) printf("rise "); else printf("fall ");
//		printf("enc1_b forward %d\r\n", g_pos1);
	}
	LPC_GPIO[enc1_b.port]->IC |= enc1_b.mask;
}

static inline void int_gpio() {
	if(LPC_GPIO[ccu_en0.port]->MIS & ccu_en0.mask) {
		int_ccu_en0();
	}
	if(LPC_GPIO[ccu_en1.port]->MIS & ccu_en1.mask) {
		int_ccu_en1();
	}

	if(g_en0) {
		if(LPC_GPIO[ccu_step0.port]->MIS & ccu_step0.mask) {
			int_ccu_step0();
		}
		if(LPC_GPIO[enc0_a.port]->MIS & enc0_a.mask) {
			int_enc0_a();
		}
		if(LPC_GPIO[enc0_b.port]->MIS & enc0_b.mask) {
			int_enc0_b();
		}
	}
	if(g_en1) {
		if(LPC_GPIO[ccu_step1.port]->MIS & ccu_step1.mask) {
			int_ccu_step1();
		}
		if(LPC_GPIO[enc1_a.port]->MIS & enc1_a.mask) {
			int_enc1_a();
		}
		if(LPC_GPIO[enc1_b.port]->MIS & enc1_b.mask) {
			int_enc1_b();
		}
	}
}

void PIOINT0_IRQHandler(void) {
//	LPC_GPIO0->NOT = 1 << 7;
	int_gpio();
}

void PIOINT1_IRQHandler(void) {
//	LPC_GPIO0->NOT = 1 << 7;
	int_gpio();
}

void PIOINT2_IRQHandler(void) {
//	LPC_GPIO0->NOT = 1 << 7;
	int_gpio();
}

int main(void)
{
	// init system timer
	LPC_SYSCON->SYSAHBCLKCTRL |= 0xE001001FUL;

	*IOCON_REGISTER_MAP[0][30] |= 0x01;
	*IOCON_REGISTER_MAP[0][31] |= 0x01;
	*IOCON_REGISTER_MAP[1][0] |= 0x01;
	*IOCON_REGISTER_MAP[1][1] |= 0x01;

	LPC_SYSCON->SYSAHBCLKCTRL |= 1<<7 | 1<<8;
	LPC_CT16B1->MCR = 1 << 10;
	LPC_CT16B1->MR0 = 0x0000;
	LPC_CT16B1->MR1 = 0x0000;
	LPC_CT16B1->MR3 = PWM_CYCLE - 1;
	LPC_CT16B1->PWMC = 0x000B;
	LPC_CT16B1->TCR = 0x0001;

	ccu_en0 = Pin_Get(ARM_P2_0);
	ccu_dir0 = Pin_Get(ARM_P2_4);
	ccu_step0 = Pin_Get(ARM_P2_5);

	mot0_en = Pin_Get(ARM_P0_29);
	mot0_mode = Pin_Get(ARM_P0_20);
	mot0_brake = Pin_Get(ARM_P0_18);
	mot0_phase = Pin_Get(ARM_P0_28);

	enc0_a = Pin_Get(ARM_P0_0);
	enc0_b = Pin_Get(ARM_P0_19);

	ccu_en1 = Pin_Get(ARM_P0_6);
	ccu_dir1 = Pin_Get(ARM_P0_21);
	ccu_step1 = Pin_Get(ARM_P0_24);

	mot1_en = Pin_Get(ARM_P1_6);
	mot1_mode = Pin_Get(ARM_P0_31);
	mot1_brake = Pin_Get(ARM_P1_0);
	mot1_phase = Pin_Get(ARM_P1_5);

	enc1_a = Pin_Get(ARM_P0_8);
	enc1_b = Pin_Get(ARM_P0_9);

	// set motor0 tracking variables
	g_en0 = Pin_Read(ccu_en0);
	g_pos0 = 0;
	g_target0 = 0;
	// set motor1 tracking variables
	g_en1 = Pin_Read(ccu_en1);
	g_pos1 = 0;
	g_target1 = 0;

	// init led
	Pin_t led = Pin_Get(LED1);
	Pin_Output(led);
	//	Pin_On(led);

	// init ccu port
//	Pin_Mode(ccu_en0, PullNone);
	Pin_Mode(ccu_en0, OpenDrain);
//	Pin_Mode(ccu_dir0, PullNone);
	Pin_Mode(ccu_dir0, OpenDrain);
//	Pin_Mode(ccu_step0, PullNone);
	Pin_Mode(ccu_step0, OpenDrain);
//	Pin_Mode(ccu_en1, PullNone);
	Pin_Mode(ccu_en1, OpenDrain);
//	Pin_Mode(ccu_dir1, PullNone);
	Pin_Mode(ccu_dir1, OpenDrain);
//	Pin_Mode(ccu_step1, PullNone);
	Pin_Mode(ccu_step1, OpenDrain);

	// init motor0 port
	Pin_Output(mot0_en);
	Pin_Output(mot0_brake);
	Pin_Output(mot0_mode);
	Pin_Output(mot0_phase);
//	Pin_Mode(enc0_a, PullNone);
	Pin_Mode(enc0_a, OpenDrain);
//	Pin_Mode(enc0_b, PullNone);
	Pin_Mode(enc0_b, OpenDrain);
	if(!g_en0) {
		Pin_On(mot0_en);
		Pin_On(mot0_brake);
	}
	// init motor1 port
	Pin_Output(mot1_en);
	Pin_Output(mot1_brake);
	Pin_Output(mot1_mode);
	Pin_Output(mot1_phase);
	*IOCON_REGISTER_MAP[1][5] |= 0x03;
//	Pin_Mode(mot1_brake, PullNone);
//	Pin_Mode(mot1_brake, OpenDrain);
//	Pin_Mode(enc1_a, PullNone);
	Pin_Mode(enc1_a, OpenDrain);
//	Pin_Mode(enc1_b, Alt1);
//	Pin_Mode(enc1_b, PullNone);
	Pin_Mode(enc1_b, OpenDrain);
	if(!g_en1) {
		Pin_On(mot1_en);
		Pin_On(mot1_brake);
	}

	// init gpio interrupts
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
	// config and enable ccu interrupts for motor0
	LPC_GPIO[ccu_step0.port]->IEV |= ccu_step0.mask;
	LPC_GPIO[ccu_en0.port]->IBE |= ccu_en0.mask;
	LPC_GPIO[ccu_en0.port]->IE |= ccu_en0.mask;
	// config and enable ccu interrupts for motor1
	LPC_GPIO[ccu_step1.port]->IEV |= ccu_step1.mask;
	LPC_GPIO[ccu_en1.port]->IBE |= ccu_en1.mask;
	LPC_GPIO[ccu_en1.port]->IE |= ccu_en1.mask;

	// init uart
	UARTInit( UART_CHAN, UART_BAUD );

	// start
	printf( "Smoothiedriver v0.0 Initialized\r\n" );
	printf( "start\r\n" );
//	printf( "Program Text Size: %05d\r\n", __data_section_table );
//	printf( "LPC_SYSCON->SYSAHBCLKCTRL: 0x%08X 0x%08X\r\n", (uint32_t) &LPC_SYSCON->SYSAHBCLKCTRL, LPC_SYSCON->SYSAHBCLKCTRL);
//	printf( "LPC_SYSCON->SYSAHBCLKCTRL: %d %d\r\n", (uint32_t) &LPC_SYSCON->SYSAHBCLKCTRL, LPC_SYSCON->SYSAHBCLKCTRL);
//	printf( "%d %s\r\n", 0, "message");
//	printf( "signed %d = unsigned %u = hex %x\r\n", -3, -3, -3);

	g_pos1 = 0;
	g_target1 = 141;
	Pin_Off(mot1_brake);
	Pin_Off(mot1_mode);
	Pin_Off(mot1_en);
	LPC_GPIO[ccu_step1.port]->IE |= ccu_step1.mask;
	if(ENC_STEPPING >= 2) {
		LPC_GPIO[enc1_a.port]->IBE |= enc1_a.mask;
		if(ENC_STEPPING == 4) {
			LPC_GPIO[enc1_b.port]->IBE |= enc1_b.mask;
			LPC_GPIO[enc1_b.port]->IE |= enc1_b.mask;
		}
	}
	LPC_GPIO[enc1_a.port]->IE |= enc1_a.mask;
	g_en1 = 1;

	int speed = 0;
	int off_steps = 0;
	while(1) {
		//Pin_Toggle(led);
		//Count(1<<20);
		//__WFI();

		// seek g_pos0 towards g_target0
/*		if(g_en0) {
			if(g_target0 > g_pos0) {
				speed = g_target0 - g_pos0;
				if(speed > 12) speed = 12;
				Pin_On(mot0_phase);
				Pin_On(mot0_brake);
				Count(1<<12);
			}
			else if(g_target0 < g_pos0) {
				speed = g_pos0 - g_target0;
				if(speed > 12) speed = 12;
				Pin_Off(mot0_phase);
				Pin_On(mot0_brake);
				Count(1<<12);
			}
			else {
				Pin_Off(mot0_brake);
				Count(1<<12);
			}
			Pin_Off(mot0_brake);
			Count(1<<13);
//			printf("pos0: %d target0: %d\r\n", g_pos0, g_target0);
		}*/
		// seek g_pos1 towards g_target1
		if(g_en1) {
			if(g_target1 > g_pos1) {
				speed = g_target1 - g_pos1;
				if(speed != off_steps) {
					off_steps = speed;
					printf("pos1: %d target1: %d\r\n", g_pos1, g_target1);
				}
				if(speed < PWM_MIN_SPEED) speed = PWM_MIN_SPEED;
				if(speed > PWM_RAMP_STEPS) speed = PWM_RAMP_STEPS;
				speed = speed * PWM_CYCLE/2 / PWM_RAMP_STEPS;
// *****					Pin_On(mot1_phase);
				LPC_CT16B1->MR0 = PWM_CYCLE/2 - speed;
//					Pin_Off(mot1_en);
				Pin_On(mot1_brake);
//					Count(1<<speed);
			}
			else if(g_target1 < g_pos1) {
				speed = g_pos1 - g_target1;
				if(speed != off_steps) {
					off_steps = speed;
					printf("pos1: %d target1: %d\r\n", g_pos1, g_target1);
				}
				if(speed < PWM_MIN_SPEED) speed = PWM_MIN_SPEED;
				if(speed > PWM_RAMP_STEPS) speed = PWM_RAMP_STEPS;
				speed = speed * PWM_CYCLE/2 / PWM_RAMP_STEPS;
				LPC_CT16B1->MR0 = PWM_CYCLE/2 - 1 + speed;
// *****					Pin_Off(mot1_phase);
//					Pin_Off(mot1_en);
				Pin_On(mot1_brake);
//					Count(1<<speed);
			}
			else {
//				Pin_On(mot1_en);
//				speed = 0 + 6;
				Pin_Off(mot1_brake);
//				Count(1<<12);
			}
//			Pin_On(mot1_en);
//			Pin_Off(mot1_brake);
//			Count(1<<(6 + 6 - speed));
		}
	}
	return 0;
}
