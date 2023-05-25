#include "Renderer.h"

#include <windows.h>
#include <GL/GL.h>

#include <cassert>
#include <array>

#include "Mesh.h"
#include "Camera.h"
#include "Viewport.h"

Renderer::Renderer()
	: clearColor{ 0.1f, 0.1f, 0.1f, 1.f }
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(3.0f);
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

void Renderer::RenderBoundingBox(DirectX::BoundingBox bb, std::array<float, 4> color)
{
	auto& c = bb.Center;
	auto& e = bb.Extents;

	std::array vertices{
		DirectX::XMFLOAT3{ c.x - e.x, c.y - e.y, c.z - e.z },
		DirectX::XMFLOAT3{ c.x - e.x, c.y + e.y, c.z - e.z },
		DirectX::XMFLOAT3{ c.x + e.x, c.y + e.y, c.z - e.z },
		DirectX::XMFLOAT3{ c.x + e.x, c.y - e.y, c.z - e.z },

		DirectX::XMFLOAT3{ c.x - e.x, c.y - e.y, c.z + e.z },
		DirectX::XMFLOAT3{ c.x - e.x, c.y + e.y, c.z + e.z },
		DirectX::XMFLOAT3{ c.x + e.x, c.y + e.y, c.z + e.z },
		DirectX::XMFLOAT3{ c.x + e.x, c.y - e.y, c.z + e.z },
	};

	std::array indices{
		// Front face
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		// Back face
		4, 5,
		5, 6,
		6, 7,
		7, 4,

		// Connecting lines
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	glMaterialfv(GL_FRONT, GL_AMBIENT, color.data());
	glBegin(GL_LINES);
	for (int index : indices) {
		const auto& v = vertices[index];
		glVertex3f(v.x, v.y, v.z);
	}
	glEnd();
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