/*
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 guruthree
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef _MICROMATRIX_H
#define _MICROMATRIX_H

// GFXcanvas8 treated as 256 greyscale

#include "pico/multicore.h"

#include "IS31FL3730/IS31FL3730.h"
#include "Adafruit_GFX_Library/Adafruit_GFX.h"

class MicroMatrix : public GFXcanvas8
{
 private:
  static const uint8_t SEGMENTWIDTH = 5;
  static const uint8_t NUMSEGMENTS = 6;
  static const uint8_t SEGMENTPADDING = 3;
  static const uint8_t MMWIDTH = (SEGMENTWIDTH+SEGMENTPADDING)*NUMSEGMENTS - SEGMENTPADDING;
  static const uint8_t MMHEIGHT = 7;
  uint8_t _displayBuffer[MMWIDTH*MMHEIGHT];
  uint8_t _lastFrameDrawn; // 1 when ready for next frame
  uint8_t _atlevel;
  struct repeating_timer timer;
  mutex_t mx1; // lock while copying buffers

  IS31FL3730 m1;
  IS31FL3730 m2;
  IS31FL3730 m3;
 public:
  MicroMatrix();
  virtual ~MicroMatrix(void); 
  
  void begin(std::function<void (uint8_t, uint8_t*, size_t)> write_fun);
  void display();
  void clearDisplay(void);
  uint8_t wasLastFrameDrawn(void);
  uint8_t flip(void);
  void forceFlip(void);
  void enableTimer(void);
  void disableTimer(void);

  uint8_t getBrightnessLevels() { return 6; }; 
};

#endif
