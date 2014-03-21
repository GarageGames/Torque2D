// Dummy file for testing out Emscripten build with plain GCC

typedef signed char        S8;      ///< Compiler independent Signed Char
typedef unsigned char      U8;      ///< Compiler independent Unsigned Char

typedef signed short       S16;     ///< Compiler independent Signed 16-bit short
typedef unsigned short     U16;     ///< Compiler independent Unsigned 16-bit short

typedef signed int         S32;     ///< Compiler independent Signed 32-bit integer
typedef unsigned int       U32;     ///< Compiler independent Unsigned 32-bit integer

typedef float              F32;     ///< Compiler independent 32-bit float
typedef double             F64;     ///< Compiler independent 64-bit float


//------------------------------------------------------------------------------
//------------------------------------- String Types

typedef char           UTF8;        ///< Compiler independent 8  bit Unicode encoded character
typedef unsigned short UTF16;       ///< Compiler independent 16 bit Unicode encoded character
typedef unsigned int   UTF32;       ///< Compiler independent 32 bit Unicode encoded character

typedef const char* StringTableEntry;

U32 _EmscriptenGetDesktopHeight()
{
	return 600;
}

U32 _EmscriptenGetDesktopWidth()
{
	return 800;
}

U32 _EmscriptenGetDesktopBpp()
{
	return 32;
}

int js_AlertOK(const char *title, const char *message)
{
	return 0;
}

int js_AlertOKCancel(const char *title, const char *message)
{
	return 0;
}

int js_AlertRetry(const char *title, const char *message)
{
	return 0;
}

int js_AlertYesNo(const char *title, const char *message)
{
	return 0;
}

void step_warn()
{
}

void js_ConsoleEnabled(const char *prompt)
{

}

void js_ConsoleDisabled()
{

}

void js_ConsoleLine(const char *message)
{

}
