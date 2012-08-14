/*        This file is part of Smoothie (http://smoothieware.org/). Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>. */

#ifndef IO_H_
#define IO_H_

#define LPC_GPIO_BASE LPC_GPIO0_BASE
//#define LPC_GPIO ((LPC_GPIO_Type*)LPC_GPIO0)
static LPC_GPIO_Type* LPC_GPIO[] = {(LPC_GPIO_Type*)LPC_GPIO0,
									(LPC_GPIO_Type*)LPC_GPIO1,
									(LPC_GPIO_Type*)LPC_GPIO2};

// Names of the Pins
typedef enum InternalPinName {

	// The ARM pin names
	ARM_P0_0 = LPC_GPIO0_BASE, ARM_P0_1, ARM_P0_2, ARM_P0_3,
	ARM_P0_4, ARM_P0_5, ARM_P0_6, ARM_P0_7, ARM_P0_8, ARM_P0_9,
	ARM_P0_10, ARM_P0_11, ARM_P0_12, ARM_P0_13, ARM_P0_14, ARM_P0_15,
	ARM_P0_16, ARM_P0_17, ARM_P0_18, ARM_P0_19, ARM_P0_20, ARM_P0_21,
	ARM_P0_22, ARM_P0_23, ARM_P0_24, ARM_P0_25, ARM_P0_26, ARM_P0_27,
	ARM_P0_28, ARM_P0_29, ARM_P0_30, ARM_P0_31, ARM_P1_0, ARM_P1_1,
	ARM_P1_2, ARM_P1_3, ARM_P1_4, ARM_P1_5, ARM_P1_6, ARM_P1_7,
	ARM_P1_8, ARM_P1_9, ARM_P1_10, ARM_P1_11, ARM_P1_12, ARM_P1_13,
	ARM_P1_14, ARM_P1_15, ARM_P1_16, ARM_P1_17, ARM_P1_18, ARM_P1_19,
	ARM_P1_20, ARM_P1_21, ARM_P1_22, ARM_P1_23, ARM_P1_24, ARM_P1_25,
	ARM_P1_26, ARM_P1_27, ARM_P1_28, ARM_P1_29, ARM_P1_30, ARM_P1_31,
	ARM_P2_0, ARM_P2_1, ARM_P2_2, ARM_P2_3, ARM_P2_4, ARM_P2_5,
	ARM_P2_6, ARM_P2_7, ARM_P2_8, ARM_P2_9, ARM_P2_10, ARM_P2_11,
	ARM_P2_12, ARM_P2_13, ARM_P2_14, ARM_P2_15, ARM_P2_16, ARM_P2_17,
	ARM_P2_18, ARM_P2_19, ARM_P2_20, ARM_P2_21, ARM_P2_22, ARM_P2_23,
	ARM_P2_24, ARM_P2_25, ARM_P2_26, ARM_P2_27, ARM_P2_28, ARM_P2_29,
	ARM_P2_30, ARM_P2_31, ARM_P3_0, ARM_P3_1, ARM_P3_2, ARM_P3_3,
	ARM_P3_4, ARM_P3_5, ARM_P3_6, ARM_P3_7, ARM_P3_8, ARM_P3_9,
	ARM_P3_10, ARM_P3_11, ARM_P3_12, ARM_P3_13, ARM_P3_14, ARM_P3_15,
	ARM_P3_16, ARM_P3_17, ARM_P3_18, ARM_P3_19, ARM_P3_20, ARM_P3_21,
	ARM_P3_22, ARM_P3_23, ARM_P3_24, ARM_P3_25, ARM_P3_26, ARM_P3_27,
	ARM_P3_28, ARM_P3_29, ARM_P3_30, ARM_P3_31, ARM_P4_0, ARM_P4_1,
	ARM_P4_2, ARM_P4_3, ARM_P4_4, ARM_P4_5, ARM_P4_6, ARM_P4_7,
	ARM_P4_8, ARM_P4_9, ARM_P4_10, ARM_P4_11, ARM_P4_12, ARM_P4_13,
	ARM_P4_14, ARM_P4_15, ARM_P4_16, ARM_P4_17, ARM_P4_18, ARM_P4_19,
	ARM_P4_20, ARM_P4_21, ARM_P4_22, ARM_P4_23, ARM_P4_24, ARM_P4_25,
	ARM_P4_26, ARM_P4_27, ARM_P4_28, ARM_P4_29, ARM_P4_30, ARM_P4_31,

	// Internal equivalencies
	TX_0 = ARM_P0_2,
	RX_0 = ARM_P0_1,
	TX_1 = ARM_P0_9,
	RX_1 = ARM_P0_8,

	ADC0 = ARM_P0_30,
	ADC1 = ARM_P0_31,
	ADC2 = ARM_P1_0,
	ADC3 = ARM_P1_1,
	ADC4 = ARM_P1_2,
	ADC5 = ARM_P1_3,
	ADC6 = ARM_P0_4,
	ADC7 = ARM_P0_5,

	SDA0 = ARM_P0_11,
	SCL0 = ARM_P0_10,
	SDA1 = SDA0,
	SCL1 = SCL0,
	SDA2 = SDA0,
	SCL2 = SCL0,
} InternalPinName;

