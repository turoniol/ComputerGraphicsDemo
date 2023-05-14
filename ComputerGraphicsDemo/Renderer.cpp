#include "Renderer.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <cassert>

#include "Mesh.h"
#include "Camera.h"
#include "Viewport.h"

Renderer::Renderer()
	: clearColor{ 0.1f, 0.1f, 0.1f, 1.f },
	lightSource{ { 100.f, 160.f, 100.f }, { 1.f, 1.f, 1.f, 1.f }}
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
}

void Renderer::BeginFrame()
{
	assert(camera);
	assert(viewport);

	glClearColor(EXPAND4f(clearColor));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, viewport->width, viewport->height);

	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(GetFloat4x4(camera->LookAt()).m[0]);

		GLfloat lightColor[] = { EXPAND4f(lightSource.color) };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

		GLfloat lightPosition[] = { EXPAND3f(lightSource.position), 0.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	}

	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(GetFloat4x4(viewport->ProjectionMatrix()).m[0]);
	}

}

void Renderer::RenderMesh(const Mesh& mesh) {
	GLfloat materialAmbientDiffuse[] = { 1.0, 1.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialAmbientDiffuse);
	glBegin(GL_TRIANGLES);
	for (const auto& [position, normal, color] : mesh.m_vertices) {
		glColor4f(EXPAND4f(color));
		glNormal3f(EXPAND3f(normal));
		glVertex3f(EXPAND3f(position));
	}
	glEnd();
}