/*
 * primitive3dBucket.h
 *
 *  Created on: 15 ao�t 2018
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

/*
 * primitive3dBucket.h
 *
 * This class to store and draw 3d lines or points in "modern" OpenGl style.
 * Each point defined by x, y, z, r, g, b, width as floating points
 * Each line defined by 2 points
 * Currently width is not used for lines drawing.
 *
 *  Created on: 15 ao�t 2018
 *      Author: girardja
 */


#ifndef UNITY
#ifndef PRIMITIVE3DBUCKET_H_
#define PRIMITIVE3DBUCKET_H_

#include <glad/glad.h>
#include <vector>
#include "shader.h"

class t_primitive3dBucket {

	struct vertexData {
		float x,y,z;
		float r,g,b;
		float width;
	};

	struct colorData{
		float r,g,b;
	};

	unsigned int m_nbVertex;  // number of stored vertex in the bucket. less or equal than vertData.size()
	std::vector<vertexData> vertexArray;
    colorData m_currentColor;
    float m_currentWidth;

    bool m_bIncreased; // To know if we shall re bind vbo.

    GLuint m_vao, m_vbo;
    GLenum m_nPrimitive; // GL_POINTS or GL_LINES

public :
    Shader *m_shader;

    // Constructor
    // Primitive can be GL_LINES or GL_POINTS according to the type of data to store and display.
    // In general the application has one bucket for lines and one for points.
    t_primitive3dBucket (int primitive);

    // To bind the shader. Should be compatible with layouts.
	void init(Shader *shader);

	// Placeholder for glColor3f
	void setColor (float r, float g, float b);

	// Placeholder for glLineWidth or GlPointSize
	void setWidth(float width);

	// Placeholder for part of drawPoint3D
	void addPoint3D (float x1, float y1, float z1);

	// Placeholder for part of drawLine3D
	void addLine3D (float x1, float y1, float z1, float x2, float y2, float z2);

	// To empty the bucket
	void empty ();

	// To draw the primitives in the bucket
	void draw ();

	// Destructor
	~t_primitive3dBucket ();
};


#endif /* PRIMITIVE3DBUCKET_H_ */
#endif /* UNITY */
