#ifndef SORTING_H
#define SORTING_H

struct fdata_ptr_vector;
struct ls_flags;

void ls_sort_file_vector(struct fdata_ptr_vector *file, const struct ls_flags *flags);

#endif
