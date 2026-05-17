#ifndef FILE_DATA_H
#define FILE_DATA_H

#include <sys/stat.h>
#include <stdbool.h>

struct file_data
{
	struct stat st;
    struct stat lst;
	char *name;
	char *path;
	char *uname;
	char *gname;
	int stat_status;
};

typedef enum
{
	FILE_DATA_SUCCESS = 0,
	FILE_DATA_NULL_ERR,
	FILE_DATA_ALLOC_ERR,
    FDATA_FILE_NOT_FOUND_ERR,
} file_data_error;

typedef enum
{
    FILE_DATA_LINK,
    FILE_DATA_TARGET
} file_data_mode;


file_data_error file_data_init(struct file_data *self, const struct stat *st, const struct stat *lst, const char *name, const char *path, int stat_status, bool follow_symlinks);
file_data_error file_data_construct(struct file_data *self, const struct file_data *other);
file_data_error file_data_set_name(struct file_data *self, const char *name);
bool fdata_equals(const struct file_data *lhs, const struct file_data *rhs);
void file_data_free(struct file_data *self);
bool is_directory                    (const struct file_data *file_info, file_data_mode mode);
bool is_symlink                      (const struct file_data *file_info, file_data_mode mode);
bool is_executable                   (const struct file_data *file_info, file_data_mode mode);
bool is_block_device                 (const struct file_data *file_info, file_data_mode mode);
bool is_character_device             (const struct file_data *file_info, file_data_mode mode);
bool is_pipe                         (const struct file_data *file_info, file_data_mode mode);
bool is_socket                       (const struct file_data *file_info, file_data_mode mode);



#endif
