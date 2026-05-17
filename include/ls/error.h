//
// Created by user on 4/12/2026.
//

#ifndef LS_LS_ERROR_H
#define LS_LS_ERROR_H

typedef enum {
    LS_SUCCESS = 0,
    LS_SORTING_ERR,
    LS_ALLOC_ERR,
    LS_BUFFER_ERR,
    LS_FILE_NOT_FOUND,
    LS_ERR,
    LS_STAT_ERR,
    LS_UNKNOWN_FLAG_ERR
} ls_error;

#endif //LS_LS_ERROR_H