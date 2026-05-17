//
// Created by user on 5/12/2026.
//
#include <ls/print/print_catalog.h>
#include <file_data.h>
#include <file_vector.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ls/print/print_utils.h>
#include <ls/print/format_info.h>
#include <ls/flags/flags.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_type(const struct file_data *file, const file_data_mode mode)
{
    if (is_directory(file, mode))
    {
        printf("d");
    }
    else if (is_symlink(file, FILE_DATA_LINK))
    {
        printf("l");
    }
    else if (is_socket(file, mode))
    {
        printf("s");
    }
    else if (is_pipe(file, mode))
    {
        printf("p");
    }
    else if (is_character_device(file, mode))
    {
        printf("c");
    }
    else if (is_block_device(file, mode))
    {
        printf("b");
    }
    else
    {
        printf("-");
    }
}

static void print_permissions(const struct file_data *file, const file_data_mode mode)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && mode == FILE_DATA_TARGET)
	{
		printf("????????? ");
		return;
	}
	
    const struct stat *st = get_stat(file, mode);
    mode_t m = st->st_mode;

    char p[10];

    // user
    p[0] = (m & S_IRUSR) ? 'r' : '-';
    p[1] = (m & S_IWUSR) ? 'w' : '-';
    p[2] = (m & S_ISUID) ? ((m & S_IXUSR) ? 's' : 'S')
                      : ((m & S_IXUSR) ? 'x' : '-');

    // group
    p[3] = (m & S_IRGRP) ? 'r' : '-';
    p[4] = (m & S_IWGRP) ? 'w' : '-';
    p[5] = (m & S_ISGID) ? ((m & S_IXGRP) ? 's' : 'S')
                      : ((m & S_IXGRP) ? 'x' : '-');

    // other
    p[6] = (m & S_IROTH) ? 'r' : '-';
    p[7] = (m & S_IWOTH) ? 'w' : '-';
    p[8] = (m & S_ISVTX) ? ((m & S_IXOTH) ? 't' : 'T')
                     : ((m & S_IXOTH) ? 'x' : '-');

    p[9] = '\0';

    printf("%s ", p);
}

static void print_hardlinks_count(const struct file_data *file,
                            const struct ls_format_info *format,
                            const file_data_mode mode)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && mode == FILE_DATA_TARGET)
	{
        print_chars(' ', format->max_links_width - 1);
		printf("? ");
		return;
	}

    const struct stat *st = get_stat(file, mode);
    const size_t hardlinks_count = st->st_nlink;
    const size_t spaces_count = format->max_links_width - len(hardlinks_count);

    print_chars(' ', spaces_count);

    printf("%zu ", hardlinks_count);
}

static void print_owner(const struct file_data *file,
                        const struct ls_format_info *format,
                        const struct ls_flags *flags)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && flags->follow_symlinks)
	{
		printf("?");
		print_chars(' ', format->max_owner_width);
		return;
	}

    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;
    const struct stat *st = get_stat(file, mode);

    if (flags->print_owner_name && file->uname != NULL) // no owner name specified
    {
        const size_t uname_len = strlen(file->uname);
        const size_t spaces_count = format->max_owner_width - uname_len;
        print_chars(' ', spaces_count);
        printf("%s ", file->uname);
    }
    else
    {
        const size_t uname_len = len(st->st_uid);
        const size_t spaces_count = format->max_owner_width - uname_len;
        print_chars(' ', spaces_count);
        printf("%u ", (unsigned)st->st_uid);
    }
}

static void print_group(const struct file_data *file,
                 const struct ls_format_info *format,
                 const struct ls_flags *flags)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && flags->follow_symlinks)
	{
		printf("?");
		print_chars(' ', format->max_group_width);
		return;
	}

    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;
    const struct stat *st = get_stat(file, mode);

    if (flags->print_group_name && file->gname != NULL) // no owner name specified
    {
        const size_t gname_len = strlen(file->gname);
        const size_t spaces_count = format->max_group_width - gname_len;
        print_chars(' ', spaces_count);
        printf("%s ", file->gname);
    }
    else
    {

        const size_t gname_len = len(st->st_gid);
        const size_t spaces_count = format->max_group_width - gname_len;
        print_chars(' ', spaces_count);
        printf("%u ", (unsigned)st->st_gid);
    }
}

