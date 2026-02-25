/*
 * constants.h
 *
 *  Created on: 28 dec. 2017
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

#include "units.h"

#define RTMoyen (6371.009 km)	// Average Earth Radius m
#define RTEquateur (6378.137 km)	// Earth Radius at equator m
#define RTPolaire (6356.7523 km)	// Earth Radius at poles m

#define RT    (RTMoyen)	   //Average Earth Radius m
#define PT	86164.1006372  //Earth Rotation period in fixed repere (période sidérale)  // SH

#define RGEO (42163128.3351360708475112915039)   //Geostationnary orbit radius	// JG

#define ALTGEO (RGEO - RTEquateur)  // 35784.991335136063396931 km

#define SUN_DIST	(149597870.7 km)

#define CLIGHT 299792458.0	// Light speed

#define GM 398600.4418e9		// G * Masse terrestre  m3/s2
#define MU_EARTH 398600.4418	// km3 / s2

#define KDB -228.6

#define EARTH_INCLINATION	(-23.44 degres)

#define LIBSAT_EPOCH 1584792000		// t libsat = 0 le 21 mars 2020 à 12 h (mais finalement l'année ne compte pas)


