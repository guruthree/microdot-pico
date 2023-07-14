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

#include <cstdlib>
#include <cstdio>
#include <functional>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "pio_i2c/pio_i2c.h"

// Drive a pimoroni Microdot pHAT with a Pico
// Demo with a plasma effect based on pico-composite-PAL-colour

#include "random.h"
#include "sine.h"
#include "MicroMatrix.h"

// In the hardware, there's i2c0 and i2c1.
// 0 is on one set of pins, 1 is on the other.
// In the sdk examples there's also a examples/pio/i2c example
#define SDA_PIN0 16
#define SCL_PIN0 17
#define I2C_BUS0 i2c0

#define SDA_PIN1 18
#define SCL_PIN1 19
#define I2C_BUS1 i2c1

// software pio I2C
#define SDA_PIN2 20
#define SCL_PIN2 21

#define SDA_PIN3 26
#define SCL_PIN3 27

// define the gfx, each one of these is a microdot display
// global so both cores can access
#define NUM_GFX 4
MicroMatrix gfx0;
MicroMatrix gfx1;
MicroMatrix gfx2;
MicroMatrix gfx3;

// timer's won't really work with multiple as it takes long enough
// that one timer interrupts the another, so use the other core instead
void core1_entry() {
    while (1) {
        gfx0.display();
        gfx1.display();
        gfx2.display();
        gfx3.display();
        sleep_us(100); // just helps keep things from locking up
    }
}

// from pico-composite-PAL-colour
uint64_t time() {
    return to_us_since_boot(get_absolute_time());
}


