//
// Created by user on 5/12/2026.
//
#include <file_vector.h>
#include <file_data.h>
#include <ls/flags/flags.h>
#include <ls/print/print_utils.h>
#include <ls/print/format_info.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define LS_IS_TERMINAL() isatty(STDOUT_FILENO)
#define LS_DEFAULT_CONSOLE_WIDTH 80

void   ls_print_columns              (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags)
{
    size_t rows;
    size_t columns;

    get_grid_size(files, flags, &rows, &columns, LS_IS_TERMINAL() ? get_console_width() : LS_DEFAULT_CONSOLE_WIDTH);

    struct ls_format_info *each_column_format = get_each_column_format_info(files, flags, rows, columns);

    if (each_column_format == NULL)
    {
        fprintf(stderr, "malloc: %s\n", strerror(errno));

        return;
    }

    if (flags->show_blocks)
    {
        print_total(files, flags);
    }


    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;

    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < columns; j++)
        {
            const size_t index = j * rows + i;

            if (index >= files->size) continue;

            const struct file_data *file_info = files->data[index];

            if (flags->show_inode)
            {
                print_inode(file_info, each_column_format + j, mode);
            }
           
		    if (flags->show_blocks)
            {
                print_blocks(file_info, each_column_format + j, flags);
            }


            print_file_name(file_info, flags);

            if (j == columns - 1)
                continue;

            const size_t next_index = (j + 1) * rows + i;

            if (next_index >= files->size) continue;

            const size_t spaces_width = 2;
            const size_t name_width   = get_fname_width(flags, strlen(file_info->name));

            print_chars(' ', each_column_format[j].max_name_width - name_width + spaces_width); // guaranteed that max_size_width >= every possible name_width
        }

        printf("\n");
    }


    free(each_column_format);
}
