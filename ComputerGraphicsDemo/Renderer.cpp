#include "Renderer.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <cassert>

#include "Mesh.h"
#include "Camera.h"
#include "Viewport.h"

Renderer::Renderer()
	: clearColor{ 0.1f, 0.1f, 0.1f, 1.f }
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void Renderer::BeginFrame(int vpWidth, int vpHeight)
{
	glClearColor(EXPAND4f(clearColor));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, vpWidth, vpHeight);
}

void Renderer::SetLight(Point position, Color color)
{
	GLfloat lightColor[] = { EXPAND4f(color) };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

	GLfloat lightPosition[] = { EXPAND3f(position), 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void Renderer::SetMatrices(Matrix4x4 world, Matrix4x4 view, Matrix4x4 proj)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(GetFloat4x4(view).m[0]);
	glMultMatrixf(GetFloat4x4(world).m[0]);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(GetFloat4x4(proj).m[0]);
}

void Renderer::RenderMesh(const Mesh& mesh) {
	const GLfloat ambient[] = { EXPAND4f(mesh.material.ambient) };
	const GLfloat diffuse[] = { EXPAND4f(mesh.material.diffuse) };
	const GLfloat specular[] = { EXPAND4f(mesh.material.specular) };
	const GLfloat shininess[] = { mesh.material.shininess };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glBegin(GL_TRIANGLES);
	for (const auto& [position, normal, color] : mesh.m_vertices) {
		glColor4f(EXPAND4f(color));
		glNormal3f(EXPAND3f(normal));
		glVertex3f(EXPAND3f(position));
	}
	glEnd();
}