//
// Created by user on 4/13/2026.
//

#ifndef LS_PRINT_CATALOG_H
#define LS_PRINT_CATALOG_H

struct fdata_ptr_vector;
struct ls_flags;
struct ls_format_info;

void   ls_print_long                 (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags);

void   ls_print_commas               (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags);

void   ls_print_one_per_line         (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags);

void   ls_print_horizontal           (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags);

void   ls_print_columns              (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags);

void   ls_print_standard             (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags);

#endif //LS_PRINT_CATALOG_H
