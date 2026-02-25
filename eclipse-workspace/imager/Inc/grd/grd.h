/*
 * grd.h
 *
 *  Created on: 3 juil. 2018
 *      Author: Jacques Girard (jb.girard@free.fr)
 */
 // Coordonnées dans le repère orientation de l'antenne
 // Dans la convention du service antennes de Toulouse, les U sont vers le X antenne et les v sont vers le -Y antenne
 //    NB Pour la libsat, une antenne est initialement montée (avant éventuellee tilts) sur le satellite avec X antenne = X satellite
 //    et Y antenne = Y satellite
 //    Si un grd antenne est donné avec un U (X satellite) vers le nord alors, pour un satellite GEOSTATIONNAIRE
 //	   dont le X est vers l'Est il faudra tilter l'antenne autour de l'axe Z du satellite de -90°
 //
 // Le constructeur permet de s'adapter à toute autre convention de grd grace aux booléens optionnels flipU, flipV
 // Ainsi entre les tilts antennes et les flips possibles des u et/ou des v toutes les situations sont gérées.


#ifndef GRD_GRD_H_
#define GRD_GRD_H_

#define NB_MAX_U 1024 // max u steps
#define NB_MAX_V 1024 // max v steps

#define FIRST_GRID_PRECISION 128
#define SECOND_GRID_PRECISION 512

#include "constants.h"
#include <list>
#include <grd/ListContour.h>
#include "geometrie3d.h"

using namespace std;

typedef list<vecteur3D> listVecteur3D;
typedef list<listVecteur3D> listStrip ;
typedef list<listStrip> listCont ;

class t_grd
{
public:
	t_grd (const char *filename, bool flipU = false, bool flipV = false, int firstGridPrecision = FIRST_GRID_PRECISION, int secondGridPrecision = SECOND_GRID_PRECISION);
	double valueGainDB (double u, double v);
	double maxGain;
	double minGain;
	double uMin, vMin, uMax, vMax;
	listCont buildContours (float tabIso[], int nbIso);

private:
	int firstGridPrecision, secondGridPrecision;
	int nbPtsU, nbPtsV;
	float pasU,pasV;
	float unSurPasU, unSurPasV;
	double gridValueDB[NB_MAX_U][NB_MAX_V];
};

#endif /* GRD_GRD_H_ */
