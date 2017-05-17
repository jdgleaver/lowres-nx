//
// Copyright 2017 Timo Kloss
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

#include "cmd_screen.h"
#include "core.h"
#include <assert.h>
#include "interpreter_utils.h"

enum ErrorCode cmd_PALETTE(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // PALETTE
    ++interpreter->pc;
    
    // n value
    struct TypedValue nValue = itp_evaluateNumericExpression(core, 0, NUM_COLORS / 4 - 1);
    if (nValue.type == ValueTypeError) return nValue.v.errorCode;
    
    // comma
    if (interpreter->pc->type != TokenComma) return ErrorExpectedComma;
    ++interpreter->pc;
    
    // c0 value
    struct TypedValue c0Value = itp_evaluateOptionalNumericExpression(core, 0, 63);
    if (c0Value.type == ValueTypeError) return c0Value.v.errorCode;

    // comma
    if (interpreter->pc->type != TokenComma) return ErrorExpectedComma;
    ++interpreter->pc;
    
    // c1 value
    struct TypedValue c1Value = itp_evaluateOptionalNumericExpression(core, 0, 63);
    if (c1Value.type == ValueTypeError) return c1Value.v.errorCode;
    
    // comma
    if (interpreter->pc->type != TokenComma) return ErrorExpectedComma;
    ++interpreter->pc;
    
    // c2 value
    struct TypedValue c2Value = itp_evaluateOptionalNumericExpression(core, 0, 63);
    if (c2Value.type == ValueTypeError) return c2Value.v.errorCode;
    
    // comma
    if (interpreter->pc->type != TokenComma) return ErrorExpectedComma;
    ++interpreter->pc;
    
    // c3 value
    struct TypedValue c3Value = itp_evaluateOptionalNumericExpression(core, 0, 63);
    if (c3Value.type == ValueTypeError) return c3Value.v.errorCode;
    
    if (interpreter->pass == PassRun)
    {
        int n = nValue.v.floatValue;
        uint8_t *palColors = &core->machine.colorRegisters.colors[n * 4];
        if (c0Value.type != ValueTypeNull) palColors[0] = c0Value.v.floatValue;
        if (c1Value.type != ValueTypeNull) palColors[1] = c1Value.v.floatValue;
        if (c2Value.type != ValueTypeNull) palColors[2] = c2Value.v.floatValue;
        if (c3Value.type != ValueTypeNull) palColors[3] = c3Value.v.floatValue;
    }
    
    return itp_endOfCommand(interpreter);
}

enum ErrorCode cmd_COLOR(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // COLOR
    ++interpreter->pc;
    
    // n value
    struct TypedValue nValue = itp_evaluateNumericExpression(core, 0, NUM_COLORS - 1);
    if (nValue.type == ValueTypeError) return nValue.v.errorCode;
    
    // comma
    if (interpreter->pc->type != TokenComma) return ErrorExpectedComma;
    ++interpreter->pc;
    
    // c value
    struct TypedValue cValue = itp_evaluateNumericExpression(core, 0, 63);
    if (cValue.type == ValueTypeError) return cValue.v.errorCode;

    if (interpreter->pass == PassRun)
    {
        int n = nValue.v.floatValue;
        core->machine.colorRegisters.colors[n] = cValue.v.floatValue;
    }
    
    return itp_endOfCommand(interpreter);
}

enum ErrorCode cmd_DISPLAY(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // DISPLAY
    ++interpreter->pc;
    
    // bg value
    struct TypedValue bgValue = itp_evaluateNumericExpression(core, 0, 1);
    if (bgValue.type == ValueTypeError) return bgValue.v.errorCode;
    
    // comma
    if (interpreter->pc->type != TokenComma) return ErrorExpectedComma;
    ++interpreter->pc;
    
    // x value
    struct TypedValue xValue = itp_evaluateExpression(core, TypeClassNumeric);
    if (xValue.type == ValueTypeError) return xValue.v.errorCode;
    
    // comma
    if (interpreter->pc->type != TokenComma) return ErrorExpectedComma;
    ++interpreter->pc;
    
    // y value
    struct TypedValue yValue = itp_evaluateExpression(core, TypeClassNumeric);
    if (yValue.type == ValueTypeError) return yValue.v.errorCode;

    if (interpreter->pass == PassRun)
    {
        int bg = bgValue.v.floatValue;
        int x = (int)xValue.v.floatValue & 0xFF;
        int y = (int)yValue.v.floatValue & 0xFF;
        if (bg == 0)
        {
            core->machine.videoRegisters.scrollAX = x;
            core->machine.videoRegisters.scrollAY = y;
        }
        else
        {
            core->machine.videoRegisters.scrollBX = x;
            core->machine.videoRegisters.scrollBY = y;
        }
    }
    
    return itp_endOfCommand(interpreter);
}

