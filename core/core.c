//
// Copyright 2016 Timo Kloss
//
// This file is part of LowRes NX.
//
// LowRes NX is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LowRes NX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with LowRes NX.  If not, see <http://www.gnu.org/licenses/>.
//

#include "core.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "text_lib.h"

void core_init(struct Core *core)
{
    memset(core, 0, sizeof(struct Core));
    
    machine_init(&core->machine);
    
    struct TextLib *textLib = &core->interpreter.textLib;
    textLib->charAttr.bank = 1;
    textLib->charAttr.priority = 1;
    textLib->charAttr.palette = 7;
    textLib->characterOffset = 192;
    textLib->areaX = 0;
    textLib->areaY = 0;
    textLib->areaWidth = 20;
    textLib->areaHeight = 16;
}

void core_update(struct Core *core)
{
    itp_runProgram(core);
    itp_runInterrupt(core, InterruptTypeVBL);
}

void core_rasterUpdate(struct Core *core)
{
    itp_runInterrupt(core, InterruptTypeRaster);
}

void core_keyPressed(struct Core *core, char key)
{
    if (key >= 32 && key < 127)
    {
        core->machine.ioRegisters.key = key;
    }
}

void core_backspacePressed(struct Core *core)
{
    core->machine.ioRegisters.key = '\b';
}

void core_returnPressed(struct Core *core)
{
    core->machine.ioRegisters.key = '\n';
}

void core_mouseMoved(struct Core *core, int x, int y)
{
    if (x < 0) x = 0; else if (x >= SCREEN_WIDTH) x = SCREEN_WIDTH - 1;
    if (y < 0) y = 0; else if (y >= SCREEN_HEIGHT) y = SCREEN_HEIGHT - 1;
    core->machine.ioRegisters.mouseX = x;
    core->machine.ioRegisters.mouseY = y;
}

void core_mouseDown(struct Core *core)
{
    core->machine.ioRegisters.status_mouseButton = 1;
}

void core_mouseUp(struct Core *core)
{
    core->machine.ioRegisters.status_mouseButton = 0;
}
