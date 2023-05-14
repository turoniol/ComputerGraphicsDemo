#pragma once

#include <array>
#include <vector>
#include <functional>
#include <string>

#include "TemplateHelper.h"

struct Mesh;

class OBJReader
{
public:
	Mesh Read(const std::string& filepath);
private:
	template <class T, std::size_t N> 
	struct Vec { 
		std::array<T, N> data;

		template <class U>
		operator U() const {
			return construct_from_array<U>(data, std::make_index_sequence<N>());
		}
	};

	using Vec2f = Vec<float, 2>;
	using Vec3f = Vec<float, 3>;
	using Vec4f = Vec<float, 4>;

	struct FaceData {
		std::size_t data[3];
		std::size_t& operator[](std::size_t i) { return data[i]; }
		const std::size_t& v() const { return data[0]; }
		const std::size_t& vt() const { return data[1]; }
		const std::size_t& vn() const { return data[2]; }
	};

	struct Face {
		std::vector<FaceData> data;
		bool useVt = true;
	};

	template <class T, std::size_t N>
	static Vec<T, N> ReadVec(std::stringstream& str) {
		Vec<T, N> vec{};

		for (std::size_t i = 0; i < N; ++i)
			str >> vec.data[i];

		return vec;
	}
	

	using ParseFunc = std::function<void(std::stringstream&)>;
#define ParsePair(val, func) { val, [this](std::stringstream& str) { func(str); } }
#define IgnorePair(val) { val, [](std::stringstream& str) {} }

	void Clear();

	Mesh BuildMesh() const;

	void ParseLine(const std::string& str);

	void ParseVertex(std::stringstream& str);
	void ParseNormal(std::stringstream& str);
	void ParseTexCoord(std::stringstream& str);
	void ParseFace(std::stringstream& str);

	std::vector<Vec3f> m_vertices;
	std::vector<Vec3f> m_normals;
	std::vector<Vec2f> m_texcoords;
	std::vector<Face> m_faces;
};

