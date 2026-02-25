/*
 * unit.h
 *
 *  Created on: 15 dec. 2017
 *      Author: Jacques Girard (jb.girard@free.fr)
 */
#ifndef UNIT_INCLUDED
#define UNIT_INCLUDED

#include <cmath>

#define degres *(M_PI/180)
#define en_degres *(180/M_PI)

#define megaHz *1.0e6
#define MHz *1.0e6
#define en_megaHz /1.0e6
#define en_MHz /1.0e6

#define gigaHz *1.0e9
#define GHz *1.0e9
#define en_gigaHz /1.0e9
#define en_GHz /1.0e9

#define km *1000.0
#define en_km /1000.0

#define jour *86400.0
#define jours *86400.0
#define days *86400.0
#define heure *3600.0
#define heures *3600.0
#define hour *3600.0
#define hours *3600.0
#define en_heure /3600.0
#define en_heures /3600.0
#define en_jour /86400.0
#define en_jours /86400.0

#define minute *60.0
#define minutes *60.0
#define en_minute /60.0
#define en_minutes /60.0

#define seconde
#define secondes
#define en_seconde
#define en_secondes

#define dB
#define dBi

#define metre
#define metres
#define en_metre
#define en_metres

#define ms	*0.001
#define en_ms *1000.0

#define par_seconde

#define par_heure en_heure		// 24 km par_heure ==> m/s
#define par_jour en_jour
#define per_hour en_heure
#define per_day en_jour

#define NATURAL *1

#endif
