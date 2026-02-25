/*
 * primitive3dBucket.cpp
 *
 *  Created on: 15 août 2018
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

#ifndef UNITY
#include "primitive3dBucket.h"

    t_primitive3dBucket::t_primitive3dBucket (int primitive)
	{
		m_nbVertex = 0;  // Initialy no vertex is stored into the bucket.
					   // evenmore the vector vertData is empty
		m_currentColor.r = 1;  // Initial color white to see something even without call to setColor
		m_currentColor.g = 1;
		m_currentColor.b = 1;
		m_currentWidth = 1;
		m_bIncreased = false;
		m_vao = 0;
		m_vbo = 0;
		m_shader = nullptr;
		m_nPrimitive = primitive;
	}

	void t_primitive3dBucket::init (Shader *shader)
	{
		m_shader = shader;
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	}

	// Placeholder for glColor3f
	void t_primitive3dBucket::setColor (float r, float g, float b)
	{
		m_currentColor.r = r;
		m_currentColor.g = g;
		m_currentColor.b = b;
	}

	// Placeholder for glLineWidth
	void t_primitive3dBucket::setWidth(float width)
	{
		m_currentWidth = width;
	}

	// Placeholder for part of drawPoint3D
	void t_primitive3dBucket::addPoint3D (float x1, float y1, float z1)
	{
		if (m_nbVertex ==  vertexArray.size()) {
			// the bucket is full
			// we add one vertice
			vertexData v1;
			v1.x = x1;
			v1.y = y1;
			v1.z = z1;
			v1.r = m_currentColor.r;
			v1.g = m_currentColor.g;
			v1.b = m_currentColor.b;
			v1.width = m_currentWidth;
			vertexArray.push_back(v1);
			m_bIncreased = true;
		}
		else {
			// The bucket has at least one not used space. Just fill it.
			vertexData &v1 = vertexArray[m_nbVertex];
			v1.x = x1;
			v1.y = y1;
			v1.z = z1;
			v1.r = m_currentColor.r;
			v1.g = m_currentColor.g;
			v1.b = m_currentColor.b;
			v1.width = m_currentWidth;
			m_bIncreased = false;
		}
		m_nbVertex = m_nbVertex + 1;
	}

	// Placeholder for part of drawLine3D
	void t_primitive3dBucket::addLine3D (float x1, float y1, float z1, float x2, float y2, float z2) {
		addPoint3D (x1, y1, z1);
		addPoint3D (x2, y2, z2);
	}

	void t_primitive3dBucket::empty ()
	{
		m_nbVertex = 0;
	}

	void t_primitive3dBucket::draw ()
	{
		if (m_shader == nullptr || m_nbVertex == 0) {  // No shader or nothing to draw ==> no draw
 			return;
		}

		if (m_vao == 0) {
			glGenVertexArrays(1, &m_vao);
		}

		if (m_bIncreased) {
			if (m_vbo != 0) {
				glDeleteBuffers (1,&m_vbo);
				m_vbo = 0;
			}
		}

		if (m_vbo == 0) {
		    glGenBuffers(1, &m_vbo);
		    glBindVertexArray(m_vao);
		    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

			GLsizei stride = sizeof(vertexData);

			// Vertex
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride , (const void *)offsetof(vertexData,x));

			// Color
			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offsetof(vertexData,r));

			// Width
			glEnableVertexAttribArray( 2 );
			glVertexAttribPointer( 2, 1, GL_FLOAT, GL_FALSE, stride, (const void *)offsetof(vertexData,width));
		    glBindBuffer(GL_ARRAY_BUFFER, 0);
		    glBindVertexArray(0);
		}
		glBindVertexArray(m_vao);
		glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertexData) * m_nbVertex, &vertexArray[0], GL_STREAM_DRAW);
		m_shader->use();
		glDrawArrays(m_nPrimitive, 0, m_nbVertex);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	t_primitive3dBucket::~t_primitive3dBucket ()
	{
		if (m_vbo != 0) {
			glDeleteBuffers (1,&m_vbo);
		}
		if (m_vao != 0) {
			glDeleteVertexArrays(1,&m_vao);
		}
	}
#endif /* UNITY */
