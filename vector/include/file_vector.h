#ifndef VECTOR_H
#define VECTOR_H

#include "file_data.h"
#include <stdbool.h>
#include <stddef.h>

struct fdata_vector
{
	struct file_data* data;
	size_t size;
	size_t capacity;
};

typedef enum
{
	FDATA_SUCCESS = 0,
	FDATA_NULL_ERR,
	FDATA_OVERFLOW_ERR,
	FDATA_ALLOC_ERR,
	FDATA_CONSTRUCT_ERR
} fdata_err;

fdata_err fdata_vector_init(struct fdata_vector *self, size_t size);
fdata_err fdata_vector_append(struct fdata_vector *self, const struct file_data *value);
fdata_err fdata_vector_extend(struct fdata_vector *self, size_t new_capacity);
void fdata_vector_free(struct fdata_vector *self);
bool vector_fdata_is_in(const struct fdata_vector *self, const struct file_data *value);

struct fdata_ptr_vector
{
	const struct file_data** data;
	size_t size;
	size_t capacity;
};


fdata_err fdata_ptr_vector_init(struct fdata_ptr_vector *self, size_t size);
fdata_err fdata_ptr_vector_append(struct fdata_ptr_vector *self, const struct file_data *value);
fdata_err fdata_ptr_vector_extend(struct fdata_ptr_vector *self, size_t new_capacity);
void fdata_ptr_vector_free(struct fdata_ptr_vector *self);

#endif
