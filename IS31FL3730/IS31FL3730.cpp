// https://github.com/jbkim/IS31FL3730

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "hardware/i2c.h"
#include "IS31FL3730.h"

// IS31FL3730 Driver

// Wire replaced with stored function pointer to a write function
// Split out set_pixel into functions for each matrix
// Removed decimal, row, and col functions as these are unused

bool IS31FL3730::begin(uint8_t addr, void (*write_fun)(uint8_t, uint8_t*, size_t)) {
  _i2caddr = addr;
  _write_fun = write_fun;

  writeRegister8(ISSI_REG_CONFIG, Default_Mode);
  writeRegister8(ISSI_REG_BRIGHT, _brightness);

  set_le(Default_reg_le);
  set_bright(_brightness);

  memset(_buf_matrix_1, 0, sizeof( _buf_matrix_1));
  memset(_buf_matrix_2, 0, sizeof( _buf_matrix_2));

  return true;
}

// ~330 us @ 2.4 MHz I2C, ~320 uz @ 3 MHz
void IS31FL3730::update(void) {
  uint8_t out[9];

  out[0] = ISSI_REG_MATRIX1;
  memcpy(out+1, _buf_matrix_1, 8);
  _write_fun(_i2caddr, out, sizeof(out));
  
  out[0] = ISSI_REG_MATRIX2;
  memcpy(out+1, _buf_matrix_2, 8);
  _write_fun(_i2caddr, out, sizeof(out));

  writeRegister8(ISSI_REG_UPDATE, 0x01);
}

void IS31FL3730::clear(void) {
  memset(_buf_matrix_1, 0, sizeof( _buf_matrix_1));
  memset(_buf_matrix_2, 0, sizeof( _buf_matrix_2));
  update();
}

void IS31FL3730::set_le(uint8_t light_effect_register) {
  writeRegister8(ISSI_REG_LE, light_effect_register);
}

void IS31FL3730::set_bright(uint8_t bright) {
  _brightness = bright;
  if (_brightness > 127) _brightness = 127;

  writeRegister8(ISSI_REG_BRIGHT, _brightness);
}

void IS31FL3730::set_pixel1(uint8_t x, uint8_t y, uint8_t c) {
  if (c == 1)
    _buf_matrix_1[y] |= (1 << x);
  else
    _buf_matrix_1[y] &= ~(1 << x);
}

void IS31FL3730::set_pixel2(uint8_t x, uint8_t y, uint8_t c) {
  if (c == 1)
    _buf_matrix_2[x] |= (1 << y);
  else
    _buf_matrix_2[x] &= ~(1 << y);
}

void IS31FL3730::writeRegister8(uint8_t reg, uint8_t data) {
  uint8_t out[2] = {reg, data};
  _write_fun(_i2caddr, out, sizeof(out));
}

