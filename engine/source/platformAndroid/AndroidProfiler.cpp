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

//
//  AndroidMiniProfiler.mm
//  Torque2D
//
//  Created by puap on 9/26/08.
//  Copyright 2008  PUAP. All rights reserved.
//

#include "AndroidProfiler.h"

//PUAP
//--------------------------------------------------------------------------------------------------------------------------------------------
AndroidProfilerData g_AndroidProfilerData[ANDROID_PROFILER_MAX_CALLS];
int	g_AndroidProfilerCount = 0;
bool g_AndroidProfilerReady = false;

void AndroidProfilerResetAll() {
	for(int i=0; i<ANDROID_PROFILER_MAX_CALLS; i++){
		g_AndroidProfilerData[i].name = NULL;
		g_AndroidProfilerData[i].timeMS = 0;
		g_AndroidProfilerData[i].type = 0;
		g_AndroidProfilerData[i].invokes = 0;
	}
	g_AndroidProfilerCount = 0;
}


void AndroidProfilerProfilerInit() {
	g_AndroidProfilerReady = true;
	
	AndroidProfilerResetAll();
	printf( "\n\nIniting PUAP Profiler\n");
}


void AndroidProfilerStart( const char *name ) {
	if( g_AndroidProfilerReady ) {

		int i = g_AndroidProfilerCount++;
		g_AndroidProfilerData[i].name = name;
		g_AndroidProfilerData[i].timeMS = Platform::getRealMilliseconds();
		g_AndroidProfilerData[i].type = 0;
		g_AndroidProfilerData[i].invokes = 0;
	
		if(g_AndroidProfilerCount >= ANDROID_PROFILER_MAX_CALLS){
			g_AndroidProfilerReady = false;
		}
	}
}	

void AndroidProfilerEnd( const char *name ) {
	if( g_AndroidProfilerReady ) {

		int i = g_AndroidProfilerCount++;
		g_AndroidProfilerData[i].name = name;
		g_AndroidProfilerData[i].timeMS = Platform::getRealMilliseconds();
		g_AndroidProfilerData[i].type = 1;
		g_AndroidProfilerData[i].invokes = 0;

		if(g_AndroidProfilerCount >= ANDROID_PROFILER_MAX_CALLS){
			g_AndroidProfilerReady = false;
	}	
	}
}

int AndroidProfilerFindProfileEnd( const char *name, int startCount){
	int invokes = 0;
	for( int i = (startCount+1); i < g_AndroidProfilerCount; i++ ) {
		if(g_AndroidProfilerData[i].name == name || dStrcmp(g_AndroidProfilerData[i].name, name)==0){
			if(g_AndroidProfilerData[i].type == 1){
				if(invokes == 0){
					return i;
	}
				else{
					g_AndroidProfilerData[i].invokes = invokes;
					invokes--;
	}
}
			else{
				invokes++;
				g_AndroidProfilerData[i].invokes = invokes;
		}
	}
	}
	return -1;
}

int AndroidProfilerGetCount(){
	return g_AndroidProfilerCount;
}

char pfbuffer[1024];
void AndroidProfilerPrintResult( int item ) {

	if(g_AndroidProfilerData[item].type == 0){
		int endItem = AndroidProfilerFindProfileEnd(g_AndroidProfilerData[item].name, item);
		if(endItem < 0){
			return; //didn't find the end of this one
}
		U32 startTimeMS = g_AndroidProfilerData[item].timeMS;
		U32 endTimeMS = g_AndroidProfilerData[endItem].timeMS;

		dSprintf(pfbuffer, 1024, "%s, %d, %d, %d\n",
				 g_AndroidProfilerData[item].name, startTimeMS, endTimeMS, endTimeMS - startTimeMS);
		printf( pfbuffer, "%s" );

	}
}

void AndroidProfilerPrintAllResults() {
	
	dSprintf(pfbuffer, 1024, "\n\n \t Time Per Frame \t Average Time \t Calls Per Frame (avg) \t Time Per Call \t Name \n" );
	printf("%s", pfbuffer);
	
	for( int i = 0; i < g_AndroidProfilerCount; i++ ) {
		AndroidProfilerPrintResult( i );
	}
	
	dSprintf(pfbuffer, 1024, "\n 	<----------------------------------------------------------------------------------->\n " );
	printf("%s", pfbuffer);
}


//-Mat get instruction names

/// The opcodes for the TorqueScript VM.
const char *InstructionName[] =  {
"OP_FUNC_DECL",
"OP_CREATE_OBJECT",
"OP_ADD_OBJECT",
"OP_END_OBJECT",
"OP_JMPIFFNOT",
"OP_JMPIFNOT",
"OP_JMPIFF",
"OP_JMPIF",
"OP_JMPIFNOT_NP",
"OP_JMPIF_NP",
"OP_JMP",
"OP_RETURN",
"OP_CMPEQ",
"OP_CMPGR",
"OP_CMPGE",
"OP_CMPLT",
"OP_CMPLE",
"OP_CMPNE",
"OP_XOR",
"OP_MOD",
"OP_BITAND",
"OP_BITOR",
"OP_NOT",
"OP_NOTF",
"OP_ONESCOMPLEMENT",

"OP_SHR",
"OP_SHL",
"OP_AND",
"OP_OR",

"OP_ADD",
"OP_SUB",
"OP_MUL",
"OP_DIV",
"OP_NEG",

"OP_SETCURVAR",
"OP_SETCURVAR_CREATE",
"OP_SETCURVAR_ARRAY",
"OP_SETCURVAR_ARRAY_CREATE",

"OP_LOADVAR_UINT",
"OP_LOADVAR_FLT",
"OP_LOADVAR_STR",

"OP_SAVEVAR_UINT",
"OP_SAVEVAR_FLT",
"OP_SAVEVAR_STR",

"OP_SETCUROBJECT",
"OP_SETCUROBJECT_NEW",
"OP_SETCUROBJECT_INTERNAL",

"OP_SETCURFIELD",
"OP_SETCURFIELD_ARRAY",

"OP_LOADFIELD_UINT",
"OP_LOADFIELD_FLT",
"OP_LOADFIELD_STR",

"OP_SAVEFIELD_UINT",
"OP_SAVEFIELD_FLT",
"OP_SAVEFIELD_STR",

"OP_STR_TO_UINT",
"OP_STR_TO_FLT",
"OP_STR_TO_NONE",
"OP_FLT_TO_UINT",
"OP_FLT_TO_STR",
"OP_FLT_TO_NONE",
"OP_UINT_TO_FLT",
"OP_UINT_TO_STR",
"OP_UINT_TO_NONE",

"OP_LOADIMMED_UINT",
"OP_LOADIMMED_FLT",
"OP_TAG_TO_STR",
"OP_LOADIMMED_STR",
"OP_DOCBLOCK_STR",
"OP_LOADIMMED_IDENT",

"OP_CALLFUNC_RESOLVE",
"OP_CALLFUNC",

"OP_ADVANCE_STR",
"OP_ADVANCE_STR_APPENDCHAR",
"OP_ADVANCE_STR_COMMA",
"OP_ADVANCE_STR_NUL",
"OP_REWIND_STR",
"OP_TERMINATE_REWIND_STR",
"OP_COMPARE_STR",

"OP_PUSH",
"OP_PUSH_FRAME",

"OP_BREAK",

"OP_INVALID"
};


const char *getInstructionName( int index ) {
	return InstructionName[index];
}
