#pragma once

#include <vector>
#include "FundamentalTypes.h"

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

	Mesh() = default;
	Mesh(std::vector<Vertex>&& vertices);

	template <std::size_t N>
	void AddPolygon(Polygon<N>&& poly) {
		for (std::size_t i = 0; i < N; ++i) {
			m_vertices.push_back(Vertex{ poly.points[i], poly.normal, poly.color });
		}
	}
private:
	std::vector<Vertex> m_vertices;
};

