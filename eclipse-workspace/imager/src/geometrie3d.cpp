/*
 * Geometry.cpp
 *
 *
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

#include <math.h>
#include "geometrie3d.h"

 point3D::point3D (const vecteur3D &v)
{
x = v.x;
y = v.y;
z = v.z;
}

/*=====================================================================*/
 vecteur3D::vecteur3D (const point3D &p)
{
x = p.x;
y = p.y;
z = p.z;
}

 vecteur3D::vecteur3D (const point3D &p1, const point3D &p2)
{
x = p2.x - p1.x;
y = p2.y - p1.y;
z = p2.z - p1.z;
}

 vecteur3D::vecteur3D (const vecteur3D &v)
{
x = v.x;
y = v.y;
z = v.z;
}

/*=====================================================================*/

 vecteur3D &vecteur3D::operator=(const point3D &p)
{
  x=p.x;
  y=p.y;
  z=p.z;
  return *this;
}

 point3D &point3D::operator=(const vecteur3D &v)
{
  x=v.x;
  y=v.y;
  z=v.z;
  return *this;
}

 bool vecteur3D::operator==(const vecteur3D &v)
{
   return (x == v.x && y == v.y && z == v.z);
}
 bool vecteur3D::operator!=(const vecteur3D &v)
{
   return (x != v.x || y != v.y || z != v.z);
}

 bool point3D::operator==(const point3D &p)
{
   return (x == p.x && y == p.y && z == p.z);
}

 bool point3D::operator!=(const point3D &p)
{
   return (x != p.x || y != p.y || z != p.z);
}

/*=====================================================================*/
 vecteur3D operator+(const vecteur3D &v1,const double n)
{
	vecteur3D v;
	v.x = v1.x + n;
	v.y = v1.y + n;
	v.z = v1.z + n;
	return v;
}

/*=====================================================================*/
 vecteur3D operator+(const double n, const vecteur3D &v1)
{
	vecteur3D v;
	v.x = v1.x + n;
	v.y = v1.y + n;
	v.z = v1.z + n;
	return v;
}

/*=====================================================================*/
 vecteur3D operator-(const vecteur3D &v1)
{
	vecteur3D v;
	v.x = -v1.x;
	v.y = -v1.y;
	v.z = -v1.z;
	return v;
}

/*=====================================================================*/
 vecteur3D operator-(const vecteur3D &v1,const double n)
{
	vecteur3D v;
	v.x = v1.x - n;
	v.y = v1.y - n;
	v.z = v1.z - n;
	return v;
}

/*=====================================================================*/
 vecteur3D operator+(const vecteur3D &v1,const vecteur3D &v2)
{
	vecteur3D v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	v.z = v1.z + v2.z;
	return v;
}

/*=====================================================================*/
 vecteur3D operator-(const vecteur3D &v1,const vecteur3D &v2)
{
	vecteur3D v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	v.z = v1.z - v2.z;
	return v;
}

