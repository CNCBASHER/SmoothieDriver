/*        This file is part of Smoothie (http://smoothieware.org/). Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>. */

#ifndef I2C_H_
#define I2C_H_

#ifdef __USE_CMSIS
#include "LPC122x.h"
#endif

#define I2C_SDA ARM_P0_10
#define I2C_SCL ARM_P0_11

static Pin_t _i2c_sda_pin;
static Pin_t _i2c_scl_pin;

static int _i2c_state = -1;
static int _i2c_tlen = 0;
#define I2C_BUFFER_SIZE 16
static unsigned char _i2c_buf[I2C_BUFFER_SIZE];

static inline void _i2c_bus_error(void) {
	_i2c_state = -1;
	LPC_I2C->CONSET = 0x14;
	LPC_I2C->CONCLR = 0x08;
}

static inline void _i2c_sr_addressed(void) {
	int i;
	_i2c_state = 0;
	for(i=0;i<I2C_BUFFER_SIZE;i++) _i2c_buf[i] = 0x00;
	LPC_I2C->CONSET = 0x04;
	LPC_I2C->CONCLR = 0x08;
}

static inline void _i2c_sr_dataready(void) {
	if(_i2c_state >= I2C_BUFFER_SIZE) {
		LPC_I2C->CONCLR = 0x0C;
		return;
	}
	if(_i2c_state >= 0) {
		_i2c_buf[_i2c_state] = LPC_I2C->DAT;
		_i2c_state++;
	}
	LPC_I2C->CONSET = 0x04;
	LPC_I2C->CONCLR = 0x08;
}

static inline void _i2c_sr_stop(void) {
	_i2c_state = -1;
	LPC_I2C->CONSET = 0x04;
	LPC_I2C->CONCLR = 0x08;
	// TODO: queue up actually looking at the completed receive buffer
}

static inline void _i2c_st_addressed(void) {
	if(_i2c_state > 0) {
		// TODO: GET SOME REAL DATA based on _i2c_buf[0]
		switch(_i2c_buf[0]) {
		default: // load buf with fake data
			_i2c_tlen=10;
			if(_i2c_buf[0] < _i2c_tlen) i2c_tlen = _i2c_buf[0];
			_i2c_buf[0] = 'S';
			_i2c_buf[1] = 'm';
			_i2c_buf[2] = 'o';
			_i2c_buf[3] = 'o';
			_i2c_buf[4] = 't';
			_i2c_buf[5] = 'h';
			_i2c_buf[6] = 'i';
			_i2c_buf[7] = 'e';
			_i2c_buf[8] = '!';
			_i2c_buf[9] = '\0';
			break;
		}

		LPC_I2C->DAT = _i2c_buf[0];
		_i2c_state = 1;
		if(_i2c_tlen < 2) {
			// no more data to send so nack and bail
			LPC_I2C->CONCLR = 0x0C;
			return;
		}
	} else {
		// no command has been sent by ccu so send 0x00, nack and bail
		LPC_I2C->DAT = 0x00;
		LPC_I2C->CONCLR = 0x0C;
		return;
	}
	// more data so ack
	LPC_I2C->CONSET = 0x04;
	LPC_I2C->CONCLR = 0x08;
}

static inline void _i2c_st_datasent(void) {
	LPC_I2C->DAT = _i2c_buf[_i2c_state];
	_i2c_state++;
	if(_i2c_state >= _i2c_tlen) {
		// no more data to send so nack and bail
		LPC_I2C->CONSET = 0x0C;
		return;
	}
	// more data so ack
	LPC_I2C->CONSET = 0x04;
	LPC_I2C->CONCLR = 0x08;
}

static inline void _i2c_st_stop(void) {
	_i2c_state = -1;
	LPC_I2C->CONSET = 0x04;
	LPC_I2C->CONCLR = 0x08;
}

static inline void _i2c_stop(void) {
	// TODO: Test that this isn't broken
	_i2c_state = -1;
	LPC_I2C->CONSET = 0x14;
	LPC_I2C->CONCLR = 0x08;
}

void I2C_IRQHandler(void) {
	switch(LPC_I2C->STAT) {
	case 0x00: _i2c_bus_error(); break;
	// master not implemented
	// slave receiver states
	case 0x60: _i2c_sr_addressed(); break;
	case 0x80: _i2c_sr_dataready(); break;
	case 0x88: _i2c_sr_stop(); break;
	case 0xA0: _i2c_sr_stop(); break;
	// slave transmitter states
	case 0xA8: _i2c_st_addressed(); break;
	case 0xB8: _i2c_st_datasent(); break;
	case 0xC0: _i2c_st_stop(); break;
	case 0xC8: _i2c_st_stop(); break;
	default: _i2c_stop(); break;
	}
}

static inline void I2C_Init(uint8_t slave_address) {
	// enable the i2c subsystem
	LPC_SYSCON->SYSAHBCLKCTRL |= 1 << 5;
	NVIC_EnableIRQ(I2C_IRQn);
	// build pins
	_i2c_sda_pin = Pin_Get(I2C_SDA);
	_i2c_scl_pin = Pin_Get(I2C_SCL);
	// configure pins
	*IOCON_REGISTER_MAP[_i2c_sda_pin.port][_i2c_sda_pin.address] |= 0x2 | 1 << 10;
	*IOCON_REGISTER_MAP[_i2c_scl_pin.port][_i2c_scl_pin.address] |= 0x2 | 1 << 10;
	// configure the i2c subsystem and turn it on
	LPC_I2C->ADR0 = slave_address << 1;
	LPC_I2C->CONSET = 1 << 6 | 1 << 2;
}

static inline int I2C_Read() {
	return -1;
}

static inline int I2C_Write() {
	return -1;
}

#endif /* I2C_H_ */
