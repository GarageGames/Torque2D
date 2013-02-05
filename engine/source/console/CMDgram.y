%{

// Make sure we don't get gram.h twice.
#define _CMDGRAM_H_

#include <stdlib.h>
#include <stdio.h>
#include "console/console.h"
#include "console/compiler.h"
#include "console/consoleInternal.h"

#ifndef YYDEBUG
#define YYDEBUG 0
#endif

#define YYSSIZE 350

int outtext(char *fmt, ...);
extern int serrors;

#define nil 0
#undef YY_ARGS
#define YY_ARGS(x)   x
 
int CMDlex();
void CMDerror(char *, ...); 

#define alloca dMalloc

%}
%{
        /* Reserved Word Definitions */
%}
%token <i> rwDEFINE rwENDDEF rwDECLARE
%token <i> rwBREAK rwELSE rwCONTINUE rwGLOBAL
%token <i> rwIF rwNIL rwRETURN rwWHILE rwDO
%token <i> rwENDIF rwENDWHILE rwENDFOR rwDEFAULT
%token <i> rwFOR rwDATABLOCK rwSWITCH rwCASE rwSWITCHSTR
%token <i> rwCASEOR rwPACKAGE rwNAMESPACE rwCLASS rwMESSAGE
%token ILLEGAL_TOKEN
%{
        /* Constants and Identifier Definitions */
%}
%token <c>   CHRCONST
%token <i>   INTCONST
%token <s>   TTAG
%token <s>   VAR
%token <s>   IDENT
%token <str> DOCBLOCK
%token <str> STRATOM
%token <str> TAGATOM
%token <f>   FLTCONST

%{
        /* Operator Definitions */
%}
%token <i> '+' '-' '*' '/' '<' '>' '=' '.' '|' '&' '%'
%token <i> '(' ')' ',' ':' ';' '{' '}' '^' '~' '!' '@'
%token <i> opINTNAME opINTNAMER
%token <i> opMINUSMINUS opPLUSPLUS
%token <i> STMT_SEP
%token <i> opSHL opSHR opPLASN opMIASN opMLASN opDVASN opMODASN opANDASN
%token <i> opXORASN opORASN opSLASN opSRASN opCAT
%token <i> opEQ opNE opGE opLE opAND opOR opSTREQ
%token <i> opCOLONCOLON

%union {
   char              c;
   int               i;
   const char *      s;
   char *            str;
   double            f;
   StmtNode *        stmt;
   ExprNode *        expr;
   SlotAssignNode *  slist;
   VarNode *         var;
   SlotDecl          slot;
   InternalSlotDecl  intslot;
   ObjectBlockDecl   odcl;
   ObjectDeclNode *  od;
   AssignDecl        asn;
   IfStmtNode *      ifnode;
}

%type <s>      parent_block
%type <ifnode> case_block
%type <stmt>   switch_stmt
%type <stmt>   decl
%type <stmt>   decl_list
%type <stmt>   package_decl
%type <stmt>   fn_decl_stmt
%type <stmt>   fn_decl_list
%type <stmt>   statement_list
%type <stmt>   stmt
%type <expr>   expr_list
%type <expr>   expr_list_decl
%type <expr>   aidx_expr
%type <expr>   funcall_expr
%type <expr>   object_name
%type <expr>   object_args
%type <expr>   stmt_expr
%type <expr>   case_expr
%type <expr>   class_name_expr
%type <stmt>   if_stmt
%type <stmt>   while_stmt
%type <stmt>   for_stmt
%type <stmt>   stmt_block
%type <stmt>   datablock_decl
%type <od>	   message_decl
%type <od>     object_decl
%type <od>     object_decl_list
%type <odcl>   object_declare_block
%type <odcl>   message_declare_block
%type <expr>   expr
%type <slist>  slot_assign_list
%type <slist>  slot_assign
%type <slot>   slot_acc
%type <intslot>	intslot_acc
%type <stmt>   expression_stmt
%type <var>    var_list
%type <var>    var_list_decl
%type <asn>    assign_op_struct

