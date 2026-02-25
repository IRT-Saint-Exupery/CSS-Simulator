#include <stdio.h>
#include <math.h>
#include "cfe.h"
#include "isl_app.h"
#include "novatel_oem615_msg.h" 

// WGS84 ellipsoid constants
#define WGS84_A 6378137.0           // Semi-major axis in meters
#define WGS84_F (1.0/298.257223563) // Flattening
#define DEG2RAD (M_PI/180.0)
    // Constants
#define MU 3.986004418e14      // Earth gravitational parameter (m^3/s^2)
#define OMEGA_EARTH 7.292115e-5 // Earth's angular velocity (rad/s)


// Convert geodetic lat/lon (degrees) and height (m) to ECEF X,Y,Z (meters)
void geodetic_to_ecef(double lat_deg, double lon_deg, double h,
                      double *x, double *y, double *z) {
    double lat = lat_deg * DEG2RAD;
    double lon = lon_deg * DEG2RAD;
    double a = WGS84_A;
    double f = WGS84_F;
    double e2 = f * (2 - f);
    double N = a / sqrt(1 - e2 * sin(lat) * sin(lat));
    *x = (N + h) * cos(lat) * cos(lon);
    *y = (N + h) * cos(lat) * sin(lon);
    *z = (N * (1 - e2) + h) * sin(lat);
}

// Computes elevation angle in degrees
double compute_elevation(double gs_lat_deg, double gs_lon_deg,
                         double gs_h_m, // ground station height (m), usually 0
                         double sat_x, double sat_y, double sat_z)
{
    // Ground station ECEF
    double gs_x, gs_y, gs_z;
    geodetic_to_ecef(gs_lat_deg, gs_lon_deg, gs_h_m, &gs_x, &gs_y, &gs_z);

    // Vector from ground station to satellite
    double dx = sat_x - gs_x;
    double dy = sat_y - gs_y;
    double dz = sat_z - gs_z;

    // Convert ground station latitude and longitude to radians
    double lat = gs_lat_deg * DEG2RAD;
    double lon = gs_lon_deg * DEG2RAD;

    // Ground station local ENU basis vectors
    double e_x = -sin(lon);
    double e_y = cos(lon);
    double e_z = 0;

    double n_x = -sin(lat)*cos(lon);
    double n_y = -sin(lat)*sin(lon);
    double n_z = cos(lat);

    double u_x = cos(lat)*cos(lon);
    double u_y = cos(lat)*sin(lon);
    double u_z = sin(lat);

    // Compute ENU coordinates of (sat - gs)
    double east  = e_x*dx + e_y*dy + e_z*dz;
    double north = n_x*dx + n_y*dy + n_z*dz;
    double up    = u_x*dx + u_y*dy + u_z*dz;

    // Elevation angle
    double slant_range = sqrt(dx*dx + dy*dy + dz*dz);
    double el_rad = asin(up / slant_range);
    return el_rad * (180.0/M_PI); // degrees
}

// Determines if satellite is visible from ground station
int is_satellite_visible(double gs_lat_deg, double gs_lon_deg,
                        double min_elev_deg,
                        double sat_x, double sat_y, double sat_z) {
    // Assume ground station height = 0 m (sea level)
    double elev = compute_elevation(gs_lat_deg, gs_lon_deg, 0,
                                    sat_x, sat_y, sat_z);
    return (elev >= min_elev_deg);
}


void Generic_ISL_ingest_generic_Gps(CFE_MSG_Message_t * Msg, Generic_Gps_Tlm_Payload_t *Gps) 
{
   NOVATEL_OEM615_Device_tlm_t *gps = (NOVATEL_OEM615_Device_tlm_t *)Msg;
   Gps->Weeks = gps->Novatel_oem615.Weeks;
   Gps->SecondsIntoWeek = gps->Novatel_oem615.SecondsIntoWeek;
   Gps->Fractions = gps->Novatel_oem615.Fractions;
   Gps->ECEFX = gps->Novatel_oem615.ECEFX;
   Gps->ECEFY = gps->Novatel_oem615.ECEFY;
   Gps->ECEFZ = gps->Novatel_oem615.ECEFZ;
   Gps->VelX = gps->Novatel_oem615.VelX;
   Gps->VelY = gps->Novatel_oem615.VelY;
   Gps->VelZ = gps->Novatel_oem615.VelZ;
}

int parseGroundStations(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        // Cannot use std::cerr or iostream, so use stderr directly
        fprintf(stderr, "Error: file cannot be opened %s\n", filename);
        return -1;
    }

#define MAX_LINE 512
    char line[MAX_LINE];
    bool inGroundStationSection = false;

    LatLong* stations = NULL;
    ISL_AppData.nb_stations = 0;

    while (fgets(line, sizeof(line), file)) {
        // Detect the "Ground Stations" section
        if (strstr(line, "Ground Stations")) {
            inGroundStationSection = true;
            continue;
        }

        // Stop when leaving section
        if (inGroundStationSection && strstr(line, "****************"))
            break;

        if (inGroundStationSection) {
            // Ignore empty lines or comments
            if (line[0] == '\0' || line[0] == '*' || line[0] == '!')
                continue;

            char existsStr[16], world[32];
            double lon = 0.0, lat = 0.0;

            int result = sscanf(line, "%15s %31s %lf %lf", existsStr, world, &lon, &lat);
            if (result < 4)
                continue; // skip invalid lines

            bool exists = (strcmp(existsStr, "TRUE") == 0 || strcmp(existsStr, "True") == 0);

            // Search for quoted name
            const char* q1 = strchr(line, '"');
            const char* q2 = q1 ? strrchr(q1 + 1, '"') : NULL;

            char label[128] = "Unknown";
            if (q1 && q2 && q2 > q1) {
                size_t len = q2 - q1 - 1;
                strncpy(label, q1 + 1, len);
                label[len] = '\0';
            }

            ISL_AppData.stations[ ISL_AppData.nb_stations].latitude = lat;
            ISL_AppData.stations[ ISL_AppData.nb_stations].longitude = lon;
            ISL_AppData.stations[ ISL_AppData.nb_stations].exists = exists;
            ISL_AppData.stations[ ISL_AppData.nb_stations].name = strdup(label);
            ISL_AppData.nb_stations++;
            
            if (ISL_AppData.nb_stations > MAX_STATIONS) {
            	printf("Warning: only %d Stations are considered! \n", MAX_STATIONS);
            	break;
            }
        }
    }

    fclose(file);
    return 0;
}

