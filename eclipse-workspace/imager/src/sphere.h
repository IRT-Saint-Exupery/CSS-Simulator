/*
 * sphere.h
 *
 *  This class as placeholder for gluSphere. To be used in "modern" opengl style.
 *
 *  Created on: 15 ao�t 2018
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

#ifndef UNITY
#ifndef SPHERE_H_
#define SPHERE_H_

#include <glad/glad.h>
#include <vector>
#include "shader.h"

class t_sphere {
	const char *m_textureFile;
	float m_radius;
	int m_nbSlices;

    GLuint m_vao, m_vbo, m_vbi;
    unsigned int m_texture;
    int m_nbVerticesIndex;
public :
    Shader *m_shader;

    // Constructor
	t_sphere (const char *textureFile_, float radius_ = 1, int nbSlices_ = 30);

	// To bind a shader program. (shader3d.vs shader3d.fs)
	void init(Shader *shader);

	~t_sphere ();

	void draw();
};



#endif /* SPHERE_H_ */
#endif /* UNITY */