%left '['
%right opMODASN opANDASN opXORASN opPLASN opMIASN opMLASN opDVASN opMDASN opNDASN opNTASN opORASN opSLASN opSRASN '='
%left '?' ':'
%left opOR
%left opAND 
%left '|'
%left '^'
%left '&'
%left opEQ opNE
%left '<' opLE '>' opGE
%left '@' opCAT opSTREQ opSTRNE
%left opSHL opSHR
%left '+' '-'
%left '*' '/' '%'
%right '!' '~' opPLUSPLUS opMINUSMINUS UNARY
%left '.'
%left opINTNAME opINTNAMER

%%

start
   : decl_list
      { }
   ;

decl_list
   :
      { $$ = nil; }
   | decl_list decl
      { if(!statementList) { statementList = $2; } else { statementList->append($2); } }
   ;
   
decl
   : stmt
      { $$ = $1; }
   | fn_decl_stmt
      { $$ = $1; }
   | package_decl
     { $$ = $1; }
   ;

package_decl
   : rwPACKAGE IDENT '{' fn_decl_list '}' ';'
      { $$ = $4; for(StmtNode *walk = ($4);walk;walk = walk->getNext() ) walk->setPackage($2); }
   ;

fn_decl_list
   : fn_decl_stmt
      { $$ = $1; }
   | fn_decl_list fn_decl_stmt
      { $$ = $1; ($1)->append($2);  }
   ;

statement_list
   :
      { $$ = nil; }
   | statement_list stmt
      { if(!$1) { $$ = $2; } else { ($1)->append($2); $$ = $1; } }
   ;

stmt
   : if_stmt
   | while_stmt
   | for_stmt
   | datablock_decl
   | switch_stmt
   | rwBREAK ';'
      { $$ = BreakStmtNode::alloc(); }
   | rwCONTINUE ';'
      { $$ = ContinueStmtNode::alloc(); }
   | rwRETURN ';'
      { $$ = ReturnStmtNode::alloc(NULL); }
   | rwRETURN expr ';'
      { $$ = ReturnStmtNode::alloc($2); }
   | expression_stmt ';'
      { $$ = $1; }
   | TTAG '=' expr ';'
      { $$ = TTagSetStmtNode::alloc($1, $3, NULL); }
   | TTAG '=' expr ',' expr ';'
      { $$ = TTagSetStmtNode::alloc($1, $3, $5); }
   | DOCBLOCK
      { $$ = StrConstNode::alloc($1, false, true); }
   ;

fn_decl_stmt
   : rwDEFINE IDENT '(' var_list_decl ')' '{' statement_list '}'
      { $$ = FunctionDeclStmtNode::alloc($2, NULL, $4, $7); }
    | rwDEFINE IDENT opCOLONCOLON IDENT '(' var_list_decl ')' '{' statement_list '}'
     { $$ = FunctionDeclStmtNode::alloc($4, $2, $6, $9); }
   ;

var_list_decl
   :
     { $$ = NULL; }
   | var_list
     { $$ = $1; }
   ;

var_list
   : VAR
      { $$ = VarNode::alloc($1, NULL); }
   | var_list ',' VAR
      { $$ = $1; ((StmtNode*)($1))->append((StmtNode*)VarNode::alloc($3, NULL)); }
   ;

datablock_decl
   : rwDATABLOCK IDENT '(' IDENT parent_block ')'  '{' slot_assign_list '}' ';'
      { $$ = ObjectDeclNode::alloc(ConstantNode::alloc($2), ConstantNode::alloc($4), NULL, $5, $8, NULL, true, false, false); }
   ;

message_decl
   : rwMESSAGE class_name_expr '(' object_name parent_block')' '{' message_declare_block '}'
      { $$ = ObjectDeclNode::alloc($2, $4, NULL, $5, $8.slots, $8.decls, false, false, true); }
   | rwMESSAGE class_name_expr '(' object_name parent_block ')'
      { $$ = ObjectDeclNode::alloc($2, $4, NULL, $5, NULL, NULL, false, false, true); }
   ;

