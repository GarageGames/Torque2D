//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/compiler.h"
#include "console/consoleInternal.h"

using namespace Compiler;

/// @file
///
/// TorqueScript AST node allocators.
///
/// These static methods exist to allocate new AST node for the compiler. They
/// all allocate memory from the consoleAllocator for efficiency, and often take
/// arguments relating to the state of the nodes. They are called from gram.y
/// (really gram.c) as the lexer analyzes the script code.

//------------------------------------------------------------

BreakStmtNode *BreakStmtNode::alloc()
{
   BreakStmtNode *ret = (BreakStmtNode *) consoleAlloc(sizeof(BreakStmtNode));
   constructInPlace(ret);
   return ret;
}

ContinueStmtNode *ContinueStmtNode::alloc()
{
   ContinueStmtNode *ret = (ContinueStmtNode *) consoleAlloc(sizeof(ContinueStmtNode));
   constructInPlace(ret);
   return ret;
}

ReturnStmtNode *ReturnStmtNode::alloc(ExprNode *expr)
{
   ReturnStmtNode *ret = (ReturnStmtNode *) consoleAlloc(sizeof(ReturnStmtNode));
   constructInPlace(ret);
   ret->expr = expr;

   return ret;
}

IfStmtNode *IfStmtNode::alloc(S32 lineNumber, ExprNode *testExpr, StmtNode *ifBlock, StmtNode *elseBlock, bool propagate)
{
   IfStmtNode *ret = (IfStmtNode *) consoleAlloc(sizeof(IfStmtNode));
   constructInPlace(ret);
   ret->dbgLineNumber = lineNumber;

   ret->testExpr = testExpr;
   ret->ifBlock = ifBlock;
   ret->elseBlock = elseBlock;
   ret->propagate = propagate;

   return ret;
}

LoopStmtNode *LoopStmtNode::alloc(S32 lineNumber, ExprNode *initExpr, ExprNode *testExpr, ExprNode *endLoopExpr, StmtNode *loopBlock, bool isDoLoop)
{
   LoopStmtNode *ret = (LoopStmtNode *) consoleAlloc(sizeof(LoopStmtNode));
   constructInPlace(ret);
   ret->dbgLineNumber = lineNumber;
   ret->testExpr = testExpr;
   ret->initExpr = initExpr;
   ret->endLoopExpr = endLoopExpr;
   ret->loopBlock = loopBlock;
   ret->isDoLoop = isDoLoop;

   // Deal with setting some dummy constant nodes if we weren't provided with
   // info... This allows us to play nice with missing parts of for(;;) for
   // instance.
   if(!ret->testExpr) ret->testExpr = IntNode::alloc(1);

   return ret;
}

FloatBinaryExprNode *FloatBinaryExprNode::alloc(S32 op, ExprNode *left, ExprNode *right)
{
   FloatBinaryExprNode *ret = (FloatBinaryExprNode *) consoleAlloc(sizeof(FloatBinaryExprNode));
   constructInPlace(ret);

   ret->op = op;
   ret->left = left;
   ret->right = right;

   return ret;
}

IntBinaryExprNode *IntBinaryExprNode::alloc(S32 op, ExprNode *left, ExprNode *right)
{
   IntBinaryExprNode *ret = (IntBinaryExprNode *) consoleAlloc(sizeof(IntBinaryExprNode));
   constructInPlace(ret);

   ret->op = op;
   ret->left = left;
   ret->right = right;

   return ret;
}

StreqExprNode *StreqExprNode::alloc(ExprNode *left, ExprNode *right, bool eq)
{
   StreqExprNode *ret = (StreqExprNode *) consoleAlloc(sizeof(StreqExprNode));
   constructInPlace(ret);
   ret->left = left;
   ret->right = right;
   ret->eq = eq;

   return ret;
}

StrcatExprNode *StrcatExprNode::alloc(ExprNode *left, ExprNode *right, int appendChar)
{
   StrcatExprNode *ret = (StrcatExprNode *) consoleAlloc(sizeof(StrcatExprNode));
   constructInPlace(ret);
   ret->left = left;
   ret->right = right;
   ret->appendChar = appendChar;

   return ret;
}

CommaCatExprNode *CommaCatExprNode::alloc(ExprNode *left, ExprNode *right)
{
   CommaCatExprNode *ret = (CommaCatExprNode *) consoleAlloc(sizeof(CommaCatExprNode));
   constructInPlace(ret);
   ret->left = left;
   ret->right = right;

   return ret;
}