static void print_size(const struct file_data *file,
                       const struct ls_flags *flags,
                       const struct ls_format_info *format)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && flags->follow_symlinks)
	{
		if (format) print_chars(' ', format->max_size_width - 1);
		printf("? ");
		return;
	}


    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;

    const struct stat *st = get_stat(file, mode);

    const size_t size = st->st_size;

    if (flags->is_human_readable)
    {
        size_t spaces_count = format->max_size_width - ls_human_readable_len(size);
        print_chars(' ', spaces_count);
        print_human_readable(size);
    }
    else
    {
        size_t spaces_count = format->max_size_width - len(size);
        print_chars(' ', spaces_count);
        printf("%lu ", size);
    }
}

time_t get_time(const struct stat *file_stat, const struct ls_flags *flags)
{
    switch (flags->time_type)
    {
        case TIME_CHANGE:
            return file_stat->st_ctime;
        case TIME_ACCESS:
            return file_stat->st_atime;
        default:
            return file_stat->st_mtime;
    }
}

static const char *get_month(const int month)
{
    if (month < 0 || month > 11)
    {
        perror("get_month");
        return NULL;
    }

    static const char *months[12] = { "Jan", "Feb", "Mar",
                                     "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep",
                                     "Oct", "Nov", "Dec" };

    return months[month];
}

static bool should_print_year(const time_t t)
{
    const time_t now = time(NULL);
    const time_t halfyear = 31556952 / 2;

    return (t < now - halfyear || t > now + halfyear);
}

void print_datetime(const struct file_data      *file,
                    const struct ls_flags       *flags)
{
	if (is_symlink(file, FILE_DATA_LINK) && !(file->stat_status) && flags->follow_symlinks)
	{
		print_chars(' ', 11);
		printf("? ");
		return;
	}

    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;

    const struct stat *st = get_stat(file, mode);
    time_t t = get_time(st, flags);

    struct tm dtime;

    if (!localtime_r(&t, &dtime))
    {
        perror("localtime_r");
        return;
    }

    printf("%s %2d ", get_month(dtime.tm_mon), dtime.tm_mday);

    if (should_print_year(t))
    {
        printf(" %d", dtime.tm_year + 1900);
    }
    else
    {
        printf("%02d:%02d",dtime.tm_hour, dtime.tm_min);
    }

    printf(" ");
}

static void print_metadata(const struct file_data *file,
                           const struct ls_flags *flags,
                           const struct ls_format_info *format_info)
{
    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;
    print_type(file, mode);
    print_permissions(file, mode);

    print_hardlinks_count(file, format_info, mode);

    if (flags->show_owner)
        print_owner(file, format_info, flags);

    if (flags->show_group)
        print_group(file, format_info, flags);

    print_size(file, flags, format_info);
    print_datetime(file, flags);
}

static void print_pointing_path(const struct file_data *file)
{
    ssize_t size = file->lst.st_size > 0 ? file->lst.st_size : 256;

    while (true)
    {
        char *buf = malloc(size);

        if (buf == NULL)
        {
            perror("malloc");
            return;
        }

        const ssize_t len = readlink(file->path, buf, size);

        if (len == -1)
        {
            free(buf);
            perror("readlink");
            return;
        }

        if (len < size)
        {
            buf[len] = '\0';
            printf(" -> %s", buf);
            free(buf);
            return;
        }

        free(buf);
        size *= 2;
    }
}

void ls_print_info_long(const struct file_data *file,
                        const struct ls_flags *flags,
                        const struct ls_format_info *format_info)
{
    const file_data_mode mode = flags->follow_symlinks ? FILE_DATA_TARGET : FILE_DATA_LINK;

    if (flags->show_inode)
        print_inode(file, format_info, mode);

    if (flags->show_blocks)
        print_blocks(file, format_info, flags);

    print_metadata(file, flags, format_info);

    printf("%s", file->name);

    if (!flags->follow_symlinks && is_symlink(file, FILE_DATA_LINK))
    {
        print_pointing_path(file);
    }

	if (should_print_suffix(flags))
	{
		print_suffix(file, flags);
	}

}

void   ls_print_long                 (const struct fdata_ptr_vector *files,
                                      const struct ls_flags *flags)
{
    struct ls_format_info format_info;
    ls_format_info_init(&format_info);

    ls_get_format_info(&format_info, files, flags);

    print_total(files, flags);

    for (size_t i = 0; i < files->size; i++)
    {
        ls_print_info_long(files->data[i], flags, &format_info);
        printf("\n");
    }
}

