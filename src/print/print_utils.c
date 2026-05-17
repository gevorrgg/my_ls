//
// Created by user on 5/12/2026.
//
#include <ls/print/print_catalog.h>
#include <file_data.h>
#include <file_vector.h>
#include <stdio.h>
#include <string.h>
#include <ls/flags/flags.h>
#include <ls/print/format_info.h>
#include <ls/print/print_utils.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>

#define LS_IS_TERMINAL() isatty(STDOUT_FILENO)
#define LS_DEFAULT_CONSOLE_WIDTH 80

const struct stat *get_stat(const struct file_data *f, const file_data_mode mode)
{
    return mode == FILE_DATA_LINK ? &(f->lst) : &(f->st);
}

size_t len(size_t num)
{
    if (num == 0)
        return 1;

    size_t size = 0;

    while (num)
    {
        size++;

        num /= 10;
    }

    return size;
}

void print_chars(const char c, const size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        printf("%c", c);
    }
}

size_t ls_human_readable_len(size_t size)
{
    bool has_unit = false;
    size_t remainder = 0;

    while (size >= 1024)
    {
        remainder = size % 1024;
        size /= 1024;
        has_unit = true;
    }

    if (!has_unit)
    {
        return len(size);
    }

    const size_t unit_len      = 1;

    if (size < 10)
    {
        remainder = (remainder * 10) / 1024;

        const size_t remainder_len = 1;
        const size_t dot_len       = 1;

        return len(size) + remainder_len + dot_len + unit_len;
    }

    return len(size) + unit_len;
}

void print_blocks(const struct file_data *file,
                  const struct ls_format_info *format_info,
                  const struct ls_flags *flags)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && flags->follow_symlinks)
	{
		print_chars(' ', format_info->max_blocks_width - 1);
		printf("? ");
		return;
	}

    const file_data_mode mode = (flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK);
    const struct stat *st = get_stat(file, mode);

    const size_t size = st->st_blocks / 2;

    if (flags->is_human_readable)
    {
		size_t space_count = format_info->max_blocks_width - ls_human_readable_len(size * 1024);
		print_chars(' ', space_count);
        print_human_readable(size * 1024);
    }
    else
    {
        size_t spaces_count = format_info->max_blocks_width - len(size);
        print_chars(' ', spaces_count);
        printf("%lu ", size);
    }
}

size_t calculate_total_block_size(const struct fdata_ptr_vector *files, file_data_mode mode)
{
    size_t total_block_size = 0;

    for (size_t i = 0; i < files->size; i++)
    {
        const struct file_data *file = files->data[i];
        const struct stat *st = get_stat(file, mode);

        total_block_size += st->st_blocks;
    }

    return total_block_size;
}

void print_total(const struct fdata_ptr_vector *files,
                 const struct ls_flags *flags)
{
    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;
    const size_t total_blocksize = calculate_total_block_size(files, mode);

    printf("total ");

    if (flags->is_human_readable)
    {
        print_human_readable(total_blocksize * 512);
    }
    else
    {
        printf("%lu", total_blocksize / 2);
    }

    printf("\n");
}

void print_human_readable(size_t size)
{
    const char *units = "KMGTP";
    int unit_index = -1;
    double value = (double)size;

    double blocks_size = 1024.0;

    while (value >= 1024.0 && unit_index < 4)
    {
        value /= blocks_size;
        unit_index++;
    }

	value = round(value * 10.0) / 10.0;

    if (value >= 10.0 || unit_index == -1)
        printf("%.0f", value);
    else
        printf("%.1f", value);

    if (unit_index != -1)
        printf("%c", units[unit_index]);

    printf(" ");
}
size_t print_suffix(const struct file_data *file,
                    const struct ls_flags *flags)
{
    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;
    size_t printed_characters = 0;

    if (is_symlink(file, mode) &&
         flags->symbols_mode == SYMBOLS_ALL &&
         flags->format != FORMAT_LONG)
    {
        printf("@");

        printed_characters = 1;
    }
    else if (is_directory(file, mode))
    {
        printf("/");

        printed_characters = 1;
    }
    else if (is_executable(file, mode) && flags->symbols_mode == SYMBOLS_ALL)
    {
        printf("*");

        printed_characters = 1;
    }


    return printed_characters;
}

bool should_print_suffix(const struct ls_flags *flags)
{
    return flags->symbols_mode != SYMBOLS_NONE;
}

