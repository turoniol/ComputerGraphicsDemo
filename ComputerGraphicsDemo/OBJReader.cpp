#include "OBJReader.h"

#include "Logger.h"

#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
#include <ranges>
#include <filesystem>

OBJReader::OBJReader()
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

void OBJReader::ParseVertex(std::stringstream& str) {
    m_vertices.emplace_back(ReadVec<float, 3>(str));
}

void OBJReader::ParseTexCoord(std::stringstream& str) {
    (void *)&str;
}

void OBJReader::ParseNormal(std::stringstream& str) {
    m_normals.emplace_back(ReadVec<float, 3>(str));
}

void OBJReader::ParseFace(std::stringstream& str) {
    std::string text;
    Face face;

    while (str >> text) {
        FaceData faceData{};

        std::size_t* index = faceData.data;

        for (auto word : std::views::split(text, '/')) {
            if (word.empty())
                face.useVt = false;
            else
                *index = std::stoi(word.data());

            index++;
        }

        face.data.emplace_back(std::move(faceData));
    }

    m_faces.emplace_back(std::move(face));
}

void OBJReader::ParseObject(std::stringstream& str)
{
    str >> m_name;

    if (!m_objects.empty())
        BuildMesh();
}

void OBJReader::ParseMtllib(std::stringstream& str)
{
    ReaderBase mtlReader;

    Mesh::Material mat{};
    std::string matName{};
    auto newmtl = [&](std::stringstream& sstr) {
        if (!m_materials.empty()) {
            m_materials.insert({ matName, mat });
            matName = {};
            mat = {};
        }

        sstr >> matName;
    };
    auto Ka = [&](std::stringstream& sstr) {
        auto v = ReadVec<float, 3>(sstr);
        mat.ambient = DirectX::XMVectorSet(v.data[0], v.data[1], v.data[2], 1.0f);
    };
    auto Kd = [&](std::stringstream& sstr) {
        auto v = ReadVec<float, 3>(sstr);
        mat.diffuse = DirectX::XMVectorSet(v.data[0], v.data[1], v.data[2], 1.0f);
    };
    auto Ks = [&](std::stringstream& sstr) {
        auto v = ReadVec<float, 3>(sstr);
        mat.specular = DirectX::XMVectorSet(v.data[0], v.data[1], v.data[2], 1.0f);

    };
    auto Ns = [&](std::stringstream& sstr) {
        sstr >> mat.shininess;
    };

    mtlReader.m_actions.insert({ "newmtl", newmtl });
    mtlReader.m_actions.insert({ "Ka", Ka });
    mtlReader.m_actions.insert({ "Kd", Kd });
    mtlReader.m_actions.insert({ "Ks", Ks });
    mtlReader.m_actions.insert({ "Ns", Ns });

    mtlReader.endFunc = [&]() {
        m_materials.insert({ matName, mat });
    };

    std::string path{};
    str >> path;
    mtlReader.Read(std::filesystem::path(workingDir).append(path).string());
}

void OBJReader::ParseUsemtl(std::stringstream& str)
{
    str >> m_material;
}

void OBJReader::Clear() {
    m_material.clear();
    m_name.clear();
    m_vertices.clear();
    m_normals.clear();
    m_texcoords.clear();
    m_faces.clear();
}
