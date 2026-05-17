#include <ls/file_scan.h>
#include <file_data.h>
#include <ls/print/format_info.h>
#include <ls/error.h>
#include <ls/flags/flags.h>
#include <dirent.h>
#include <file_vector.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static bool should_skip(const char *file_name, const struct ls_flags* flags)
{
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
    {
        return flags->hidden != SHOW_ALL;
    }

    if (file_name[0] == '.')
        return flags->hidden == HIDE_DEFAULT;

    return false;
}

ls_error ls_get_file_info(struct file_data *fdata, const char *path, const char *name, bool follow_symlinks)
{
    struct stat st;
    struct stat lst;
	int stat_succeed = 1;

	if (lstat(path, &lst) != 0)
	{
		return LS_STAT_ERR;
	}

    if (stat(path, &st) != 0)
	{
		stat_succeed = 0;
		memset(&st, 0, sizeof(st));
	}

    if (file_data_init(fdata, &st, &lst, name, path, stat_succeed, follow_symlinks) != FILE_DATA_SUCCESS)
    {    
		return LS_ERR;
	}

    return follow_symlinks ? (stat_succeed ? LS_SUCCESS : LS_STAT_ERR) :  LS_SUCCESS;
}

static bool is_flag(const char *arg)
{
    if (strcmp(arg, "--") == 0)
        return false;

    if (strcmp(arg, "-") == 0)

        return false;

    return arg[0] == '-';
}

ls_error ls_retrieve_files(struct fdata_vector *vec,
                           int argc,
                           const char *argv[],
						   bool follow_symlinks,
						   int *file_args)
{
    bool read_files = false;
    bool seen_args  = false;

	int args_count = 0;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--") == 0)
        {
            read_files = true;
            continue;
        }

        if (is_flag(argv[i]) && !read_files)
            continue;  // skip flags

		args_count += 1;

        struct file_data fdata;

        seen_args = true;

        if (ls_get_file_info(&fdata, argv[i], argv[i], follow_symlinks) != LS_SUCCESS)
        {
            fprintf(stderr, "ls: cannot access '%s': %s\n", argv[i], strerror(errno));
            continue;
        }

        const fdata_err append_status = fdata_vector_append(vec, &fdata);

        if (append_status != FDATA_SUCCESS)
        {
			if (file_args)
			{
				*file_args = args_count;
			}

            file_data_free(&fdata);

            return LS_BUFFER_ERR;
        }

        file_data_free(&fdata);
    }

    if (!seen_args) // no file arguments
    {
		args_count += 1;
		
		if (file_args)
		{
			*file_args = args_count;
		}

        struct file_data fdata;

        ls_error finfo_status = ls_get_file_info(&fdata, ".", ".", follow_symlinks);

        if (finfo_status != LS_SUCCESS)
        {
            return finfo_status;
        }

        if (fdata_vector_append(vec, &fdata) != FDATA_SUCCESS)
        {
            file_data_free(&fdata);

            return LS_BUFFER_ERR;
        }
        
        file_data_free(&fdata);
    }

	if (file_args)
	{
		*file_args = args_count;
	}

    return LS_SUCCESS;
}

char *ls_make_path(const char *dir, const char *name)
{
    if (dir == NULL || name == NULL)
        return NULL;

    size_t dir_len = strlen(dir);

    if (dir_len == 0)
    {
        dir = ".";
        dir_len = 1;
    }

    size_t buf_len = dir_len + strlen(name) + 1;
    const char last_char = dir[dir_len - 1];
    const bool need_slash = last_char != '/';

    if (need_slash)
    {
        buf_len += 1;
    }

    char *buf = malloc(sizeof(char) * buf_len);

    if (buf == NULL)
        return NULL;

    snprintf(buf, buf_len, "%s%s%s", dir, need_slash ? "/" : "", name);

    return buf;
}

ls_error ls_get_fdata_vector(struct fdata_vector *vec, 
							 const char *directory,
							 const struct ls_flags *flags)
{
        DIR *dir = opendir(directory);

        if (dir == NULL)
        {
			    fprintf(stderr, "ls: cannot open directory '%s': %s\n", directory, strerror(errno));

                return LS_FILE_NOT_FOUND;
        }

        struct dirent *dent;

        while ((dent = readdir(dir)) != NULL)
        {
                const char *name = dent->d_name;

                if (should_skip(name, flags))
                    continue;  // skip unnecessary files
	
				char *path = ls_make_path(directory, name);

				if (path == NULL)
				{
					closedir(dir);

					return LS_ALLOC_ERR;
				}
		
				struct file_data fdata;

                const ls_error stat_status = ls_get_file_info(&fdata, path, name, flags->follow_symlinks);

                if (stat_status != LS_SUCCESS)
                {
					fprintf(stderr, "ls: cannot access '%s': %s\n", name, strerror(errno));
                }

                const fdata_err append_status = fdata_vector_append(vec, &fdata);

                if (append_status != FDATA_SUCCESS)
				{	
						free(path);
						closedir(dir);
						file_data_free(&fdata);

                        return LS_BUFFER_ERR;
				}

				file_data_free(&fdata);
				free(path);
        }

		closedir(dir);

        return LS_SUCCESS;
}

ls_error ls_get_fdata_span(struct fdata_ptr_vector *span,
                           const struct fdata_vector *file_vec,
                           const struct ls_flags *flags,
                           bool (*filter)(const struct file_data *file, const struct ls_flags *flags))
{
    for (size_t i = 0; i < file_vec->size; i++)
    {
        if (!filter || filter(file_vec->data + i, flags))
        {
            const fdata_err append_status = fdata_ptr_vector_append(span, file_vec->data + i);

            if (append_status != FDATA_SUCCESS)
                return LS_BUFFER_ERR;
        }
    }

    return LS_SUCCESS;
}

