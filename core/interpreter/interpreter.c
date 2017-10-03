//
// Copyright 2016-2017 Timo Kloss
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

#include "interpreter.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "core.h"
#include "default_characters.h"
#include "cmd_control.h"
#include "cmd_variables.h"
#include "cmd_data.h"
#include "cmd_strings.h"
#include "cmd_memory.h"
#include "cmd_text.h"
#include "cmd_maths.h"
#include "cmd_background.h"
#include "cmd_screen.h"
#include "cmd_sprites.h"
#include "cmd_io.h"
#include "cmd_files.h"

struct TypedValue itp_evaluateExpressionLevel(struct Core *core, int level);
struct TypedValue itp_evaluatePrimaryExpression(struct Core *core);
struct TypedValue itp_evaluateFunction(struct Core *core);
enum ErrorCode itp_evaluateCommand(struct Core *core);

enum ErrorCode itp_compileProgram(struct Core *core, const char *sourceCode)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // Tokenize
    
    struct Token *errorToken = NULL;
    enum ErrorCode errorCode = tok_tokenizeProgram(&interpreter->tokenizer, sourceCode, &errorToken);
    if (errorCode != ErrorNone)
    {
        interpreter->pc = errorToken;
        return errorCode;
    }
    
    // Prepare
    
    interpreter->pc = interpreter->tokenizer.tokens;
    interpreter->pass = PassPrepare;
    
    do
    {
        errorCode = itp_evaluateCommand(core);
    }
    while (errorCode == ErrorNone && interpreter->pc->type != TokenUndefined);
    
    if (errorCode != ErrorNone) return errorCode;
    
    if (interpreter->numLabelStackItems > 0)
    {
        struct LabelStackItem *item = &interpreter->labelStackItems[0];
        switch (item->type)
        {
            case LabelTypeIF:
            case LabelTypeELSEIF:
                errorCode = ErrorIfWithoutEndIf;
                break;
                
            case LabelTypeFOR:
                errorCode =  ErrorForWithoutNext;
                
            case LabelTypeDO:
                errorCode =  ErrorDoWithoutLoop;
                
            case LabelTypeREPEAT:
                errorCode =  ErrorRepeatWithoutUntil;
                
            case LabelTypeWHILE:
                errorCode =  ErrorWhileWithoutWend;
                
            case LabelTypeFORVar:
            case LabelTypeFORLimit:
            case LabelTypeELSE:
            case LabelTypeGOSUB:
            case LabelTypeONGOSUB:
                // should not happen in compile time
                break;
        }
        if (errorCode != ErrorNone)
        {
            interpreter->pc = item->token;
            return errorCode;
        }
    }
    
    // global null string
    interpreter->nullString = rcstring_new(NULL, 0);
    
    itp_resetProgram(core);
    
    return ErrorNone;
}

void itp_resetProgram(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    interpreter->pc = interpreter->tokenizer.tokens;
    interpreter->pass = PassRun;
    interpreter->state = StateEvaluate;
    interpreter->mode = ModeNone;
    interpreter->exitErrorCode = ErrorNone;
    interpreter->numLabelStackItems = 0;
    interpreter->isSingleLineIf = false;
    interpreter->numSimpleVariables = 0;
    interpreter->numArrayVariables = 0;
    interpreter->currentDataToken = interpreter->firstData;
    interpreter->currentDataValueToken = interpreter->firstData + 1;
    
    txtlib_init(core);
}

void itp_runProgram(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    switch (interpreter->state)
    {
        case StateEvaluate:
        {
            interpreter->mode = ModeMain;
            interpreter->exitEvaluation = false;
            enum ErrorCode errorCode = ErrorNone;
            int cycles = 0;
            
            do
            {
                errorCode = itp_evaluateCommand(core);
                cycles++;
            }
            while (errorCode == ErrorNone && cycles < MAX_CYCLES_PER_FRAME && interpreter->state == StateEvaluate && !interpreter->exitEvaluation);
            
            interpreter->mode = ModeNone;
            if (cycles == MAX_CYCLES_PER_FRAME)
            {
                printf("Warning: Max cycles per frame reached.\n");
            }
            if (errorCode != ErrorNone)
            {
                interpreter->exitErrorCode = errorCode;
                interpreter->state = StateEnd;
                core->delegate->interpreterDidFail(core->delegate->context);
            }
            break;
        }
            
        case StateWait:
        {
            if (interpreter->waitCount > 0)
            {
                --interpreter->waitCount;
            }
            else
            {
                interpreter->state = StateEvaluate;
            }
            break;
        }
            
        case StateInput:
        {
            if (txtlib_inputUpdate(core))
            {
                interpreter->state = StateEvaluate;
                cmd_endINPUT(core);
            }
            break;
        }
            
        case StateNoProgram:
        case StatePaused:
        case StateEnd:
            break;
    }
}