IntUnaryExprNode *IntUnaryExprNode::alloc(S32 op, ExprNode *expr)
{
   IntUnaryExprNode *ret = (IntUnaryExprNode *) consoleAlloc(sizeof(IntUnaryExprNode));
   constructInPlace(ret);
   ret->op = op;
   ret->expr = expr;
   return ret;
}

FloatUnaryExprNode *FloatUnaryExprNode::alloc(S32 op, ExprNode *expr)
{
   FloatUnaryExprNode *ret = (FloatUnaryExprNode *) consoleAlloc(sizeof(FloatUnaryExprNode));
   constructInPlace(ret);
   ret->op = op;
   ret->expr = expr;
   return ret;
}

VarNode *VarNode::alloc(StringTableEntry varName, ExprNode *arrayIndex)
{
   VarNode *ret = (VarNode *) consoleAlloc(sizeof(VarNode));
   constructInPlace(ret);
   ret->varName = varName;
   ret->arrayIndex = arrayIndex;
   return ret;
}

IntNode *IntNode::alloc(S32 value)
{
   IntNode *ret = (IntNode *) consoleAlloc(sizeof(IntNode));
   constructInPlace(ret);
   ret->value = value;
   return ret;
}

ConditionalExprNode *ConditionalExprNode::alloc(ExprNode *testExpr, ExprNode *trueExpr, ExprNode *falseExpr)
{
   ConditionalExprNode *ret = (ConditionalExprNode *) consoleAlloc(sizeof(ConditionalExprNode));
   constructInPlace(ret);
   ret->testExpr = testExpr;
   ret->trueExpr = trueExpr;
   ret->falseExpr = falseExpr;
   ret->integer = false;
   return ret;
}

FloatNode *FloatNode::alloc(F64 value)
{
   FloatNode *ret = (FloatNode *) consoleAlloc(sizeof(FloatNode));
   constructInPlace(ret);
   ret->value = value;
   return ret;
}

StrConstNode *StrConstNode::alloc(char *str, bool tag, bool doc)
{
   StrConstNode *ret = (StrConstNode *) consoleAlloc(sizeof(StrConstNode));
   constructInPlace(ret);
   ret->str = (char *) consoleAlloc(dStrlen(str) + 1);
   ret->tag = tag;
   ret->doc = doc;
   dStrcpy(ret->str, str);

   return ret;
}

ConstantNode *ConstantNode::alloc(StringTableEntry value)
{
   ConstantNode *ret = (ConstantNode *) consoleAlloc(sizeof(ConstantNode));
   constructInPlace(ret);
   ret->value = value;
   return ret;
}

AssignExprNode *AssignExprNode::alloc(StringTableEntry varName, ExprNode *arrayIndex, ExprNode *expr)
{
   AssignExprNode *ret = (AssignExprNode *) consoleAlloc(sizeof(AssignExprNode));
   constructInPlace(ret);
   ret->varName = varName;
   ret->expr = expr;
   ret->arrayIndex = arrayIndex;

   return ret;
}

AssignOpExprNode *AssignOpExprNode::alloc(StringTableEntry varName, ExprNode *arrayIndex, ExprNode *expr, S32 op)
{
   AssignOpExprNode *ret = (AssignOpExprNode *) consoleAlloc(sizeof(AssignOpExprNode));
   constructInPlace(ret);
   ret->varName = varName;
   ret->expr = expr;
   ret->arrayIndex = arrayIndex;
   ret->op = op;
   return ret;
}

TTagSetStmtNode *TTagSetStmtNode::alloc(StringTableEntry tag, ExprNode *valueExpr, ExprNode *stringExpr)
{
   TTagSetStmtNode *ret = (TTagSetStmtNode *) consoleAlloc(sizeof(TTagSetStmtNode));
   constructInPlace(ret);
   ret->tag = tag;
   ret->valueExpr = valueExpr;
   ret->stringExpr = stringExpr;
   return ret;
}

TTagDerefNode *TTagDerefNode::alloc(ExprNode *expr)
{
   TTagDerefNode *ret = (TTagDerefNode *) consoleAlloc(sizeof(TTagDerefNode));
   constructInPlace(ret);
   ret->expr = expr;
   return ret;
}

