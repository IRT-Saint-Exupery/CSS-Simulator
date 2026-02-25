
#include <cstdarg>
#include "FE_error.h"

feErrorException_t feErrorException;

void feErrorException_t::setMsg (const char *fmt,...)
{
    va_list args;
    va_start(args, fmt);

    vsnprintf(serror, sizeof(serror), fmt, args);

    va_end(args);
}

void feError (const char *fmt,...)
{
	char fmt1 [512];
	snprintf (fmt1, sizeof(fmt1), "FE ERROR : %s", fmt);
	va_list args;
	va_start (args,fmt);
	feErrorException.setMsg(fmt1,args);
	va_end (args);
	throw feErrorException;
}

void feWarning (const char *fmt,...)
{
	va_list args;
	fprintf (stdout, "FE WARNING : ");
	va_start (args,fmt);
	vfprintf(stdout, fmt,args);
	va_end (args);
	fprintf (stdout, "\n");
}

void feInfo (const char *fmt,...)
{
	va_list args;
	va_start (args,fmt);
	vfprintf(stdout, fmt,args);
	va_end (args);
	fprintf (stdout, "\n");
}


