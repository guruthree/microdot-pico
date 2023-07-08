#ifndef _MICROMATRIX_H
#define _MICROMATRIX_H

#include <Adafruit_GFX_Library/Adafruit_GFX.h>

#define MATRIXWIDTH 30
#define MATRIXHEIGHT 7

class MicroMatrix : public GFXcanvas8
{

private:
//  uint8_t _displayBuffer[MATRIXWIDTH*MATRIXHEIGHT];
  uint8_t _lastFrameDrawn; // 1 when ready for next frame
  uint8_t _atlevel;

public:
  MicroMatrix();
  virtual ~MicroMatrix(void); 
  
  void begin(void (*write_fun)(uint8_t, uint8_t*, size_t));
  void display();
  void clearDisplay(void);
  uint8_t wasLastFrameDrawn(void);
  uint8_t flip(void);
  void forceFlip(void);

  uint8_t getBrightnessLevels() { return 10; };
  
};

#endif
