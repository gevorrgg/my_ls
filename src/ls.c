//
// Created by user on 4/12/2026.
//
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <ls.h>
#include <ls/file_scan.h>
#include <ls/print/format_info.h>
#include <ls/flags/flags.h>
#include <ls/sorting.h>
#include <ls/print/print.h>
#include <file_vector.h>
#include <ls/flags/flag_parser.h>
#include <ls/flags/flag_setters.h>


static bool should_sort(const struct ls_flags* flags)
{
    return flags->sort != SORT_NONE;
}

static ls_error visited_append(struct fdata_vector *visited, const char *directory)
{
    struct file_data file_info;

    const ls_error stat_status = ls_get_file_info(&file_info, directory, directory, true);

    if (stat_status != LS_SUCCESS)
    {
        return LS_STAT_ERR;
    }

    if (fdata_vector_append(visited, &file_info) != FDATA_SUCCESS)
  	{     
		return LS_ALLOC_ERR;
   	}
	
	file_data_free(&file_info);

    return LS_SUCCESS;
}

static bool should_recurse(const struct file_data *file_info,
                           const struct ls_flags *flags,
						   const struct fdata_vector *visited,
						   bool *is_visited)
{
	if (is_visited)
		*is_visited = false;

    if (!flags->is_recursive)
        return false;

    if (strcmp(file_info->name, ".") == 0 ||
        strcmp(file_info->name, "..") == 0)
        return false;

    const bool is_link = is_symlink(file_info, FILE_DATA_LINK);

    const file_data_mode mode = is_link ? FILE_DATA_TARGET : FILE_DATA_LINK;

    if (is_link && !flags->follow_symlinks)
    {   
		return false;
	}

    if (!is_directory(file_info, mode))
    {  
		return false;
	}

	if (vector_fdata_is_in(visited, file_info))
	{
		if (is_visited)
		{
			*is_visited = true;	
		}

		return false;
	}
 
   return true;
}

void clean(struct fdata_vector *files_vec, struct fdata_ptr_vector *file_ptrs)
{
    fdata_ptr_vector_free(file_ptrs);
    fdata_vector_free(files_vec);
}

static ls_error ls_recursive(const char *directory, const struct ls_flags *flags, struct fdata_vector *visited, bool is_first)
{
    struct fdata_vector files_vec;

    if (fdata_vector_init(&files_vec, 0) != FDATA_SUCCESS)
    {
        return LS_BUFFER_ERR;
    }

    struct ls_format_info format;
    ls_format_info_init(&format);

    const ls_error fdata_status = ls_get_fdata_vector(&files_vec, directory, flags);

    if (fdata_status != LS_SUCCESS)
    {
        return fdata_status;
    }

    struct fdata_ptr_vector file_ptrs;

    if (fdata_ptr_vector_init(&file_ptrs, files_vec.size) != FDATA_SUCCESS)
    {
        fdata_vector_free(&files_vec);

        return LS_ALLOC_ERR;
    }

    if (ls_get_fdata_span(&file_ptrs, &files_vec, NULL, NULL) != LS_SUCCESS)
    {
        clean(&files_vec, &file_ptrs);

        return LS_BUFFER_ERR;
    }

    if (should_sort(flags))
    {
        ls_sort_file_vector(&file_ptrs, flags);
    }

    if (flags->is_recursive)
    {
		if (!is_first) 
			printf("\n");

        printf("%s:\n", directory);
    }

	ls_print_catalog(&file_ptrs, flags);

    const ls_error append_status = visited_append(visited, directory);

    if (append_status != LS_SUCCESS)
    {
        clean(&files_vec, &file_ptrs);

        return append_status;
    }

	ls_error ls_status = LS_SUCCESS;
	
    // recursively printing all directories
    if (flags->is_recursive)
    {
        for (size_t i = 0; i < file_ptrs.size; i++)
        {
            const struct file_data *file_info = file_ptrs.data[i];
			bool is_visited;

            if (!should_recurse(file_info, flags, visited, &is_visited))
			{
				if (is_visited)
				{
					fprintf(stderr, "ls: %s: not listing already-listed directory\n", file_info->name);
				}

                continue;
			}

            char *next_path = ls_make_path(directory, file_info->name);
            const ls_error ls_current_status = ls_recursive(next_path, flags, visited, false);	// recursive step

			if (ls_current_status != LS_SUCCESS)
			{
				ls_status = ls_current_status;
			}

            free(next_path);
        }
    }

    clean(&files_vec, &file_ptrs);

    return ls_status;
}