void itp_runInterrupt(struct Core *core, enum InterruptType type)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    switch (interpreter->state)
    {
        case StateEvaluate:
        case StateWait:
        case StateInput:
        case StatePaused:
        {
            struct Token *startToken;
            int maxCycles;
            switch (type)
            {
                case InterruptTypeRaster:
                    startToken = interpreter->currentOnRasterToken;
                    maxCycles = MAX_CYCLES_PER_RASTER;
                    break;
                    
                case InterruptTypeVBL:
                    startToken = interpreter->currentOnVBLToken;
                    maxCycles = MAX_CYCLES_PER_VBL;
                    break;
            }
            
            if (startToken)
            {
                interpreter->mode = ModeInterrupt;
                interpreter->exitEvaluation = false;
                struct Token *pc = interpreter->pc;
                interpreter->pc = startToken;
                
                enum ErrorCode errorCode = lab_pushLabelStackItem(interpreter, LabelTypeONGOSUB, NULL);
                int cycles = 0;
                
                while (errorCode == ErrorNone && cycles < maxCycles && !interpreter->exitEvaluation)
                {
                    errorCode = itp_evaluateCommand(core);
                    cycles++;
                }
                
                interpreter->mode = ModeNone;
                if (cycles == maxCycles)
                {
                    interpreter->exitErrorCode = ErrorTooManyCommandCycles;
                    interpreter->state = StateEnd;
                    core->delegate->interpreterDidFail(core->delegate->context);
                }
                else if (errorCode != ErrorNone)
                {
                    interpreter->exitErrorCode = errorCode;
                    interpreter->state = StateEnd;
                    core->delegate->interpreterDidFail(core->delegate->context);
                }
                else
                {
                    interpreter->pc = pc;
                }
            }
            break;
        }
            
        case StateNoProgram:
        case StateEnd:
            break;
    }

}

void itp_didFinishVBL(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // remember this frame's IO
    for (int i = 0; i < NUM_GAMEPADS; i++)
    {
        interpreter->lastFrameGamepads[i] = core->machine.ioRegisters.gamepads[i];
    }
    interpreter->lastFrameIOStatus = core->machine.ioRegisters.status;
    
    // timer
    interpreter->timer++;
    if (interpreter->timer >= TIMER_WRAP_VALUE)
    {
        interpreter->timer = 0;
    }
    
    if (core->machine.ioRegisters.status.pause)
    {
        if (interpreter->state == StateEvaluate || interpreter->state == StateWait)
        {
            interpreter->state = StatePaused;
            overlay_updateState(core);
        }
        else if (interpreter->state == StatePaused)
        {
            interpreter->state = StateEvaluate;
            overlay_updateState(core);
        }
        core->machine.ioRegisters.status.pause = 0;
    }
}

void itp_freeProgram(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    interpreter->state = StateNoProgram;
    
    var_freeSimpleVariables(interpreter);
    var_freeArrayVariables(interpreter);
    tok_freeTokens(&interpreter->tokenizer);
    
    // Free null string
    if (interpreter->nullString)
    {
        rcstring_release(interpreter->nullString);
    }
    
    assert(rcstring_count == 0);
}

enum ErrorCode itp_getExitErrorCode(struct Core *core)
{
    return core->interpreter.exitErrorCode;
}

int itp_getPcPositionInSourceCode(struct Core *core)
{
    return core->interpreter.pc->sourcePosition;
}

