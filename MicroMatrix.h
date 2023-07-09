#ifndef _MICROMATRIX_H
#define _MICROMATRIX_H

// GFXcanvas8 treated as 256 greyscale

#include "pico/multicore.h"

#include <Adafruit_GFX_Library/Adafruit_GFX.h>

#define MATRIXWIDTH 30
#define MATRIXHEIGHT 7

class MicroMatrix : public GFXcanvas8
{
 private:
  uint8_t _displayBuffer[MATRIXWIDTH*MATRIXHEIGHT];
  uint8_t _lastFrameDrawn; // 1 when ready for next frame
  uint8_t _atlevel;
  struct repeating_timer timer;
  mutex_t mx1; // lock while copying buffers
 public:
  MicroMatrix();
  virtual ~MicroMatrix(void); 
  
  void begin(void (*write_fun)(uint8_t, uint8_t*, size_t));
  void display();
  void clearDisplay(void);
  uint8_t wasLastFrameDrawn(void);
  uint8_t flip(void);
  void forceFlip(void);
  void enableTimer(void);
  void disableTimer(void);

  uint8_t getBrightnessLevels() { return 12; }; 
};

#endif
