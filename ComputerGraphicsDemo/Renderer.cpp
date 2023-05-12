#include "Renderer.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "Mesh.h"

void Renderer::RenderMesh(const Mesh& mesh) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(10.0, 10.0, 10.0,   
			  0.0, 0.0, 0.0,   
			  0.0, 1.0, 0.0);  

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0); 


	glBegin(GL_TRIANGLES);
	for (const auto& [position, normal, color] : mesh.m_vertices) {
		glColor4f(color.x, color.y, color.z, color.w);
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(position.x, position.y, position.z);
	}
	glEnd();
}