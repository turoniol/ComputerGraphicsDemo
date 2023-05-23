#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <concepts>

#include "ReaderBase.h"
#include "FundamentalTypes.h"
#include "MeshNode.h"

class Scene;
struct Mesh;

class MOSDReader : public ReaderBase
{
public:
    MOSDReader(const std::string& file);

    Scene GetScene() const;

private:
    friend struct MOSDScene;
    friend struct MOSDNode;
    
    struct MOSDEntity {
        std::string name;
    };
    struct MOSDScene : MOSDEntity {
        struct Node {
            std::string name;
            std::vector<MOSDScene::Node> children;

            std::unique_ptr<MeshNode> Instance(const MOSDReader& reader) const;
        };

        std::string camera;
        std::string light;
        std::vector<MOSDScene::Node> nodes;

        MOSDScene::Node ParseSceneNode(std::istream& str);

        Scene Instance(const MOSDReader& reader) const;
    };
    struct MOSDNode : MOSDEntity {
        std::string geometry;
        std::string matrix;

        std::unique_ptr<MeshNode> Instance(const MOSDReader& reader) const;
    };
    struct MOSDCamera : MOSDEntity {
        Vector eye, target, up;
    };
    struct MOSDLight : MOSDEntity {
        Point position;
        Color color;
    };
    struct MOSDGeometry : MOSDEntity {
        std::string source;
        std::string object;

        std::unique_ptr<Mesh> Instance(std::filesystem::path workingDir) const;
    };
    struct MOSDMatrix : MOSDEntity {
        Matrix4x4 data;
    };

    void ParseScene();
    void ParseNode();
    void ParseCamera();
    void ParseLight();
    void ParseGeometry();
    void ParseMatrix();

    template <class T>
    using MOSDContainer = std::map<std::string, T>;

    template <class T> MOSDContainer<T>& GetContainer() {}

    template <std::derived_from<MOSDEntity> T>
    void Parse(std::function<void(const std::string&, T&)> handler) {
        T entity{};
        m_fileStr >> entity.name;

        std::string token{};

        do {
            m_fileStr >> token;
            handler(token, entity);
        } while (token != "end");

        GetContainer<T>().insert({entity.name, entity});
    };

#define CREATE_CONTAINER(Type, name) MOSDContainer<Type> name; template <> MOSDContainer<Type>& GetContainer() { return name; };
    CREATE_CONTAINER(MOSDScene, m_scenes);
    CREATE_CONTAINER(MOSDNode, m_nodes);
    CREATE_CONTAINER(MOSDCamera, m_cameras);
    CREATE_CONTAINER(MOSDLight, m_lights);
    CREATE_CONTAINER(MOSDGeometry, m_geometries);
    CREATE_CONTAINER(MOSDMatrix, m_matrices);
#undef CREATE_CONTAINER
};