int main() {
    // Flash an LED for debugging
    const uint LED_PIN = 15;
    bool pinstate = false;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, pinstate);
    sleep_ms(500);
    gpio_put(LED_PIN, !pinstate);
    sleep_ms(500);
    gpio_put(LED_PIN, !pinstate);

    // setup the I2C for each Micro Dot pHAT

    // any higher then 2.4 MHz I2C and the IS31FL3730
    // chips wouldn't reliably respond for me
    i2c_init(I2C_BUS0, 2400000);
    gpio_set_function(SDA_PIN0, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN0, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN0);
    gpio_pull_up(SCL_PIN0);

    i2c_init(I2C_BUS1, 2400000);
    gpio_set_function(SDA_PIN1, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN1, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN1);
    gpio_pull_up(SCL_PIN1);

    PIO gfx2_pio = pio0;
    uint gfx2_sm = 0;
    uint gfx2_offset = pio_add_program(gfx2_pio, &i2c_program);
    i2c_program_init(gfx2_pio, gfx2_sm, gfx2_offset, SDA_PIN2, SCL_PIN2);

    // the PIO program uses so much code that the second one needs to be on another pio?
    PIO gfx3_pio = pio1;
    uint gfx3_sm = 1;
    uint gfx3_offset = pio_add_program(gfx3_pio, &i2c_program);
    i2c_program_init(gfx3_pio, gfx3_sm, gfx3_offset, SDA_PIN3, SCL_PIN3);

    // put all the gfx in an array for easier manipulation
    MicroMatrix *gfxs[NUM_GFX] = {&gfx0, &gfx1, &gfx2, &gfx3};

    // assign I2C to each gfx
    gfx0.begin(std::function<void (uint8_t, uint8_t*, size_t)> ([](uint8_t addr, uint8_t *src, size_t len){
        i2c_write_blocking(I2C_BUS0, addr, src, len, false);
    }));

    gfx1.begin(std::function<void (uint8_t, uint8_t*, size_t)> ([](uint8_t addr, uint8_t *src, size_t len){
        i2c_write_blocking(I2C_BUS1, addr, src, len, false);
    }));

    gfx2.begin(std::function<void (uint8_t, uint8_t*, size_t)> ([&gfx2_pio, &gfx2_sm](uint8_t addr, uint8_t *src, size_t len){
        pio_i2c_write_blocking(gfx2_pio, gfx2_sm, addr, src, len);
    }));

    gfx3.begin(std::function<void (uint8_t, uint8_t*, size_t)> ([&gfx3_pio, &gfx3_sm](uint8_t addr, uint8_t *src, size_t len){
        pio_i2c_write_blocking(gfx3_pio, gfx3_sm, addr, src, len);
    }));

    // flash the panels for a second to show they're working
    for (uint8_t i = 0; i < NUM_GFX; i++) {
        gfxs[i]->fillScreen(255);
        gfxs[i]->forceFlip();
        gfxs[i]->display();
    }
    sleep_ms(1000);
    for (uint8_t i = 0; i < NUM_GFX; i++) {
        gfxs[i]->fillScreen(0);
        gfxs[i]->forceFlip();
        gfxs[i]->display();
    }

    // start the automated display routine
    multicore_launch_core1(core1_entry);

    // work on a gobal canvas that we'll copy onto each invidiual display
    #define XOFFSET 7
    #define YOFFSET 10
    GFXcanvas8 canvas(gfx0.width()*2+XOFFSET, gfx0.height()*2+YOFFSET);

    // randomise the plasma
    seed_random_from_rosc();

    int8_t plasmavals[6] = {1, 3, 8, 2, -1, 4};
    uint64_t lastchange = 0;

    while (1) {
        uint64_t now = time();

        // combine (up to) three sine waves to make the plasma effect
        uint32_t phase2 = ((time()*32)/1000000) % 128;
        uint32_t phase3 = ((time()*24)/1000000) % 128;
        int32_t val2, val3;
        uint8_t r; // the final intensity of the plasma

        for (uint8_t xcoord = 0; xcoord < canvas.width(); xcoord++) {
            for (uint8_t ycoord = 0; ycoord < canvas.height(); ycoord++) {

                val2 = (5 * (plasmavals[0] * xcoord + plasmavals[1] * ycoord ) / (1*plasmavals[2])) + phase2;
                val3 = (5 * (plasmavals[3] * xcoord + plasmavals[4] * ycoord ) / (1*plasmavals[5])) + phase3;

                // needs to end up 0-63, sine table ranges 0-128 so mod to get in range
                // the table returns -127 to 127 so add + numthings*128 to remove negatives
                //  then divide by numthings*20 to end up offset 0-12
                // (matching the 12 brightness levels of MicroMatrix)
                r = ((sine_table[val2 % 128] + sine_table[val3 % 128]) + 2*128) / (2*30);
//                r = ((sine_table[val3 % 128]) + 1*128) / (1*30);

                canvas.drawPixel(xcoord, ycoord, r);

            }
        }

        // change the pattern every Xe6 microseconds (X seconds)
        if ((time() - lastchange) > 8e6) {
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
            for (uint8_t i = 0; i < NUM_GFX; i++) {
                gfxs[i]->fillScreen(0);
                gfxs[i]->flip();
            }
            sleep_ms(50);
        }

        // copy onto the invidial microdot panels
        for (uint8_t xcoord = 0; xcoord < gfx0.width(); xcoord++) {
            for (uint8_t ycoord = 0; ycoord < gfx0.height(); ycoord++) {
                // recall top left is (0,0)

                // top left
                gfx0.drawPixel(xcoord, ycoord, canvas.getPixel(xcoord, ycoord));

                // top right
                gfx1.drawPixel(xcoord, ycoord, canvas.getPixel(xcoord+gfx0.width()+XOFFSET, ycoord));

                // bottom left, upside down
                gfx2.drawPixel(gfx0.width()-xcoord-1, gfx0.height()-ycoord-1, canvas.getPixel(xcoord, ycoord+gfx0.height()+YOFFSET));

                // bottom right, upside down
                gfx3.drawPixel(gfx0.width()-xcoord-1, gfx0.height()-ycoord-1, canvas.getPixel(xcoord+gfx0.width()+XOFFSET, ycoord+gfx0.height()+YOFFSET));
            }
        }

        pinstate = !pinstate;
        gpio_put(LED_PIN, pinstate);

        for (uint8_t i = 0; i < NUM_GFX; i++) {
            gfxs[i]->flip();
        }
        // sleep nominal Xe3 us (X ms)
        sleep_us(10e3 - (time()-now));
    } 
}
