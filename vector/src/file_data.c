#include "../include/file_data.h"

#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <assert.h>

file_data_error file_data_init(struct file_data *self, const struct stat *st, const struct stat *lst, const char *name, const char *path, int stat_status, bool follow_symlinks)
{
	if (self == NULL || st == NULL || name == NULL)
		return FILE_DATA_NULL_ERR;

	char* self_name = strdup(name);

	if (self_name == NULL)
		return FILE_DATA_ALLOC_ERR;	// couldn't allocate

    char *self_path = strdup(path);

	if (self_path == NULL)
	{
		free(self_name);

		return FILE_DATA_ALLOC_ERR;
	}
	
	const struct stat *stat = follow_symlinks ? (stat_status ? st : lst) : lst;

	struct passwd *pw = getpwuid(stat->st_uid);
	struct group  *gr = getgrgid(stat->st_gid);

	char *self_uname;

	if (pw && pw->pw_name)
	{
		self_uname = strdup(pw->pw_name);

		if (self_uname == NULL)
		{
			free(self_name);
			free(self_path);

			return FILE_DATA_ALLOC_ERR;
		}
	}
	else
	{
		self_uname = NULL;
	}

	char *self_gname;

	if (gr && gr->gr_name)
	{
		self_gname = strdup(gr->gr_name);

		if (self_gname == NULL)
		{
			free(self_name);
			free(self_uname);
			free(self_path);

			return FILE_DATA_ALLOC_ERR;
		}
	}
	else
	{
		self_gname = NULL;
	}
	
	

	self->st          = *st;
    self->lst         = *lst;
	self->name        = self_name;
	self->uname       = self_uname;
	self->gname       = self_gname;
	self->path        = self_path;
	self->stat_status = stat_status;

	return FILE_DATA_SUCCESS;
}

file_data_error file_data_construct(struct file_data* self, const struct file_data* other)
{	
	if (self == NULL || other == NULL)
		return FILE_DATA_NULL_ERR;

	char *new_name = strdup(other->name);

	if (new_name == NULL)
		return FILE_DATA_ALLOC_ERR;

	char *gname = other->gname ? strdup(other->gname) : NULL;

	if (other->gname && gname == NULL)
	{
		free(new_name);

		return FILE_DATA_ALLOC_ERR;
	}

	char *uname = other->gname ? strdup(other->uname) : NULL;

	if (other->uname && uname == NULL)
	{
		free(new_name);
		free(gname);
		
		return FILE_DATA_ALLOC_ERR;
	}

	char *path = strdup(other->path);

	if (path == NULL)
	{
		free(new_name);
		free(uname);
		free(gname);

		return FILE_DATA_ALLOC_ERR;
	}

	self->name        = new_name;
	self->st          = other->st;
    self->lst         = other->lst;
	self->uname       = uname;
	self->gname       = gname;
	self->path        = path;
	self->stat_status = other->stat_status;

	return FILE_DATA_SUCCESS;
}

file_data_error file_data_set_name(struct file_data* self, const char* name)
{
	if (self == NULL || name == NULL)
		return FILE_DATA_NULL_ERR;

	char *new_name = strdup(name);

	if (new_name == NULL)
		return FILE_DATA_ALLOC_ERR;		// couldn't allocate

	free(self->name);
	self->name = new_name;

	return FILE_DATA_SUCCESS;
}

static bool stat_equals(const struct stat *lhs, const struct stat *rhs)
{
    return lhs->st_dev == rhs->st_dev && lhs->st_ino == rhs->st_ino;
}

bool fdata_equals(const struct file_data* lhs, const struct file_data* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return false;

    if (!stat_equals(&lhs->lst, &rhs->lst))
        return false;

    return true;
}

void file_data_free(struct file_data* self)
{
	free(self->name);
    free(self->uname);
    free(self->gname);
	free(self->path);
}

static const struct stat *get_stat(const struct file_data *f,
                            file_data_mode mode)
{
    return mode == FILE_DATA_LINK ? &(f->lst) : &(f->st);
}

bool is_directory(const struct file_data *file_info, file_data_mode mode)
{
    const struct stat *st = get_stat(file_info, mode);
	
    return S_ISDIR(st->st_mode) && file_info->stat_status;
}

bool is_symlink(const struct file_data *file_info, file_data_mode mode)
{
    const struct stat *st = get_stat(file_info, mode);

    return S_ISLNK(st->st_mode);
}

bool is_executable(const struct file_data *file_info, file_data_mode mode)
{
    const struct stat *st = get_stat(file_info, mode);

    return st->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH) && file_info->stat_status;
}

bool is_block_device(const struct file_data *file_info, file_data_mode mode)
{
    const struct stat *st = get_stat(file_info, mode);

    return S_ISBLK(st->st_mode) && file_info->stat_status;
}
bool is_character_device(const struct file_data *file_info, file_data_mode mode)
{
    const struct stat *st = get_stat(file_info, mode);

    return S_ISCHR(st->st_mode) && file_info->stat_status;
}
bool is_pipe(const struct file_data *file_info, file_data_mode mode)
{
    const struct stat *st = get_stat(file_info, mode);

    return S_ISFIFO(st->st_mode) && file_info->stat_status;
}

bool is_socket(const struct file_data *file_info, file_data_mode mode)
{
    const struct stat *st = get_stat(file_info, mode);

    return S_ISSOCK(st->st_mode) && file_info->stat_status;
}

