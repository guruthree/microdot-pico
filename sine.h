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

// sine lookup table 0 to 127 is 0 to 357.19 degrees
// ranging between -127 and 127

// from pico-composite-PAL-colour

int8_t sine_table[128] = { 0x00, 0x06, 0x0c, 0x13, 0x19, 0x1f, 0x25, 0x2b, 0x31, 0x36, 0x3c, 0x41, 0x47, 0x4c, 0x51, 0x55, 0x5a, 0x5e, 0x62, 0x66, 0x6a, 0x6d, 0x70, 0x73, 0x75, 0x78, 0x7a, 0x7b, 0x7d, 0x7e, 0x7e, 0x7f, 0x7f, 0x7f, 0x7e, 0x7e, 0x7d, 0x7b, 0x7a, 0x78, 0x75, 0x73, 0x70, 0x6d, 0x6a, 0x66, 0x62, 0x5e, 0x5a, 0x55, 0x51, 0x4c, 0x47, 0x41, 0x3c, 0x36, 0x31, 0x2b, 0x25, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x00, -0x06, -0x0c, -0x13, -0x19, -0x1f, -0x25, -0x2b, -0x31, -0x36, -0x3c, -0x41, -0x47, -0x4c, -0x51, -0x55, -0x5a, -0x5e, -0x62, -0x66, -0x6a, -0x6d, -0x70, -0x73, -0x75, -0x78, -0x7a, -0x7b, -0x7d, -0x7e, -0x7e, -0x7f, -0x7f, -0x7f, -0x7e, -0x7e, -0x7d, -0x7b, -0x7a, -0x78, -0x75, -0x73, -0x70, -0x6d, -0x6a, -0x66, -0x62, -0x5e, -0x5a, -0x55, -0x51, -0x4c, -0x47, -0x41, -0x3c, -0x36, -0x31, -0x2b, -0x25, -0x1f, -0x19, -0x13, -0x0c, -0x06 };