union Value *itp_readVariable(struct Core *core, enum ValueType *type, enum ErrorCode *errorCode)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    struct Token *tokenIdentifier = interpreter->pc;
    
    if (tokenIdentifier->type != TokenIdentifier && tokenIdentifier->type != TokenStringIdentifier)
    {
        *errorCode = ErrorExpectedVariableIdentifier;
        return NULL;
    }
    
    enum ValueType varType = ValueTypeNull;
    if (tokenIdentifier->type == TokenIdentifier)
    {
        varType = ValueTypeFloat;
    }
    else if (tokenIdentifier->type == TokenStringIdentifier)
    {
        varType = ValueTypeString;
    }
    if (type)
    {
        *type = varType;
    }
    
    int symbolIndex = tokenIdentifier->symbolIndex;
    ++interpreter->pc;
    
    if (interpreter->pc->type == TokenBracketOpen)
    {
        // array
        ++interpreter->pc;
        
        struct ArrayVariable *variable = NULL;
        if (interpreter->pass == PassRun)
        {
            variable = var_getArrayVariable(interpreter, symbolIndex);
            if (!variable)
            {
                *errorCode = ErrorArrayNotDimensionized;
                return NULL;
            }
        }
        
        int indices[MAX_ARRAY_DIMENSIONS];
        int numDimensions = 0;
        
        for (int i = 0; i < MAX_ARRAY_DIMENSIONS; i++)
        {
            struct TypedValue indexValue = itp_evaluateExpression(core, TypeClassNumeric);
            if (indexValue.type == ValueTypeError)
            {
                *errorCode = indexValue.v.errorCode;
                return NULL;
            }
            
            numDimensions++;
            
            if (interpreter->pass == PassRun)
            {
                if (numDimensions <= variable->numDimensions && (indexValue.v.floatValue < 0 || indexValue.v.floatValue >= variable->dimensionSizes[i]))
                {
                    *errorCode = ErrorIndexOutOfBounds;
                    return NULL;
                }
                
                indices[i] = indexValue.v.floatValue;
            }
            
            if (interpreter->pc->type == TokenComma)
            {
                ++interpreter->pc;
            }
            else
            {
                break;
            }
        }
        
        if (interpreter->pc->type != TokenBracketClose)
        {
            *errorCode = ErrorExpectedRightParenthesis;
            return NULL;
        }
        ++interpreter->pc;
        
        if (interpreter->pass == PassRun)
        {
            if (numDimensions != variable->numDimensions)
            {
                *errorCode = ErrorWrongNumberOfDimensions;
                return NULL;
            }
            return var_getArrayValue(interpreter, variable, indices);
        }
    }
    else
    {
        // simple variable
        if (interpreter->pass == PassRun)
        {
            struct SimpleVariable *variable = var_getSimpleVariable(interpreter, errorCode, symbolIndex, varType);
            if (!variable) return NULL;
            return &variable->v;
        }
    }
    return &ValueDummy;
}

enum ErrorCode itp_checkTypeClass(struct Interpreter *interpreter, enum ValueType valueType, enum TypeClass typeClass)
{
    if (interpreter->pass == PassPrepare && valueType != ValueTypeError)
    {
        if (typeClass == TypeClassString && valueType != ValueTypeString)
        {
            return ErrorExpectedStringExpression;
        }
        else if (typeClass == TypeClassNumeric && valueType != ValueTypeFloat)
        {
            return ErrorExpectedNumericExpression;
        }
    }
    return ErrorNone;
}

struct TypedValue itp_evaluateExpression(struct Core *core, enum TypeClass typeClass)
{
    struct TypedValue value = itp_evaluateExpressionLevel(core, 0);
    enum ErrorCode errorCode = itp_checkTypeClass(&core->interpreter, value.type, typeClass);
    if (errorCode != ErrorNone)
    {
        value.type = ValueTypeError;
        value.v.errorCode = errorCode;
    }
    return value;
}