object_decl
   : rwDECLARE class_name_expr '(' object_name parent_block object_args ')' '{' object_declare_block '}'
      { $$ = ObjectDeclNode::alloc($2, $4, $6, $5, $9.slots, $9.decls, false, false, false); }
   | rwDECLARE class_name_expr '(' object_name parent_block object_args ')'
      { $$ = ObjectDeclNode::alloc($2, $4, $6, $5, NULL, NULL, false, false, false); }
   | rwDECLARE class_name_expr '(' '[' object_name ']' parent_block object_args ')' '{' object_declare_block '}'
      { $$ = ObjectDeclNode::alloc($2, $5, $8, $7, $11.slots, $11.decls, false, true, false); }
   | rwDECLARE class_name_expr '(' '[' object_name ']' parent_block object_args ')'
      { $$ = ObjectDeclNode::alloc($2, $5, $8, $7, NULL, NULL, false, true, false); }
   ;

parent_block
   :
      { $$ = NULL; }
   | ':' IDENT
      { $$ = $2; }
   ;

object_name
   :
      { $$ = StrConstNode::alloc("", false); }
   | expr
      { $$ = $1; }
   ;

object_args
   :
      { $$ = NULL; }
   | ',' expr_list
      { $$ = $2; }
   ;
   
message_declare_block
	:
	  { $$.slots = NULL; $$.decls = NULL; }
    | slot_assign_list
      { $$.slots = $1; $$.decls = NULL; }
	;
	
object_declare_block
   :
      { $$.slots = NULL; $$.decls = NULL; }
   | slot_assign_list
      { $$.slots = $1; $$.decls = NULL; }
   | object_decl_list
      { $$.slots = NULL; $$.decls = $1; }
   | slot_assign_list object_decl_list
      { $$.slots = $1; $$.decls = $2; }
   ;

object_decl_list
   : object_decl ';'
      { $$ = $1; }
   | object_decl_list object_decl ';'
      { $1->append($2); $$ = $1; }
   ;

stmt_block
   : '{' statement_list '}'
      { $$ = $2; }
   | stmt
      { $$ = $1; }
   ;

switch_stmt
   : rwSWITCH '(' expr ')' '{' case_block '}'
      { $$ = $6; $6->propagateSwitchExpr($3, false); }
   | rwSWITCHSTR '(' expr ')' '{' case_block '}'
      { $$ = $6; $6->propagateSwitchExpr($3, true); }
   ;

case_block
   : rwCASE case_expr ':' statement_list
      { $$ = IfStmtNode::alloc($1, $2, $4, NULL, false); }
   | rwCASE case_expr ':' statement_list rwDEFAULT ':' statement_list
      { $$ = IfStmtNode::alloc($1, $2, $4, $7, false); }
   | rwCASE case_expr ':' statement_list case_block
      { $$ = IfStmtNode::alloc($1, $2, $4, $5, true); }
   ;

case_expr
   : expr
      { $$ = $1;}
   | case_expr rwCASEOR expr
      { ($1)->append($3); $$=$1; }
   ;

if_stmt
   : rwIF '(' expr ')' stmt_block
      { $$ = IfStmtNode::alloc($1, $3, $5, NULL, false); }
   | rwIF '(' expr ')' stmt_block rwELSE stmt_block
      { $$ = IfStmtNode::alloc($1, $3, $5, $7, false); }
   ;

while_stmt
   : rwWHILE '(' expr ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, nil, $3, nil, $5, false); }
   | rwDO stmt_block rwWHILE '(' expr ')'
      { $$ = LoopStmtNode::alloc($3, nil, $5, nil, $2, true); }
   ;