size_t print_file_name(const struct file_data *file,
                       const struct ls_flags *flags)
{
    //const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;
    size_t printed_symbols = printf("%s", file->name);

    if (should_print_suffix(flags))
    {
        const size_t suffix_len = print_suffix(file, flags);

        printed_symbols += suffix_len;
    }

    return printed_symbols;
}

size_t get_console_width()
{
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0)
        return LS_DEFAULT_CONSOLE_WIDTH;

    return w.ws_col;
}

size_t get_fname_width(const struct ls_flags *flags, size_t fname_len)
{
    size_t fname_width = fname_len;

    if (flags->symbols_mode != SYMBOLS_NONE)
        fname_width++; // symbol
   

    return fname_width;
}

void print_inode(const struct file_data *file,
                 const struct ls_format_info *format_info,
                 const file_data_mode mode)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && mode == FILE_DATA_TARGET)
	{
		if (format_info) print_chars(' ', format_info->max_inode_width - 1);
		printf("? ");
		return;
	}

    const struct stat *st = get_stat(file, mode);
    const size_t inode_len = len(st->st_ino);

    if (format_info) print_chars(' ', format_info->max_inode_width - inode_len);
    printf("%ju",  (uintmax_t) st->st_ino);
    printf(" ");
}

struct ls_format_info * get_each_column_format_info(const struct fdata_ptr_vector *files,
                                                    const struct ls_flags *flags,
                                                    const size_t max_rows,
                                                    const size_t max_columns)
{
    struct ls_format_info *each_column_format = calloc(max_columns, sizeof(struct ls_format_info));

    if (each_column_format == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < max_columns; i++)
    {
        ls_format_info_init(each_column_format + i);
    };

	struct ls_format_info format;
	ls_format_info_init(&format);

	ls_get_format_info(&format, files, flags);

    for (size_t i = 0; i < max_rows; i++)
    {
        for (size_t j = 0; j < max_columns; j++)
        {
            size_t index = (flags->format == FORMAT_HORIZONTAL) ? i * max_columns + j : j * max_rows + i;

            if (index >= files->size) continue;

            ls_format_info_update(each_column_format + j, (files->data)[index], flags);
		}
    }

	for (size_t j = 0; j < max_columns; j++)
	{
		each_column_format[j].max_inode_width = format.max_inode_width;
		each_column_format[j].max_blocks_width = format.max_blocks_width;
	}

    return each_column_format;
}

static size_t get_column_width(const struct ls_format_info *format, const struct ls_flags *flags)
{
    size_t column_width = 0;

    if (flags->show_inode)
    {
        const size_t space_width  = 1;
        column_width += format->max_inode_width + space_width;
    }

	if (flags->show_blocks)
	{
		const size_t space_width  = 1;
		column_width += format->max_blocks_width + space_width;
	}

    column_width += get_fname_width(flags, format->max_name_width);

    return column_width;
}

static size_t * get_columns_width(const struct ls_format_info *each_column_format_info, const struct ls_flags *flags, const size_t columns)
{
    size_t *each_column_width = malloc(sizeof(size_t) * columns);

    if (each_column_width == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < columns; i++)
    {
        each_column_width[i] = get_column_width(each_column_format_info + i, flags);
    }

    return each_column_width;
}


void get_grid_size(const struct fdata_ptr_vector *files,
                   const struct ls_flags *flags,
                   size_t *rows,
                   size_t *columns,
                   const size_t console_width)
{

    for (size_t cols = files->size; cols > 0; cols--)
    {
        size_t max_rows = (files->size + cols - 1) / cols;

        struct ls_format_info *each_column_format_info = get_each_column_format_info(files, flags, max_rows, cols);

        if (each_column_format_info == NULL)
        {
            if (rows)    *rows    = files->size;
            if (columns) *columns = 1;

            return;
        }

        size_t *each_col_width = get_columns_width(each_column_format_info, flags, cols);

        free(each_column_format_info);

        if (each_col_width == NULL)
        {
            if (rows)    *rows    = files->size;
            if (columns) *columns = 1;

            return;
        }

        size_t total_width = 0;

        for (size_t col = 0; col < cols; col++)
        {
            total_width += each_col_width[col];
        }

        free(each_col_width);

        const size_t spaces_width = 2;

        total_width += spaces_width * (cols - 1); // last column does not require alignment

        if (total_width < console_width)
        {
            if (columns) *columns = cols;
            if (rows)    *rows    = max_rows;

            return;
        }
    }

    if (columns) *columns = 1;
    if (rows)    *rows    = files->size;
}