struct TypedValue itp_evaluateNumericExpression(struct Core *core, int min, int max)
{
    struct TypedValue value = itp_evaluateExpressionLevel(core, 0);
    if (value.type != ValueTypeError)
    {
        enum ErrorCode errorCode = ErrorNone;
        if (core->interpreter.pass == PassPrepare)
        {
            if (value.type != ValueTypeFloat)
            {
                errorCode = ErrorExpectedNumericExpression;
            }
        }
        else if (core->interpreter.pass == PassRun)
        {
            if ((int)value.v.floatValue < min || (int)value.v.floatValue > max)
            {
                errorCode = ErrorInvalidParameter;
            }
        }
        if (errorCode != ErrorNone)
        {
            value.type = ValueTypeError;
            value.v.errorCode = errorCode;
        }
    }
    return value;
}

struct TypedValue itp_evaluateOptionalExpression(struct Core *core, enum TypeClass typeClass)
{
    if (core->interpreter.pc->type == TokenComma || core->interpreter.pc->type == TokenBracketClose || itp_isEndOfCommand(&core->interpreter))
    {
        struct TypedValue value;
        value.type = ValueTypeNull;
        return value;
    }
    return itp_evaluateExpression(core, typeClass);
}

struct TypedValue itp_evaluateOptionalNumericExpression(struct Core *core, int min, int max)
{
    if (core->interpreter.pc->type == TokenComma || core->interpreter.pc->type == TokenBracketClose || itp_isEndOfCommand(&core->interpreter))
    {
        struct TypedValue value;
        value.type = ValueTypeNull;
        return value;
    }
    return itp_evaluateNumericExpression(core, min, max);
}

bool itp_isTokenLevel(enum TokenType token, int level)
{
    switch (level)
    {
        case 0:
            return token == TokenXOR || token == TokenOR;
        case 1:
            return token == TokenAND;
//        case 2:
//            return token == TokenNOT;
        case 3:
            return token == TokenEq || token == TokenUneq || token == TokenGr || token == TokenLe || token == TokenGrEq || token == TokenLeEq;
        case 4:
            return token == TokenPlus || token == TokenMinus;
        case 5:
            return token == TokenMOD;
        case 6:
            return token == TokenMul || token == TokenDiv;
//        case 7:
//            return token == TokenPlus || token == TokenMinus; // unary
        case 8:
            return token == TokenPow;
    }
    return false;
}

struct TypedValue itp_evaluateExpressionLevel(struct Core *core, int level)
{
    struct Interpreter *interpreter = &core->interpreter;
    enum TokenType type = interpreter->pc->type;
    
    if (level == 2 && type == TokenNOT)
    {
        ++interpreter->pc;
        struct TypedValue value = itp_evaluateExpressionLevel(core, level + 1);
        if (value.type == ValueTypeError) return value;
        enum ErrorCode errorCode = itp_checkTypeClass(&core->interpreter, value.type, TypeClassNumeric);
        if (errorCode != ErrorNone)
        {
            value.type = ValueTypeError;
            value.v.errorCode = errorCode;
        }
        else
        {
            value.v.floatValue = ~((int)value.v.floatValue);
        }
        return value;
    }
    if (level == 7 && (type == TokenPlus || type == TokenMinus)) // unary
    {
        ++interpreter->pc;
        struct TypedValue value = itp_evaluateExpressionLevel(core, level + 1);
        if (value.type == ValueTypeError) return value;
        enum ErrorCode errorCode = itp_checkTypeClass(&core->interpreter, value.type, TypeClassNumeric);
        if (errorCode != ErrorNone)
        {
            value.type = ValueTypeError;
            value.v.errorCode = errorCode;
        }
        else if (type == TokenMinus)
        {
            value.v.floatValue = -value.v.floatValue;
        }
        return value;
    }
    if (level == 9)
    {
        return itp_evaluatePrimaryExpression(core);
    }
    
    struct TypedValue value = itp_evaluateExpressionLevel(core, level + 1);
    if (value.type == ValueTypeError) return value;
    