for_stmt
   : rwFOR '(' expr ';' expr ';' expr ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, $3, $5, $7, $9, false); }
   | rwFOR '(' expr ';' expr ';'      ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, $3, $5, NULL, $8, false); }
   | rwFOR '(' expr ';'      ';' expr ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, $3, NULL, $6, $8, false); }
   | rwFOR '(' expr ';'      ';'      ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, $3, NULL, NULL, $7, false); }
   | rwFOR '('      ';' expr ';' expr ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, NULL, $4, $6, $8, false); }
   | rwFOR '('      ';' expr ';'      ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, NULL, $4, NULL, $7, false); }
   | rwFOR '('      ';'      ';' expr ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, NULL, NULL, $5, $7, false); }
   | rwFOR '('      ';'      ';'      ')' stmt_block
      { $$ = LoopStmtNode::alloc($1, NULL, NULL, NULL, $6, false); }
   ;  

expression_stmt
   : stmt_expr
      { $$ = $1; }
   ;

expr
   : stmt_expr
      { $$ = $1; }
   | '(' expr ')'
      { $$ = $2; }
   | expr '^' expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr '%' expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr '&' expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr '|' expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr '+' expr
      { $$ = FloatBinaryExprNode::alloc($2, $1, $3); }
   | expr '-' expr
      { $$ = FloatBinaryExprNode::alloc($2, $1, $3); }
   | expr '*' expr
      { $$ = FloatBinaryExprNode::alloc($2, $1, $3); }
   | expr '/' expr
      { $$ = FloatBinaryExprNode::alloc($2, $1, $3); }
   | '-' expr  %prec UNARY
      { $$ = FloatUnaryExprNode::alloc($1, $2); }
   | '*' expr %prec UNARY
      { $$ = TTagDerefNode::alloc($2); }
   | TTAG
      { $$ = TTagExprNode::alloc($1); }
   | expr '?' expr ':' expr
      { $$ = ConditionalExprNode::alloc($1, $3, $5); }
   | expr '<' expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr '>' expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opGE expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opLE expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opEQ expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opNE expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opOR expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opSHL expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opSHR expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opAND expr
      { $$ = IntBinaryExprNode::alloc($2, $1, $3); }
   | expr opSTREQ expr
      { $$ = StreqExprNode::alloc($1, $3, true); }
   | expr opSTRNE expr
      { $$ = StreqExprNode::alloc($1, $3, false); }
   | expr '@' expr
      { $$ = StrcatExprNode::alloc($1, $3, $2); }
   | '!' expr
      { $$ = IntUnaryExprNode::alloc($1, $2); }
   | '~' expr
      { $$ = IntUnaryExprNode::alloc($1, $2); }
   | TAGATOM
      { $$ = StrConstNode::alloc($1, true); }
   | FLTCONST
      { $$ = FloatNode::alloc($1); }
   | INTCONST
      { $$ = IntNode::alloc($1); }
   | rwBREAK
      { $$ = ConstantNode::alloc(StringTable->insert("break")); }
   | slot_acc
      { $$ = SlotAccessNode::alloc($1.object, $1.array, $1.slotName); }
   | intslot_acc
      { $$ = InternalSlotAccessNode::alloc($1.object, $1.slotExpr, $1.recurse); }
   | IDENT
      { $$ = ConstantNode::alloc($1); }
   | STRATOM
      { $$ = StrConstNode::alloc($1, false); }
   | VAR
      { $$ = (ExprNode*)VarNode::alloc($1, NULL); }
   | VAR '[' aidx_expr ']'
      { $$ = (ExprNode*)VarNode::alloc($1, $3); }
   ;

slot_acc
   : expr '.' IDENT
      { $$.object = $1; $$.slotName = $3; $$.array = NULL; }
   | expr '.' IDENT '[' aidx_expr ']'
      { $$.object = $1; $$.slotName = $3; $$.array = $5; }
   ;

intslot_acc
   : expr opINTNAME class_name_expr
	  { $$.object = $1; $$.slotExpr = $3; $$.recurse = false; }
   | expr opINTNAMER class_name_expr
	  { $$.object = $1; $$.slotExpr = $3; $$.recurse = true; }
   ;
   
