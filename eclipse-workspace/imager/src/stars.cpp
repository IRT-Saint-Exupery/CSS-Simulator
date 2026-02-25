/*
 * stars.cpp
 *
 *  Created on: 1 juin 2023
 *      Author: girardja
 */

#include "stars.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "matcher.h"
#include "units.h"

using namespace std;

list<t_stars> t_stars::listStars;

float calculerIntensite(float magnitude)
{
	if (magnitude < magMin) {
		magnitude = magMin;
	}
	else if (magnitude > magMax) {
		magnitude = magMax;
	}
    return  1.0 - magDyn * (magnitude - magMin) / (magMax - magMin);
}

t_stars::t_stars (float ascensionDroite, float declinaison, float magB, float magV, float magR)
{
	position.x =  distanceEtoiles * cos (declinaison) * sin (ascensionDroite);
	position.y = -distanceEtoiles * cos (declinaison) * cos (ascensionDroite);
	position.z =  distanceEtoiles * sin (declinaison);

	intensityB = calculerIntensite (magB);
	intensityV = calculerIntensite (magV);
	intensityR = calculerIntensite (magR);
}

void t_stars::initStars (string starFile)
{
	ifstream ficStars;
	ficStars.open(starFile);
	if (ficStars.good()) {
		char line[512];
		char bidon[512];
		float magB;
		float magV;
		float magR;
		float ascension;
		float declinaison;
		while (!ficStars.eof()) {
			ficStars.getline(line, sizeof(line));
			if (MatchScan (line,"%f;%f;%f;%f;%f%s", &ascension, &declinaison, &magB, &magV ,&magR, bidon )) {
					listStars.emplace_back (ascension, declinaison, magB, magV, magR);
			}
		}
		ficStars.close();
	}
	else {
		cerr << "Erreur ouverture " << starFile << endl;
	}
}