    while (itp_isTokenLevel(interpreter->pc->type, level))
    {
        enum TokenType type = interpreter->pc->type;
        ++interpreter->pc;
        struct TypedValue rightValue = itp_evaluateExpressionLevel(core, level + 1);
        if (rightValue.type == ValueTypeError) return rightValue;
        
        struct TypedValue newValue;
        if (value.type != rightValue.type)
        {
            newValue.type = ValueTypeError;
            newValue.v.errorCode = ErrorTypeMismatch;
            return newValue;
        }
        
        if (value.type == ValueTypeFloat)
        {
            newValue.type = ValueTypeFloat;
            switch (type)
            {
                case TokenXOR: {
                    int leftInt = value.v.floatValue;
                    int rightInt = rightValue.v.floatValue;
                    newValue.v.floatValue = (leftInt ^ rightInt);
                    break;
                }
                case TokenOR: {
                    int leftInt = value.v.floatValue;
                    int rightInt = rightValue.v.floatValue;
                    newValue.v.floatValue = (leftInt | rightInt);
                    break;
                }
                case TokenAND: {
                    int leftInt = value.v.floatValue;
                    int rightInt = rightValue.v.floatValue;
                    newValue.v.floatValue = (leftInt & rightInt);
                    break;
                }
                case TokenEq: {
                    newValue.v.floatValue = (value.v.floatValue == rightValue.v.floatValue) ? BAS_TRUE : BAS_FALSE;
                    break;
                }
                case TokenUneq: {
                    newValue.v.floatValue = (value.v.floatValue != rightValue.v.floatValue) ? BAS_TRUE : BAS_FALSE;
                    break;
                }
                case TokenGr: {
                    newValue.v.floatValue = (value.v.floatValue > rightValue.v.floatValue) ? BAS_TRUE : BAS_FALSE;
                    break;
                }
                case TokenLe: {
                    newValue.v.floatValue = (value.v.floatValue < rightValue.v.floatValue) ? BAS_TRUE : BAS_FALSE;
                    break;
                }
                case TokenGrEq: {
                    newValue.v.floatValue = (value.v.floatValue >= rightValue.v.floatValue) ? BAS_TRUE : BAS_FALSE;
                    break;
                }
                case TokenLeEq: {
                    newValue.v.floatValue = (value.v.floatValue <= rightValue.v.floatValue) ? BAS_TRUE : BAS_FALSE;
                    break;
                }
                case TokenPlus: {
                    newValue.v.floatValue = value.v.floatValue + rightValue.v.floatValue;
                    break;
                }
                case TokenMinus: {
                    newValue.v.floatValue = value.v.floatValue - rightValue.v.floatValue;
                    break;
                }
                case TokenMOD: {
                    newValue.v.floatValue = (int)value.v.floatValue % (int)rightValue.v.floatValue;
                    break;
                }
                case TokenMul: {
                    newValue.v.floatValue = value.v.floatValue * rightValue.v.floatValue;
                    break;
                }
                case TokenDiv: {
                    newValue.v.floatValue = value.v.floatValue / rightValue.v.floatValue;
                    break;
                }
                case TokenPow: {
                    newValue.v.floatValue = powf(value.v.floatValue, rightValue.v.floatValue);
                    break;
                }
                default: {
                    newValue.type = ValueTypeError;
                    newValue.v.errorCode = ErrorSyntax;
                }
            }
        }
        else if (value.type == ValueTypeString)
        {
            switch (type)
            {
                case TokenEq: {
                    newValue.type = ValueTypeFloat;
                    if (interpreter->pass == PassRun)
                    {
                        newValue.v.floatValue = (strcmp(value.v.stringValue->chars, rightValue.v.stringValue->chars) == 0) ? BAS_TRUE : BAS_FALSE;
                    }
                    break;
                }
                case TokenUneq: {
                    newValue.type = ValueTypeFloat;
                    if (interpreter->pass == PassRun)
                    {
                        newValue.v.floatValue = (strcmp(value.v.stringValue->chars, rightValue.v.stringValue->chars) != 0) ? BAS_TRUE : BAS_FALSE;
                    }
                    break;
                }
                case TokenGr: {
                    newValue.type = ValueTypeFloat;
                    if (interpreter->pass == PassRun)
                    {
                        newValue.v.floatValue = (strcmp(value.v.stringValue->chars, rightValue.v.stringValue->chars) > 0) ? BAS_TRUE : BAS_FALSE;
                    }
                    break;
                }
                case TokenLe: {
                    newValue.type = ValueTypeFloat;
                    if (interpreter->pass == PassRun)
                    {
                        newValue.v.floatValue = (strcmp(value.v.stringValue->chars, rightValue.v.stringValue->chars) < 0) ? BAS_TRUE : BAS_FALSE;
                    }
                    break;
                }
                case TokenGrEq: {
                    newValue.type = ValueTypeFloat;
                    if (interpreter->pass == PassRun)
                    {
                        newValue.v.floatValue = (strcmp(value.v.stringValue->chars, rightValue.v.stringValue->chars) >= 0) ? BAS_TRUE : BAS_FALSE;
                    }
                    break;
                }
                case TokenLeEq: {
                    newValue.type = ValueTypeFloat;
                    if (interpreter->pass == PassRun)
                    {
                        newValue.v.floatValue = (strcmp(value.v.stringValue->chars, rightValue.v.stringValue->chars) <= 0) ? BAS_TRUE : BAS_FALSE;
                    }
                    break;
                }
                case TokenPlus: {
                    newValue.type = ValueTypeString;
                    if (interpreter->pass == PassRun)
                    {
                        size_t len1 = strlen(value.v.stringValue->chars);
                        size_t len2 = strlen(rightValue.v.stringValue->chars);
                        newValue.v.stringValue = rcstring_new(NULL, len1 + len2);
                        strcpy(newValue.v.stringValue->chars, value.v.stringValue->chars);
                        strcpy(&newValue.v.stringValue->chars[len1], rightValue.v.stringValue->chars);
                    }
                    break;
                }
                case TokenXOR:
                case TokenOR:
                case TokenAND:
                case TokenMinus:
                case TokenMOD:
                case TokenMul:
                case TokenDiv:
                case TokenPow: {
                    newValue.type = ValueTypeError;
                    newValue.v.errorCode = ErrorTypeMismatch;
                    break;
                }
                default: {
                    newValue.type = ValueTypeError;
                    newValue.v.errorCode = ErrorSyntax;
                }
            }
            if (interpreter->pass == PassRun)
            {
                rcstring_release(value.v.stringValue);
                rcstring_release(rightValue.v.stringValue);
            }
        }
        else
        {
            assert(0);
            newValue.v.floatValue = 0;
        }
        
        value = newValue;
        if (value.type == ValueTypeError) break;
    }
    return value;
}

