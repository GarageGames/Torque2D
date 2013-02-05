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

#ifndef _AST_H_
#define _AST_H_

class ExprEvalState;
class Namespace;
class SimObject;
class SimGroup;

enum TypeReq {
   TypeReqNone,
   TypeReqUInt,
   TypeReqFloat,
   TypeReqString
};

/// Representation of a node for the scripting language parser.
///
/// When the scripting language is evaluated, it is turned from a string representation,
/// into a parse tree, thence into byte code, which is ultimately interpreted by the VM.
///
/// This is the base class for the nodes in the parse tree. There are a great many subclasses,
/// each representing a different language construct.
struct StmtNode
{
   StmtNode *next;   ///< Next entry in parse tree.

   StmtNode();

   /// @name next Accessors
   /// @{

   ///
   void append(StmtNode *next);
   StmtNode *getNext() { return next; }

   /// @}

   /// @name Debug Info
   /// @{

   StringTableEntry dbgFileName; ///< Name of file this node is associated with.
   S32 dbgLineNumber;            ///< Line number this node is associated with.
   /// @}

   /// @name Breaking
   /// @{

   void addBreakCount();
   void addBreakLine(U32 ip);
   /// @}

   /// @name Compilation
   /// @{

   virtual U32 precompileStmt(U32 loopCount) = 0;
   virtual U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint) = 0;
   virtual void setPackage(StringTableEntry packageName);
   /// @}
};

struct BreakStmtNode : StmtNode
{
   static BreakStmtNode *alloc();

   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);
};

struct ContinueStmtNode : StmtNode
{
   static ContinueStmtNode *alloc();
   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);
};

/// A mathematical expression.
struct ExprNode : StmtNode
{
   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);

   virtual U32 precompile(TypeReq type) = 0;
   virtual U32 compile(U32 *codeStream, U32 ip, TypeReq type) = 0;
   virtual TypeReq getPreferredType() = 0;
};

struct ReturnStmtNode : StmtNode
{
   ExprNode *expr;

   static ReturnStmtNode *alloc(ExprNode *expr);
   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);
};

struct IfStmtNode : StmtNode
{
   ExprNode *testExpr;
   StmtNode *ifBlock, *elseBlock;
   U32 endifOffset;
   U32 elseOffset;
   bool integer;
   bool propagate;

   static IfStmtNode *alloc(S32 lineNumber, ExprNode *testExpr, StmtNode *ifBlock, StmtNode *elseBlock, bool propagateThrough);
   void propagateSwitchExpr(ExprNode *left, bool string);
   ExprNode *getSwitchOR(ExprNode *left, ExprNode *list, bool string);
   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);
};

struct LoopStmtNode : StmtNode
{
   ExprNode *testExpr;
   ExprNode *initExpr;
   ExprNode *endLoopExpr;
   StmtNode *loopBlock;
   bool isDoLoop;
   U32 breakOffset;
   U32 continueOffset;
   U32 loopBlockStartOffset;
   bool integer;

   static LoopStmtNode *alloc(S32 lineNumber, ExprNode *testExpr, ExprNode *initExpr, ExprNode *endLoopExpr, StmtNode *loopBlock, bool isDoLoop);
   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);
};

/// A binary mathematical expression (ie, left op right).
struct BinaryExprNode : ExprNode
{
   S32 op;
   ExprNode *left;
   ExprNode *right;
};

struct FloatBinaryExprNode : BinaryExprNode
{
   static FloatBinaryExprNode *alloc(S32 op, ExprNode *left, ExprNode *right);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct ConditionalExprNode : ExprNode
{
   ExprNode *testExpr;
   ExprNode *trueExpr;
   ExprNode *falseExpr;
   bool integer;
   static ConditionalExprNode *alloc(ExprNode *testExpr, ExprNode *trueExpr, ExprNode *falseExpr);
   virtual U32 precompile(TypeReq type);
   virtual U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   virtual TypeReq getPreferredType();
};

struct IntBinaryExprNode : BinaryExprNode
{
   TypeReq subType;
   U32 operand;

   static IntBinaryExprNode *alloc(S32 op, ExprNode *left, ExprNode *right);

