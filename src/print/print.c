#include <ls/print/print.h>
#include <ls/print/print_catalog.h>
#include <file_data.h>
#include <ls/flags/flags.h>
#include <stdio.h>
#include <assert.h>

void ls_print_catalog (const struct fdata_ptr_vector *file_ptrs,
                       const struct ls_flags *flags)
{
    switch (flags->format)
    {
        case FORMAT_COMMAS:
            ls_print_commas(file_ptrs, flags);

            break;
        case FORMAT_LONG:
            ls_print_long(file_ptrs, flags);

            break;
        case FORMAT_ONE_PER_LINE:
            ls_print_one_per_line(file_ptrs, flags);

            break;
        case FORMAT_HORIZONTAL:
            ls_print_horizontal(file_ptrs, flags);

            break;
        case FORMAT_COLUMNS:
            ls_print_columns(file_ptrs, flags);

            break;
        case FORMAT_STANDARD:
            ls_print_standard(file_ptrs, flags);

            break;
        default:
			printf("Format: %d\n", flags->format);
            assert(false);
    }
}
