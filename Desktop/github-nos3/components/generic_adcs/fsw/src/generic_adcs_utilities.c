/*******************************************************************************
** Purpose:
**   This file implements utility functions used by ADCS.
**
*******************************************************************************/

#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "generic_adcs_utilities.h"

/**********************************************************************/
/*  Vector Dot Product                                                */
double VoV(double A[3], double B[3])
{
      return(A[0]*B[0]+A[1]*B[1]+A[2]*B[2]);
}
/**********************************************************************/
/*  Vector Cross Product                                              */
void  VxV(double A[3], double B[3], double C[3])
{
      C[0]=A[1]*B[2]-A[2]*B[1];
      C[1]=A[2]*B[0]-A[0]*B[2];
      C[2]=A[0]*B[1]-A[1]*B[0];
}
// Magnitude of v vector
double MAGV(double v[3])
{
    return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

/**********************************************************************/
/* Find components of V in A, given components of V in B, and qab     */
void QxV(double QAB[4],double Vb[3],double Va[3])
{
      double qq[4][4];
      long i,j;

      for(i=0;i<4;i++) {
         for(j=i;j<4;j++) qq[i][j] = QAB[i]*QAB[j];
      }

      Va[0] = ( qq[0][0]-qq[1][1]-qq[2][2]+qq[3][3])*Vb[0]
                          + 2.0*((qq[0][1]+qq[2][3])*Vb[1]
                                +(qq[0][2]-qq[1][3])*Vb[2]);
      Va[1] = (-qq[0][0]+qq[1][1]-qq[2][2]+qq[3][3])*Vb[1]
                          + 2.0*((qq[1][2]+qq[0][3])*Vb[2]
                                +(qq[0][1]-qq[2][3])*Vb[0]);
      Va[2] = (-qq[0][0]-qq[1][1]+qq[2][2]+qq[3][3])*Vb[2]
                          + 2.0*((qq[0][2]+qq[1][3])*Vb[0]
                                +(qq[1][2]-qq[0][3])*Vb[1]);
}
/**********************************************************************/
/* Find components of V in B, given components of V in A, and qab     */
void QTxV(double QAB[4],double Va[3],double Vb[3])
{
      double qq[4][4];
      long i,j;

      for(i=0;i<4;i++) {
         for(j=i;j<4;j++) qq[i][j] = QAB[i]*QAB[j];
      }

      Vb[0] = ( qq[0][0]-qq[1][1]-qq[2][2]+qq[3][3])*Va[0]
                          + 2.0*((qq[0][1]-qq[2][3])*Va[1]
                                +(qq[0][2]+qq[1][3])*Va[2]);
      Vb[1] = (-qq[0][0]+qq[1][1]-qq[2][2]+qq[3][3])*Va[1]
                          + 2.0*((qq[1][2]-qq[0][3])*Va[2]
                                +(qq[0][1]+qq[2][3])*Va[0]);
      Vb[2] = (-qq[0][0]-qq[1][1]+qq[2][2]+qq[3][3])*Va[2]
                          + 2.0*((qq[0][2]-qq[1][3])*Va[0]
                                +(qq[1][2]+qq[0][3])*Va[1]);
}
/**********************************************************************/
/*  Normalize a 3-vector if it is non-zero.                           */
void UNITV(double V[3])
{
      double A;

      A=sqrt(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
      if (A > 0.0) {
         V[0]/=A;
         V[1]/=A;
         V[2]/=A;
      }
}

double arccos(double x)
{
   if (x > 1.0)    x = 1.0;
   if (x < -1.0)   x = -1.0;

   return (acos(x));
}
/**********************************************************************/
double Limit(double x,double min, double max)
{
      return(x < min ? min : (x > max ? max : x));
}
/**********************************************************************/
/*  Scalar times 3x1 Vector                                           */
void SxV(double S, double V[3], double W[3])
{
      W[0] = S*V[0];
      W[1] = S*V[1];
      W[2] = S*V[2];
}
/**********************************************************************/
/*  Copy and normalize a 3-vector.  Return its magnitude              */
double CopyUnitV(double V[3], double W[3])
{
      double A;

      A=sqrt(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
      if (A > 0.0) {
         W[0] = V[0]/A;
         W[1] = V[1]/A;
         W[2] = V[2]/A;
      }
      else {
         W[0] = 0.0;
         W[1] = 0.0;
         W[2] = 0.0;
      }
      return(A);
}
/*********************************************************************/

// Function to convert a (normalized) quaternion to a 3x3 rotation matrix represented by column vectors  // to be checked
void Q2Rn(double QAB[4], double X[3], double Y[3], double Z[3]) {
    // Extract the quaternion components
    double w = QAB[0];
    double x = QAB[1];
    double y = QAB[2];
    double z = QAB[3];
    double s = 1.0;
    
    /*if(w < 0) { // convention with positive quaternion ?
      w = -w;
      x = -x;
      y = -y;
      z = -z;
    }*/
    
    // Precompute repeated terms for efficiency
    double ww = w * w;
    double xx = x * x;
    double yy = y * y;
    double zz = z * z;

    double wx = w * x;
    double wy = w * y;
    double wz = w * z;

    double xy = x * y;
    double xz = x * z;
    double yz = y * z;

    // Compute the columns of the rotation matrix
    X[0] = s*(1 - 2 * (yy + zz));
    X[1] = s*(2 * (xy + wz));
    X[2] = s*(2 * (xz - wy));

    Y[0] = s*(2 * (xy - wz));
    Y[1] = s*(1 - 2 * (xx + zz));
    Y[2] = s*(2 * (yz + wx));

    Z[0] = s*(2 * (xz + wy));
    Z[1] = s*(2 * (yz - wx));
    Z[2] = s*(1 - 2 * (xx + yy));
    
    //UNITV(X);
    //UNITV(Y);
    //UNITV(Z);
   
}

/*******************************************************************/

// Function to multiply the column-based rotation matrix by a 3D vector
void RxVn(double X[3], double Y[3], double Z[3], double vec[3], double result[3]) {
    // Compute the resulting vector by treating the columns as basis vectors
    result[0] = X[0] * vec[0] + Y[0] * vec[1] + Z[0] * vec[2];
    result[1] = X[1] * vec[0] + Y[1] * vec[1] + Z[1] * vec[2];
    result[2] = X[2] * vec[0] + Y[2] * vec[1] + Z[2] * vec[2];
}

// Function to multiply the column-based rotation (transpose) matrix by a 3D vector
void RTxVn(double X[3], double Y[3], double Z[3], double vec[3], double result[3]) {
    // Compute the resulting vector by treating the columns as basis vectors
    result[0] = X[0] * vec[0] + X[1] * vec[1] + X[2] * vec[2];
    result[1] = Y[0] * vec[0] + Y[1] * vec[1] + Y[2] * vec[2];
    result[2] = Z[0] * vec[0] + Z[1] * vec[1] + Z[2] * vec[2];
}

/****************************************************************/

void sumV(double X[3], double Y[3], double Z[3]) {
     Z[0] = X[0]+Y[0];
     Z[1] = X[1]+Y[1];
     Z[2] = X[2]+Y[2];
}


double gps_mod1024_to_gmst(int weeks_mod1024, double seconds_into_week, double fraction, int current_epoch) { //TODO
    // Constants
    const double GPS_EPOCH_JD = 2444244.5;  // Julian Date for GPS epoch (1980-01-06)
    const double SECONDS_IN_DAY = 86400.0;
    const double SECONDS_IN_WEEK = 7 * SECONDS_IN_DAY;
    const int WEEKS_PER_EPOCH = 1024;

    // Calculate actual GPS week number
    int actual_weeks = weeks_mod1024 + (current_epoch - 1) * WEEKS_PER_EPOCH;

    // Calculate total elapsed seconds since GPS epoch
    double total_seconds = actual_weeks * SECONDS_IN_WEEK + seconds_into_week + fraction;
    
    // Convert GPS time to Julian Date
    double jd = GPS_EPOCH_JD + total_seconds / SECONDS_IN_DAY;

    // Calculate days since J2000.0 epoch
    const double JD_J2000 = 2451545.0;  // Julian Date for J2000.0 epoch (2000-01-01 12:00 TT)
    double days_since_j2000 = jd - (JD_J2000 + 0.5);

    // Calculate GMST in seconds
    double T = days_since_j2000 / 36525.0;  // Julian centuries since J2000.0
    double gmst_seconds = 24110.54841 +
                          8640184.812866 * T +
                          0.093104 * T * T -
                          6.2e-6 * T * T * T +
                          (total_seconds - ((long long) (total_seconds / SECONDS_IN_DAY) * SECONDS_IN_DAY)) * 1.00273790935;

    // Convert GMST to degrees and normalize to [0, 360)
    double gmst_degrees = fmod(gmst_seconds / 240.0, 360.0);
    if (gmst_degrees < 0) {
        gmst_degrees += 360.0;
    }
        
    return gmst_degrees;
}


void computeRotationMatrix(double gmst, double rotationMatrix[3][3]) {
    
    gmst = gmst * (M_PI / 180.0);// convert to rad
    
    // Compute cosine and sine of GMST
    double cosTheta = cos(gmst);
    double sinTheta = sin(gmst);

    // Define the rotation matrix for a rotation about the Z-axis
    rotationMatrix[0][0] = cosTheta;  // R[0][0]
    rotationMatrix[0][1] = -sinTheta; // R[0][1]
    rotationMatrix[0][2] = 0;         // R[0][2]

    rotationMatrix[1][0] = sinTheta;  // R[1][0]
    rotationMatrix[1][1] = cosTheta;  // R[1][1]
    rotationMatrix[1][2] = 0;         // R[1][2]

    rotationMatrix[2][0] = 0;         // R[2][0]
    rotationMatrix[2][1] = 0;         // R[2][1]
    rotationMatrix[2][2] = 1;         // R[2][2]
}

void rotationMatrixToQuaternion(double R[3][3], double q[4]) {
    double trace = R[0][0] + R[1][1] + R[2][2];

    if (trace > 0) {
        double s = 2 * sqrt(trace + 1.0);
        q[0] = 0.25 * s; // w
        q[1] = (R[2][1] - R[1][2]) / s; // x
        q[2] = (R[0][2] - R[2][0]) / s; // y
        q[3] = (R[1][0] - R[0][1]) / s; // z
    } else {
        if (R[0][0] > R[1][1] && R[0][0] > R[2][2]) {
            double s = 2.0 * sqrt(1.0 + R[0][0] - R[1][1] - R[2][2]);
            q[0] = (R[2][1] - R[1][2]) / s; // w
            q[1] = 0.25 * s; // x
            q[2] = (R[0][1] + R[1][0]) / s; // y
            q[3] = (R[0][2] + R[2][0]) / s; // z
        } else if (R[1][1] > R[2][2]) {
            double s = 2.0 * sqrt(1.0 + R[1][1] - R[0][0] - R[2][2]);
            q[0] = (R[0][2] - R[2][0]) / s; // w
            q[1] = (R[0][1] + R[1][0]) / s; // x
            q[2] = 0.25 * s; // y
            q[3] = (R[1][2] + R[2][1]) / s; // z
        } else {
            double s = 2.0 * sqrt(1.0 + R[2][2] - R[0][0] - R[1][1]);
            q[0] = (R[1][0] - R[0][1]) / s; // w
            q[1] = (R[0][2] + R[2][0]) / s; // x
            q[2] = (R[1][2] + R[2][1]) / s; // y
            q[3] = 0.25 * s; // z
        }
    }
}
  
void multiplyQuaternions(double q1[4], double q2[4], double result[4]) {
    // Extract components from the first quaternion
    double w1 = q1[0], x1 = q1[1], y1 = q1[2], z1 = q1[3];
    
    // Extract components from the second quaternion
    double w2 = q2[0], x2 = q2[1], y2 = q2[2], z2 = q2[3];

    // Compute the product
    result[0] = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2; // w
    result[1] = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2; // x
    result[2] = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2; // y
    result[3] = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2; // z
}  


// Function to compute RAAN from GMST and ECEF position
double computeRAAN(double gmstDegrees, double x, double y) {
    // Convert GMST from degrees to radians
    double gmstRad = gmstDegrees * (M_PI / 180.0);

    // Calculate longitude from ECEF position
    double lambda = atan2(y, x); // Longitude in radians

    // Compute RAAN
    double raan = lambda - gmstRad;

    // Normalize RAAN to the range [0, 2π]
    raan = fmod(raan + 2 * M_PI, 2 * M_PI);
    
    return raan;
} 

void latLonToEcef(double lat, double lon, double alt, double *x, double *y, double *z) {
    // Convert degrees to radians
    double latRad = lat * M_PI / 180.0;
    double lonRad = lon * M_PI / 180.0;
    
    double WGS84_A =  6378137.0; // Equatorial radius in meters
    double WGS84_E2 = 0.00669437999014; // Square of first eccentricity

    // Calculate N (the radius of curvature in the prime vertical)
    double N = WGS84_A / sqrt(1 - WGS84_E2 * sin(latRad) * sin(latRad));

    // Calculate ECEF coordinates
    *x = (N + alt) * cos(latRad) * cos(lonRad);
    *y = (N + alt) * cos(latRad) * sin(lonRad);
    *z = (N * (1 - WGS84_E2) + alt) * sin(latRad);
}

// Function to normalize a quaternion represented as a 4-element array
void normalize_quaternion(double q[4]) {

    // Calculate the magnitude of the quaternion
    double magnitude = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);

    // Avoid division by zero
    if (magnitude > 1e-14) {
        q[0] /= magnitude;
        q[1] /= magnitude;
        q[2] /= magnitude;
        q[3] /= magnitude;
    } else {
        // If the magnitude is zero, default to a valid quaternion
        q[0] = 1.0;
        q[1] = 0.0;
        q[2] = 0.0;
        q[3] = 0.0;
    }
}

void columntomatrix(double X[3], double Y[3], double Z[3], double R[3][3]) {

	R[0][0] =  X[0];
	R[0][1] =  Y[0];
	R[0][2] =  Z[0];
	
	R[1][0] =  X[1];
	R[1][1] =  Y[1];
	R[1][2] =  Z[1];
	
	R[2][0] =  X[2];
	R[2][1] =  Y[2];
	R[2][2] =  Z[2];	
}
		

// Quaternion conjugate
void quaternion_conjugate(const double q[4], double result[4]) {
    result[0] = q[0];
    result[1] = -q[1];
    result[2] = -q[2];
    result[3] = -q[3];
}


// Vector subtraction
void vector_subtract(const double v1[3], const double v2[3], double result[3]) {
    result[0] = v1[0] - v2[0];
    result[1] = v1[1] - v2[1];
    result[2] = v1[2] - v2[2];
}

// Vector normalization
void vector_normalize(const double v[3], double result[3]) {
    double magnitude = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    result[0] = v[0] / magnitude;
    result[1] = v[1] / magnitude;
    result[2] = v[2] / magnitude;
}


// Function to rotate a quaternion by arbitrary angles (theta_x, theta_y, theta_z) in deg 
void rotate_quaternion(double q_eci[4], double theta_x, double theta_y, double theta_z, double q_new[4]) { //TODO
    
    theta_x = theta_x * M_PI /180.0;
    theta_y = theta_y * M_PI /180.0;
    theta_z = theta_z * M_PI /180.0;
    
    // Step 1: Create the rotation quaternion for the X axis (rotation around X by theta_x)
    double q_rot_x[4] = {cos(theta_x / 2.0), sin(theta_x / 2.0), 0.0, 0.0};
    normalize_quaternion(q_rot_x);

    // Step 2: Create the rotation quaternion for the Y axis (rotation around Y by theta_y)
    double q_rot_y[4] = {cos(theta_y / 2.0), 0.0, sin(theta_y / 2.0), 0.0};
    normalize_quaternion(q_rot_y);

    // Step 3: Create the rotation quaternion for the Z axis (rotation around Z by theta_z)
    double q_rot_z[4] = {cos(theta_z / 2.0), 0.0, 0.0, sin(theta_z / 2.0)};
    normalize_quaternion(q_rot_z);

    // Step 4: Combine the individual rotation quaternions (order matters: X, Y, Z)
    double q_rot_ex[4], q_rot_ey[4];
    normalize_quaternion(q_eci);
    multiplyQuaternions(q_rot_x, q_eci, q_rot_ex); 
    normalize_quaternion(q_rot_ex);
    multiplyQuaternions(q_rot_y, q_rot_ex, q_rot_ey);
    normalize_quaternion(q_rot_ey); 
    multiplyQuaternions(q_rot_z, q_rot_ey, q_new); 
    normalize_quaternion(q_new);

}

// Function to rotate a vector by given angles around the x, y, and z axes
void rotateVector(double vector[3], double thetax, double thetay, double thetaz, double result[3]) { //TODO
    // Convert angles from degrees to radians
    double rx = thetax * (M_PI / 180.0);
    double ry = thetay * (M_PI / 180.0);
    double rz = thetaz * (M_PI / 180.0);

    // Rotation matrix components for each axis
    double cosRx = cos(rx), sinRx = sin(rx);
    double cosRy = cos(ry), sinRy = sin(ry);
    double cosRz = cos(rz), sinRz = sin(rz);

    // Rotation matrix around the X-axis
    double R_x[3][3] = {
        {1, 0, 0},
        {0, cosRx, -sinRx},
        {0, sinRx, cosRx}
    };

    // Rotation matrix around the Y-axis
    double R_y[3][3] = {
        {cosRy, 0, sinRy},
        {0, 1, 0},
        {-sinRy, 0, cosRy}
    };

    // Rotation matrix around the Z-axis
    double R_z[3][3] = {
        {cosRz, -sinRz, 0},
        {sinRz, cosRz, 0},
        {0, 0, 1}
    };

    // Multiply R_z * R_y * R_x
    // First, apply rotation around the X-axis
    double tempX[3];
    for (int i = 0; i < 3; i++) {
        tempX[i] = 0;
        for (int j = 0; j < 3; j++) {
            tempX[i] += R_x[i][j] * vector[j];
        }
    }

    // Then, apply rotation around the Y-axis
    double tempY[3];
    for (int i = 0; i < 3; i++) {
        tempY[i] = 0;
        for (int j = 0; j < 3; j++) {
            tempY[i] += R_y[i][j] * tempX[j];
        }
    }

    // Finally, apply rotation around the Z-axis
    for (int i = 0; i < 3; i++) {
        result[i] = 0;
        for (int j = 0; j < 3; j++) {
            result[i] += R_z[i][j] * tempY[j];
        }
    }
}


void kalman_update(double dt, double meas, int valid,
                            double *pos, double *vel, double P[2][2]) {
                            
	                        // Tuning constants (adjust as needed)
        double R = 1e-2;       // Measurement noise //TODO
        double Q_pos = 1e-4;   // Process noise for position //TODO
        double Q_vel = 1e-4;   // Process noise for velocity //TODO
        
        // --- Prediction step ---
        double new_pos = (*pos) + (*vel)*dt;
        double new_vel = (*vel);

        // Predict covariance: P = F*P*F^T + Q
        double F00=1, F01=dt, F10=0, F11=1;
        double P00 = F00*P[0][0] + F01*P[1][0];
        double P01 = F00*P[0][1] + F01*P[1][1];
        double P10 = F10*P[0][0] + F11*P[1][0];
        double P11 = F10*P[0][1] + F11*P[1][1];

        double P00n = P00*F00 + P01*F01 + Q_pos;
        double P01n = P00*F10 + P01*F11;
        double P10n = P10*F00 + P11*F01;
        double P11n = P10*F10 + P11*F11 + Q_vel;

        P[0][0]=P00n; P[0][1]=P01n;
        P[1][0]=P10n; P[1][1]=P11n;

        (*pos)=new_pos;
        (*vel)=new_vel;

        // --- Update step if measurement valid ---
        if (valid) {
            double z_pred = *pos;
            double y = meas - z_pred;   // innovation

            // Innovation covariance S
            double S = P[0][0] + R;

            // Kalman gains
            double K0 = P[0][0]/S;
            double K1 = P[1][0]/S;

            // Update state
            *pos += K0 * y;
            *vel += K1 * y;

            // Update covariance
            double P00u = (1 - K0)*P[0][0];
            double P01u = (1 - K0)*P[0][1];
            double P10u = P[1][0] - K1*P[0][0];
            double P11u = P[1][1] - K1*P[0][1];

            P[0][0]=P00u; P[0][1]=P01u;
            P[1][0]=P10u; P[1][1]=P11u;
        }
    }
    
void estimatedPos(double dt,
                  double xpos, double ypos, double zpos,
                  double *xest, double *yest, double *zest) {
    // Static Kalman states (x, y, z independent filters)
    static int initialized = 0;

    // State variables: position and velocity for each axis
    static double pos_x, vel_x, pos_y, vel_y, pos_z, vel_z;

    // Covariance matrices
    static double P_x[2][2], P_y[2][2], P_z[2][2];

    // Tuning constants (adjust as needed)
    double R = 1e-2;       // Measurement noise //TODO
    double Q_pos = 1e-4;   // Process noise for position //TODO
    double Q_vel = 1e-4;   // Process noise for velocity //TODO

    if (!initialized) {
        // Initialize states with first measurement
        pos_x = xpos; vel_x = 0;
        pos_y = ypos; vel_y = 0;
        pos_z = zpos; vel_z = 0;

        // Initialize covariance matrices
        P_x[0][0]=1; P_x[0][1]=0; P_x[1][0]=0; P_x[1][1]=1;
        P_y[0][0]=1; P_y[0][1]=0; P_y[1][0]=0; P_y[1][1]=1;
        P_z[0][0]=1; P_z[0][1]=0; P_z[1][0]=0; P_z[1][1]=1;

        initialized = 1;
    }

    // Determine validity of each input (not NaN)
    int valid_x = !isnan(xpos);
    int valid_y = !isnan(ypos);
    int valid_z = !isnan(zpos);

    // Run Kalman updates per axis
    kalman_update(dt, xpos, valid_x, &pos_x, &vel_x, P_x);
    kalman_update(dt, ypos, valid_y, &pos_y, &vel_y, P_y);
    kalman_update(dt, zpos, valid_z, &pos_z, &vel_z, P_z);

    // Return estimated positions
    *xest = pos_x;
    *yest = pos_y;
    *zest = pos_z;
}


// Estimate next quaternion assuming constant relative rotation
void estimateNextQuaternion(const double q_prev[4], const double q_curr[4], double q_next[4]) {
    double q_prev_conj[4];
    double q_delta[4];

    // Compute conjugate of previous quaternion
    quaternion_conjugate(q_prev, q_prev_conj);

    // Compute relative rotation: q_delta = q_curr * conj(q_prev)
    multiplyQuaternions(q_curr, q_prev_conj, q_delta);

    // Predict next quaternion: q_next = q_delta * q_curr
    multiplyQuaternions(q_delta, q_curr, q_next);

    // Normalize result
    normalize_quaternion(q_next);
}

// Simple helper to multiply two 4x4 matrices: out = A*B
void multiply4x4Matrices(const double A[4][4], const double B[4][4], double out[4][4]) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            out[i][j] = 0.0;
            for (int k=0; k<4; k++) {
                out[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Helper to invert a 4x4 matrix - here use a naive method for symmetric positive definite matrix for simplicity (implement as needed)
bool invert4x4Matrix(const double m[4][4], double invOut[4][4]) {
    // For brevity, assume diagonal dominant and invert approximately
    // This is a placeholder. Use a robust library or implement full inversion.
    for (int i=0;i<4;i++) {
        for(int j=0; j<4; j++) {
            invOut[i][j] = 0.0;
        }
        invOut[i][i] = 1.0 / m[i][i];
    }
    return true;
}

// This is the Kalman filter update function using internal static state
void estimateQuaternion(const double q_meas[4], double q_estimate[4]) {
    // Static variables maintain filter state between calls
    static double q_prev[4] = {1, 0, 0, 0};
    static double P[4][4] = {
        {1e-3, 0, 0, 0},
        {0, 1e-3, 0, 0},
        {0, 0, 1e-3, 0},
        {0, 0, 0, 1e-3}
    };
    const double Q[4][4] = {
        {1e-5, 0, 0, 0},
        {0, 1e-5, 0, 0},
        {0, 0, 1e-5, 0},
        {0, 0, 0, 1e-5}
    };
    const double R[4][4] = {
        {1e-2, 0, 0, 0},
        {0, 1e-2, 0, 0},
        {0, 0, 1e-2, 0},
        {0, 0, 0, 1e-2}
    };

    double q_pred[4], q_prev_conj[4], q_delta[4];

    // Prediction step: q_pred = q_delta * q_prev
    q_prev_conj[0] = q_prev[0];
    q_prev_conj[1] = -q_prev[1];
    q_prev_conj[2] = -q_prev[2];
    q_prev_conj[3] = -q_prev[3];

    multiplyQuaternions(q_meas, q_prev_conj, q_delta);
    multiplyQuaternions(q_delta, q_prev, q_pred);
    normalize_quaternion(q_pred);

    // Covariance prediction
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            P[i][j] += Q[i][j];
        }
    }

    // Innovation covariance S = P + R
    double S[4][4];
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            S[i][j] = P[i][j] + R[i][j];
        }
    }

    double S_inv[4][4];
    invert4x4Matrix((const double (*)[4]) S, S_inv);

    // Calculate Kalman gain K = P * inv(S)
    double K[4][4];
    multiply4x4Matrices((const double (*)[4]) P, (const double (*)[4]) S_inv, K);

    // Innovation y = measurement - prediction
    double y[4];
    for (int i=0; i<4; i++) y[i] = q_meas[i] - q_pred[i];

    // Update state: q_next = q_pred + K * y
    double q_next[4];
    for (int i=0; i<4; i++) {
        q_next[i] = q_pred[i];
        for (int j=0; j<4; j++) {
            q_next[i] += K[i][j] * y[j];
        }
    }
    normalize_quaternion(q_next);

    // Update covariance: P = (I - K) * P
    double I_K[4][4];
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            I_K[i][j] = (i == j ? 1.0 : 0.0) - K[i][j];
        }
    }
    double newP[4][4];
    multiply4x4Matrices((const double (*)[4]) I_K, (const double (*)[4]) P, newP);

    memcpy(P, newP, sizeof(newP));
    memcpy(q_prev, q_next, sizeof(q_next));
    memcpy(q_estimate, q_next, sizeof(q_next));
}

void assignQuaternion(const double source[4], double destination[4]) {
    for (int i = 0; i < 4; i++) {
        destination[i] = source[i];
    }
}

