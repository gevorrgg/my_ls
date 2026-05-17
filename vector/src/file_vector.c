#include <file_vector.h>
#include <file_data.h>
#include <stdlib.h>
#define CAPACITY_GROWTH_FACTOR 2

#define VECTOR_EXTEND(type, vec, new_capacity, err_var) \
	do { \
		type *tmp = realloc(vec->data, new_capacity * sizeof(type)); \
		if (tmp == NULL) return err_var; \
		(vec)->data = tmp; \
		(vec)->capacity = new_capacity;	\
	} while(0);	

fdata_err fdata_vector_init(struct fdata_vector* self, size_t capacity)
{
	struct file_data* data_ptr = malloc(sizeof(struct file_data) * capacity);

	if (data_ptr == NULL)
		return FDATA_ALLOC_ERR;	// could not allocate

	self->data     = data_ptr;
	self->size 	   = 0;
	self->capacity = capacity;

    return FDATA_SUCCESS;
}

fdata_err fdata_vector_append(struct fdata_vector* self, const struct file_data* value)
{
	if (self == NULL)
		return FDATA_NULL_ERR;

	if (self->size == self->capacity)
	{
		size_t new_capacity;
		
		if (self->capacity == 0)
		{
			new_capacity = 1;
		}
		else
		{
			new_capacity = self->capacity * CAPACITY_GROWTH_FACTOR;
			
			if (new_capacity / 2 != self->capacity) // overflow
				return FDATA_OVERFLOW_ERR;
		}

		fdata_err extend_status = fdata_vector_extend(self, new_capacity);

		if (extend_status != FDATA_SUCCESS)	// failed to resize
			return extend_status;
	}

	struct file_data* vector_end = self->data + self->size;

	if (file_data_construct(vector_end, value) != FILE_DATA_SUCCESS) // failed to construct 
		return FDATA_CONSTRUCT_ERR;

	self->size += 1;

	return FDATA_SUCCESS;
}

bool vector_fdata_is_in(const struct fdata_vector* self, const struct file_data *value)
{
    for (size_t i = 0; i < self->size; i++)
    {
        if (fdata_equals(self->data + i, value))
        {
            return true;
        }
    }

    return false;
}

void fdata_vector_free(struct fdata_vector* self)
{
	if (self == NULL)
		return;

	for (size_t i = 0; i < self->size; i++)
	{
		file_data_free(self->data + i);
	}

	free(self->data);

	self->data     = NULL;
	self->size 	   = 0;
	self->capacity = 0;
}

fdata_err fdata_vector_extend(struct fdata_vector* self, size_t new_capacity)
{
	if (self == NULL)
		return FDATA_NULL_ERR;

	VECTOR_EXTEND(struct file_data, self, new_capacity, FDATA_ALLOC_ERR);

	return FDATA_SUCCESS;
}

fdata_err fdata_ptr_vector_init(struct fdata_ptr_vector* self, size_t capacity)
{
	const struct file_data** data_ptr = malloc(sizeof(struct file_data*) * capacity);

	if (data_ptr == NULL)
		return FDATA_ALLOC_ERR;	// could not allocate

	self->data     = data_ptr;
	self->size 	   = 0;
	self->capacity = capacity;

    return FDATA_SUCCESS;
}

fdata_err fdata_ptr_vector_append(struct fdata_ptr_vector* self, const struct file_data* value)
{
	if (self == NULL)
		return FDATA_NULL_ERR;

	if (self->size == self->capacity)
	{
		size_t new_capacity;
		
		if (self->capacity == 0)
		{
			new_capacity = 1;
		}
		else
		{
			new_capacity = self->capacity * CAPACITY_GROWTH_FACTOR;
			
			if (new_capacity / 2 != self->capacity) // overflow
				return FDATA_OVERFLOW_ERR;
		}

		fdata_err extend_status = fdata_ptr_vector_extend(self, new_capacity);

		if (extend_status != FDATA_SUCCESS)	// failed to resize
			return extend_status;
	}

	self->data[self->size] = value;
	self->size += 1;

	return FDATA_SUCCESS;
}

void fdata_ptr_vector_free(struct fdata_ptr_vector* self)
{
	if (self == NULL)
		return;

	free(self->data);

	self->data     = NULL;
	self->size 	   = 0;
	self->capacity = 0;
}

fdata_err fdata_ptr_vector_extend(struct fdata_ptr_vector* self, size_t new_capacity)
{
	if (self == NULL)
		return FDATA_NULL_ERR;

	VECTOR_EXTEND(const struct file_data*, self, new_capacity, FDATA_ALLOC_ERR);

	return FDATA_SUCCESS;
}