class_name_expr
   : IDENT
      { $$ = ConstantNode::alloc($1); }
   | '(' expr ')'
      { $$ = $2; }
   ;

assign_op_struct
   : opPLUSPLUS
      { $$.token = '+'; $$.expr = FloatNode::alloc(1); }
   | opMINUSMINUS
      { $$.token = '-'; $$.expr = FloatNode::alloc(1); }
   | opPLASN expr
      { $$.token = '+'; $$.expr = $2; }
   | opMIASN expr
      { $$.token = '-'; $$.expr = $2; }
   | opMLASN expr
      { $$.token = '*'; $$.expr = $2; }
   | opDVASN expr
      { $$.token = '/'; $$.expr = $2; }
   | opMODASN expr
      { $$.token = '%'; $$.expr = $2; }
   | opANDASN expr
      { $$.token = '&'; $$.expr = $2; }
   | opXORASN expr
      { $$.token = '^'; $$.expr = $2; }
   | opORASN expr
      { $$.token = '|'; $$.expr = $2; }
   | opSLASN expr
      { $$.token = opSHL; $$.expr = $2; }
   | opSRASN expr
      { $$.token = opSHR; $$.expr = $2; }
   ;

stmt_expr
   : funcall_expr
      { $$ = $1; }
   | object_decl
      { $$ = $1; }
   | message_decl
      { $$ = $1; }
   | VAR '=' expr
      { $$ = AssignExprNode::alloc($1, NULL, $3); }
   | VAR '[' aidx_expr ']' '=' expr
      { $$ = AssignExprNode::alloc($1, $3, $6); }
   | VAR assign_op_struct
      { $$ = AssignOpExprNode::alloc($1, NULL, $2.expr, $2.token); }
   | VAR '[' aidx_expr ']' assign_op_struct
      { $$ = AssignOpExprNode::alloc($1, $3, $5.expr, $5.token); }
   | slot_acc assign_op_struct
      { $$ = SlotAssignOpNode::alloc($1.object, $1.slotName, $1.array, $2.token, $2.expr); }
   | slot_acc '=' expr
      { $$ = SlotAssignNode::alloc($1.object, $1.array, $1.slotName, $3); }
   | slot_acc '=' '{' expr_list '}'
      { $$ = SlotAssignNode::alloc($1.object, $1.array, $1.slotName, $4); }
   ;

funcall_expr
   : IDENT '(' expr_list_decl ')'
     { $$ = FuncCallExprNode::alloc($1, NULL, $3, false); }
   | IDENT opCOLONCOLON IDENT '(' expr_list_decl ')'
     { $$ = FuncCallExprNode::alloc($3, $1, $5, false); }
   | expr '.' IDENT '(' expr_list_decl ')'
      { $1->append($5); $$ = FuncCallExprNode::alloc($3, NULL, $1, true); }
   ;

expr_list_decl
   :
      { $$ = NULL; }
   | expr_list
      { $$ = $1; }
   ;

expr_list
   : expr
      { $$ = $1; }
   | expr_list ',' expr
      { ($1)->append($3); $$ = $1; }
   ;

slot_assign_list
   : slot_assign
      { $$ = $1; }
   | slot_assign_list slot_assign
      { $1->append($2); $$ = $1; }
   ;

slot_assign
   : IDENT '=' expr ';'
      { $$ = SlotAssignNode::alloc(NULL, NULL, $1, $3); }
   | rwDATABLOCK '=' expr ';'
      { $$ = SlotAssignNode::alloc(NULL, NULL, StringTable->insert("datablock"), $3); }
   | IDENT '[' aidx_expr ']' '=' expr ';'
      { $$ = SlotAssignNode::alloc(NULL, $3, $1, $6); }
   ;

aidx_expr
   : expr
      { $$ = $1; }
   | aidx_expr ',' expr
      { $$ = CommaCatExprNode::alloc($1, $3); }
   ;
%%

