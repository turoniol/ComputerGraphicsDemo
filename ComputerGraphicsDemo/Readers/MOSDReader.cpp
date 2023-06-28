#include "MOSDReader.h"

#include "../Scene.h"
#include "../Mesh.h"
#include "OBJReader.h"

#include <filesystem>

namespace fs = std::filesystem;

MOSDReader::MOSDReader(const std::string& file)
    : ReaderBase(file)
{
    IgnorePair("#");
    ParsePair("scene", ParseScene);
    ParsePair("node", ParseNode);
    ParsePair("camera", ParseCamera);
    ParsePair("light", ParseLight);
    ParsePair("geometry", ParseGeometry);
    ParsePair("matrix", ParseMatrix);

    endFunc = [] {};
}

Scene MOSDReader::GetScene() const
{
    return m_scenes.begin()->second.Instance(*this);
}

MOSDReader::MOSDScene::Node MOSDReader::MOSDScene::ParseSceneNode(std::istream& str) {
    Node node{};
    str >> node.name;

    std::string token{};

    do {
        str >> token;

        if (token == "end")
            break;

        node.children.push_back(ParseSceneNode(str));
    } while (token == "node");

    return node;
}

void MOSDReader::ParseScene()
{
    Parse<MOSDScene>([this](const std::string& token, auto& scene) {
        if (token == "usecamera") {
            m_fileStr >> scene.camera;
        }
        else if (token == "uselight") {
            m_fileStr >> scene.light;
        }
        else if (token == "node") {
            scene.nodes.push_back(scene.ParseSceneNode(m_fileStr));
        }
        });
}

void MOSDReader::ParseNode()
{
    Parse<MOSDNode>([this](const std::string& token, MOSDNode& node) {
        if (token == "usematrix") {
            m_fileStr >> node.matrix;
        }
        else if (token == "usegeom") {
            m_fileStr >> node.geometry;
        }
        });
}

void MOSDReader::ParseCamera()
{
    Parse<MOSDCamera>([this](const std::string& token, MOSDCamera& camera) {
        if (token == "eye") {
            camera.eye = ReadVec<float, 3>(m_fileStr);
        }
        else if (token == "target") {
            camera.target = ReadVec<float, 3>(m_fileStr);
        }
        else if (token == "up") {
            camera.up = ReadVec<float, 3>(m_fileStr);
        }
        });
}

void MOSDReader::ParseLight()
{
    Parse<MOSDLight>([this](const std::string& token, MOSDLight& light) {
        if (token == "color") {
            auto color = ReadVec<float, 3>(m_fileStr);
            light.color = DirectX::XMVectorSet(color.data[0], color.data[1], color.data[2], 1);
        }
        else if (token == "position") {
            light.position = ReadVec<float, 3>(m_fileStr);
        }
        });
}

void MOSDReader::ParseGeometry()
{
    Parse<MOSDGeometry>([this](const std::string& token, MOSDGeometry& geometry) {
        if (token == "source") {
            m_fileStr >> geometry.source;
        }
        else if (token == "object") {
            m_fileStr >> geometry.object;
        }
        });
}

void MOSDReader::ParseMatrix()
{
    Parse<MOSDMatrix>([this](const std::string& token, MOSDMatrix& matrix) {
        if (token == "data") {
            matrix.data = Matrix4x4(ReadVec<float, 16>(m_fileStr).data.data());
        } 
        });
}

Scene MOSDReader::MOSDScene::Instance(const MOSDReader& reader) const
{
    Scene scene{};

    const auto &mosdcamera = reader.m_cameras.at(camera);
    scene.SetupCamera(mosdcamera.eye, mosdcamera.target, mosdcamera.up);

    const auto& mosdlight = reader.m_lights.at(light);
    scene.SetupLight(mosdlight.position, mosdlight.color);

    for (const auto& node : nodes)
        scene.AddNode(node.Instance(reader));

    return scene;
}

std::unique_ptr<Mesh> MOSDReader::MOSDGeometry::Instance(fs::path workingDir) const
{
    OBJReader objreader(workingDir.append(source).string());
    objreader.Read();

    const auto& meshes = objreader.GetMeshes();

    return std::make_unique<Mesh>(meshes.at(object));
}

std::unique_ptr<MeshNode> MOSDReader::MOSDNode::Instance(const MOSDReader& reader) const
{
    MeshNode meshNode(std::move(reader.m_geometries.at(geometry).Instance(reader.workingDir)));
    meshNode.SetName(name);
    meshNode.Transform(reader.m_matrices.at(matrix).data);
    return std::make_unique<MeshNode>(std::move(meshNode));
}

std::unique_ptr<MeshNode> MOSDReader::MOSDScene::Node::Instance(const MOSDReader& reader) const
{
    auto node = reader.m_nodes.at(name).Instance(reader);

    for (const auto& child : children)
        node->AddChild(std::move(child.Instance(reader)));
    
    return node;
}
