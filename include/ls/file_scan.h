#ifndef FILE_SCAN_H
#define FILE_SCAN_H

#include <ls/error.h>
#include <stdbool.h>

struct ls_format_info;
struct fdata_vector;
struct fdata_ptr_vector;
struct ls_flags;
struct stat;
struct file_data;


ls_error ls_get_file_info(struct file_data *fdata,
						  const char *path,
                          const char *name,
						  bool follow_symlinks);

ls_error ls_get_fdata_vector(struct fdata_vector *vec,
                             const char *directory,
                             const struct ls_flags *flags);

ls_error ls_retrieve_files(struct fdata_vector *vec,
                           int argc,
                           const char *argv[],
						   bool follow_symlinks,
						   int *file_args);


ls_error ls_get_fdata_span(struct fdata_ptr_vector *span,
                           const struct fdata_vector *file_vec,
                           const struct ls_flags *flags,
                           bool (*filter)(const struct file_data *file, const struct ls_flags *flags));

char * ls_make_path        (const char *directory, 
                            const char *name);

#endif
