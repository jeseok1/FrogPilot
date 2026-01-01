#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_9079467366256257445);
void live_err_fun(double *nom_x, double *delta_x, double *out_6787024702700764570);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_1256298413682825874);
void live_H_mod_fun(double *state, double *out_5350491835002202754);
void live_f_fun(double *state, double dt, double *out_8270562207744455257);
void live_F_fun(double *state, double dt, double *out_1396582482101012414);
void live_h_4(double *state, double *unused, double *out_8496584591823651264);
void live_H_4(double *state, double *unused, double *out_3194627430905255393);
void live_h_9(double *state, double *unused, double *out_7325618275165026407);
void live_H_9(double *state, double *unused, double *out_7351795167260032876);
void live_h_10(double *state, double *unused, double *out_5460056851981353526);
void live_H_10(double *state, double *unused, double *out_8309008289674392161);
void live_h_12(double *state, double *unused, double *out_2227185882421942334);
void live_H_12(double *state, double *unused, double *out_2573528405857661726);
void live_h_35(double *state, double *unused, double *out_8364003727397408696);
void live_H_35(double *state, double *unused, double *out_172034626467351983);
void live_h_32(double *state, double *unused, double *out_7367896386725749875);
void live_H_32(double *state, double *unused, double *out_5973338998674964541);
void live_h_13(double *state, double *unused, double *out_2331775519727822623);
void live_H_13(double *state, double *unused, double *out_5553219346348398486);
void live_h_14(double *state, double *unused, double *out_7325618275165026407);
void live_H_14(double *state, double *unused, double *out_7351795167260032876);
void live_h_33(double *state, double *unused, double *out_6202811009168306974);
void live_H_33(double *state, double *unused, double *out_3322591631106209587);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}