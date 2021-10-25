#include "LAnimation.h"
#include "additional.h"

#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/quaternion.hpp"
#include <assimp/scene.h>

#include "constants.h"

namespace LGraphics
{
    Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        :
        m_Name(name),
        m_ID(ID),
        m_LocalTransform(1.0f)
    {
        m_NumPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = assimpToGLM(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = assimpToGLM(aiOrientation);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = assimpToGLM(scale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }

    void Bone::Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        m_LocalTransform = translation * rotation * scale;
    }

    int Bone::GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < m_NumPositions - 1; ++index)
            if (animationTime < m_Positions[index + 1].timeStamp)
                return index;
        assert(0);
    }

    int Bone::GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < m_NumRotations - 1; ++index)
            if (animationTime < m_Rotations[index + 1].timeStamp)
                return index;
        assert(0);
    }

    int Bone::GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < m_NumScalings - 1; ++index)
            if (animationTime < m_Scales[index + 1].timeStamp)
                return index;
        assert(0);
    }

    float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::mat4 Bone::InterpolatePosition(float animationTime)
    {
        if (1 == m_NumPositions)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
            m_Positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position,
            m_Positions[p1Index].position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 Bone::InterpolateRotation(float animationTime)
    {
        if (1 == m_NumRotations)
        {
            auto rotation = glm::normalize(m_Rotations[0].orientation);
            return glm::mat4_cast(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
            m_Rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation,
            m_Rotations[p1Index].orientation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::mat4_cast(finalRotation);
    }

    glm::mat4 Bone::InterpolateScaling(float animationTime)
    {
        if (1 == m_NumScalings)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
            m_Scales[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
            , scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }

    Animation::Animation(const Animation& anime)
    {
        m_BoneInfoMap = anime.m_BoneInfoMap;
        m_Bones = anime.m_Bones;
        m_Duration = anime.m_Duration;
        m_RootNode = anime.m_RootNode;
        m_TicksPerSecond = anime.m_TicksPerSecond;
    }

    Bone* Animation::FindBone(const std::string& name)
    {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone)
        {
            return Bone.GetBoneName() == name;
        }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }

    const NodeData& Animation::GetRootNode() const
    {
        return m_RootNode;
    }

    Animator::Animator(Animation anime)
    {
        m_CurrentAnimation = anime;
        init();
    }

    Animator::~Animator()
    {
    }

    void Animator::UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        m_CurrentTime += m_CurrentAnimation.GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation.GetDuration());
        CalculateBoneTransform(&m_CurrentAnimation.GetRootNode(), glm::mat4(1.0f));
    }

    void Animator::PlayAnimation(Animation Animation)
    {
        if (!m_FinalBoneMatrices.capacity())
            init();
        m_CurrentAnimation = Animation;
    }

    void Animator::init()
    {
        m_CurrentTime = 0.0;
        m_FinalBoneMatrices.reserve(MAX_BONES);

        for (size_t i = 0; i < MAX_BONES; i++)
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void Animator::CalculateBoneTransform(const NodeData* node, glm::mat4 parentTransform)
    {
        const std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation.FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        const glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto& boneInfoMap = m_CurrentAnimation.GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            const int index = boneInfoMap[nodeName].id;
            const glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (size_t i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}
