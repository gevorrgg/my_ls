#include <ls/flags/flag_parser.h>
#include <stdbool.h>
#include <ls/flags/flags.h>
#include <ls/error.h>
#include <string.h>

static bool is_valid_flag(unsigned char flag, const flag_setter available_flags[])
{
        return available_flags[flag] != NULL;
}

ls_error ls_parse_flags(struct ls_flags *flags, int argc, const char *args[], char *error_flag, const flag_setter flag_setters[])
{
        for (int i = 0; i < argc; i++)
        {
                const char *name = args[i];

                if (name[0] != '-')
                        continue;

                if (strcmp(name, "--") == 0)
                        break;  // end of flags

                for (int j = 1; name[j]; j++)
                {
                        unsigned char flag = (unsigned char) name[j];

                        if (is_valid_flag(flag, flag_setters))
                        {
                                flag_setters[flag](flags);
                        }
                        else
                        {
                                if (error_flag)
                                        *error_flag = (char) flag;

                                return LS_UNKNOWN_FLAG_ERR;
                        }
                }
        }

        return LS_SUCCESS;
}
