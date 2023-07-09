#include "pico/stdlib.h"
#include <cstdlib>
#include <cstdio>
#include "hardware/gpio.h"
#include "hardware/i2c.h"

// Drive a pimoroni Microdot pHAT with a Pico

#include "MicroMatrix.h"

#define SDA_PIN 16
#define SCL_PIN 17


uint64_t time() {
    return to_us_since_boot(get_absolute_time());
}


int main() {
    // Enable UART so we can print status output
    stdio_init_all();

    // Flash an LED for debugging
    const uint LED_PIN = 15;
    bool pinstate = false;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, pinstate);
    sleep_ms(1000);
    gpio_put(LED_PIN, !pinstate);
    sleep_ms(500);
    gpio_put(LED_PIN, !pinstate);

    // This is hardware gpio, there's i2c0 and i2c1.
    // 0 is on one set of pins, 1 is on the other.
    // In the sdk examples there's also a examples/pio/i2c example
    i2c_init(i2c0, 2400000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("i2c initted.\n");


    MicroMatrix gfx;
    gfx.begin([](uint8_t addr, uint8_t *src, size_t len){
        i2c_write_blocking(i2c0, addr, src, len, false);
    });
    gfx.fillScreen(255);
    gfx.forceFlip();
    gfx.display();
    sleep_ms(1000);
    gfx.fillScreen(0);
    gfx.forceFlip();
    gfx.display();

    gfx.enableTimer();

    uint8_t atx = 0;
    uint8_t aty = 0;
    while (1) {
        gfx.drawPixel(atx, aty, (atx)/3+1); // brightness matches coordinates
        if ((aty % 2 == 0) && (atx % 2 == 0))
            gfx.drawPixel(atx, aty, 1);
 
        atx++;
        if (atx == 30) {
            atx = 0;
            aty++;
        }
        if (aty == 8) {
           aty = 0;
        }

        pinstate = !pinstate;
        gpio_put(LED_PIN, pinstate);

        gfx.flip();
        sleep_ms(50);
    } 
}
