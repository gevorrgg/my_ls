//
// Created by user on 5/12/2026.
//
#include <file_vector.h>
#include <ls/flags/flags.h>
#include <ls/print/print_utils.h>
#include <ls/print/format_info.h>
#include <stdio.h>

void   ls_print_one_per_line         (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags)
{
    struct ls_format_info format;
    ls_format_info_init(&format);

    ls_get_format_info(&format, files, flags);

    for (size_t i = 0; i < files->size; i++)
    {
        const struct file_data *file = (files->data)[i];
        const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_TARGET;

        if (flags->show_inode)
        {
            print_inode(file, &format, mode);
        }

		if (flags->show_blocks)
		{
			print_blocks(file, &format, flags);
		}

        print_file_name(files->data[i], flags);

        printf("\n");
    }
}
