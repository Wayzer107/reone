/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "program.h"

#include "../../../common/exception/argument.h"
#include "../../../common/exception/notimplemented.h"
#include "../../../common/exception/validation.h"
#include "../../../script/routine.h"
#include "../../../script/routines.h"

using namespace std;

using namespace reone::script;

namespace reone {

NwscriptProgram NwscriptProgram::fromCompiled(const ScriptProgram &compiled, const IRoutines &routines) {
    auto functions = vector<shared_ptr<Function>>();
    auto expressions = vector<shared_ptr<Expression>>();
    auto decompilationCtx = DecompilationContext(compiled, routines, functions, expressions);

    auto funcMain = make_shared<Function>();
    funcMain->name = "main";
    funcMain->offset = 13;
    funcMain->block = decompile(13, decompilationCtx);

    decompilationCtx.functions.push_back(move(funcMain));

    return NwscriptProgram(
        decompilationCtx.functions,
        decompilationCtx.expressions);
}

NwscriptProgram::BlockExpression *NwscriptProgram::decompile(uint32_t start, DecompilationContext &ctx) {
    auto block = make_shared<BlockExpression>();
    block->offset = start;

    for (uint32_t offset = start; offset < ctx.compiled.length();) {
        const Instruction &ins = ctx.compiled.getInstruction(offset);

        if (ins.type == InstructionType::RETN) {
            auto retExpr = make_shared<ReturnExpression>();
            block->expressions.push_back(retExpr.get());
            ctx.expressions.push_back(move(retExpr));
            break;

        } else if (ins.type == InstructionType::JMP) {
            if (ins.jumpOffset < 0) {
                throw NotImplementedException("Negative jump offsets are not supported yet");
            }
            offset = ins.offset + ins.jumpOffset;
            continue;

        } else if (ins.type == InstructionType::JSR) {
            if (ins.jumpOffset < 0) {
                throw NotImplementedException("Negative jump offsets are not supported yet");
            }
            auto sub = make_shared<Function>();
            sub->offset = ins.offset + ins.jumpOffset;
            sub->block = decompile(ins.offset + ins.jumpOffset, ctx);

            auto callExpr = make_shared<CallExpression>();
            callExpr->offset = ins.offset;
            callExpr->function = sub.get();
            block->expressions.push_back(callExpr.get());

            ctx.functions.push_back(move(sub));
            ctx.expressions.push_back(move(callExpr));

        } else if (ins.type == InstructionType::JZ ||
                   ins.type == InstructionType::JNZ) {
            if (ins.jumpOffset < 0) {
                throw NotImplementedException("Negative jump offsets are not supported yet");
            }
            auto operand = ctx.stack.top().first;
            ctx.stack.pop();

            auto testExpr = make_shared<UnaryExpression>(ins.type == InstructionType::JZ ? ExpressionType::Zero : ExpressionType::NotZero);
            testExpr->offset = ins.offset;
            testExpr->operand = operand;

            auto condExpr = make_shared<ConditionalExpression>();
            condExpr->test = testExpr.get();
            auto trueCtx = DecompilationContext(ctx);
            condExpr->ifTrue = decompile(ins.offset + ins.jumpOffset, trueCtx);
            auto falseCtx = DecompilationContext(ctx);
            condExpr->ifFalse = decompile(ins.nextOffset, falseCtx);
            block->expressions.push_back(condExpr.get());

            ctx.expressions.push_back(move(testExpr));
            ctx.expressions.push_back(move(condExpr));
            break;

        } else if (ins.type == InstructionType::RSADDI ||
                   ins.type == InstructionType::RSADDF ||
                   ins.type == InstructionType::RSADDS ||
                   ins.type == InstructionType::RSADDO ||
                   ins.type == InstructionType::RSADDEFF ||
                   ins.type == InstructionType::RSADDEVT ||
                   ins.type == InstructionType::RSADDLOC ||
                   ins.type == InstructionType::RSADDTAL) {
            auto expression = parameterExpression(ins);
            block->expressions.push_back(expression.get());
            ctx.stack.push(make_pair(expression.get(), 0));
            ctx.expressions.push_back(move(expression));

        } else if (ins.type == InstructionType::CONSTI ||
                   ins.type == InstructionType::CONSTF ||
                   ins.type == InstructionType::CONSTS ||
                   ins.type == InstructionType::CONSTO) {
            auto expression = constantExpression(ins);
            block->expressions.push_back(expression.get());
            ctx.stack.push(make_pair(expression.get(), 0));
            ctx.expressions.push_back(move(expression));

        } else if (ins.type == InstructionType::ACTION) {
            auto &routine = ctx.routines.get(ins.routine);

            vector<Expression *> arguments;
            for (int i = 0; i < ins.argCount; ++i) {
                Expression *argument;
                auto argType = routine.getArgumentType(i);
                if (argType == VariableType::Vector) {
                    auto xParam = ctx.stack.top();
                    ctx.stack.pop();
                    auto yParam = ctx.stack.top();
                    ctx.stack.pop();
                    auto zParam = ctx.stack.top();
                    ctx.stack.pop();
                    if ((xParam.first->type != ExpressionType::Parameter || xParam.second != 0) ||
                        (yParam.first->type != ExpressionType::Parameter || yParam.second != 1) ||
                        (zParam.first->type != ExpressionType::Parameter || zParam.second != 2)) {
                        throw ValidationException("Not a vector on top of the stack");
                    }
                    argument = xParam.first;
                } else {
                    argument = ctx.stack.top().first;
                    ctx.stack.pop();
                }
                arguments.push_back(argument);
            }

            auto actionExpr = make_shared<ActionExpression>();
            actionExpr->offset = ins.offset;
            actionExpr->action = ins.routine;
            actionExpr->arguments = move(arguments);

            if (routine.returnType() != VariableType::Void) {
                auto retValExpr = make_shared<ParameterExpression>();
                retValExpr->offset = ins.offset;
                retValExpr->variableType = routine.returnType();
                block->expressions.push_back(retValExpr.get());

                auto assignExpr = make_shared<BinaryExpression>(ExpressionType::Assign);
                assignExpr->offset = ins.offset;
                assignExpr->left = retValExpr.get();
                assignExpr->right = actionExpr.get();
                block->expressions.push_back(assignExpr.get());

                if (routine.returnType() == VariableType::Vector) {
                    ctx.stack.push(make_pair(retValExpr.get(), 2));
                    ctx.stack.push(make_pair(retValExpr.get(), 1));
                    ctx.stack.push(make_pair(retValExpr.get(), 0));
                } else {
                    ctx.stack.push(make_pair(retValExpr.get(), 0));
                }
                ctx.expressions.push_back(move(retValExpr));
                ctx.expressions.push_back(move(assignExpr));

            } else {
                block->expressions.push_back(actionExpr.get());
            }

            ctx.expressions.push_back(move(actionExpr));

        } else if (ins.type == InstructionType::CPDOWNSP) {

        } else if (ins.type == InstructionType::CPTOPSP) {

        } else if (ins.type == InstructionType::MOVSP) {

        } else {
            throw ArgumentException("Cannot decompile expression of type " + to_string(static_cast<int>(ins.type)));
        }

        offset = ins.nextOffset;
    }

    ctx.expressions.push_back(block);

    return block.get();
}

unique_ptr<NwscriptProgram::ConstantExpression> NwscriptProgram::constantExpression(const Instruction &ins) {
    switch (ins.type) {
    case InstructionType::CONSTI:
    case InstructionType::CONSTF:
    case InstructionType::CONSTS:
    case InstructionType::CONSTO: {
        auto constExpr = make_unique<ConstantExpression>();
        constExpr->offset = ins.offset;
        if (ins.type == InstructionType::CONSTI) {
            constExpr->value = Variable::ofInt(ins.intValue);
        } else if (ins.type == InstructionType::CONSTF) {
            constExpr->value = Variable::ofFloat(ins.floatValue);
        } else if (ins.type == InstructionType::CONSTS) {
            constExpr->value = Variable::ofString(ins.strValue);
        } else if (ins.type == InstructionType::CONSTO) {
            constExpr->value = Variable::ofObject(ins.objectId);
        }
        return move(constExpr);
    }
    default:
        throw ArgumentException("Instruction is not of CONSTx type: " + to_string(static_cast<int>(ins.type)));
    }
}

unique_ptr<NwscriptProgram::ParameterExpression> NwscriptProgram::parameterExpression(const Instruction &ins) {
    switch (ins.type) {
    case InstructionType::RSADDI:
    case InstructionType::RSADDF:
    case InstructionType::RSADDS:
    case InstructionType::RSADDO:
    case InstructionType::RSADDEFF:
    case InstructionType::RSADDEVT:
    case InstructionType::RSADDLOC:
    case InstructionType::RSADDTAL: {
        auto paramExpr = make_unique<ParameterExpression>();
        paramExpr->offset = ins.offset;
        if (ins.type == InstructionType::RSADDI) {
            paramExpr->variableType = VariableType::Int;
        } else if (ins.type == InstructionType::RSADDF) {
            paramExpr->variableType = VariableType::Float;
        } else if (ins.type == InstructionType::RSADDS) {
            paramExpr->variableType = VariableType::String;
        } else if (ins.type == InstructionType::RSADDO) {
            paramExpr->variableType = VariableType::Object;
        } else if (ins.type == InstructionType::RSADDEFF) {
            paramExpr->variableType = VariableType::Effect;
        } else if (ins.type == InstructionType::RSADDEVT) {
            paramExpr->variableType = VariableType::Event;
        } else if (ins.type == InstructionType::RSADDLOC) {
            paramExpr->variableType = VariableType::Location;
        } else if (ins.type == InstructionType::RSADDTAL) {
            paramExpr->variableType = VariableType::Talent;
        }
        return move(paramExpr);
    }
    default:
        throw ArgumentException("Instruction is not of RSADDx type: " + to_string(static_cast<int>(ins.type)));
    }
}

} // namespace reone