bool filter_files(const struct file_data *file, const struct ls_flags *flags)
{
    return !flags->enter_dir || !is_directory(file, FILE_DATA_TARGET);
}

bool filter_directories(const struct file_data *file, const struct ls_flags *flags)
{
	return flags->enter_dir && is_directory(file, FILE_DATA_TARGET);
}

static ls_error ls_dir(const char *directory, const struct ls_flags *flags)
{
    struct fdata_vector visited;

    if (fdata_vector_init(&visited, 0) != FDATA_SUCCESS)
        return LS_BUFFER_ERR;

    const ls_error ls_status =  ls_recursive(directory, flags, &visited, true);

    fdata_vector_free(&visited);

    return ls_status;
}

ls_error ls(const int argc, const char *argv[])
{
    const flag_setter *setters = ls_get_flag_setters();
    char error_flag;
	
    struct ls_flags flags;

	ls_flags_init(&flags);

    const ls_error parsing_status = ls_parse_flags(&flags, argc, argv, &error_flag, setters);

    if (parsing_status != LS_SUCCESS)
    {
        fprintf(stderr, "ls: invalid option -- '%c'\n", error_flag);

        return LS_ERR;
    }

    struct fdata_vector files_vec;

    if (fdata_vector_init(&files_vec, 0) != FDATA_SUCCESS)
    {
        return LS_BUFFER_ERR;
    }

	int file_args;

    const ls_error retrieve_status = ls_retrieve_files(&files_vec, argc, argv, flags.follow_symlinks, &file_args);

    if (retrieve_status == LS_BUFFER_ERR)
    {
        fdata_vector_free(&files_vec);

        return LS_BUFFER_ERR;
    }

    struct fdata_ptr_vector file_ptrs;

    if (fdata_ptr_vector_init(&file_ptrs, 0) != FDATA_SUCCESS)
    {
        fdata_vector_free(&files_vec);

        return LS_BUFFER_ERR;
    }

    const ls_error span_status = ls_get_fdata_span(&file_ptrs, &files_vec, &flags, filter_files);

    if (span_status != LS_SUCCESS)
    {
        fdata_vector_free(&files_vec);
        fdata_ptr_vector_free(&file_ptrs);

        return span_status;
    }

    struct fdata_ptr_vector dir_ptrs;

    if (fdata_ptr_vector_init(&dir_ptrs, 0) != FDATA_SUCCESS)
    {
        fdata_vector_free(&files_vec);
        fdata_ptr_vector_free(&file_ptrs);

        return LS_BUFFER_ERR;
    }

    if (ls_get_fdata_span(&dir_ptrs, &files_vec, &flags, filter_directories) != LS_SUCCESS)
    {
        fdata_vector_free(&files_vec);
        fdata_ptr_vector_free(&dir_ptrs);
        fdata_ptr_vector_free(&file_ptrs);

        return LS_BUFFER_ERR;
    }

    if (file_ptrs.size > 0)
	{	
		ls_print_catalog(&file_ptrs, &flags);

		if (dir_ptrs.size > 0)
			printf("\n");
	}

    fdata_ptr_vector_free(&file_ptrs);

    for (size_t i = 0; i < dir_ptrs.size; i++)
    {
        const struct file_data *dir = dir_ptrs.data[i];

        if (file_args > 1)
        {
            if (!flags.is_recursive)
            {
                printf("%s:\n", dir->name);
            }
        }

        const ls_error ls_dir_status = ls_dir(dir->name, &flags);

        if (ls_dir_status != LS_SUCCESS)
        {
		    fdata_ptr_vector_free(&dir_ptrs);
            fdata_vector_free(&files_vec);

			return ls_dir_status;
		}

		if (i != dir_ptrs.size - 1)
			printf("\n");
    }

    fdata_ptr_vector_free(&dir_ptrs);
    fdata_vector_free(&files_vec);

    return LS_SUCCESS;
}


