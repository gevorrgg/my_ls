#ifndef LS_PRINT_H
#define LS_PRINT_H

struct fdata_ptr_vector;
struct ls_flags;
struct ls_format_info;

void ls_print_catalog(const struct fdata_ptr_vector *file_ptrs,
                      const struct ls_flags *flags);

#endif