struct TypedValue itp_evaluatePrimaryExpression(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    
    // check for function
    struct TypedValue value = itp_evaluateFunction(core);
    if (value.type != ValueTypeNull) return value;
    
    // native types
    switch (interpreter->pc->type)
    {
        case TokenFloat: {
            value.type = ValueTypeFloat;
            value.v.floatValue = interpreter->pc->floatValue;
            ++interpreter->pc;
            break;
        }
        case TokenString: {
            value.type = ValueTypeString;
            value.v.stringValue = interpreter->pc->stringValue;
            if (interpreter->pass == PassRun)
            {
                rcstring_retain(interpreter->pc->stringValue);
            }
            ++interpreter->pc;
            break;
        }
        case TokenIdentifier:
        case TokenStringIdentifier: {
            enum ErrorCode errorCode = ErrorNone;
            enum ValueType valueType = ValueTypeNull;
            union Value *varValue = itp_readVariable(core, &valueType, &errorCode);
            if (varValue)
            {
                value.type = valueType;
                value.v = *varValue;
                if (interpreter->pass == PassRun && valueType == ValueTypeString)
                {
                    rcstring_retain(varValue->stringValue);
                }
            }
            else
            {
                value.type = ValueTypeError;
                value.v.errorCode = errorCode;
            }
            break;
        }
        case TokenBracketOpen: {
            ++interpreter->pc;
            value = itp_evaluateExpression(core, TypeClassAny);
            if (interpreter->pc->type != TokenBracketClose)
            {
                value.type = ValueTypeError;
                value.v.errorCode = ErrorExpectedRightParenthesis;
            }
            else
            {
                ++interpreter->pc;
            }
            break;
        }
        default: {
            value.type = ValueTypeError;
            value.v.errorCode = ErrorSyntax;
        }
    }
    return value;
}

