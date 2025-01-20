#pragma once


struct KeyPosition
{
    glm::vec3 m_Position;
    float m_Time;
};

struct KeyRotation
{
    glm::quat m_Rotation;
    float m_Time;
};

struct KeyScale
{
    glm::vec3 m_Scale;
    float m_Time;
};

class Bone
{
private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumnPositions;
    int m_NumRotations;
    int m_NumScalings;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;
public:
    Bone(const std:: string& name, int ID, const aiNodeAnim* channel) 
        : m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
    {
        n_NumPositions = channel.mNumPositionKeys;
        for (int positionIndex = 0; positionIndex < m_NumPositions; positionIndex++)
        {
            aiVector3D aiPosition = channel.mPositionKeys[positionIndex].mValue;
            float time = channel.mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = assimpGLMHelpers::GetGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_NumRotations = channel.mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; rotationIndex++)
        {
            aiQuaternion aiQuaternion = channel.mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel.mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.rotation = assimpGLMHelpers::GetGLMQuat(aiQuaternion);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_NumScalings = channel.mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; keyIndex++)
        {
            aiVector3D scale = channel.mScalingKeys[keyIndex].mvalue;
            float timeStamp = channel.mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = assimpGLMHelpers::GetGLMVec(scale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }
    /*interpolates  b/w positions,rotations & scaling keys based on the curren time of 
    the animation and prepares the local transformation matrix by combining all keys 
    tranformations*/

    void Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        m_LocalTransform = translation * rotation * scale;
    }

    glm::mat4 GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() const { return m_ID; }

    /* Gets the current index on mKeyPositions to interpolate to based on 
    the current animation time*/
    int GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < m_NumPositions - 1; index++)
        {
            if (m_Positions[index + 1].m_Time > animationTime)
                return index;
        }
        assert(0);
    }

    int GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < m_NumRotations - 1; index++)
        {
            if (m_Rotations[index + 1].m_Time > animationTime)
                return index;
        }
        assert(0);
    }

    int GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < m_NumScalings - 1; index++)
        {
            if (m_Scales[index + 1].m_Time > animationTime)
                return index;
        }
        assert(0);
    }

private:

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
      {
          float scaleFactor = 0.0f;
          float midWayLength = animationTime - lastTimeStamp;
          float framesDiff = nextTimeStamp - lastTimeStamp;
          scaleFactor = midWayLength / framesDiff;
          return scaleFactor;
      }

    glm::mat4 InterpolatePosition(float animationTime)
    {
        if (1 == m_NumPositions)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].m_Position);
        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Positions[p0Index].m_Time, m_Positions[p1Index].m_Time, animationTime);
        glm::vec3 finalPosition =
            glm::mix(m_Positions[p0Index].m_Position, m_Positions[p1Index].m_Position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 InterpolateRotation(float animationTime)
    {
        if (1 == m_NumRotations)
            return glm::toMat4(m_Rotations[0].m_Rotation);
        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].m_Time, m_Rotations[p1Index].m_Time, animationTime);
        glm::quat finalRotation =
            glm::slerp(m_Rotations[p0Index].m_Rotation, m_Rotations[p1Index].m_Rotation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 InterpolateScaling(float animationTime)
    {
        if (1 == m_NumScalings)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].m_Scale);
        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Scales[p0Index].m_Time, m_Scales[p1Index].m_Time, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[p0Index].m_Scale, m_Scales[p1Index].m_Scale, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }

};
    
