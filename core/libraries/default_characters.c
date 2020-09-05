//
// Copyright 2016 Timo Kloss
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "default_characters.h"

uint8_t DefaultCharacters[][16] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x18, 0x14, 0x04, 0x04, 0x0C, 0x10, 0x0C, 0x00, 0x00, 0x0C, 0x1C, 0x1C, 0x0C, 0x08, 0x0C},
    {0x00, 0x48, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x24, 0x7E, 0x36, 0x12, 0x00, 0x00, 0x00},
    {0x00, 0x24, 0x60, 0x1B, 0x12, 0x40, 0x1B, 0x12, 0x00, 0x00, 0x1E, 0x3F, 0x36, 0x3E, 0x3F, 0x12},
    {0x00, 0x08, 0x30, 0x27, 0x10, 0x21, 0x17, 0x04, 0x00, 0x00, 0x0E, 0x1F, 0x1E, 0x1F, 0x1F, 0x04},
    {0x00, 0x40, 0x11, 0x32, 0x04, 0x0C, 0x11, 0x23, 0x00, 0x22, 0x75, 0x3A, 0x14, 0x2A, 0x57, 0x23},
    {0x00, 0x10, 0x2A, 0x02, 0x10, 0x13, 0x00, 0x1D, 0x00, 0x0C, 0x1E, 0x3A, 0x7E, 0x77, 0x3A, 0x1D},
    {0x00, 0x10, 0x04, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x3C, 0x18, 0x00, 0x00, 0x00},
    {0x00, 0x08, 0x16, 0x0C, 0x08, 0x00, 0x00, 0x06, 0x00, 0x04, 0x0E, 0x3C, 0x38, 0x18, 0x0C, 0x06},
    {0x00, 0x30, 0x00, 0x00, 0x02, 0x06, 0x0C, 0x18, 0x00, 0x00, 0x18, 0x0C, 0x0E, 0x1E, 0x3C, 0x18},
    {0x00, 0x00, 0x20, 0x12, 0x40, 0x27, 0x08, 0x12, 0x00, 0x00, 0x04, 0x0A, 0x3E, 0x3F, 0x2C, 0x12},
    {0x00, 0x00, 0x18, 0x14, 0x40, 0x27, 0x04, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x3E, 0x3F, 0x1C, 0x0C},
    {0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x3C, 0x18},
    {0x00, 0x00, 0x00, 0x00, 0x60, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x3F, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x0C},
    {0x00, 0x04, 0x0B, 0x06, 0x0C, 0x18, 0x30, 0x20, 0x00, 0x02, 0x07, 0x1E, 0x3C, 0x78, 0x70, 0x20},
    {0x00, 0x20, 0x58, 0x11, 0x01, 0x19, 0x03, 0x1E, 0x00, 0x1C, 0x3E, 0x7F, 0x77, 0x7F, 0x3F, 0x1E},
    {0x00, 0x10, 0x24, 0x04, 0x04, 0x04, 0x40, 0x3F, 0x00, 0x08, 0x1C, 0x1C, 0x1C, 0x1C, 0x3E, 0x3F},
    {0x00, 0x20, 0x58, 0x33, 0x06, 0x0C, 0x00, 0x3F, 0x00, 0x1C, 0x3E, 0x3F, 0x1E, 0x3C, 0x7E, 0x3F},
    {0x00, 0x20, 0x58, 0x33, 0x00, 0x41, 0x03, 0x1E, 0x00, 0x1C, 0x3E, 0x3F, 0x06, 0x27, 0x3F, 0x1E},
    {0x00, 0x66, 0x55, 0x01, 0x39, 0x01, 0x01, 0x03, 0x00, 0x00, 0x33, 0x7F, 0x3F, 0x07, 0x07, 0x03},
    {0x00, 0x60, 0x5F, 0x00, 0x38, 0x01, 0x43, 0x3E, 0x00, 0x1E, 0x3F, 0x7C, 0x3E, 0x07, 0x3F, 0x3E},
    {0x00, 0x10, 0x2E, 0x00, 0x18, 0x11, 0x03, 0x1E, 0x00, 0x0C, 0x1E, 0x7C, 0x7E, 0x77, 0x3F, 0x1E},
    {0x00, 0x60, 0x39, 0x03, 0x06, 0x0C, 0x08, 0x18, 0x00, 0x1E, 0x3F, 0x0F, 0x1E, 0x3C, 0x38, 0x18},
    {0x00, 0x20, 0x58, 0x03, 0x18, 0x11, 0x03, 0x1E, 0x00, 0x1C, 0x3E, 0x3F, 0x7E, 0x77, 0x3F, 0x1E},
    {0x00, 0x20, 0x58, 0x01, 0x19, 0x41, 0x03, 0x1E, 0x00, 0x1C, 0x3E, 0x3F, 0x1F, 0x27, 0x3F, 0x1E},
    {0x00, 0x00, 0x00, 0x10, 0x0C, 0x10, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x08, 0x0C, 0x08, 0x0C, 0x00},
    {0x00, 0x00, 0x00, 0x10, 0x0C, 0x10, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x08, 0x0C, 0x08, 0x3C, 0x18},
    {0x00, 0x00, 0x08, 0x16, 0x0C, 0x00, 0x00, 0x06, 0x00, 0x00, 0x04, 0x0E, 0x3C, 0x18, 0x0C, 0x06},
    {0x00, 0x00, 0x00, 0x60, 0x3F, 0x40, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x3F, 0x3E, 0x3F, 0x00},
    {0x00, 0x00, 0x30, 0x00, 0x00, 0x06, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x18, 0x0C, 0x1E, 0x3C, 0x18},
    {0x00, 0x20, 0x58, 0x33, 0x06, 0x0C, 0x10, 0x0C, 0x00, 0x1C, 0x3E, 0x3F, 0x1E, 0x0C, 0x08, 0x0C},
    {0x00, 0x20, 0x58, 0x19, 0x11, 0x17, 0x00, 0x1E, 0x00, 0x1C, 0x3E, 0x77, 0x7F, 0x77, 0x3C, 0x1E},
    {0x00, 0x10, 0x20, 0x18, 0x01, 0x19, 0x11, 0x33, 0x00, 0x08, 0x1C, 0x7E, 0x7F, 0x7F, 0x77, 0x33},
    {0x00, 0x60, 0x58, 0x03, 0x18, 0x11, 0x03, 0x3E, 0x00, 0x1C, 0x3E, 0x7F, 0x7E, 0x77, 0x7F, 0x3E},
    {0x00, 0x20, 0x58, 0x13, 0x10, 0x10, 0x03, 0x1E, 0x00, 0x1C, 0x3E, 0x73, 0x70, 0x76, 0x3F, 0x1E},
    {0x00, 0x60, 0x50, 0x10, 0x11, 0x13, 0x06, 0x3C, 0x00, 0x18, 0x3C, 0x76, 0x77, 0x7F, 0x7E, 0x3C},
    {0x00, 0x60, 0x5F, 0x00, 0x1C, 0x10, 0x00, 0x3F, 0x00, 0x1E, 0x3F, 0x78, 0x7C, 0x70, 0x7E, 0x3F},
    {0x00, 0x60, 0x5F, 0x00, 0x1C, 0x10, 0x10, 0x30, 0x00, 0x1E, 0x3F, 0x78, 0x7C, 0x70, 0x70, 0x30},
    {0x00, 0x20, 0x5E, 0x1C, 0x11, 0x11, 0x03, 0x1E, 0x00, 0x1C, 0x3E, 0x72, 0x77, 0x77, 0x3F, 0x1E},
    {0x00, 0x66, 0x55, 0x01, 0x19, 0x11, 0x11, 0x33, 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x77, 0x77, 0x33},
    {0x00, 0x30, 0x06, 0x04, 0x04, 0x04, 0x00, 0x1E, 0x00, 0x0C, 0x1E, 0x1C, 0x1C, 0x1C, 0x3C, 0x1E},
    {0x00, 0x1C, 0x09, 0x01, 0x01, 0x41, 0x03, 0x1E, 0x00, 0x02, 0x0F, 0x07, 0x07, 0x27, 0x3F, 0x1E},
    {0x00, 0x64, 0x5B, 0x06, 0x04, 0x10, 0x10, 0x33, 0x00, 0x02, 0x37, 0x7E, 0x7C, 0x7C, 0x76, 0x33},
    {0x00, 0x60, 0x50, 0x10, 0x10, 0x10, 0x00, 0x3F, 0x00, 0x00, 0x30, 0x70, 0x70, 0x70, 0x7E, 0x3F},
    {0x00, 0x42, 0x45, 0x09, 0x01, 0x19, 0x11, 0x33, 0x00, 0x00, 0x23, 0x77, 0x7F, 0x7F, 0x77, 0x33},
    {0x00, 0x66, 0x45, 0x01, 0x11, 0x11, 0x11, 0x33, 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x77, 0x77, 0x33},
    {0x00, 0x20, 0x58, 0x11, 0x11, 0x11, 0x03, 0x1E, 0x00, 0x1C, 0x3E, 0x77, 0x77, 0x77, 0x3F, 0x1E},
    {0x00, 0x60, 0x58, 0x03, 0x1E, 0x10, 0x10, 0x30, 0x00, 0x1C, 0x3E, 0x7F, 0x7E, 0x70, 0x70, 0x30},
    {0x00, 0x20, 0x58, 0x11, 0x11, 0x11, 0x00, 0x1F, 0x00, 0x1C, 0x3E, 0x77, 0x7B, 0x7D, 0x3E, 0x1F},
    {0x00, 0x60, 0x58, 0x03, 0x06, 0x10, 0x10, 0x33, 0x00, 0x1C, 0x3E, 0x7F, 0x7E, 0x7C, 0x76, 0x33},
    {0x00, 0x20, 0x5F, 0x00, 0x18, 0x01, 0x03, 0x3E, 0x00, 0x1E, 0x3F, 0x3C, 0x1E, 0x07, 0x7F, 0x3E},
    {0x00, 0x70, 0x27, 0x04, 0x04, 0x04, 0x04, 0x0C, 0x00, 0x0E, 0x3F, 0x1C, 0x1C, 0x1C, 0x1C, 0x0C},
    {0x00, 0x66, 0x55, 0x11, 0x11, 0x11, 0x03, 0x1E, 0x00, 0x00, 0x33, 0x77, 0x77, 0x77, 0x3F, 0x1E},
    {0x00, 0x66, 0x55, 0x11, 0x11, 0x03, 0x06, 0x0C, 0x00, 0x00, 0x33, 0x77, 0x77, 0x3F, 0x1E, 0x0C},
    {0x00, 0x66, 0x55, 0x15, 0x01, 0x19, 0x31, 0x21, 0x00, 0x00, 0x33, 0x6B, 0x7F, 0x7F, 0x73, 0x21},
    {0x00, 0x64, 0x0B, 0x06, 0x00, 0x18, 0x11, 0x33, 0x00, 0x02, 0x37, 0x1E, 0x3C, 0x7E, 0x77, 0x33},
    {0x00, 0x66, 0x55, 0x0B, 0x06, 0x04, 0x04, 0x0C, 0x00, 0x00, 0x33, 0x37, 0x1E, 0x1C, 0x1C, 0x0C},
    {0x00, 0x60, 0x33, 0x06, 0x0C, 0x58, 0x00, 0x3F, 0x00, 0x1E, 0x3F, 0x1E, 0x3C, 0x38, 0x7E, 0x3F},
    {0x00, 0x30, 0x2E, 0x08, 0x08, 0x08, 0x00, 0x1E, 0x00, 0x0C, 0x1E, 0x38, 0x38, 0x38, 0x3C, 0x1E},
    {0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01},
    {0x00, 0x38, 0x12, 0x02, 0x02, 0x02, 0x02, 0x1E, 0x00, 0x04, 0x1E, 0x0E, 0x0E, 0x0E, 0x3E, 0x1E},
    {0x00, 0x10, 0x20, 0x18, 0x33, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x7E, 0x33, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x3F},
};
