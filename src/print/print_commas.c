#include <ls/print/print_utils.h>
#include <stdio.h>
#include <file_vector.h>
#include <ls/print/format_info.h>
#include <ls/flags/flags.h>
#include <string.h>
#include <unistd.h>

#define LS_IS_TERMINAL() isatty(STDOUT_FILENO)
#define LS_DEFAULT_CONSOLE_WIDTH 80

void ls_print_commas(const struct fdata_ptr_vector *files,
                     const struct ls_flags *flags)
{
    size_t printed_characters = 0;

    size_t console_width = LS_IS_TERMINAL()
        ? get_console_width()
        : LS_DEFAULT_CONSOLE_WIDTH;

    for (size_t i = 0; i < files->size; i++)
    {
        const file_data_mode mode =
            flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;

        const struct file_data *file = files->data[i];

        const struct stat *st = get_stat(file, mode);

        size_t fname_width = get_fname_width(flags, strlen(file->name));

        size_t comma_width = 1;
        size_t space_width = 1;

        size_t inode_width = 0;
        size_t blocks_width = 0;

        bool should_print_comma = (i != files->size - 1);

        if (flags->show_inode)
            inode_width = len(st->st_ino);

        if (flags->show_blocks)
            blocks_width = len(st->st_blocks);

        size_t total_len = printed_characters + fname_width;

        if (flags->show_inode)
            total_len += inode_width + 2 * space_width;

        if (flags->show_blocks)
            total_len += blocks_width + space_width;

        if (should_print_comma)
            total_len += comma_width;

        bool should_print_space = should_print_comma &&
                                   total_len < console_width;

        if (total_len > console_width)
        {
            printf("\n");
            printed_characters = 0;
        }

        /* inode */
        if (flags->show_inode)
        {
            printf("%zu ", (size_t)st->st_ino);
            printed_characters += inode_width + space_width;
        }

        /* blocks (-s) */
        if (flags->show_blocks)
        {
            printf("%zu ", (size_t)st->st_blocks);
            printed_characters += blocks_width + space_width;
        }

        print_file_name(file, flags);
        printed_characters += fname_width;

        if (should_print_comma)
        {
            printf(",");
            printed_characters += comma_width;

            if (should_print_space)
            {
                printf(" ");
                printed_characters += space_width;
            }
        }
    }

    if (files->size == 0 || printed_characters != 0)
        printf("\n");
}