typedef enum PinMode {
	// Input Modes
	PullUp = 0, Repeater = 1, PullNone = 2, PullDown = 3,
	// OpenDrain vs NormalMode
	OpenDrain = 4, NormalMode = 5,
	// Pin Functions, start at 8
	Primary = 8, Alt1, Alt2, Alt3,
	// Input/Output Modes
	Output = 16, Input,
} PinMode;

struct _pin_t {
	uint8_t port;
	uint8_t address;
	uint32_t mask;
};
typedef struct _pin_t Pin_t;

// Port structure for buses
    typedef struct Port_t {
        uint32_t mask;
        uint32_t half_mask[2];
    } Port_t;

// Bus structure
#define MAX_PORTS 3
    struct _pinBus_t {
        Port_t ports[MAX_PORTS];
    };

typedef enum PinName {

    // LPCXPresso pinout
    P4  = ARM_P0_13,
    P5  = ARM_P0_17,
    P6  = ARM_P0_16,
    P7  = ARM_P0_14,
    P8  = ARM_P0_15,
    P9  = ARM_P0_2,
    P10 = ARM_P0_1,
    P11 = ARM_P0_7,
    P12 = ARM_P0_3,
    P13 = ARM_P0_9,
    P14 = ARM_P0_8,
    P15 = ARM_P0_30,
    P16 = ARM_P0_31,
    P17 = ARM_P1_0,
    P18 = ARM_P1_5,
    P19 = ARM_P1_2,
    P20 = ARM_P1_3,
    P21 = ARM_P0_0,
    P22 = ARM_P0_19,
    P23 = ARM_P0_20,
    P24 = ARM_P0_18,
    P25 = ARM_P0_28,
    P26 = ARM_P0_29,
    P27 = ARM_P1_6,

    P38 = ARM_P0_12,
    P39 = ARM_P0_4,
    P40 = ARM_P0_11,
    P41 = ARM_P0_10,
    P42 = ARM_P2_0,
    P43 = ARM_P2_4,
    P44 = ARM_P2_5,
    P45 = ARM_P0_6,
    P46 = ARM_P0_21,
    P47 = ARM_P0_24,
    P48 = ARM_P0_23,
    P49 = ARM_P0_22,
    P50 = ARM_P0_25,
    P51 = ARM_P0_26,
    P52 = ARM_P0_27,

    // Extra pins
    LED1 = ARM_P0_7,
    LED2 = LED1,
    LED3 = LED1,
    LED4 = LED1,

    PAD1  = ARM_P2_8,
    PAD2  = ARM_P2_9,
    PAD3  = ARM_P2_10,
    PAD4  = ARM_P2_11,
    PAD5  = ARM_P2_12,
    PAD6  = ARM_P2_13,
    PAD7  = ARM_P2_14,
    PAD8  = ARM_P2_15,
    PAD9  = ARM_P1_1,
    PAD10 = ARM_P1_4,
    PAD11 = ARM_P2_7,
    PAD12 = ARM_P2_6,
    PAD13 = ARM_P2_3,
    PAD14 = ARM_P2_2,
    PAD15 = ARM_P2_1,
    PAD16 = ARM_P0_5,

} PinName;

typedef __IO uint32_t* IOCON_PORT[32];

