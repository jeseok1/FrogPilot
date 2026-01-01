#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_2905119886940077120);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6780339822011947096);
void car_H_mod_fun(double *state, double *out_626774766696730126);
void car_f_fun(double *state, double dt, double *out_426499701889368823);
void car_F_fun(double *state, double dt, double *out_2152918147334440483);
void car_h_25(double *state, double *unused, double *out_1302972804247986075);
void car_H_25(double *state, double *unused, double *out_2567826205095177496);
void car_h_24(double *state, double *unused, double *out_3135959750372348064);
void car_H_24(double *state, double *unused, double *out_1414496946302590402);
void car_h_30(double *state, double *unused, double *out_2216469330072659531);
void car_H_30(double *state, double *unused, double *out_8962227527122757365);
void car_h_26(double *state, double *unused, double *out_8462920387532832999);
void car_H_26(double *state, double *unused, double *out_3224680269205489400);
void car_h_27(double *state, double *unused, double *out_5096068713165863071);
void car_H_27(double *state, double *unused, double *out_7309753234786369340);
void car_h_29(double *state, double *unused, double *out_6579840134193337609);
void car_H_29(double *state, double *unused, double *out_8451996182808365181);
void car_h_28(double *state, double *unused, double *out_8008808124657657982);
void car_H_28(double *state, double *unused, double *out_4912348873831655861);
void car_h_31(double *state, double *unused, double *out_2334476724006036275);
void car_H_31(double *state, double *unused, double *out_2598472166972137924);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}