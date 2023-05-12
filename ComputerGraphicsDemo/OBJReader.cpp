#include "OBJReader.h"

#include "Mesh.h"
#include "Logger.h"

#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
#include <ranges>

Mesh OBJReader::Read(const std::string& filepath) {
    Clear();

    std::ifstream str(filepath);

    std::string line;
    while (std::getline(str, line)) {
        ParseLine(line);
    }

    return BuildMesh();
}

Mesh OBJReader::BuildMesh() const {
    Mesh mesh;

    for (const auto& face : m_faces) {
        Mesh::Triangle triangle{};
        triangle.normal = { 0, 1, 0 };
        triangle.color = { 1, 1, 0, 1 };

        for (auto i = 0u; i < 3; ++i) {
            auto index = face.data[i].v;

            if (index < 0)
                index = m_vertices.size() - index;
            else
                index = index - 1;

            const auto& vertex = m_vertices[index];
            triangle.points[i] = Mesh::Point{ vertex.data[0], vertex.data[1], vertex.data[2] };
        }

        mesh.AddPolygon(std::move(triangle));
    }

    return mesh;
}

void OBJReader::ParseVertex(std::stringstream& str) {
    m_vertices.emplace_back(ReadVec<float, 3>(str));
}

void OBJReader::ParseTexCoord(std::stringstream& str) {
    (void *)&str;
}

void OBJReader::ParseNormal(std::stringstream& str) {
    (void*)&str;
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

void OBJReader::ParseLine(const std::string& str) {
    static std::map<std::string, ParseFunc> nameToAction{
        ParsePair("v", ParseVertex),
        ParsePair("vt", ParseTexCoord),
        ParsePair("vn", ParseNormal),
        ParsePair("f", ParseFace),
        IgnorePair("#")
    };

    std::string prefix;
    std::stringstream sstr(str);
    sstr >> prefix;

    if (prefix.empty())
        return;

    auto finded = nameToAction.find(prefix);

    if (finded == nameToAction.end())
        LOGERRRET("No such option: ", prefix);

    finded->second(sstr);
}

void OBJReader::Clear() {
    m_vertices.clear();
    m_normals.clear();
    m_texcoords.clear();
    m_faces.clear();
}
