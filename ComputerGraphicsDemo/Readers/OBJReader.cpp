#include "OBJReader.h"

#include "../Logger.h"

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

static std::vector<std::string> Triangulate(const std::vector<std::string>& v) {
    std::vector<std::string> res;

    res.push_back(v[0]);
    res.push_back(v[1]);
    res.push_back(v[2]);

    res.push_back(v[2]);
    res.push_back(v[3]);
    res.push_back(v[0]);

    return res;
}

static std::vector<std::string> ReadFaceInfoTriangulated(std::istream& istr) {
    std::string line;

    std::getline(istr, line);

    std::vector<std::string> faceInfo;

    std::stringstream str(line);

    std::string data;
    while (str >> data)
        faceInfo.push_back(data);

    if (faceInfo.size() > 3)
        return Triangulate(faceInfo);

    return faceInfo;
}

OBJReader::Face OBJReader::ReadFace(std::vector<std::string>::iterator faceBegin, std::vector<std::string>::iterator faceEnd) {
    Face face;

    for (; faceBegin != faceEnd; ++faceBegin) {
        FaceData fdata{};

        std::size_t internalIndex = 0;
        constexpr std::string_view sep{ "/" };
        for (const auto word : std::views::split(*faceBegin, sep)) {
            int res = 0;

            if (!word.empty())
                res = std::stoi(word.data());

            fdata[internalIndex++] = res;
        }

        face.data.emplace_back(std::move(fdata));
    }

    return face;
}

void OBJReader::ParseFace() {
    auto faceInfo = ReadFaceInfoTriangulated(m_fileStr);

    for (std::size_t i = 0; i < faceInfo.size(); i += 3) {
        auto currentIt = faceInfo.begin() + i;
        m_faces.push_back(ReadFace(currentIt, currentIt + 3));
    }
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
