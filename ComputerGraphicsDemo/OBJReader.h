#pragma once

#include <array>
#include <vector>
#include <functional>
#include <string>
#include <map>

#include "Mesh.h"

#include "TemplateHelper.h"
#include "ReaderBase.h"

class MTLReader : public ReaderBase
{
public:
	MTLReader(const std::string& file);

	const std::map<std::string, Mesh::Material> GetMaterials() const;
private:
	void ParseNewmtl();
	void ParseKa();
	void ParseKd();
	void ParseKs();
	void ParseNs();

	std::string m_materialName;
	Mesh::Material m_material;
	std::map<std::string, Mesh::Material> m_materials;
};

class OBJReader : public ReaderBase
{
public:
	OBJReader(const std::string& file);

	const std::map<std::string, Mesh>& GetMeshes() const;
private:
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


	void Clear();

	void BuildMesh();

	void ParseVertex();
	void ParseNormal();
	void ParseTexCoord();
	void ParseFace();
	void ParseObject();
	void ParseMtllib();
	void ParseUsemtl();

	std::string m_name;
	std::string m_material;
	std::vector<Vec3f> m_vertices;
	std::vector<Vec3f> m_normals;
	std::vector<Vec2f> m_texcoords;
	std::vector<Face> m_faces;

	std::map<std::string, Mesh::Material> m_materials;
	std::map<std::string, Mesh> m_objects;
};