/*=====================================================================*/
// produit scalaire
 double operator*(const vecteur3D &v1,const vecteur3D &v2)
{
return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/*=====================================================================*/
 vecteur3D operator*(const vecteur3D &v1,const double n)
{
	vecteur3D v;
	v.x = v1.x * n;
	v.y = v1.y * n;
	v.z = v1.z * n;
	return v;
}

/*=====================================================================*/
 vecteur3D operator*(const double n, const vecteur3D &v1)
{
	vecteur3D v;
	v.x = v1.x * n;
	v.y = v1.y * n;
	v.z = v1.z * n;
	return v;
}

/*=====================================================================*/
 vecteur3D operator/(const vecteur3D &v1,const double n)
{
	double invn = 1.0f/n;
	return v1*invn;
}

/*=====================================================================*/
// produit vectoriel
 vecteur3D operator^(const vecteur3D &v1,const vecteur3D &v2)
{
vecteur3D v;
v.x = v1.y * v2.z - v1.z * v2.y;
v.y = v1.z * v2.x - v1.x * v2.z;
v.z = v1.x * v2.y - v1.y * v2.x;
return v;
}

 /*=====================================================================*/
bool point3D::pointDansGellule (const point3D &p1, const point3D &p2, const double rayon)
 {
	 double rayonCarre = rayon * rayon;
	 if (vecteur(p1,*this).normeCarree() <= rayonCarre ||
		 vecteur(p2,*this).normeCarree() <= rayonCarre) {	// Proximité P1 ou P2
		 return true;
	 }
	 // Sinon on recherche la proximité au segment P1,P2
	 vecteur v1 (p1,p2);
	 double dCarre = v1.normeCarree();
	 v1.normer();
	 vecteur v2 = v1.vecteurPerpendiculaire();
	 vecteur v3 = v1^v2;
	 vecteur v (p1,*this);
	 v.exprimerVecteurDansBase(v1,v2,v3);
	 if (v.x >= 0 &&
	     v.x * v.x <= dCarre &&
		 v.y * v.y + v.z * v.z <= rayonCarre) {
		 return true;
	 }
	 return false;
 }

/*=====================================================================*/
// vecteur perpendiculaire
 vecteur3D vecteur3D::vecteurPerpendiculaire()
{
	vecteur3D v;
	if (x == 0 && y == 0) {
		v.x = 1;
		v.y = 0;
		v.z = 0;
	} else {
		v.x = - y;
		v.y = x;
		v.z = 0;
	}
	return v;
}

/*=====================================================================*/
 double vecteur3D::norme ()
{
return sqrt(x * x + y * y + z * z);
}

/*=====================================================================*/
 double vecteur3D::normeCarree ()
{
return x * x + y * y + z * z;
}

/*=====================================================================*/
 double vecteur3D::angle (const vecteur3D &v)		// Pas de contrainte sur les vecteurs this et v.
{
    vecteur3D v1(x,y,z);
    vecteur3D v2 = v;

    v1.normer();
    v2.normer();

    double produitScal = v1*v2;
    if (produitScal > 1.0) {	// Peut arriver à cause d'erreurs d'approximation numérique
	return 0;
    }
    return acos (produitScal);
}

 double vecteur3D::angleN_ (const vecteur3D &v)	// This est normé
{
    vecteur3D v2 = v;

    v2.normer();

    double produitScal = *this*v2;
    if (produitScal > 1.0) {	// Peut arriver à cause d'erreurs d'approximation numérique
	return 0;
    }
    return acos (produitScal);
}

 double vecteur3D::angle_N (const vecteur3D &v)		// v est normé
{
    vecteur3D v1(x,y,z);

    v1.normer();

    double produitScal = v1*v;
    if (produitScal > 1.0) {	// Peut arriver à cause d'erreurs d'approximation numérique
	return 0;
    }
    return acos (produitScal);
}

 double vecteur3D::angleNN (const vecteur3D &v)		// This et v sont normés
{
    double produitScal = *this*v;
    if (produitScal > 1.0) {	// Peut arriver à cause d'erreurs d'approximation numérique
	return 0;
    }
    return acos (produitScal);
}

/*=====================================================================*/
 void vecteur3D::normer(double l)
{
double invNorme = l/norme();  // pour utiliser des multiplications (plus rapides que des divisions)
x *= invNorme;
y *= invNorme;
z *= invNorme;
}

/*=====================================================================*/
 vecteur3D vecteur3D::vecteurUnitaire(const vecteur3D &v1, double l)
{
vecteur3D v;
double invNorme = l/norme();  // pour utiliser des multiplications (plus rapides que des divisions)
v.x = v1.x * invNorme;
v.y = v1.y * invNorme;
v.z = v1.z * invNorme;
return v;
}

/*=====================================================================*/
 vecteur3D vecteur3D::vecteurDansBase(const vecteur3D &b1,const vecteur3D &b2,const vecteur3D &b3)
{
vecteur3D v;
v.x = x * b1.x + y * b1.y + z * b1.z;
v.y = x * b2.x + y * b2.y + z * b2.z;
v.z = x * b3.x + y * b3.y + z * b3.z;
return v;
}


void vecteur3D::exprimerVecteurDansBase(const vecteur3D &b1,const vecteur3D &b2,const vecteur3D &b3)
{
double xs = x;
double ys = y;
double zs = z;
x = xs * b1.x + ys * b1.y + zs * b1.z;
y = xs * b2.x + ys * b2.y + zs * b2.z;
z = xs * b3.x + ys * b3.y + zs * b3.z;
}

/*=====================================================================*/
 matrice3D vecteur3D::vecteurFoisVecteurTranspose ()
{
matrice3D m;
m._11 = x * x;
m._12 = x * y;
m._13 = x * z;
m._21 = m._12;
m._22 = y * y;
m._23 = y * z;
m._31 = m._13;
m._32 = m._23;
m._33 = z * z;
return m;
}

/*=====================================================================*/
std::ostream &operator<<(std::ostream &s,const vecteur3D &v) {
  s << /*std::setprecision(3) <<*/ "[ " << v.x << " , " << v.y << " , " << v.z << "]";
	return s;
}
std::ostream &operator<<(std::ostream &s,const point3D &p) {
	s << "( " << p.x << " , " << p.y << " , " << p.z << ")";
	return s;
}


vecteur3D operator*(const matrice3D &m, const vecteur3D &v)
{
	vecteur3D res;
	res.x = m._11* v.x +  m._12* v.y + m._13* v.z;
	res.y = m._21* v.x +  m._22* v.y + m._23* v.z;
	res.z = m._31* v.x +  m._32* v.y + m._33* v.z;
	return res;
}

matrice3D operator+ (const matrice3D &m1, double n)
{
	matrice3D res;
	res._11 = m1._11 + n;
	res._12 = m1._12 + n;
	res._13 = m1._13 + n;
	res._21 = m1._21 + n;
	res._22 = m1._22 + n;
	res._23 = m1._23 + n;
	res._31 = m1._31 + n;
	res._32 = m1._32 + n;
	res._33 = m1._33 + n;	
	return res;
}

matrice3D operator+ (double f, const matrice3D &m1)
{
	return m1 + f;
}

matrice3D operator- (const matrice3D &m1, double n)
{
	return m1 + -1.0f * n;
}

matrice3D operator* (const matrice3D &m1, double n)
{
	matrice3D res;
	res._11 = m1._11 * n;
	res._12 = m1._12 * n;
	res._13 = m1._13 * n;
	res._21 = m1._21 * n;
	res._22 = m1._22 * n;
	res._23 = m1._23 * n;
	res._31 = m1._31 * n;
	res._32 = m1._32 * n;
	res._33 = m1._33 * n;	
	return res;
}

matrice3D operator* (double n, const matrice3D &m1)
{
	matrice3D res;
	res._11 = m1._11 * n;
	res._12 = m1._12 * n;
	res._13 = m1._13 * n;
	res._21 = m1._21 * n;
	res._22 = m1._22 * n;
	res._23 = m1._23 * n;
	res._31 = m1._31 * n;
	res._32 = m1._32 * n;
	res._33 = m1._33 * n;	
	return res;
}

matrice3D operator/ (const matrice3D &m1, double n)
{
	matrice3D res;
	res = m1 * (1.0f / n);
	return res;
}

matrice3D operator+ (const matrice3D &m1, const matrice3D &m2)
{
	matrice3D res;
	res._11 = m1._11 + m2._11;
	res._12 = m1._12 + m2._12;
	res._13 = m1._13 + m2._13;
	res._21 = m1._21 + m2._21;
	res._22 = m1._22 + m2._22;
	res._23 = m1._23 + m2._23;
	res._31 = m1._31 + m2._31;
	res._32 = m1._32 + m2._32;
	res._33 = m1._33 + m2._33;	
	return res;
}

matrice3D operator- (const matrice3D &m1, const matrice3D &m2)
{
	matrice3D res;
	res._11 = m1._11 - m2._11;
	res._12 = m1._12 - m2._12;
	res._13 = m1._13 - m2._13;
	res._21 = m1._21 - m2._21;
	res._22 = m1._22 - m2._22;
	res._23 = m1._23 - m2._23;
	res._31 = m1._31 - m2._31;
	res._32 = m1._32 - m2._32;
	res._33 = m1._33 - m2._33;	
	return res;
}

matrice3D operator* (const matrice3D &m1, const matrice3D &m2)
{
	matrice3D res;
	res._11 = m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31;
	res._12 = m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32;
	res._13 = m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33;
	res._21 = m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31;
	res._22 = m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32;
	res._23 = m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33;
	res._31 = m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31;
	res._32 = m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32;
	res._33 = m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33;
	return res;
}

 double matrice3D::determinant ()
{
  return _11*_22*_33 + _12*_23*_31 + _13*_21*_32 - _13*_22*_31 - _23*_32*_11 - _33*_12*_21; 
}

 matrice3D matrice3D::inverse ()
{
  matrice3D res;
  res._11 = _22*_33 - _23*_32 ;
  res._12 = _13*_32 - _12*_33 ;
  res._13 = _12*_23 - _13*_22 ;
  res._21 = _23*_31 - _21*_33 ;
  res._22 = _11*_33 - _13*_31 ;
  res._23 = _13*_21 - _11*_23 ;
  res._31 = _21*_32 - _22*_31 ;
  res._32 = _12*_31 - _11*_32 ;
  res._33 = _11*_22 - _12*_21 ;
 
  return res/determinant(); 
}

/*=====================================================================*/
std::ostream &operator<<(std::ostream &s,const matrice3D &m) {
	s << "[ " << m._11 << " , " << m._12 << " , " << m._13 << std::endl;
	s << "  " << m._21 << " , " << m._22 << " , " << m._23 << std::endl;
	s << "  " << m._31 << " , " << m._32 << " , " << m._33 << " ]" << std::endl;
	return s;
}



//correspondant à l'application linéaire  v -> this ^ v
 matrice3D vecteur3D::matriceAntisymetrique ()
{
	matrice3D res;
	res._11 =  0;
	res._12 = -z;
	res._13 =  y;
	res._21 =  z;
	res._22 =  0;
	res._23 = -x;
	res._31 = -y;
	res._32 =  x;
	res._33 =  0;
	return res;
}

 matrice3D vecteur3D::matriceRotationAutourVecteur(double angle)  // Le vecteur doit etre norme !
{
	// de http://fr.wikipedia.org/wiki/Matrice_de_rotation

matrice3D p;
static const matrice3D i(1,0,0,0,1,0,0,0,1);
matrice3D q;

p = vecteurFoisVecteurTranspose ();  
q = matriceAntisymetrique();

// p = projection sur l'axe de rotation
// (i-p) = Projection sur le plan orthogonal à l'axe de rotation
return p + cos(angle)*(i - p) + sin(angle)*q;
}
