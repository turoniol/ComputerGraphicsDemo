#pragma once

#include <vector>
#include "FundamentalTypes.h"
#include <DirectXCollision.h>

struct Mesh {
	friend struct Renderer;

	template <std::size_t N>
	struct Polygon {
		std::array<Point, N> points;
		Vector normal;
		Color color;
	};

	using Line = Polygon<2>;
	using Triangle = Polygon<3>;

	struct Vertex {
		Point position;
		Vector normal;
		Color color;
	};

	struct Material {
		Color ambient, diffuse, specular;
		float shininess;
	};


	Mesh() = default;
	Mesh(const Mesh&) = default;
	Mesh(Mesh&&) = default;
	Mesh(std::vector<Vertex>&& vertices);

	DirectX::BoundingBox& GetBoundingBox();

	template <std::size_t N>
	void AddPolygon(Polygon<N>&& poly) {
		for (std::size_t i = 0; i < N; ++i) {
			m_vertices.push_back(Vertex{ poly.points[i], poly.normal, poly.color });
		}
	}
	
	Material material{ { 0.2f, 0.2f, 0.2f, 1.0f }, { 0.2f, 0.2f, 0.2f, 1.0f }, { 0.6f, 0.6f, 0.6f, 1.0f }, 32.0f };
private:
	std::vector<Vertex> m_vertices;
	DirectX::BoundingBox m_boundingBox;
};

