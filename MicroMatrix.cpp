#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "IS31FL3730/IS31FL3730.h"
#include "MicroMatrix.h"

#define M1 0
#define M2 1

IS31FL3730 m1;
IS31FL3730 m2;
IS31FL3730 m3;

MicroMatrix::MicroMatrix() : GFXcanvas8(MATRIXWIDTH, MATRIXHEIGHT), _lastFrameDrawn(1), _atlevel(1)
{
  //_displayBuffer = (uint8_t*)malloc(MATRIXWIDTH * MATRIXHEIGHT * sizeof(uint8_t));
//  memset(_displayBuffer, 0, MATRIXWIDTH * MATRIXHEIGHT * sizeof(uint8_t));
}

MicroMatrix::~MicroMatrix()
{
//  free(_displayBuffer);
}

void MicroMatrix::begin(void (*write_fun)(uint8_t, uint8_t*, size_t))
{
  m1.begin(0x61, write_fun);
  m2.begin(0x62, write_fun);
  m3.begin(0x63, write_fun);

  // set Lighting Effect Register to 10mA
  m1.set_le(0xE);
  m2.set_le(0xE);
  m3.set_le(0xE);
  //m1.set_bright(20);

  m1.clear();
  m2.clear();
  m3.clear();
}

void MicroMatrix::display()
{
printf("display\n");
  for (uint16_t y = 0; y < 7; y++) {
    for (uint16_t x = 0; x < 5; x++) {
//      m1.set_pixel(M1, 4-x, 6-y, getRawPixel(x, y));
//      m1.set_pixel(M2, 4-x, 6-y, getRawPixel(x+5, y));
//      m2.set_pixel(M1, 4-x, 6-y, getRawPixel(x+10, y));
//      m2.set_pixel(M2, 4-x, 6-y, getRawPixel(x+15, y));
//      m3.set_pixel(M1, 4-x, 6-y, getRawPixel(x+20, y));
//      m3.set_pixel(M2, 4-x, 6-y, getRawPixel(x+25, y));
      
//_displayBuffer[(whichRow * WIDTH) + (i * 8) + j]
// [(x / 8) + y * ((WIDTH + 7) / 8)]
//      m1.set_pixel(M1, 4-x, 6-y, _displayBuffer[x + y * WIDTH] > _atlevel);
//      m1.set_pixel(M2, 4-x, 6-y, _displayBuffer[(x+5) + y * WIDTH] > _atlevel);
//      m2.set_pixel(M1, 4-x, 6-y, _displayBuffer[(x+10) + y * WIDTH] > _atlevel);
//      m2.set_pixel(M2, 4-x, 6-y, _displayBuffer[(x+15) + y * WIDTH] > _atlevel);
//      m3.set_pixel(M1, 4-x, 6-y, _displayBuffer[(x+20) + y * WIDTH] > _atlevel);
//      m3.set_pixel(M2, 4-x, 6-y, _displayBuffer[(x+25) + y * WIDTH] > _atlevel);
      m1.set_pixel(M1, 4-x, y, getBuffer()[x + y * WIDTH] > _atlevel);
      m1.set_pixel(M2, 4-x, y, getBuffer()[(x+5) + y * WIDTH] > _atlevel);
      m2.set_pixel(M1, 4-x, y, getBuffer()[(x+10) + y * WIDTH] > _atlevel);
      m2.set_pixel(M2, 4-x, y, getBuffer()[(x+15) + y * WIDTH] > _atlevel);
      m3.set_pixel(M1, 4-x, y, getBuffer()[(x+20) + y * WIDTH] > _atlevel);
      m3.set_pixel(M2, 4-x, y, getBuffer()[(x+25) + y * WIDTH] > _atlevel);
    }
  }
//m1.set_pixel(M1, 0, 1, 1);

/*char temp[100];
  for (uint16_t y = 0; y < HEIGHT; y++) {
    for (uint16_t x = 0; x < WIDTH; x++) {
//        sprintf(temp, "%s%d", temp, getBuffer()[(x+20) + y * WIDTH]);
        printf("full map %d, %d is \n", x, y);//, getBuffer()[x + y * WIDTH]);
}
//    printf("%s\n", temp);
}*/

  m1.update();
  m2.update();
  m3.update();
  _lastFrameDrawn = 1;
  
  _atlevel++;
  if (_atlevel == getBrightnessLevels())
    _atlevel = 0;
printf("end display\n");
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
//  noInterrupts();
printf("forceflip\n");

//memcpy(_displayBuffer, getBuffer(), WIDTH * HEIGHT * sizeof(uint8_t));

//for (int i = 0; i < WIDTH * HEIGHT * sizeof(uint8_t); i++)
//    _displayBuffer[i] =  getBuffer()[i];

//_displayBuffer[1] = 100;

//_displayBuffer[1] = getBuffer()[1];

//printf("buffer2: %p\n", getBuffer());
//   printf("_dB is %d\n", _displayBuffer[1]);
//   printf("gB) is %d\n", getBuffer()[1]);

//printf("val %d\n", getPixel(1, 1));

  _lastFrameDrawn = 0; // we now need to draw this frame
//  interrupts();
}
