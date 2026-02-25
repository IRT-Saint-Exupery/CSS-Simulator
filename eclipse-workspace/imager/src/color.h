/*
 * color.h
 *
 *  Created on: 20 jan. 2022
 *      Author: girardja
 */

#ifndef COLOR_H_
#define COLOR_H_


#include <strings.h>
#include <stdio.h>

using namespace std;

class t_couleur
{
	char chaineCoul[20];
public:
	float r,g,b,a;
	t_couleur() : r(0),g(0),b(0),a(1) {};
	t_couleur(float _r,float _g, float _b) :r(_r),g(_g),b(_b),a(1) {};
	t_couleur(float _r,float _g, float _b, float _a) :r(_r),g(_g),b(_b),a(_a) {};
	t_couleur(const char*colstr);
	const char *toChar();
	/*=====================================================================*/
};

t_couleur operator*(const float n, const t_couleur &v1);
t_couleur operator*(const t_couleur &v1, const float n);
t_couleur operator/(const t_couleur &v1, const float n);
t_couleur operator+(const t_couleur &v1, const t_couleur &v2);
bool operator==(const t_couleur &v1, const t_couleur &v2);
bool operator!=(const t_couleur &v1, const t_couleur &v2);

#
#endif /*COLOR_H_ */