   void getSubTypeOperand();
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct StreqExprNode : BinaryExprNode
{
   bool eq;
   static StreqExprNode *alloc(ExprNode *left, ExprNode *right, bool eq);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct StrcatExprNode : BinaryExprNode
{
   int appendChar;
   static StrcatExprNode *alloc(ExprNode *left, ExprNode *right, int appendChar);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct CommaCatExprNode : BinaryExprNode
{
   static CommaCatExprNode *alloc(ExprNode *left, ExprNode *right);

   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct IntUnaryExprNode : ExprNode
{
   S32 op;
   ExprNode *expr;
   bool integer;

   static IntUnaryExprNode *alloc(S32 op, ExprNode *expr);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct FloatUnaryExprNode : ExprNode
{
   S32 op;
   ExprNode *expr;

   static FloatUnaryExprNode *alloc(S32 op, ExprNode *expr);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct VarNode : ExprNode
{
   StringTableEntry varName;
   ExprNode *arrayIndex;

   static VarNode *alloc(StringTableEntry varName, ExprNode *arrayIndex);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct IntNode : ExprNode
{
   S32 value;
   U32 index; // if it's converted to float/string

   static IntNode *alloc(S32 value);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct FloatNode : ExprNode
{
   F64 value;
   U32 index;

   static FloatNode *alloc(F64 value);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct StrConstNode : ExprNode
{
   char *str;
   F64 fVal;
   U32 index;
   bool tag;
   bool doc; // Specifies that this string is a documentation block.

   static StrConstNode *alloc(char *str, bool tag, bool doc = false);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct ConstantNode : ExprNode
{
   StringTableEntry value;
   F64 fVal;
   U32 index;

   static ConstantNode *alloc(StringTableEntry value);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct AssignExprNode : ExprNode
{
   StringTableEntry varName;
   ExprNode *expr;
   ExprNode *arrayIndex;
   TypeReq subType;

   static AssignExprNode *alloc(StringTableEntry varName, ExprNode *arrayIndex, ExprNode *expr);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct AssignDecl
{
   S32 token;
   ExprNode *expr;
   bool integer;
};

struct AssignOpExprNode : ExprNode
{
   StringTableEntry varName;
   ExprNode *expr;
   ExprNode *arrayIndex;
   S32 op;
   U32 operand;
   TypeReq subType;

   static AssignOpExprNode *alloc(StringTableEntry varName, ExprNode *arrayIndex, ExprNode *expr, S32 op);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct TTagSetStmtNode : StmtNode
{
   StringTableEntry tag;
   ExprNode *valueExpr;
   ExprNode *stringExpr;

   static TTagSetStmtNode *alloc(StringTableEntry tag, ExprNode *valueExpr, ExprNode *stringExpr);
   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);
};

struct TTagDerefNode : ExprNode
{
   ExprNode *expr;

   static TTagDerefNode *alloc(ExprNode *expr);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct TTagExprNode : ExprNode
{
   StringTableEntry tag;

   static TTagExprNode *alloc(StringTableEntry tag);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct FuncCallExprNode : ExprNode
{
   StringTableEntry funcName;
   StringTableEntry nameSpace;
   ExprNode *args;
   U32 callType;
   enum {
      FunctionCall,
      MethodCall,
      ParentCall
   };

   static FuncCallExprNode *alloc(StringTableEntry funcName, StringTableEntry nameSpace, ExprNode *args, bool dot);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct SlotDecl
{
   ExprNode         *object;
   StringTableEntry slotName;
   ExprNode         *array;
};

struct SlotAccessNode : ExprNode
{
   ExprNode *objectExpr, *arrayExpr;
   StringTableEntry slotName;

   static SlotAccessNode *alloc(ExprNode *objectExpr, ExprNode *arrayExpr, StringTableEntry slotName);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct InternalSlotDecl
{
   ExprNode         *object;
   ExprNode         *slotExpr;
   bool             recurse;
};

struct InternalSlotAccessNode : ExprNode
{
   ExprNode *objectExpr, *slotExpr;
   bool recurse;

   static InternalSlotAccessNode *alloc(ExprNode *objectExpr, ExprNode *slotExpr, bool recurse);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct SlotAssignNode : ExprNode
{
   ExprNode *objectExpr, *arrayExpr;
   StringTableEntry slotName;
   ExprNode *valueExpr;

   static SlotAssignNode *alloc(ExprNode *objectExpr, ExprNode *arrayExpr, StringTableEntry slotName, ExprNode *valueExpr);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct SlotAssignOpNode : ExprNode
{
   ExprNode *objectExpr, *arrayExpr;
   StringTableEntry slotName;
   S32 op;
   ExprNode *valueExpr;
   U32 operand;
   TypeReq subType;

   static SlotAssignOpNode *alloc(ExprNode *objectExpr, StringTableEntry slotName, ExprNode *arrayExpr, S32 op, ExprNode *valueExpr);
   U32 precompile(TypeReq type);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   TypeReq getPreferredType();
};

struct ObjectDeclNode : ExprNode
{
   ExprNode *classNameExpr;
   StringTableEntry parentObject;
   ExprNode *objectNameExpr;
   ExprNode *argList;
   SlotAssignNode *slotDecls;
   ObjectDeclNode *subObjects;
   bool structDecl;
   U32 failOffset;
   bool isClassNameInternal;
   bool isMessage;

   static ObjectDeclNode *alloc(ExprNode *classNameExpr, ExprNode *objectNameExpr, ExprNode *argList, StringTableEntry parentObject, SlotAssignNode *slotDecls, ObjectDeclNode *subObjects, bool structDecl, bool classNameInternal, bool isMessage);
   U32 precompile(TypeReq type);
   U32 precompileSubObject(bool);
   U32 compile(U32 *codeStream, U32 ip, TypeReq type);
   U32 compileSubObject(U32 *codeStream, U32 ip, bool);
   TypeReq getPreferredType();
};

struct ObjectBlockDecl
{
   SlotAssignNode *slots;
   ObjectDeclNode *decls;
};

struct FunctionDeclStmtNode : StmtNode
{
   StringTableEntry fnName;
   VarNode *args;
   StmtNode *stmts;
   StringTableEntry nameSpace;
   StringTableEntry package;
   U32 endOffset;
   U32 argc;

   static FunctionDeclStmtNode *alloc(StringTableEntry fnName, StringTableEntry nameSpace, VarNode *args, StmtNode *stmts);
   U32 precompileStmt(U32 loopCount);
   U32 compileStmt(U32 *codeStream, U32 ip, U32 continuePoint, U32 breakPoint);
   void setPackage(StringTableEntry packageName);
};

extern StmtNode *statementList;
extern void createFunction(const char *fnName, VarNode *args, StmtNode *statements);
extern ExprEvalState gEvalState;
extern bool lookupFunction(const char *fnName, VarNode **args, StmtNode **statements);
typedef const char *(*cfunc)(S32 argc, char **argv);
extern bool lookupCFunction(const char *fnName, cfunc *f);

#endif