int itp_isEndOfCommand(struct Interpreter *interpreter)
{
    enum TokenType type = interpreter->pc->type;
    return (type == TokenEol || type == TokenELSE);
}

enum ErrorCode itp_endOfCommand(struct Interpreter *interpreter)
{
    enum TokenType type = interpreter->pc->type;
    if (type == TokenEol)
    {
        interpreter->isSingleLineIf = false;
        ++interpreter->pc;
        return ErrorNone;
    }
    return (type == TokenELSE) ? ErrorNone : ErrorUnexpectedToken;
}

enum TokenType itp_getNextTokenType(struct Interpreter *interpreter)
{
    return (interpreter->pc + 1)->type;
}

struct TypedValue itp_evaluateFunction(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    switch (interpreter->pc->type)
    {
        case TokenASC:
            return fnc_ASC(core);
            
        case TokenBIN:
        case TokenHEX:
            return fnc_BIN_HEX(core);
            
        case TokenCHR:
            return fnc_CHR(core);
            
        case TokenINSTR:
            return fnc_INSTR(core);
            
        case TokenLEFTStr:
        case TokenRIGHTStr:
            return fnc_LEFTStr_RIGHTStr(core);
            
        case TokenLEN:
            return fnc_LEN(core);
            
        case TokenMID:
            return fnc_MID(core);
            
        case TokenSTR:
            return fnc_STR(core);
            
        case TokenVAL:
            return fnc_VAL(core);

        case TokenPEEK:
            return fnc_PEEK(core);
            
        case TokenPI:
        case TokenRND:
            return fnc_math0(core);
            
        case TokenABS:
        case TokenATN:
        case TokenCOS:
        case TokenEXP:
        case TokenINT:
        case TokenLOG:
        case TokenSGN:
        case TokenSIN:
        case TokenSQR:
        case TokenTAN:
            return fnc_math1(core);

        case TokenMAX:
        case TokenMIN:
            return fnc_math2(core);
            
        case TokenINKEY:
            return fnc_INKEY(core);
        
        case TokenSTART:
        case TokenLENGTH:
            return fnc_START_LENGTH(core);
            
        case TokenCOLOR:
            return fnc_COLOR(core);
            
        case TokenTIMER:
        case TokenRASTER:
        case TokenDISPLAYA:
            return fnc_screen0(core);
            
        case TokenDISPLAYX:
        case TokenDISPLAYY:
            return fnc_DISPLAY_X_Y(core);
            
        case TokenCELLA:
        case TokenCELLC:
            return fnc_CELL(core);
            
        case TokenUP:
        case TokenDOWN:
        case TokenLEFT:
        case TokenRIGHT:
            return fnc_UP_DOWN_LEFT_RIGHT(core);
            
        case TokenBUTTON:
            return fnc_BUTTON(core);
            
        case TokenSPRITEX:
        case TokenSPRITEY:
        case TokenSPRITEC:
        case TokenSPRITEA:
            return fnc_SPRITE(core);
            
        case TokenSPRITE:
            return fnc_SPRITE_HIT(core);
            
        case TokenHIT:
            return fnc_HIT(core);
            
        case TokenTOUCH:
            return fnc_TOUCH(core);

        case TokenTAP:
            return fnc_TAP(core);
            
        case TokenTOUCHX:
        case TokenTOUCHY:
            return fnc_TOUCH_X_Y(core);
            
        default:
            break;
    }
    struct TypedValue value;
    value.type = ValueTypeNull;
    return value;
}

