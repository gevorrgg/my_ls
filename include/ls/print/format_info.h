#ifndef LS_FORMAT_INFO_H
#define LS_FORMAT_INFO_H

#include <stddef.h>
#include <stdbool.h>

struct ls_format_info
{
    size_t max_links_width;
    size_t max_owner_width;
    size_t max_group_width;
    size_t max_size_width;
    size_t max_name_width;
    size_t max_blocks_width;
    size_t max_inode_width;
    bool is_human_readable;
};

struct file_data;
struct ls_flags;
struct fdata_ptr_vector;

void ls_format_info_init(struct ls_format_info *self);
void ls_format_info_update(struct ls_format_info *info,
                           const struct file_data *file,
						   const struct ls_flags *flags);
void ls_get_format_info(struct ls_format_info *info,
                        const struct fdata_ptr_vector *files,
                        const struct ls_flags *flags);

#endif
