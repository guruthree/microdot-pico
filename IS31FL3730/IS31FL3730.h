// https://github.com/jbkim/IS31FL3730

#ifndef _IS31FL3730_H_
#define _IS31FL3730_H_

#include "hardware/i2c.h"

// IS31FL3730 Driver

// Wire replaced with stored function pointer to a write function

#define ISSI_ADDR_DEFAULT 0x61

#define ISSI_REG_CONFIG    0x00
#define ISSI_REG_LE        0x0D
#define ISSI_REG_BRIGHT    0x19
#define ISSI_REG_UPDATE    0x0C

#define ISSI_REG_MATRIX1   0x01    
#define ISSI_REG_MATRIX2   0x0E    

#define Default_Mode       0x18
#define Default_reg_le     0xE // 35 mA
#define Default_brightness 127

class IS31FL3730 {
 public:  
  bool begin(uint8_t addr, void (*write_fun)(uint8_t, uint8_t*, size_t));
  void update(void);
  void clear(void);
  void set_bright(uint8_t bright);
  void set_le(uint8_t light_effect_register);
  void set_pixel1(uint8_t x, uint8_t y, uint8_t c);
  void set_pixel2(uint8_t x, uint8_t y, uint8_t c);
 private:
  void writeRegister8(uint8_t reg, uint8_t data);
  uint8_t _i2caddr, _brightness = Default_brightness;
  uint8_t _buf_matrix_1[8];
  uint8_t _buf_matrix_2[8];
  void (*_write_fun)(uint8_t, uint8_t*, size_t);
};

#endif
