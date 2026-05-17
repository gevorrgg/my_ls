#include <ls/sorting.h>
#include <ls/flags/flags.h>
#include <file_data.h>
#include <file_vector.h>
#include <string.h>
#include <stdlib.h>

/* Global context for qsort */

static const struct ls_flags *g_flags = NULL;

/* Comparators */

static int cmp_ll(long a, long b)
{
    return (a > b) - (a < b);
}

static int cmp_name(const struct file_data *a,
                    const struct file_data *b)
{
    return strcmp(a->name, b->name);
}

static int cmp_size(const struct file_data *a,
                    const struct file_data *b)
{
    if (a->st.st_size != b->st.st_size)
        return cmp_ll(a->st.st_size, b->st.st_size);

    return cmp_name(a, b);
}

static int cmp_mtime(const struct file_data *a,
                     const struct file_data *b)
{
    if (a->st.st_mtime != b->st.st_mtime)
        return cmp_ll(a->st.st_mtime, b->st.st_mtime);

    return cmp_name(a, b);
}

static int cmp_ctime(const struct file_data *a,
                     const struct file_data *b)
{
    if (a->st.st_ctime != b->st.st_ctime)
        return cmp_ll(a->st.st_ctime, b->st.st_ctime);

    return cmp_name(a, b);
}

static int cmp_atime(const struct file_data *a,
                     const struct file_data *b)
{
    if (a->st.st_atime != b->st.st_atime)
        return cmp_ll(a->st.st_atime, b->st.st_atime);

    return cmp_name(a, b);
}

/* helpers */

static int (*get_base_cmp(void))
    (const struct file_data *, const struct file_data *)
{
    switch (g_flags->sort)
    {
        case SORT_SIZE:
            return cmp_size;

        case SORT_NAME:
            return cmp_name;

        case SORT_TIME:
        {
            switch (g_flags->time_type)
            {
                case TIME_ACCESS:
                    return cmp_atime;
                case TIME_CHANGE:
                    return cmp_ctime;
                case TIME_MODIFICATION:
                default:
                    return cmp_mtime;
            }
        }

        default:
            return NULL;
    }
}

static int qsort_cmp(const void *a, const void *b)
{
    const struct file_data *x = *(const struct file_data **)a;
    const struct file_data *y = *(const struct file_data **)b;

    int (*cmp)(const struct file_data *,
               const struct file_data *) = get_base_cmp();

    if (!cmp)
        return 0;

    int res = cmp(x, y);

    return g_flags->sort_reversed ? -res : res;
}

/* public API */

void ls_sort_file_vector(struct fdata_ptr_vector *file,
                         const struct ls_flags *flags)
{
    if (!file || file->size == 0)
        return;

    g_flags = flags;

    qsort(file->data,
          file->size,
          sizeof(struct file_data *),
          qsort_cmp);

    g_flags = NULL;
}
