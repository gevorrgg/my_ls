#ifndef LS_FLAGS_H
#define LS_FLAGS_H

#include <stdbool.h>

typedef enum {
        SORT_NAME,
        SORT_NONE,
        SORT_TIME,
        SORT_SIZE
} SortType;

typedef enum {
        FORMAT_STANDARD,
		FORMAT_LONG,
        FORMAT_COLUMNS,
        FORMAT_ONE_PER_LINE,
        FORMAT_COMMAS,
        FORMAT_HORIZONTAL,
		FORMAT_NONE
} OutputFormat;

typedef enum {
        TIME_MODIFICATION,
        TIME_ACCESS,
        TIME_CHANGE
} TimeType;

typedef enum {
        HIDE_DEFAULT,
        SHOW_ALL,
        SHOW_ALMOST
} HiddenMode;

typedef enum {
        SYMBOLS_NONE,
        SYMBOLS_DIRECTORIES,
        SYMBOLS_ALL
} AddSymbolsMode;

struct ls_flags
{
        bool is_recursive;
        bool is_human_readable;
        bool show_inode;
        bool sort_reversed;
        bool show_owner;
        bool show_group;
        bool follow_symlinks;
        bool show_blocks;
        bool enter_dir;
		bool print_owner_name;
		bool print_group_name;

        SortType sort;
        OutputFormat format;
        TimeType time_type;
        HiddenMode hidden;
        AddSymbolsMode symbols_mode;
};

void ls_flags_init(struct ls_flags *self);

#endif
