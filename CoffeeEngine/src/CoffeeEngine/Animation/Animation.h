#pragma once
#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/IO/ResourceLoader.h"
#include "CoffeeEngine/IO/Serialization/GLMSerialization.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

#include"CoffeeEngine/Renderer/Model.h"
#include "CoffeeEngine/Animation/Bone.h"
#define GLM_ENABLE_EXPERIMENTAL

namespace Coffee {
     struct BoneInfo;

    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Animation
    {
    public:
        Animation() = default;

        Bone* FindBone(const std::string& name)
        {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                [&](const Bone& bone) { return bone.GetBoneName() == name; });
            return iter != m_Bones.end() ? &(*iter) : nullptr;
        }

        float GetTicksPerSecond() const { return m_TicksPerSecond; }
        float GetDuration() const { return m_Duration; }
        const AssimpNodeData& GetRootNode() const { return m_RootNode; }
        const std::map<std::string, BoneInfo>& GetBoneIDMap() const { return m_BoneInfoMap; }

    private:

        float m_Duration = 0.0f;
        int m_TicksPerSecond = 0;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;

        friend class Model;
    };
}