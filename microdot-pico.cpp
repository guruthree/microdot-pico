#include "pico/stdlib.h"
#include <cstdlib>
#include <cstdio>
#include "hardware/gpio.h"
#include "hardware/i2c.h"

//#include "gfxfont.h"
//#include "IS31FL3730/IS31FL3730.h"
#include "MicroMatrix.h"

#define SDA_PIN 16
#define SCL_PIN 17

//#include "gfxfont.h"

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}


int main() {
    // Enable UART so we can print status output
    stdio_init_all();

    // built in LED for flashing
    const uint LED_PIN = 15;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    sleep_ms(1000);
    gpio_put(LED_PIN, 1);
    sleep_ms(500);
    gpio_put(LED_PIN, 0);

//    sleep_ms(10000);
//    gpio_put(LED_PIN, 1);
//    sleep_ms(500);
//    gpio_put(LED_PIN, 0);

    // this is hardware gpio, there's i2c0 and i2c1
    // 0 is on one set of pins, 1 is on the other
    // in the sdk examples there's also a pio-i2c example
    i2c_init(i2c0, 100000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("i2c initted.\n");

    bool pinstate = false;

/*    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c0, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");

        pinstate = !pinstate;
        gpio_put(LED_PIN, pinstate);
        sleep_ms(25);
    }
    printf("Done.\n"); */



//    IS31FL3730 m1;
//printf("writing\n"); 
    auto wrapper0 = [](uint8_t addr, uint8_t *src, size_t len){i2c_write_blocking(i2c0, addr, src, len, false);};
//    m1.begin(0x61, wrapper0);
//    m1.set_le(0xE);
//    m1.clear();
    MicroMatrix gfx;
    gfx.begin(wrapper0);

    uint8_t atx = 0;
    uint8_t aty = 0;
//    bool pinstate = false;
    while (1) {

//        m1.clear();
//        m1.set_pixel(0, atx, aty, 1);
//      m1.update();

//if (gfx.wasLastFrameDrawn()) {
        gfx.fillScreen(0);
       gfx.drawPixel(atx, aty, 100);
 
//        aty++;
        atx++;
//        if (aty == 8) {
//           aty = 0;
//            atx++;
//        }
        if (atx == 30) {
            atx = 0;
        aty++;
        }
        if (aty == 8) {
           aty = 0;
        }
        printf("%d, %d.\n", atx, aty);

        pinstate = !pinstate;
        gpio_put(LED_PIN, pinstate);

//        gfx.flip();
//}
        gfx.forceFlip();
        gfx.display();

printf("\n");
//        sleep_ms(50);
    } 
}
