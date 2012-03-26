#include "log.h"
#ifdef LOG_ENABLE
#include <stdlib.h>
#include <stdarg.h>

static char _buff[512];
static char _filename[256];
static char _ext[5];

void log_fprint(FILE* file, const char* func, const char* filename, int line, const char* format, ...){
	_splitpath_s(filename, NULL, 0, NULL, 0, _filename, sizeof(_filename), _ext, sizeof(_ext));
	va_list args;
    va_start(args, format);
	sprintf_s(_buff, sizeof(_buff), "%s %s%s(%6d): %s", func, _filename, _ext, line, format);
	vfprintf_s(file, _buff, args);
	va_end(args);
}
#endif // LOG_ENABLE