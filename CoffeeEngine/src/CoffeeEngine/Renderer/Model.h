#pragma once

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Renderer/Material.h"
#include "CoffeeEngine/Renderer/Mesh.h"
#include "CoffeeEngine/Renderer/Texture.h"
#include "CoffeeEngine/Scene/Scene.h"
#include "CoffeeEngine/IO/ResourceLoader.h"
#include "CoffeeEngine/IO/Serialization/GLMSerialization.h"
#include <assimp/scene.h>
#include <cereal/access.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <filesystem>
#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Coffee {

    /**
     * @defgroup renderer Renderer
     * @brief Renderer components of the CoffeeEngine.
     * @{
     */
    struct BoneInfo
    {
        int id;
        glm::mat4 offset;               
    };
    /**
     * @brief Class representing a 3D model.
     */


    class Model : public Resource, public std::enable_shared_from_this<Model>
    {
    public:
        /**
         * @brief Default constructor for the Model class.
         */
        Model() : Resource(ResourceType::Model) {};

        /**
         * @brief Constructs a Model from a file path.
         * @param filePath The file path to the model.
         */
        Model(const std::filesystem::path& path);

        /**
         * @brief Gets the meshes of the model.
         * @return A reference to the vector of meshes.
         */
        const std::vector<Ref<Mesh>>& GetMeshes() const { return m_Meshes; };

        /**
         * @brief Adds a mesh to the model.
         * @param mesh A reference to the mesh to add.
         */
        void AddMesh(const Ref<Mesh> mesh) { m_Meshes.push_back(mesh); };

        const std::string& GetNodeName() { return m_NodeName; }

        /**
         * @brief Gets the parent model.
         * @return A pointer to the parent model.
         */
        const std::weak_ptr<Model> GetParent() const { return m_Parent; }

        /**
         * @brief Gets the children models.
         * @return A reference to the vector of children models.
         */
        const std::vector<Ref<Model>> GetChildren() const { return m_Children; }

        /**
         * @brief Gets the transformation matrix of the model.
         * @return The transformation matrix.
         */
        const glm::mat4 GetTransform() const { return m_Transform; }

        /**
        * Loads a model from the specified file path.
        *
        * @param path The path to the model file.
        */
        static Ref<Model> Load(const std::filesystem::path& path);

    private:
        /**
         * @brief Processes a mesh from the Assimp mesh and scene.
         * @param mesh The Assimp mesh.
         * @param scene The Assimp scene.
         * @return A reference to the processed mesh.
         */
        Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

        /**
         * @brief Processes a node from the Assimp node and scene.
         * @param node The Assimp node.
         * @param scene The Assimp scene.
         */
        void processNode(aiNode* node, const aiScene* scene);

        /**
         * @brief Loads a texture from the Assimp material and texture type.
         * @param material The Assimp material.
         * @param type The Assimp texture type.
         * @return A reference to the loaded texture.
         */
        Ref<Texture2D> LoadTexture2D(aiMaterial* material, aiTextureType type);

        /**
         * @brief Loads material textures from the Assimp material.
         * @param material The Assimp material.
         * @return The loaded material textures.
         */
        MaterialTextures LoadMaterialTextures(aiMaterial* material);

        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        
        int& GetBoneCount() { return m_BoneCounter; } 

        void ExtractBoneWeightForVertices(std::vector <Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

        void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
        
        friend class cereal::access;
        template<class Archive>
        void save(Archive& archive) const
        {
            // convert this to UUIDs
            std::vector<UUID> meshUUIDs;
            for (const auto& mesh : m_Meshes)
            {
                meshUUIDs.push_back(mesh->GetUUID());
            }
            archive(meshUUIDs, m_Parent, m_Children, m_Transform, m_NodeName, cereal::base_class<Resource>(this));
        }
        template<class Archive>
        void load(Archive& archive)
        {
            std::vector<UUID> meshUUIDs;
            archive(meshUUIDs, m_Parent, m_Children, m_Transform, m_NodeName, cereal::base_class<Resource>(this));
            for (const auto& meshUUID : meshUUIDs)
            {
                m_Meshes.push_back(ResourceLoader::LoadMesh(meshUUID));
            }
        }



    private:
        std::vector<Ref<Mesh>> m_Meshes; ///< The meshes of the model.

        std::weak_ptr<Model> m_Parent; ///< The parent model.
        std::vector<Ref<Model>> m_Children; ///< The children models.

        glm::mat4 m_Transform; ///< The transformation matrix of the model.

        std::string m_NodeName; ///< The name of the node.

        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;
    };





    /** @} */
}

CEREAL_REGISTER_TYPE(Coffee::Model);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Coffee::Resource, Coffee::Model);