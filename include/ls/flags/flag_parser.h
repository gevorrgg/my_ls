#ifndef FLAG_PARSER_H
#define FLAG_PARSER_H

#include <stdbool.h>

#include <ls/error.h>
#include <ls/flags/flags.h>

typedef void (*flag_setter)(struct ls_flags* f);

ls_error ls_parse_flags(struct ls_flags *flags, int argc, const char *args[], char *error_flag, const flag_setter flag_setters[]);

#endif
