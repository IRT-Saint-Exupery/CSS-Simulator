/*
 * sphere.cpp
 *
 *  Created on: 15 août 2018
 *      Author: Jacques Girard (jb.girard@free.fr)
 */

#ifndef UNITY
#include "sphere.h"
//#define STB_IMAGE_IMPLEMENTATION			// Pas dans les bibliothèques (uniquement dans le prog principal)
#include "stb_image.h"

    t_sphere::t_sphere (const char *textureFile, float radius, int nbSlices) {
		m_textureFile = textureFile;
		m_radius = radius;
		m_nbSlices = nbSlices;
	}

	void t_sphere::init(Shader *shader)	{
		m_shader = shader;
		std::vector<float> vertData;
		std::vector<unsigned int> vertIndex;
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

	    glGenBuffers(1, &m_vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		float azimuth, azimuth_step;
		float elevation, elevation_step;
		int k, j;

		azimuth_step = 2.*M_PI/m_nbSlices;
		elevation_step = M_PI/m_nbSlices;

		for (k = 0, azimuth = -M_PI; k <= m_nbSlices; k++, azimuth += azimuth_step) {
			for (j = 0, elevation = M_PI/2; j <= m_nbSlices; j++, elevation -= elevation_step) {
			  float x = cos(elevation)*sin(azimuth);
			  float z = sin(elevation);
			  float y = -cos(elevation)*cos(azimuth);
			  vertData.push_back (x*m_radius);   // Vertex
			  vertData.push_back (y*m_radius);
			  vertData.push_back (z*m_radius);
			  vertData.push_back (x);	// Normal
			  vertData.push_back (y);
			  vertData.push_back (z);
			  vertData.push_back ((float)k/m_nbSlices);  // Texture coord
			  vertData.push_back ((float)j/m_nbSlices);
			}
		  }

		glBufferData( GL_ARRAY_BUFFER, sizeof(float) * vertData.size(), &vertData[0], GL_STATIC_DRAW);

		GLsizei stride = (3 + 3 + 2) * sizeof(GL_FLOAT);
		uintptr_t offset = 0;

		// Vertex
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride , (const void *)offset);

		// Normal
		offset += 3 * sizeof(GL_FLOAT);
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offset);

		// Texture
		offset += 3 * sizeof(GL_FLOAT);
		glEnableVertexAttribArray( 2 );
		glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, stride, (const void *)offset);

	    glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Indexes
		glGenBuffers(1, &m_vbi);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbi);

		for (k = 0; k < m_nbSlices; k++) {
			for (j = 0; j < m_nbSlices; j++) {
				vertIndex.push_back ((m_nbSlices+1)*(k+1)+j);
				vertIndex.push_back ((m_nbSlices+1)*k+j+0);
				vertIndex.push_back ((m_nbSlices+1)*k+j+1);

				vertIndex.push_back ((m_nbSlices+1)*k+j+1);
				vertIndex.push_back ((m_nbSlices+1)*(k+1)+j+1);
				vertIndex.push_back ((m_nbSlices+1)*(k+1)+j);
			}
		  }


		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertIndex.size(), &vertIndex[0], GL_STATIC_DRAW);
		m_nbVerticesIndex = vertIndex.size();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	    glBindVertexArray(0);


	    int textureWidth, textureHeight, textureNrChannels;
	    glGenTextures(1, &m_texture);
	    glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	    stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	    unsigned char *textureData = stbi_load(m_textureFile, &textureWidth, &textureHeight, &textureNrChannels, 0);
	    if (textureData)
	    {
	        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	        //glGenerateMipmap(GL_TEXTURE_2D);
	    }
	    else
	    {
	        std::cout << "Failed to load texture" << std::endl;
	    }
	    stbi_image_free(textureData);
	}

	t_sphere::~t_sphere ()
	{
		glDeleteBuffers (1,&m_vbo);
		glDeleteBuffers (1,&m_vbi);
		glDeleteVertexArrays(1,&m_vao);
	}

	void t_sphere::draw()
	{
		m_shader->use();

        glBindTexture(GL_TEXTURE_2D, m_texture);

        glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbi);  // Il faut binder le vbi  car il ne semble pas faire partie du vao
													   // (pas nécessaire pour le vbo)

        glDrawElements(GL_TRIANGLES, m_nbVerticesIndex,GL_UNSIGNED_INT,(const void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

#endif /* UNITY */

