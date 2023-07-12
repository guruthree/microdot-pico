#include <cstdlib>
#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

// Drive a pimoroni Microdot pHAT with a Pico
// Demo with a plasma effect based on pico-composite-PAL-colour

#include "random.h"
#include "sine.h"
#include "MicroMatrix.h"

#define SDA_PIN 16
#define SCL_PIN 17

// from pico-composite-PAL-colour
uint64_t time() {
    return to_us_since_boot(get_absolute_time());
}


int main() {
    // Enable UART so we can print status output
//    stdio_init_all();

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

//    printf("i2c initted.\n");


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

    // randomise the plasma
    seed_random_from_rosc();

    int8_t plasmavals[6] = {1, 3, 8, 2, -1, 5};
    uint64_t lastchange = 0;

    while (1) {
        uint64_t now = time();

        // combine (up to) three sine waves to make the plasma effect
        uint32_t phase2 = ((time()*32)/1000000) % 128;
        uint32_t phase3 = ((time()*24)/1000000) % 128;
        int32_t val2, val3;
        uint8_t r; // the final intensity of the plasma

        for (uint8_t xcoord = 0; xcoord < gfx.width(); xcoord++) {
            for (uint8_t ycoord = 0; ycoord < gfx.height(); ycoord++) {

                val2 = (8 * (plasmavals[0] * xcoord + plasmavals[1] * ycoord ) / (1*plasmavals[2])) + phase2;
                val3 = (8 * (plasmavals[3] * xcoord + plasmavals[4] * ycoord ) / (1*plasmavals[5])) + phase3;

                // needs to end up 0-63, sine table ranges 0-128 so mod to get in range
                // the table returns -127 to 127 so add + numthings*128 to remove negatives
                //  then divide by numthings*20 to end up offset 0-12
                // (matching the 12 brightness levels of MicroMatrix)
                r = ((sine_table[val2 % 128] + sine_table[val3 % 128]) + 2*128) / (2*20);
//                r = ((sine_table[val3 % 128]) + 1*128) / (1*20);

                gfx.drawPixel(xcoord, ycoord, r);

            }          
        }

        // change the pattern every Xe6 microseconds (X seconds)
        if ((time() - lastchange) > 20e6) {
            for (uint8_t i = 0; i < sizeof(plasmavals); i++) {
                int8_t l = 5; // scale of the plasma bubbles(?)

                if (i == 0 || i == 3 || i == 6) {
                    plasmavals[i] = randi(0, l);
                }
                else if (i == 1 || i == 4 || i == 7) {
                    plasmavals[i] = plasmavals[i-1]+randi(-l/3, l/3);
                    if (rand() > 0.5)
                        plasmavals[i] = -plasmavals[i];
                }
                else if (i == 2 || i == 5 || i == 8) {
                    plasmavals[i] = randi(1, l);
                }
            }

            lastchange = time();

            // flash the screen to indicate new pattern
            gfx.fillScreen(0);
            gfx.flip();
            sleep_ms(50);
        }


        pinstate = !pinstate;
        gpio_put(LED_PIN, pinstate);

        gfx.flip();
        // sleep nominal Xe3 us (X ms)
        sleep_us(20e3 - (time()-now));
    } 
}
