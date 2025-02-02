#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <assimp/scene.h>
#include <string>
#include <vector>


namespace Coffee {
    class Bone {
    public:
        struct KeyPosition {
            glm::vec3 position;
            float timeStamp;
        };

        struct KeyRotation {
            glm::quat orientation;
            float timeStamp;
        };

        struct KeyScale {
            glm::vec3 scale;
            float timeStamp;
        };

        Bone(const std::string& name, int ID, const aiNodeAnim* channel)
            : m_Name(name), m_ID(ID), m_LocalTransform(1.0f) {
            // Position keys
            m_NumPositions = channel->mNumPositionKeys;
            for (int i = 0; i < m_NumPositions; ++i) {
                KeyPosition data;
                data.position = glm::vec3(
                    channel->mPositionKeys[i].mValue.x,
                    channel->mPositionKeys[i].mValue.y,
                    channel->mPositionKeys[i].mValue.z
                );
                data.timeStamp = channel->mPositionKeys[i].mTime;
                m_Positions.push_back(data);
            }

            // Rotation keys
            m_NumRotations = channel->mNumRotationKeys;
            for (int i = 0; i < m_NumRotations; ++i) {
                KeyRotation data;
                data.orientation = glm::quat(
                    channel->mRotationKeys[i].mValue.w,
                    channel->mRotationKeys[i].mValue.x,
                    channel->mRotationKeys[i].mValue.y,
                    channel->mRotationKeys[i].mValue.z
                );
                data.timeStamp = channel->mRotationKeys[i].mTime;
                m_Rotations.push_back(data);
            }

            // Scaling keys
            m_NumScalings = channel->mNumScalingKeys;
            for (int i = 0; i < m_NumScalings; ++i) {
                KeyScale data;
                data.scale = glm::vec3(
                    channel->mScalingKeys[i].mValue.x,
                    channel->mScalingKeys[i].mValue.y,
                    channel->mScalingKeys[i].mValue.z
                );
                data.timeStamp = channel->mScalingKeys[i].mTime;
                m_Scales.push_back(data);
            }
        }

        void Update(float animationTime) {
            m_LocalTransform = InterpolatePosition(animationTime) *
                InterpolateRotation(animationTime) *
                InterpolateScaling(animationTime);
        }

        glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
        std::string GetBoneName() const { return m_Name; }
        int GetBoneID() const { return m_ID; }

    private:
        int GetPositionIndex(float animationTime) const {
            for (int index = 0; index < m_NumPositions - 1; ++index) {
                if (animationTime < m_Positions[index + 1].timeStamp)
                    return index;
            }
            return m_NumPositions - 1;
        }

        int GetRotationIndex(float animationTime) const {
            for (int index = 0; index < m_NumRotations - 1; ++index) {
                if (animationTime < m_Rotations[index + 1].timeStamp)
                    return index;
            }
            return m_NumRotations - 1;
        }

        int GetScaleIndex(float animationTime) const {
            for (int index = 0; index < m_NumScalings - 1; ++index) {
                if (animationTime < m_Scales[index + 1].timeStamp)
                    return index;
            }
            return m_NumScalings - 1;
        }

        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const {
            float midWayLength = animationTime - lastTimeStamp;
            float framesDiff = nextTimeStamp - lastTimeStamp;
            return midWayLength / framesDiff;
        }

        glm::mat4 InterpolatePosition(float animationTime) const {
            if (m_NumPositions == 1)
                return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

            int p0Index = GetPositionIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
                m_Positions[p1Index].timeStamp, animationTime);
            glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position,
                m_Positions[p1Index].position, scaleFactor);
            return glm::translate(glm::mat4(1.0f), finalPosition);
        }

        glm::mat4 InterpolateRotation(float animationTime) const {
            if (m_NumRotations == 1) {
                auto rotation = glm::normalize(m_Rotations[0].orientation);
                return glm::toMat4(rotation);
            }

            int p0Index = GetRotationIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
                m_Rotations[p1Index].timeStamp, animationTime);
            glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation,
                m_Rotations[p1Index].orientation, scaleFactor);
            finalRotation = glm::normalize(finalRotation);
            return glm::toMat4(finalRotation);
        }

        glm::mat4 InterpolateScaling(float animationTime) const {
            if (m_NumScalings == 1)
                return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

            int p0Index = GetScaleIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
                m_Scales[p1Index].timeStamp, animationTime);
            glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale,
                m_Scales[p1Index].scale, scaleFactor);
            return glm::scale(glm::mat4(1.0f), finalScale);
        }

        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;
        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;

        glm::mat4 m_LocalTransform;
        std::string m_Name;
        int m_ID;
    };
}