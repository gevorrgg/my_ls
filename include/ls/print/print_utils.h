//
// Created by user on 5/12/2026.
//

#ifndef LS_PRINT_UTILS_H
#define LS_PRINT_UTILS_H

#include <file_data.h>
#include <stddef.h>

struct ls_flags;
struct ls_format_info;
struct fdata_ptr_vector;

const struct stat *get_stat                         (const struct file_data *f, 
                                                    file_data_mode mode);

size_t len                                          (size_t num);

void print_chars                                    (char c, 
                                                    size_t count);

size_t ls_human_readable_len                        (size_t size);

void print_human_readable                           (size_t size);

bool should_print_suffix                            (const struct ls_flags *flags);

size_t print_suffix                                 (const struct file_data *file,
                                                     const struct ls_flags *flags);

size_t print_file_name                              (const struct file_data *file,
                                                     const struct ls_flags *flags);
size_t get_console_width                            ();

size_t get_fname_width                              (const struct ls_flags *flags,
                                                     size_t fname_len);

void print_inode                                    (const struct file_data *file,
                                                     const struct ls_format_info *format_info, 
                                                     file_data_mode mode);


void print_blocks                                   (const struct file_data *file,
                                                     const struct ls_format_info *format_info,
                                                     const struct ls_flags *flags);

void print_total                                    (const struct fdata_ptr_vector *files,
                                                    const struct ls_flags *flags);


struct ls_format_info * get_each_column_format_info (const struct fdata_ptr_vector *files,
                                                     const struct ls_flags *flags,
                                                     size_t max_rows,
                                                     size_t max_columns);

void get_grid_size                                  (const struct fdata_ptr_vector *files,
                                                     const struct ls_flags *flags,
                                                     size_t *rows,
                                                     size_t *columns,
                                                     size_t console_width);

#endif //LS_PRINT_UTILS_H