TTagExprNode *TTagExprNode::alloc(StringTableEntry tag)
{
   TTagExprNode *ret = (TTagExprNode *) consoleAlloc(sizeof(TTagExprNode));
   constructInPlace(ret);
   ret->tag = tag;
   return ret;
}

FuncCallExprNode *FuncCallExprNode::alloc(StringTableEntry funcName, StringTableEntry nameSpace, ExprNode *args, bool dot)
{
   FuncCallExprNode *ret = (FuncCallExprNode *) consoleAlloc(sizeof(FuncCallExprNode));
   constructInPlace(ret);
   ret->funcName = funcName;
   ret->nameSpace = nameSpace;
   ret->args = args;
   if(dot)
      ret->callType = MethodCall;
   else
   {
      if(nameSpace && !dStricmp(nameSpace, "Parent"))
         ret->callType = ParentCall;
      else
         ret->callType = FunctionCall;
   }
   return ret;
}

SlotAccessNode *SlotAccessNode::alloc(ExprNode *objectExpr, ExprNode *arrayExpr, StringTableEntry slotName)
{
   SlotAccessNode *ret = (SlotAccessNode *) consoleAlloc(sizeof(SlotAccessNode));
   constructInPlace(ret);
   ret->objectExpr = objectExpr;
   ret->arrayExpr = arrayExpr;
   ret->slotName = slotName;
   return ret;
}

InternalSlotAccessNode *InternalSlotAccessNode::alloc(ExprNode *objectExpr, ExprNode *slotExpr, bool recurse)
{
   InternalSlotAccessNode *ret = (InternalSlotAccessNode *) consoleAlloc(sizeof(InternalSlotAccessNode));
   constructInPlace(ret);
   ret->objectExpr = objectExpr;
   ret->slotExpr = slotExpr;
   ret->recurse = recurse;
   return ret;
}

SlotAssignNode *SlotAssignNode::alloc(ExprNode *objectExpr, ExprNode *arrayExpr, StringTableEntry slotName, ExprNode *valueExpr)
{
   SlotAssignNode *ret = (SlotAssignNode *) consoleAlloc(sizeof(SlotAssignNode));
   constructInPlace(ret);
   ret->objectExpr = objectExpr;
   ret->arrayExpr = arrayExpr;
   ret->slotName = slotName;
   ret->valueExpr = valueExpr;
   return ret;
}

SlotAssignOpNode *SlotAssignOpNode::alloc(ExprNode *objectExpr, StringTableEntry slotName, ExprNode *arrayExpr, S32 op, ExprNode *valueExpr)
{
   SlotAssignOpNode *ret = (SlotAssignOpNode *) consoleAlloc(sizeof(SlotAssignOpNode));
   constructInPlace(ret);
   ret->objectExpr = objectExpr;
   ret->arrayExpr = arrayExpr;
   ret->slotName = slotName;
   ret->op = op;
   ret->valueExpr = valueExpr;
   return ret;
}

ObjectDeclNode *ObjectDeclNode::alloc(ExprNode *classNameExpr, ExprNode *objectNameExpr, ExprNode *argList, StringTableEntry parentObject, SlotAssignNode *slotDecls, ObjectDeclNode *subObjects, bool structDecl, bool classNameInternal, bool isMessage)
{
   ObjectDeclNode *ret = (ObjectDeclNode *) consoleAlloc(sizeof(ObjectDeclNode));
   constructInPlace(ret);
   ret->classNameExpr = classNameExpr;
   ret->objectNameExpr = objectNameExpr;
   ret->argList = argList;
   ret->slotDecls = slotDecls;
   ret->subObjects = subObjects;
   ret->structDecl = structDecl;
   ret->isClassNameInternal = classNameInternal;
   ret->isMessage = isMessage;
   if(parentObject)
      ret->parentObject = parentObject;
   else
      ret->parentObject = StringTable->EmptyString;
   return ret;
}

FunctionDeclStmtNode *FunctionDeclStmtNode::alloc(StringTableEntry fnName, StringTableEntry nameSpace, VarNode *args, StmtNode *stmts)
{
   FunctionDeclStmtNode *ret = (FunctionDeclStmtNode *) consoleAlloc(sizeof(FunctionDeclStmtNode));
   constructInPlace(ret);
   ret->fnName = fnName;
   ret->args = args;
   ret->stmts = stmts;
   ret->nameSpace = nameSpace;
   ret->package = NULL;
   return ret;
}
