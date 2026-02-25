/*
 * Geometry.h
 *
 *
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

#ifndef geometrie3D_included
#define geometrie3D_included
#include <iostream>
#include <iomanip>

class point3D;
class vecteur3D;
class matrice3D;

class point3D {
public:
	double x,y,z;
	point3D(){}
	point3D (const vecteur3D &p);
	point3D (double xi,double yi,double zi):x(xi),y(yi),z(zi){}
	point3D &operator=(const vecteur3D &p);  // permet la conversion implicite des vecteurs en points
	bool pointDansGellule (const point3D &p1, const point3D &p2, const double r);
	bool operator==(const point3D &p);
	bool operator!=(const point3D &p);
};

class vecteur3D {
	public:
	double x,y,z;
	vecteur3D(){}
	vecteur3D (double xi,double yi,double zi):x(xi),y(yi),z(zi){}
	vecteur3D (const point3D &p);
	vecteur3D (const point3D &p1, const point3D &p2);
	vecteur3D (const vecteur3D &v);
	vecteur3D &operator=(const point3D &p); // permet la conversion implicite des points en vecteurs
	bool operator==(const vecteur3D &v);
	bool operator!=(const vecteur3D &v);
	double norme();
	double normeCarree(); // permet d'éviter les racines carrées dans certaines comparaisons.
	double angle(const vecteur3D &v);
										// Accélérateurs pour le calcul de l'angle
	double angleN_(const vecteur3D &v);	// Seulement si this est normé
	double angle_N(const vecteur3D &v);	// seulement si le paramètre est normé
	double angleNN(const vecteur3D &v);	// Seulement si this et le paramètres sont normés
	void normer(double l=1.0f);
	vecteur3D vecteurPerpendiculaire();
	vecteur3D vecteurUnitaire(const vecteur3D &v1, double l = 1.0f);
	vecteur3D vecteurDansBase(const vecteur3D &b1,const vecteur3D &b2,const vecteur3D &b3);
	void exprimerVecteurDansBase(const vecteur3D &b1,const vecteur3D &b2,const vecteur3D &b3);
	matrice3D vecteurFoisVecteurTranspose ();
	matrice3D matriceAntisymetrique ();
	matrice3D matriceRotationAutourVecteur(double angle);
};

class matrice3D {
	public:
	double _11,_12,_13;
	double _21,_22,_23;
	double _31,_32,_33;
	matrice3D(){}
	matrice3D (double m11, double m12, double m13, double m21, double m22, double  m23, double m31, double m32, double  m33):
		_11(m11),_12(m12),_13(m13),_21(m21),_22(m22),_23(m23),_31(m31),_32(m32),_33(m33) {}
	matrice3D (const vecteur3D &col1, const vecteur3D &col2, const vecteur3D &col3):
		_11(col1.x),_12(col2.x),_13(col3.x),_21(col1.y),_22(col2.y),_23(col3.y),_31(col1.z),_32(col2.z),_33(col3.z) {}
	double determinant(void);
	matrice3D inverse(void);
};

vecteur3D operator+(const vecteur3D &v1,const double n);
vecteur3D operator+(const double n, const vecteur3D &v1);
vecteur3D operator+(const vecteur3D &v1, const vecteur3D &v2);

vecteur3D operator-(const vecteur3D &v1);
vecteur3D operator-(const vecteur3D &v1,const double n);
vecteur3D operator-(const vecteur3D &v1,const vecteur3D &v2);

double operator*(const vecteur3D &v1, const vecteur3D &v2);
vecteur3D operator*(const vecteur3D &v1, const double n);
vecteur3D operator*(const double n, const vecteur3D &v1);
vecteur3D operator/(const vecteur3D &v1,const double n);
vecteur3D operator^(const vecteur3D &v1,const vecteur3D &v2);


matrice3D operator+ (const matrice3D &m1, double n);
matrice3D operator+ (double f, const matrice3D &m1);
matrice3D operator+ (const matrice3D &m1, const matrice3D &m2);

matrice3D operator- (const matrice3D &m1, double n);
matrice3D operator- (const matrice3D &m1, const matrice3D &m2);

matrice3D operator* (const matrice3D &m1, double n);
matrice3D operator* (double n, const matrice3D &m1);
vecteur3D operator*(const matrice3D &m, const vecteur3D &v);
matrice3D operator* (const matrice3D &m1, const matrice3D &m2);

matrice3D operator/ (const matrice3D &m1, double n);

std::ostream &operator<<(std::ostream &s,const point3D &p);   // si on ne dÃ©finit pas l'un des deux
std::ostream &operator<<(std::ostream &s,const vecteur3D &v); // l'autre est utilisÃ© (conversion implicite)
std::ostream &operator<<(std::ostream &s,const matrice3D &m);

#define point point3D
#define vecteur vecteur3D
#define matrice matrice3D

#endif
