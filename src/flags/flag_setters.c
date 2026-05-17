#include <ls/flags/flag_setters.h>

void set_l(struct ls_flags* f) { f->format =  FORMAT_LONG; }
void set_R(struct ls_flags* f) { f->is_recursive = true; }
void set_a(struct ls_flags* f) { f->hidden = SHOW_ALL; }
void set_A(struct ls_flags* f) { f->hidden = SHOW_ALMOST; }
void set_h(struct ls_flags* f) { f->is_human_readable = true; }
void set_d(struct ls_flags* f) { f->enter_dir = false; }
void set_S(struct ls_flags* f) { f->sort = SORT_SIZE; }
void set_t(struct ls_flags* f) { f->sort = SORT_TIME; }
void set_r(struct ls_flags* f) { f->sort_reversed = true; }
void set_U(struct ls_flags* f) { f->sort = SORT_NONE; }
void set_f(struct ls_flags* f) { f->hidden = SHOW_ALL; f->sort = SORT_NONE; }
void set_1(struct ls_flags* f) { f->format = FORMAT_ONE_PER_LINE; }
void set_m(struct ls_flags* f) { f->format = FORMAT_COMMAS; }
void set_x(struct ls_flags* f) { f->format = FORMAT_HORIZONTAL; }
void set_C(struct ls_flags* f) { f->format = FORMAT_COLUMNS; }
void set_i(struct ls_flags* f) { f->show_inode = true; }
void set_n(struct ls_flags* f) { f->format=FORMAT_LONG; f->print_owner_name=false; f->print_group_name=false; }
void set_g(struct ls_flags* f) { f->format=FORMAT_LONG; f->show_owner=false; f->show_group=true; }
void set_o(struct ls_flags* f) { f->format=FORMAT_LONG; f->show_owner=true; f->show_group=false; }
void set_F(struct ls_flags* f) { f->symbols_mode=SYMBOLS_ALL; }
void set_p(struct ls_flags* f) { f->symbols_mode=SYMBOLS_DIRECTORIES; }
void set_s(struct ls_flags* f) { f->show_blocks=true; }
void set_u(struct ls_flags* f) { f->time_type=TIME_ACCESS; }
void set_c(struct ls_flags* f) { f->time_type=TIME_CHANGE; }
void set_L(struct ls_flags* f) { f->follow_symlinks = true; }

const flag_setter *ls_get_flag_setters()
{
    static const flag_setter flag_setters[256] = {
        ['l'] = set_l,
        ['R'] = set_R,
        ['a'] = set_a,
        ['A'] = set_A,
        ['h'] = set_h,
        ['d'] = set_d,
        ['S'] = set_S,
        ['t'] = set_t,
        ['r'] = set_r,
        ['U'] = set_U,
        ['f'] = set_f,
        ['1'] = set_1,
        ['m'] = set_m,
        ['x'] = set_x,
        ['C'] = set_C,
        ['i'] = set_i,
        ['n'] = set_n,
        ['g'] = set_g,
        ['o'] = set_o,
        ['F'] = set_F,
        ['p'] = set_p,
        ['s'] = set_s,
        ['u'] = set_u,
        ['c'] = set_c,
        ['L'] = set_L,
    };

    return flag_setters;
}
