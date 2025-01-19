#pragma once
#include "Animation.h"

#include <glm/glm.hpp>


class Animator
{
  public:
    Animator(Animation* Animation);

    void UpdateAnimation(float DeltaTime);

    void PlayAnimation(Animation* pAnimation);

    void CalculateBoneTransform(const AssimpNodeData* node, glm : mat4 parentTransform);

    std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

  private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};