enum ErrorCode cmd_DISPLAY_A(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // DISPLAY.A
    ++interpreter->pc;
    
    // atrb value
    struct TypedValue aValue = itp_evaluateDisplayAttributes(core, core->machine.videoRegisters.attr);
    if (aValue.type == ValueTypeError) return aValue.v.errorCode;
    
    if (interpreter->pass == PassRun)
    {
         core->machine.videoRegisters.attr.value = aValue.v.floatValue;
    }
    
    return itp_endOfCommand(interpreter);
}

struct TypedValue fnc_COLOR(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // COLOR
    ++interpreter->pc;
    
    // bracket open
    if (interpreter->pc->type != TokenBracketOpen) return val_makeError(ErrorExpectedLeftParenthesis);
    ++interpreter->pc;
    
    // expression
    struct TypedValue nValue = itp_evaluateNumericExpression(core, 0, NUM_COLORS - 1);
    if (nValue.type == ValueTypeError) return nValue;
    
    // bracket close
    if (interpreter->pc->type != TokenBracketClose) return val_makeError(ErrorExpectedRightParenthesis);
    ++interpreter->pc;
    
    struct TypedValue value;
    value.type = ValueTypeFloat;
    
    if (interpreter->pass == PassRun)
    {
        int n = nValue.v.floatValue;
        value.v.floatValue = core->machine.colorRegisters.colors[n];
    }
    return value;
}

struct TypedValue fnc_screen0(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // function
    enum TokenType type = interpreter->pc->type;
    ++interpreter->pc;
    
    struct TypedValue value;
    value.type = ValueTypeFloat;
    
    if (interpreter->pass == PassRun)
    {
        switch (type)
        {
            case TokenTIMER:
                value.v.floatValue = 0; //TODO
                break;
                
            case TokenRASTER:
                value.v.floatValue = core->machine.videoRegisters.rasterLine;
                break;
                
            case TokenDISPLAYA:
                value.v.floatValue = core->machine.videoRegisters.attr.value;
                break;
                
            default:
                assert(0);
                break;
        }
    }
    return value;
}

struct TypedValue fnc_DISPLAY_X_Y(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // DISPLAY.?
    enum TokenType type = interpreter->pc->type;
    ++interpreter->pc;
    
    // bracket open
    if (interpreter->pc->type != TokenBracketOpen) return val_makeError(ErrorExpectedLeftParenthesis);
    ++interpreter->pc;
    
    // bg value
    struct TypedValue bgValue = itp_evaluateNumericExpression(core, 0, 1);
    if (bgValue.type == ValueTypeError) return bgValue;
    
    // bracket close
    if (interpreter->pc->type != TokenBracketClose) return val_makeError(ErrorExpectedRightParenthesis);
    ++interpreter->pc;
    
    struct TypedValue value;
    value.type = ValueTypeFloat;
    
    if (interpreter->pass == PassRun)
    {
        int bg = bgValue.v.floatValue;
        switch (type)
        {
            case TokenDISPLAYX:
                value.v.floatValue = (bg == 0) ? core->machine.videoRegisters.scrollAX : core->machine.videoRegisters.scrollBX;
                break;
                
            case TokenDISPLAYY:
                value.v.floatValue = (bg == 0) ? core->machine.videoRegisters.scrollAY : core->machine.videoRegisters.scrollBY;
                break;
                
            default:
                assert(0);
                break;
        }
    }
    return value;
}