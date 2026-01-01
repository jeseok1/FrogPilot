#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_2905119886940077120) {
   out_2905119886940077120[0] = delta_x[0] + nom_x[0];
   out_2905119886940077120[1] = delta_x[1] + nom_x[1];
   out_2905119886940077120[2] = delta_x[2] + nom_x[2];
   out_2905119886940077120[3] = delta_x[3] + nom_x[3];
   out_2905119886940077120[4] = delta_x[4] + nom_x[4];
   out_2905119886940077120[5] = delta_x[5] + nom_x[5];
   out_2905119886940077120[6] = delta_x[6] + nom_x[6];
   out_2905119886940077120[7] = delta_x[7] + nom_x[7];
   out_2905119886940077120[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6780339822011947096) {
   out_6780339822011947096[0] = -nom_x[0] + true_x[0];
   out_6780339822011947096[1] = -nom_x[1] + true_x[1];
   out_6780339822011947096[2] = -nom_x[2] + true_x[2];
   out_6780339822011947096[3] = -nom_x[3] + true_x[3];
   out_6780339822011947096[4] = -nom_x[4] + true_x[4];
   out_6780339822011947096[5] = -nom_x[5] + true_x[5];
   out_6780339822011947096[6] = -nom_x[6] + true_x[6];
   out_6780339822011947096[7] = -nom_x[7] + true_x[7];
   out_6780339822011947096[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_626774766696730126) {
   out_626774766696730126[0] = 1.0;
   out_626774766696730126[1] = 0;
   out_626774766696730126[2] = 0;
   out_626774766696730126[3] = 0;
   out_626774766696730126[4] = 0;
   out_626774766696730126[5] = 0;
   out_626774766696730126[6] = 0;
   out_626774766696730126[7] = 0;
   out_626774766696730126[8] = 0;
   out_626774766696730126[9] = 0;
   out_626774766696730126[10] = 1.0;
   out_626774766696730126[11] = 0;
   out_626774766696730126[12] = 0;
   out_626774766696730126[13] = 0;
   out_626774766696730126[14] = 0;
   out_626774766696730126[15] = 0;
   out_626774766696730126[16] = 0;
   out_626774766696730126[17] = 0;
   out_626774766696730126[18] = 0;
   out_626774766696730126[19] = 0;
   out_626774766696730126[20] = 1.0;
   out_626774766696730126[21] = 0;
   out_626774766696730126[22] = 0;
   out_626774766696730126[23] = 0;
   out_626774766696730126[24] = 0;
   out_626774766696730126[25] = 0;
   out_626774766696730126[26] = 0;
   out_626774766696730126[27] = 0;
   out_626774766696730126[28] = 0;
   out_626774766696730126[29] = 0;
   out_626774766696730126[30] = 1.0;
   out_626774766696730126[31] = 0;
   out_626774766696730126[32] = 0;
   out_626774766696730126[33] = 0;
   out_626774766696730126[34] = 0;
   out_626774766696730126[35] = 0;
   out_626774766696730126[36] = 0;
   out_626774766696730126[37] = 0;
   out_626774766696730126[38] = 0;
   out_626774766696730126[39] = 0;
   out_626774766696730126[40] = 1.0;
   out_626774766696730126[41] = 0;
   out_626774766696730126[42] = 0;
   out_626774766696730126[43] = 0;
   out_626774766696730126[44] = 0;
   out_626774766696730126[45] = 0;
   out_626774766696730126[46] = 0;
   out_626774766696730126[47] = 0;
   out_626774766696730126[48] = 0;
   out_626774766696730126[49] = 0;
   out_626774766696730126[50] = 1.0;
   out_626774766696730126[51] = 0;
   out_626774766696730126[52] = 0;
   out_626774766696730126[53] = 0;
   out_626774766696730126[54] = 0;
   out_626774766696730126[55] = 0;
   out_626774766696730126[56] = 0;
   out_626774766696730126[57] = 0;
   out_626774766696730126[58] = 0;
   out_626774766696730126[59] = 0;
   out_626774766696730126[60] = 1.0;
   out_626774766696730126[61] = 0;
   out_626774766696730126[62] = 0;
   out_626774766696730126[63] = 0;
   out_626774766696730126[64] = 0;
   out_626774766696730126[65] = 0;
   out_626774766696730126[66] = 0;
   out_626774766696730126[67] = 0;
   out_626774766696730126[68] = 0;
   out_626774766696730126[69] = 0;
   out_626774766696730126[70] = 1.0;
   out_626774766696730126[71] = 0;
   out_626774766696730126[72] = 0;
   out_626774766696730126[73] = 0;
   out_626774766696730126[74] = 0;
   out_626774766696730126[75] = 0;
   out_626774766696730126[76] = 0;
   out_626774766696730126[77] = 0;
   out_626774766696730126[78] = 0;
   out_626774766696730126[79] = 0;
   out_626774766696730126[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_426499701889368823) {
   out_426499701889368823[0] = state[0];
   out_426499701889368823[1] = state[1];
   out_426499701889368823[2] = state[2];
   out_426499701889368823[3] = state[3];
   out_426499701889368823[4] = state[4];
   out_426499701889368823[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_426499701889368823[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_426499701889368823[7] = state[7];
   out_426499701889368823[8] = state[8];
}
void F_fun(double *state, double dt, double *out_2152918147334440483) {
   out_2152918147334440483[0] = 1;
   out_2152918147334440483[1] = 0;
   out_2152918147334440483[2] = 0;
   out_2152918147334440483[3] = 0;
   out_2152918147334440483[4] = 0;
   out_2152918147334440483[5] = 0;
   out_2152918147334440483[6] = 0;
   out_2152918147334440483[7] = 0;
   out_2152918147334440483[8] = 0;
   out_2152918147334440483[9] = 0;
   out_2152918147334440483[10] = 1;
   out_2152918147334440483[11] = 0;
   out_2152918147334440483[12] = 0;
   out_2152918147334440483[13] = 0;
   out_2152918147334440483[14] = 0;
   out_2152918147334440483[15] = 0;
   out_2152918147334440483[16] = 0;
   out_2152918147334440483[17] = 0;
   out_2152918147334440483[18] = 0;
   out_2152918147334440483[19] = 0;
   out_2152918147334440483[20] = 1;
   out_2152918147334440483[21] = 0;
   out_2152918147334440483[22] = 0;
   out_2152918147334440483[23] = 0;
   out_2152918147334440483[24] = 0;
   out_2152918147334440483[25] = 0;
   out_2152918147334440483[26] = 0;
   out_2152918147334440483[27] = 0;
   out_2152918147334440483[28] = 0;
   out_2152918147334440483[29] = 0;
   out_2152918147334440483[30] = 1;
   out_2152918147334440483[31] = 0;
   out_2152918147334440483[32] = 0;
   out_2152918147334440483[33] = 0;
   out_2152918147334440483[34] = 0;
   out_2152918147334440483[35] = 0;
   out_2152918147334440483[36] = 0;
   out_2152918147334440483[37] = 0;
   out_2152918147334440483[38] = 0;
   out_2152918147334440483[39] = 0;
   out_2152918147334440483[40] = 1;
   out_2152918147334440483[41] = 0;
   out_2152918147334440483[42] = 0;
   out_2152918147334440483[43] = 0;
   out_2152918147334440483[44] = 0;
   out_2152918147334440483[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_2152918147334440483[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_2152918147334440483[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_2152918147334440483[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_2152918147334440483[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_2152918147334440483[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_2152918147334440483[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_2152918147334440483[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_2152918147334440483[53] = -9.8000000000000007*dt;
   out_2152918147334440483[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_2152918147334440483[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_2152918147334440483[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2152918147334440483[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2152918147334440483[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_2152918147334440483[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_2152918147334440483[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_2152918147334440483[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2152918147334440483[62] = 0;
   out_2152918147334440483[63] = 0;
   out_2152918147334440483[64] = 0;
   out_2152918147334440483[65] = 0;
   out_2152918147334440483[66] = 0;
   out_2152918147334440483[67] = 0;
   out_2152918147334440483[68] = 0;
   out_2152918147334440483[69] = 0;
   out_2152918147334440483[70] = 1;
   out_2152918147334440483[71] = 0;
   out_2152918147334440483[72] = 0;
   out_2152918147334440483[73] = 0;
   out_2152918147334440483[74] = 0;
   out_2152918147334440483[75] = 0;
   out_2152918147334440483[76] = 0;
   out_2152918147334440483[77] = 0;
   out_2152918147334440483[78] = 0;
   out_2152918147334440483[79] = 0;
   out_2152918147334440483[80] = 1;
}
void h_25(double *state, double *unused, double *out_1302972804247986075) {
   out_1302972804247986075[0] = state[6];
}
void H_25(double *state, double *unused, double *out_2567826205095177496) {
   out_2567826205095177496[0] = 0;
   out_2567826205095177496[1] = 0;
   out_2567826205095177496[2] = 0;
   out_2567826205095177496[3] = 0;
   out_2567826205095177496[4] = 0;
   out_2567826205095177496[5] = 0;
   out_2567826205095177496[6] = 1;
   out_2567826205095177496[7] = 0;
   out_2567826205095177496[8] = 0;
}
void h_24(double *state, double *unused, double *out_3135959750372348064) {
   out_3135959750372348064[0] = state[4];
   out_3135959750372348064[1] = state[5];
}
void H_24(double *state, double *unused, double *out_1414496946302590402) {
   out_1414496946302590402[0] = 0;
   out_1414496946302590402[1] = 0;
   out_1414496946302590402[2] = 0;
   out_1414496946302590402[3] = 0;
   out_1414496946302590402[4] = 1;
   out_1414496946302590402[5] = 0;
   out_1414496946302590402[6] = 0;
   out_1414496946302590402[7] = 0;
   out_1414496946302590402[8] = 0;
   out_1414496946302590402[9] = 0;
   out_1414496946302590402[10] = 0;
   out_1414496946302590402[11] = 0;
   out_1414496946302590402[12] = 0;
   out_1414496946302590402[13] = 0;
   out_1414496946302590402[14] = 1;
   out_1414496946302590402[15] = 0;
   out_1414496946302590402[16] = 0;
   out_1414496946302590402[17] = 0;
}
void h_30(double *state, double *unused, double *out_2216469330072659531) {
   out_2216469330072659531[0] = state[4];
}
void H_30(double *state, double *unused, double *out_8962227527122757365) {
   out_8962227527122757365[0] = 0;
   out_8962227527122757365[1] = 0;
   out_8962227527122757365[2] = 0;
   out_8962227527122757365[3] = 0;
   out_8962227527122757365[4] = 1;
   out_8962227527122757365[5] = 0;
   out_8962227527122757365[6] = 0;
   out_8962227527122757365[7] = 0;
   out_8962227527122757365[8] = 0;
}
void h_26(double *state, double *unused, double *out_8462920387532832999) {
   out_8462920387532832999[0] = state[7];
}
void H_26(double *state, double *unused, double *out_3224680269205489400) {
   out_3224680269205489400[0] = 0;
   out_3224680269205489400[1] = 0;
   out_3224680269205489400[2] = 0;
   out_3224680269205489400[3] = 0;
   out_3224680269205489400[4] = 0;
   out_3224680269205489400[5] = 0;
   out_3224680269205489400[6] = 0;
   out_3224680269205489400[7] = 1;
   out_3224680269205489400[8] = 0;
}
void h_27(double *state, double *unused, double *out_5096068713165863071) {
   out_5096068713165863071[0] = state[3];
}
void H_27(double *state, double *unused, double *out_7309753234786369340) {
   out_7309753234786369340[0] = 0;
   out_7309753234786369340[1] = 0;
   out_7309753234786369340[2] = 0;
   out_7309753234786369340[3] = 1;
   out_7309753234786369340[4] = 0;
   out_7309753234786369340[5] = 0;
   out_7309753234786369340[6] = 0;
   out_7309753234786369340[7] = 0;
   out_7309753234786369340[8] = 0;
}
void h_29(double *state, double *unused, double *out_6579840134193337609) {
   out_6579840134193337609[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8451996182808365181) {
   out_8451996182808365181[0] = 0;
   out_8451996182808365181[1] = 1;
   out_8451996182808365181[2] = 0;
   out_8451996182808365181[3] = 0;
   out_8451996182808365181[4] = 0;
   out_8451996182808365181[5] = 0;
   out_8451996182808365181[6] = 0;
   out_8451996182808365181[7] = 0;
   out_8451996182808365181[8] = 0;
}
void h_28(double *state, double *unused, double *out_8008808124657657982) {
   out_8008808124657657982[0] = state[0];
}
void H_28(double *state, double *unused, double *out_4912348873831655861) {
   out_4912348873831655861[0] = 1;
   out_4912348873831655861[1] = 0;
   out_4912348873831655861[2] = 0;
   out_4912348873831655861[3] = 0;
   out_4912348873831655861[4] = 0;
   out_4912348873831655861[5] = 0;
   out_4912348873831655861[6] = 0;
   out_4912348873831655861[7] = 0;
   out_4912348873831655861[8] = 0;
}
void h_31(double *state, double *unused, double *out_2334476724006036275) {
   out_2334476724006036275[0] = state[8];
}
void H_31(double *state, double *unused, double *out_2598472166972137924) {
   out_2598472166972137924[0] = 0;
   out_2598472166972137924[1] = 0;
   out_2598472166972137924[2] = 0;
   out_2598472166972137924[3] = 0;
   out_2598472166972137924[4] = 0;
   out_2598472166972137924[5] = 0;
   out_2598472166972137924[6] = 0;
   out_2598472166972137924[7] = 0;
   out_2598472166972137924[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_2905119886940077120) {
  err_fun(nom_x, delta_x, out_2905119886940077120);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6780339822011947096) {
  inv_err_fun(nom_x, true_x, out_6780339822011947096);
}
void car_H_mod_fun(double *state, double *out_626774766696730126) {
  H_mod_fun(state, out_626774766696730126);
}
void car_f_fun(double *state, double dt, double *out_426499701889368823) {
  f_fun(state,  dt, out_426499701889368823);
}
void car_F_fun(double *state, double dt, double *out_2152918147334440483) {
  F_fun(state,  dt, out_2152918147334440483);
}
void car_h_25(double *state, double *unused, double *out_1302972804247986075) {
  h_25(state, unused, out_1302972804247986075);
}
void car_H_25(double *state, double *unused, double *out_2567826205095177496) {
  H_25(state, unused, out_2567826205095177496);
}
void car_h_24(double *state, double *unused, double *out_3135959750372348064) {
  h_24(state, unused, out_3135959750372348064);
}
void car_H_24(double *state, double *unused, double *out_1414496946302590402) {
  H_24(state, unused, out_1414496946302590402);
}
void car_h_30(double *state, double *unused, double *out_2216469330072659531) {
  h_30(state, unused, out_2216469330072659531);
}
void car_H_30(double *state, double *unused, double *out_8962227527122757365) {
  H_30(state, unused, out_8962227527122757365);
}
void car_h_26(double *state, double *unused, double *out_8462920387532832999) {
  h_26(state, unused, out_8462920387532832999);
}
void car_H_26(double *state, double *unused, double *out_3224680269205489400) {
  H_26(state, unused, out_3224680269205489400);
}
void car_h_27(double *state, double *unused, double *out_5096068713165863071) {
  h_27(state, unused, out_5096068713165863071);
}
void car_H_27(double *state, double *unused, double *out_7309753234786369340) {
  H_27(state, unused, out_7309753234786369340);
}
void car_h_29(double *state, double *unused, double *out_6579840134193337609) {
  h_29(state, unused, out_6579840134193337609);
}
void car_H_29(double *state, double *unused, double *out_8451996182808365181) {
  H_29(state, unused, out_8451996182808365181);
}
void car_h_28(double *state, double *unused, double *out_8008808124657657982) {
  h_28(state, unused, out_8008808124657657982);
}
void car_H_28(double *state, double *unused, double *out_4912348873831655861) {
  H_28(state, unused, out_4912348873831655861);
}
void car_h_31(double *state, double *unused, double *out_2334476724006036275) {
  h_31(state, unused, out_2334476724006036275);
}
void car_H_31(double *state, double *unused, double *out_2598472166972137924) {
  H_31(state, unused, out_2598472166972137924);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