enum ErrorCode itp_evaluateCommand(struct Core *core)
{
    struct Interpreter *interpreter = &core->interpreter;
    switch (interpreter->pc->type)
    {
        case TokenUndefined:
            if (interpreter->pass == PassRun)
            {
                interpreter->state = StateEnd;
            }
            break;
            
        case TokenREM:
        case TokenApostrophe:
            ++interpreter->pc;
            break;
            
        case TokenLabel:
            ++interpreter->pc;
            if (interpreter->pc->type != TokenEol) return ErrorExpectedEndOfLine;
            ++interpreter->pc;
            break;
        
        case TokenEol:
            interpreter->isSingleLineIf = false;
            ++interpreter->pc;
            break;
            
        case TokenEND:
            if (itp_getNextTokenType(interpreter) == TokenIF)
            {
                return cmd_END_IF(core);
            }
            return cmd_END(core);
            
        case TokenLET:
        case TokenIdentifier:
        case TokenStringIdentifier:
            return cmd_LET(core);
            
        case TokenDIM:
            return cmd_DIM(core);
        
        case TokenPRINT:
            return cmd_PRINT(core);
            
        case TokenCLS:
            return cmd_CLS(core);
            
        case TokenINPUT:
            return cmd_INPUT(core);
        
        case TokenIF:
            return cmd_IF(core, false);
        
        case TokenELSE:
            return cmd_ELSE(core);

        case TokenFOR:
            return cmd_FOR(core);

        case TokenNEXT:
            return cmd_NEXT(core);

        case TokenGOTO:
            return cmd_GOTO(core);

        case TokenGOSUB:
            return cmd_GOSUB(core);
            
        case TokenRETURN:
            return cmd_RETURN(core);
            
        case TokenDATA:
            return cmd_DATA(core);

        case TokenREAD:
            return cmd_READ(core);

        case TokenRESTORE:
            return cmd_RESTORE(core);

        case TokenPOKE:
            return cmd_POKE(core);
            
        case TokenFILL:
            return cmd_FILL(core);
            
        case TokenCOPY:
            return cmd_COPY(core);
            
        case TokenWAIT:
            return cmd_WAIT(core);
            
        case TokenON:
            return cmd_ON(core);
            
        case TokenSWAP:
            return cmd_SWAP(core);
        
        case TokenTEXT:
            return cmd_TEXT(core);

        case TokenNUMBER:
            return cmd_NUMBER(core);
            
        case TokenDO:
            return cmd_DO(core);
            
        case TokenLOOP:
            return cmd_LOOP(core);
        
        case TokenREPEAT:
            return cmd_REPEAT(core);
            
        case TokenUNTIL:
            return cmd_UNTIL(core);
            
        case TokenWHILE:
            return cmd_WHILE(core);
            
        case TokenWEND:
            return cmd_WEND(core);

        case TokenRANDOMIZE:
            return cmd_RANDOMIZE(core);
            
        case TokenLEFTStr:
        case TokenRIGHTStr:
            return cmd_LEFT_RIGHT(core);
            
        case TokenMID:
            return cmd_MID(core);
            
        case TokenWINDOW:
            return cmd_WINDOW(core);
            
        case TokenFONT:
            return cmd_FONT(core);
            
        case TokenLOCATE:
            return cmd_LOCATE(core);
            
        case TokenCLW:
            return cmd_CLW(core);
            
        case TokenBG:
            switch (itp_getNextTokenType(interpreter))
            {
                case TokenSOURCE:
                    return cmd_BG_SOURCE(core);
                    
                case TokenCOPY:
                    return cmd_BG_COPY(core);
                    
                case TokenSCROLL:
                    return cmd_BG_SCROLL(core);
                    
                case TokenFILL:
                    return cmd_BG_FILL(core);
                    
                default:
                    return cmd_BG(core);
            }
            break;
            
        case TokenCHAR:
            return cmd_CHAR(core);
            
        case TokenCELL:
            return cmd_CELL(core);
            
        case TokenPALETTE:
            return cmd_PALETTE(core);
            
        case TokenDISPLAY:
            return cmd_DISPLAY(core);

        case TokenDISPLAYA:
            return cmd_DISPLAY_A(core);
            
        case TokenSPRITEA:
            return cmd_SPRITE_A(core);
            
        case TokenSPRITE:
            return cmd_SPRITE(core);
            
        case TokenSAVE:
            return cmd_SAVE(core);
            
        case TokenLOAD:
            return cmd_LOAD(core);
            
        case TokenGAMEPAD:
            return cmd_GAMEPAD(core);
            
        case TokenKEYBOARD:
            return cmd_KEYBOARD(core);
            
        default:
            printf("Command not implemented: %s\n", TokenStrings[interpreter->pc->type]);
            return ErrorUnexpectedToken;
    }
    return ErrorNone;
}
