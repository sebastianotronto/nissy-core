#ifndef LOGGING_H
#define LOGGING_H

#include "../cpp/nissy.h"
#include <emscripten/bind.h>
#include <string>

#define LOGGING_EMBIND \
    emscripten::function("addCallbackFunction", &addCallbackFunction);

extern "C" {
	extern int addCallbackFunction();
	extern void callFunction(int, const char *);
}

static int logger_id = -1;

void log(std::string s)
{
	if (logger_id == -1)
		return;

	callFunction(logger_id, s.c_str());
}

void log_wrapper(const char *cstr, void *data)
{
	log(cstr);
}

void set_logger(int id)
{
	logger_id = id;
	nissy::set_logger(log_wrapper, NULL);
}

#endif
