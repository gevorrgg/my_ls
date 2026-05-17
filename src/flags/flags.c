//
// Created by user on 4/12/2026.
//
#include <ls/flags/flags.h>

void ls_flags_init(struct ls_flags *self)
{
    self->is_recursive          = false;
    self->is_human_readable     = false;
    self->show_inode            = false;
    self->sort_reversed         = false;
    self->show_owner            = true;
    self->show_group            = true;
    self->follow_symlinks       = false;
    self->show_blocks           = false;
    self->enter_dir             = true;
	self->print_owner_name      = true;
	self->print_group_name      = true;

    self->sort                  = SORT_NAME;
    self->format                = FORMAT_STANDARD;
    self->time_type             = TIME_MODIFICATION;
    self->hidden                = HIDE_DEFAULT;
    self->symbols_mode          = SYMBOLS_NONE;
}
