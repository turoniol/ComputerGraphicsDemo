#include "OBJReader.h"

#include "Logger.h"

#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
#include <ranges>
#include <filesystem>

OBJReader::OBJReader(const std::string& file)
    : ReaderBase(file)
{
    ParsePair("v", ParseVertex);
    ParsePair("vt", ParseTexCoord);
    ParsePair("vn", ParseNormal);
    ParsePair("f", ParseFace);
    ParsePair("o", ParseObject);
    ParsePair("mtllib", ParseMtllib);
    ParsePair("usemtl", ParseUsemtl);
    IgnorePair("#");

    endFunc = [this] { BuildMesh(); };
}

const std::map<std::string, Mesh>& OBJReader::GetMeshes() const
{
    return m_objects;
}

void OBJReader::BuildMesh() {
    auto adjustIndex = [this](std::size_t index) {
        if (index < 0)
            index = m_vertices.size() - index;
        else
            index = index - 1;

        return index;
    };

    Mesh mesh;

    for (const auto& face : m_faces) {
        Mesh::Triangle triangle{};

        for (auto i = 0u; i < 3; ++i) {
            auto index = adjustIndex(face.data[i].v());
            triangle.points[i] = m_vertices[index];
        }

        triangle.normal = m_normals[adjustIndex(face.data[0].vn())];

        mesh.AddPolygon(std::move(triangle));
    }

    mesh.material = m_materials[m_material];
    m_objects.insert({ m_name, std::move(mesh) });

    Clear();
}

void OBJReader::ParseVertex() {
    m_vertices.emplace_back(ReadVec<float, 3>(m_fileStr));
}

void OBJReader::ParseTexCoord() {
    Ignore();
}

void OBJReader::ParseNormal() {
    m_normals.emplace_back(ReadVec<float, 3>(m_fileStr));
}

void OBJReader::ParseFace() {

    std::string text;
    Face face;

    auto faceInfo = ReadVec<std::string, 3>(m_fileStr);

    constexpr std::string_view indexDelim{ "/" };
    for (std::size_t i = 0; i < 3; ++i) {
        FaceData fdata{};
        
        std::size_t internalIndex = 0;
        for (const auto word : std::views::split(faceInfo.data[i], indexDelim)) {
            int res = 0;

            if (!word.empty())
                res = std::stoi(word.data());

            fdata.data[internalIndex++] = res;
        }

        face.data.emplace_back(std::move(fdata));
    }

    m_faces.emplace_back(std::move(face));
}

void OBJReader::ParseObject()
{
    m_fileStr >> m_name;

    if (!m_objects.empty())
        BuildMesh();
}

void OBJReader::ParseMtllib()
{
    std::string path{};
    m_fileStr >> path;
    MTLReader mtlReader(std::filesystem::path(workingDir).append(path).string());
    mtlReader.Read();

    m_materials = mtlReader.GetMaterials();
}

void OBJReader::ParseUsemtl()
{
    m_fileStr >> m_material;
}

void OBJReader::Clear() {
    m_material.clear();
    m_name.clear();
    m_vertices.clear();
    m_normals.clear();
    m_texcoords.clear();
    m_faces.clear();
}

MTLReader::MTLReader(const std::string& file)
    : ReaderBase(file)
{
    ParsePair("newmtl", ParseNewmtl);
    ParsePair("Ka", ParseKa);
    ParsePair("Kd", ParseKd);
    ParsePair("Ks", ParseKs);
    ParsePair("Ns", ParseNs);

    endFunc = [this] {
        m_materials.insert({ m_materialName, m_material });
    };
}

const std::map<std::string, Mesh::Material> MTLReader::GetMaterials() const
{
    return m_materials;
}

void MTLReader::ParseNewmtl()
{
    if (!m_materials.empty())
        m_materials.insert({ m_materialName, m_material });

    m_fileStr >> m_materialName;
    m_material = {};
}

void MTLReader::ParseKa()
{
    auto v = ReadVec<float, 3>(m_fileStr);
    m_material.ambient = DirectX::XMVectorSet(v.data[0], v.data[1], v.data[2], 1.f);
}

void MTLReader::ParseKd()
{
    auto v = ReadVec<float, 3>(m_fileStr);
    m_material.diffuse = DirectX::XMVectorSet(v.data[0], v.data[1], v.data[2], 1.f);
}

void MTLReader::ParseKs()
{
    auto v = ReadVec<float, 3>(m_fileStr);
    m_material.specular = DirectX::XMVectorSet(v.data[0], v.data[1], v.data[2], 1.f);
}

void MTLReader::ParseNs()
{
    m_fileStr >> m_material.shininess;
}