// test visibility for all ground stations defined in Inp_Sim.txt

int is_satellite_visible_allMCS(double min_elev_deg, double sat_x, double sat_y, double sat_z,double sat_nx, double sat_ny, double sat_nz) { // nx is forecasted position

	int globalVisi = 0;

        if (!ISL_AppData.stations || ISL_AppData.nb_stations == 0) {
		printf("No ground stations found.\n");
		return 1;
        }	
		
	for (int i = 0; i < ISL_AppData.nb_stations; ++i) {
		int currVisi = 0;
		int nextVisi = 0;
		if(ISL_AppData.stations[i].exists == true) {
			currVisi = is_satellite_visible(ISL_AppData.stations[i].latitude, ISL_AppData.stations[i].longitude, MIN_ELEV_DEG, sat_x, sat_y, sat_z);
			nextVisi = is_satellite_visible(ISL_AppData.stations[i].latitude, ISL_AppData.stations[i].longitude, MIN_ELEV_DEG, sat_nx, sat_ny, sat_nz);
		}		
		if(currVisi == 1 && nextVisi == 1) {
			globalVisi = 1;
			break;
		}					
	}
	   		
	return globalVisi;
}

int is_satellite_visible_now_allMCS(double min_elev_deg, double sat_x, double sat_y, double sat_z) {

	int globalVisi = 0;

        if (!ISL_AppData.stations || ISL_AppData.nb_stations == 0) {
		printf("No ground stations found.\n");
		return 1;
        }	
		
	for (int i = 0; i < ISL_AppData.nb_stations; ++i) {
		int currVisi = 0;
		if(ISL_AppData.stations[i].exists == true) {
			currVisi = is_satellite_visible(ISL_AppData.stations[i].latitude, ISL_AppData.stations[i].longitude, MIN_ELEV_DEG, sat_x, sat_y, sat_z);
		}		
		if(currVisi == 1) {
			globalVisi = 1;
			break;
		}					
	}
	   		
	return globalVisi;
}


// Compute acceleration in ECEF frame including gravity, Coriolis, centripetal terms
void compute_acceleration(double rx, double ry, double rz,
                          double vx, double vy, double vz,
                          double *ax, double *ay, double *az) {
    double r = sqrt(rx*rx + ry*ry + rz*rz);

    // Gravitational acceleration
    double agx = -MU * rx / (r*r*r);
    double agy = -MU * ry / (r*r*r);
    double agz = -MU * rz / (r*r*r);

    // Coriolis acceleration: -2 omega x v
    double acx =  2 * OMEGA_EARTH * vz;
    double acy = -2 * OMEGA_EARTH * vx;
    double acz = 0;

    // Centripetal acceleration: - omega x (omega x r)
    double accx = OMEGA_EARTH * OMEGA_EARTH * rx;
    double accy = OMEGA_EARTH * OMEGA_EARTH * ry;
    double accz = 0;

    *ax = agx + acx + accx;
    *ay = agy + acy + accy;
    *az = agz + acz + accz;
}

/**
 * Propagate satellite position in ECEF over time T using Euler integration.
 * Velocity is estimated from difference between current and previous position.
 *
 * @param x_prev, y_prev, z_prev  Previous satellite position in ECEF (m)
 * @param x_curr, y_curr, z_curr  Current satellite position in ECEF (m)
 * @param T                       Propagation time in seconds
 * @param x, y, z                 Output propagated satellite position in ECEF (m)
 */
void satellite_propagation_ecef(double x_prev, double y_prev, double z_prev,
                                double x_curr, double y_curr, double z_curr,
                                double *x, double *y, double *z) {
    
    double T = 60; //(s)
    double dt = 1.0;  // Integration timestep (s)
    int n = (int)(T / dt);

    // Estimate initial velocity by finite difference over last known step
    double vx0 = (x_curr - x_prev) / dt;
    double vy0 = (y_curr - y_prev) / dt;
    double vz0 = (z_curr - z_prev) / dt;

    // Initialize current position and velocity
    double rx = x_curr, ry = y_curr, rz = z_curr;
    double vx = vx0, vy = vy0, vz = vz0;

    for (int i = 0; i < n; i++) {
        double ax, ay, az;
        compute_acceleration(rx, ry, rz, vx, vy, vz, &ax, &ay, &az);

        // Euler integration step
        vx += ax * dt;
        vy += ay * dt;
        vz += az * dt;

        rx += vx * dt;
        ry += vy * dt;
        rz += vz * dt;
    }

    *x = rx;
    *y = ry;
    *z = rz;
}


