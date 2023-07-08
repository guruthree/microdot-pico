// https://github.com/jbkim/IS31FL3730

#include <cstring>
#include <cstdlib>

#include "hardware/i2c.h"
#include "IS31FL3730.h"

// IS31FL3730 Driver

// Wire replaced with stored function pointer to a write function

#define Default_Mode      0x18
#define Default_reg_le    0xE
#define Matrix_1          0
#define Matrix_2          1

bool IS31FL3730::begin(uint8_t addr, void (*write_fun)(uint8_t, uint8_t*, size_t)) {

  _i2caddr = addr;
  _brightness = 127;
  _write_fun = write_fun;

  writeRegister8(ISSI_REG_CONFIG, Default_Mode);
  writeRegister8(ISSI_REG_BRIGHT, _brightness);

  set_le(Default_reg_le);

  for (uint8_t i=0; i < 8; i++) {
      _buf_matrix_1[i] = 0;
      _buf_matrix_2[i] = 0;
  }

  return true;
}

void IS31FL3730::update(void) {

  uint8_t out[9];

  memset(out, 0, sizeof(out));
  out[0] = ISSI_REG_MATRIX1;
  memcpy(out+1, _buf_matrix_1, 8);
  _write_fun(_i2caddr, out, sizeof(out));
  
  memset(out, 0, sizeof(out));
  out[0] = ISSI_REG_MATRIX2;
  memcpy(out+1, _buf_matrix_2, 8);
  _write_fun(_i2caddr, out, sizeof(out));

  writeRegister8(ISSI_REG_UPDATE, 0x01);  
}

void IS31FL3730::clear(void) {

  for (uint8_t i=0; i < 8; i++) {
      _buf_matrix_1[i] = 0;
      _buf_matrix_2[i] = 0;
  }
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

void IS31FL3730::set_decimal(uint8_t matrix, uint8_t c) {
  if (matrix == Matrix_1) {
    if (c == 1) _buf_matrix_1[6] |= 0x80;
    else _buf_matrix_1[6] &= 0x7F;

  }else if (matrix == Matrix_2) {
    if (c == 1) _buf_matrix_2[7] |= 0x40;
    else _buf_matrix_2[7] &= 0xBF;
  }

}

void IS31FL3730::set_pixel(uint8_t matrix, uint8_t x, uint8_t y, uint8_t c) {
  if (matrix == Matrix_1) {
    if (c == 1) _buf_matrix_1[y] |= (1 << x);
    else _buf_matrix_1[y] &= ~(1 << x);

  }else if (matrix == Matrix_2) {
    if (c == 1) _buf_matrix_2[x] |= (1 << y);
    else _buf_matrix_2[x] &= ~(1 << y);
  }
}

void IS31FL3730::set_col(uint8_t matrix, uint8_t col, uint8_t data) {
  for (uint8_t y = 0; y < 7; y++)
    set_pixel(matrix, col, y, (data & (1 << y)) > 0);
}

void IS31FL3730::set_row(uint8_t matrix, uint8_t row, uint8_t data) {
  for (uint8_t x = 0; x < 5; x++)
    set_pixel(matrix, x, row, (data & (1 << (4-x))) > 0);
}

void IS31FL3730::set_data(uint8_t matrix, uint8_t *data) {
  for (uint8_t y = 0; y < 7; y++)
    set_row(matrix, y, data[y]);
}

void IS31FL3730::writeRegister8(uint8_t reg, uint8_t data) {

  uint8_t out[2] = {reg, data};
  _write_fun(_i2caddr, out, sizeof(out));
}

