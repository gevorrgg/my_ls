#ifndef FLAG_SETTERS_H
#define FLAG_SETTERS_H

#include <ls/flags/flags.h>

typedef void (*flag_setter) (struct ls_flags *f);

void set_l(struct ls_flags* f);
void set_R(struct ls_flags* f);
void set_a(struct ls_flags* f);
void set_A(struct ls_flags* f);
void set_h(struct ls_flags* f);
void set_d(struct ls_flags* f);
void set_S(struct ls_flags* f);
void set_t(struct ls_flags* f);
void set_r(struct ls_flags* f);
void set_U(struct ls_flags* f);
void set_f(struct ls_flags* f);
void set_1(struct ls_flags* f);
void set_m(struct ls_flags* f);
void set_x(struct ls_flags* f);
void set_C(struct ls_flags* f);
void set_i(struct ls_flags* f);
void set_n(struct ls_flags* f);
void set_g(struct ls_flags* f);
void set_o(struct ls_flags* f);
void set_F(struct ls_flags* f);
void set_p(struct ls_flags* f);
void set_s(struct ls_flags* f);
void set_u(struct ls_flags* f);
void set_c(struct ls_flags* f);
void set_L(struct ls_flags* f);

const flag_setter * ls_get_flag_setters();



#endif
