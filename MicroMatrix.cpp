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

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "MicroMatrix.h"

MicroMatrix::MicroMatrix() : GFXcanvas8(MMWIDTH, MMHEIGHT), _lastFrameDrawn(1), _atlevel(1)
{
  memset(_displayBuffer, 0, MMHEIGHT * MMHEIGHT * sizeof(uint8_t));
}

MicroMatrix::~MicroMatrix()
{
}

void MicroMatrix::begin(void (*write_fun)(uint8_t, uint8_t*, size_t))
{
  m1.begin(0x61, write_fun);
  m2.begin(0x62, write_fun);
  m3.begin(0x63, write_fun);

  // set Lighting Effect Register to 10mA
  m1.set_le(0xB); // 20 mA
  m2.set_le(0xB);
  m3.set_le(0xB);

  m1.set_bright(20);
  m2.set_bright(20);
  m3.set_bright(20);

  m1.clear();
  m2.clear();
  m3.clear();

  mutex_init(&mx1);
}

// ~50 us + the time the update takes
void MicroMatrix::display()
{
  // read off of display buffer as soon as possible to keep to timing for display
  // so block until we can
  mutex_enter_blocking(&mx1); 
  for (uint16_t y = 0; y < 7; y++) {
    uint16_t w = y * WIDTH;
    for (uint16_t x = 0; x < 5; x++) {
      m1.set_pixel1(x, y, _displayBuffer[(x+(SEGMENTWIDTH+SEGMENTPADDING)*5) + w] > _atlevel);
      m1.set_pixel2(x, y, _displayBuffer[(x+(SEGMENTWIDTH+SEGMENTPADDING)*4) + w] > _atlevel);
      m2.set_pixel1(x, y, _displayBuffer[(x+(SEGMENTWIDTH+SEGMENTPADDING)*3) + w] > _atlevel);
      m2.set_pixel2(x, y, _displayBuffer[(x+(SEGMENTWIDTH+SEGMENTPADDING)*2) + w] > _atlevel);
      m3.set_pixel1(x, y, _displayBuffer[(x+(SEGMENTWIDTH+SEGMENTPADDING)) + w] > _atlevel);
      m3.set_pixel2(x, y, _displayBuffer[x + w] > _atlevel);
    }
  }
  mutex_exit(&mx1);

  m1.update();
  m2.update();
  m3.update();

  _lastFrameDrawn = 1;
  
  _atlevel++;
  if (_atlevel == getBrightnessLevels())
    _atlevel = 0;
}

void MicroMatrix::clearDisplay()
{
  fillScreen(0);
  display();
}

uint8_t MicroMatrix::wasLastFrameDrawn()
{
  return _lastFrameDrawn;
}

uint8_t MicroMatrix::flip()
{
  // note, not actually a flip because buffer is private in GFXcanvas8
  if (_lastFrameDrawn == 1)
  {
    forceFlip();
    return 0; // SUCCESS
  }
  return 1; // FAIL
}

void MicroMatrix::forceFlip()
{
  bool locked = mutex_enter_timeout_ms(&mx1, 2); // request lock
  if (locked) {
    memcpy(_displayBuffer, getBuffer(), WIDTH * HEIGHT * sizeof(uint8_t));
    _lastFrameDrawn = 0; // we now need to draw this frame
    mutex_exit(&mx1); // unlock
  }
}

void MicroMatrix::enableTimer()
{
  repeating_timer_callback_t loopTask = [](repeating_timer_t *rt)->bool
  {
    MicroMatrix* instance = (MicroMatrix*)(rt->user_data);
    instance->display();
    return true;
  };
  add_repeating_timer_us(1050, loopTask, this, &timer);
}

void MicroMatrix::disableTimer()
{
  cancel_repeating_timer(&timer);
}
