typedef union {
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
} YYSTYPE;
#define	rwDEFINE	258
#define	rwENDDEF	259
#define	rwDECLARE	260
#define	rwBREAK	261
#define	rwELSE	262
#define	rwCONTINUE	263
#define	rwGLOBAL	264
#define	rwIF	265
#define	rwNIL	266
#define	rwRETURN	267
#define	rwWHILE	268
#define	rwDO	269
#define	rwENDIF	270
#define	rwENDWHILE	271
#define	rwENDFOR	272
#define	rwDEFAULT	273
#define	rwFOR	274
#define	rwDATABLOCK	275
#define	rwSWITCH	276
#define	rwCASE	277
#define	rwSWITCHSTR	278
#define	rwCASEOR	279
#define	rwPACKAGE	280
#define	rwNAMESPACE	281
#define	rwCLASS	282
#define	rwMESSAGE	283
#define	ILLEGAL_TOKEN	284
#define	CHRCONST	285
#define	INTCONST	286
#define	TTAG	287
#define	VAR	288
#define	IDENT	289
#define	DOCBLOCK	290
#define	STRATOM	291
#define	TAGATOM	292
#define	FLTCONST	293
#define	opINTNAME	294
#define	opINTNAMER	295
#define	opMINUSMINUS	296
#define	opPLUSPLUS	297
#define	STMT_SEP	298
#define	opSHL	299
#define	opSHR	300
#define	opPLASN	301
#define	opMIASN	302
#define	opMLASN	303
#define	opDVASN	304
#define	opMODASN	305
#define	opANDASN	306
#define	opXORASN	307
#define	opORASN	308
#define	opSLASN	309
#define	opSRASN	310
#define	opCAT	311
#define	opEQ	312
#define	opNE	313
#define	opGE	314
#define	opLE	315
#define	opAND	316
#define	opOR	317
#define	opSTREQ	318
#define	opCOLONCOLON	319
#define	opMDASN	320
#define	opNDASN	321
#define	opNTASN	322
#define	opSTRNE	323
#define	UNARY	324


extern YYSTYPE CMDlval;
