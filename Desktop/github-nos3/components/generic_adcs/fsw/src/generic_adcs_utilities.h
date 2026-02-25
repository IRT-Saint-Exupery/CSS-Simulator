/*******************************************************************************
** Purpose:
**   This file has utility functions used by ADCS.
**
*******************************************************************************/
#ifndef _GENERIC_ADCS_UTILITIES_H_
#define _GENERIC_ADCS_UTILITIES_H_

double VoV(double A[3], double B[3]);
void  VxV(double A[3], double B[3], double C[3]);
double MAGV(double v[3]);
void QxV(double QAB[4],double Vb[3],double Va[3]);
void QTxV(double QAB[4],double Va[3],double Vb[3]);
void UNITV(double V[3]);
double arccos(double x);
double Limit(double x,double min, double max);
void SxV(double S, double V[3], double W[3]);
double CopyUnitV(double V[3], double W[3]);
void Q2Rn(double QAB[4], double X[3], double Y[3], double Z[3]);
void RxVn(double X[3], double Y[3], double Z[3], double vec[3], double result[3]);
void RTxVn(double X[3], double Y[3], double Z[3], double vec[3], double result[3]);
void sumV(double X[3], double Y[3], double Z[3]);
double gps_mod1024_to_gmst(int weeks_mod1024, double seconds_into_week, double fraction, int current_epoch);
void computeRotationMatrix(double gmst, double rotationMatrix[3][3]);
void rotationMatrixToQuaternion(double R[3][3], double q[4]);
void multiplyQuaternions(double q1[4], double q2[4], double result[4]);
double computeRAAN(double gmstDegrees, double x, double y);
void latLonToEcef(double lat, double lon, double alt, double *x, double *y, double *z);
void normalize_quaternion(double q[4]);
void columntomatrix(double X[3], double Y[3], double Z[3], double R[3][3]);
void quaternion_conjugate(const double q[4], double result[4]);
void vector_subtract(const double v1[3], const double v2[3], double result[3]);
void vector_normalize(const double v[3], double result[3]);
void rotate_quaternion(double q_eci[4], double theta_x, double theta_y, double theta_z, double q_new[4]);
void rotateVector(double vector[3], double thetax, double thetay, double thetaz, double result[3]);
void estimatedPos(double dt, double xpos, double ypos, double zpos, double *xest, double *yest, double *zest);
void estimateNextQuaternion(const double q_prev[4], const double q_curr[4], double q_next[4]);
void multiply4x4Matrices(const double A[4][4], const double B[4][4], double out[4][4]);
bool invert4x4Matrix(const double m[4][4], double invOut[4][4]);
void estimateQuaternion(const double q_meas[4], double q_estimate[4]);
void assignQuaternion(const double source[4], double destination[4]);
#endif
