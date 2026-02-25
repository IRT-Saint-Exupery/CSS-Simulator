/*
 * stars.h
 *
 *  Created on: 1 juin 2023
 *      Author: girardja
 */

#ifndef STARS_H_
#define STARS_H_

#include <list>
#include <string>
#include "geometrie3d.h"

#define distanceEtoiles (20'000'000 km)
#define magMax 6
#define magMin -1.46
#define magDyn	0.8

using namespace std;

class t_stars {

public:
	static list<t_stars> listStars;
	static void initStars(string starFile);
	t_stars (float ascensionDroite, float declinaison, float magB, float magV, float magR);
	point3D position;
	float intensityB;
	float intensityV;
	float intensityR;
};



#endif /* STARS_H_ */