static const IOCON_PORT IOCON_REGISTER_MAP[] = {
		{ &(LPC_IOCON->PIO0_0), &(LPC_IOCON->PIO0_1), &(LPC_IOCON->PIO0_2), &(LPC_IOCON->PIO0_3),
				&LPC_IOCON->PIO0_4, &LPC_IOCON->PIO0_5, &LPC_IOCON->PIO0_6, &LPC_IOCON->PIO0_7,
				&LPC_IOCON->PIO0_8, &LPC_IOCON->PIO0_9, &LPC_IOCON->PIO0_10, &LPC_IOCON->PIO0_11,
				&LPC_IOCON->PIO0_12, &LPC_IOCON->RESET_PIO0_13, &LPC_IOCON->PIO0_14, &LPC_IOCON->PIO0_15,
				&LPC_IOCON->PIO0_16, &LPC_IOCON->PIO0_17, &LPC_IOCON->PIO0_18, &LPC_IOCON->PIO0_19,
				&LPC_IOCON->PIO0_20, &LPC_IOCON->PIO0_21, &LPC_IOCON->PIO0_22, &LPC_IOCON->PIO0_23,
				&LPC_IOCON->PIO0_24, &LPC_IOCON->SWDIO_PIO0_25, &LPC_IOCON->SWCLK_PIO0_26, &LPC_IOCON->PIO0_27,
				&LPC_IOCON->PIO0_28, &LPC_IOCON->PIO0_29, &LPC_IOCON->R_PIO0_30, &LPC_IOCON->R_PIO0_31 },
		{ &LPC_IOCON->R_PIO1_0, &LPC_IOCON->R_PIO1_1, &LPC_IOCON->PIO1_2, &LPC_IOCON->PIO1_3,
				&LPC_IOCON->PIO1_4, &LPC_IOCON->PIO1_5, &LPC_IOCON->PIO1_6, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0 },
		{ &LPC_IOCON->PIO2_0, &LPC_IOCON->PIO2_1, &LPC_IOCON->PIO2_2, &LPC_IOCON->PIO2_3,
				&LPC_IOCON->PIO2_4, &LPC_IOCON->PIO2_5, &LPC_IOCON->PIO2_6, &LPC_IOCON->PIO2_7,
				&LPC_IOCON->PIO2_8, &LPC_IOCON->PIO2_9, &LPC_IOCON->PIO2_10, &LPC_IOCON->PIO2_11,
				&LPC_IOCON->PIO2_12, &LPC_IOCON->PIO2_13, &LPC_IOCON->PIO2_14, &LPC_IOCON->PIO2_15,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0 }
		};

#include "inline.h"

Pin_t Pin_Get(PinName pin_name) {
    uint32_t address = pin_name - LPC_GPIO_BASE;
    Pin_t pin = { address / 32, address % 32, 1 << (address % 32) };
    return pin;
}

void Pin_Mode(Pin_t pin, PinMode mode) {
// TODO: lock for concurrency

    switch(mode) {
    case Output:	Pin_Output(pin); break;
    case Input:		Pin_Input(pin); break;
    case PullUp:	_set_pull_up(pin); break;
    case Repeater:	_set_repeater(pin); break;
    case PullNone:	_set_pull_none(pin); break;
    case PullDown:	_set_pull_down(pin); break;
    case OpenDrain:	_set_open_drain(pin); break;
    case NormalMode:
    				_set_normal_mode(pin); break;
    default:		_set_function(pin, mode); break;
    }
}

static inline void Count(uint64_t n) {
	volatile uint64_t i;
	for(i = 0; i < n; i++);
}

/*** **************************************************** ***
static inline void Pin_Set_Input(int port, int pin) {
	LPC_GPIO[port].DIR ^= 1 << pin;
}

static inline int Pin_Read(int port, int pin) {
	return LPC_GPIO[port].PIN & 1 << pin;
}

static inline void Pin_Set_Output(int port, int pin) {
	LPC_GPIO[port].DIR |= 1 << pin;
}

static inline void Pin_On(int port, int pin) {
	LPC_GPIO[port].SET = 1 << pin;
}

static inline void Pin_Off(int port, int pin) {
	LPC_GPIO[port].CLR = 1 << pin;
}

static inline void Pin_Toggle(int port, int pin) {
	LPC_GPIO[port].NOT = 1 << pin;
}
*/

#endif /* IO_H_ */
