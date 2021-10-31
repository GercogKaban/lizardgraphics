#pragma once

#include <vector>
#include <unordered_map>
#include "include/glm/glm.hpp"

class aiNodeAnim;
namespace LGraphics
{
    class LResourceManager;
    struct BoneInfo
    {
        int id;
        glm::mat4 offset;
    };

    struct KeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

    struct NodeData
    {
        glm::mat4 transformation = glm::mat4(1.0f);
        std::string name;
        int childrenCount;
        std::vector<NodeData> children;
    };

    class Bone
    {
    private:
        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;
        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;

        glm::mat4 m_LocalTransform;
        std::string m_Name;
        int m_ID;

    public:

        /*reads keyframes from aiNodeAnim*/
        Bone(const std::string& name, int ID, const aiNodeAnim* channel);

        /*interpolates  b/w positions,rotations & scaling keys based on the curren time of
        the animation and prepares the local transformation matrix by combining all keys
        tranformations*/
        void Update(float animationTime);

        const glm::mat4& GetLocalTransform() const { return m_LocalTransform; }
        const std::string& GetBoneName() const { return m_Name; }
        int GetBoneID() { return m_ID; }


        /* Gets the current index on mKeyPositions to interpolate to based on
        the current animation time*/
        int GetPositionIndex(float animationTime);

        /* Gets the current index on mKeyRotations to interpolate to based on the
        current animation time*/
        int GetRotationIndex(float animationTime);

        /* Gets the current index on mKeyScalings to interpolate to based on the
        current animation time */
        int GetScaleIndex(float animationTime);

    private:

        /* Gets normalized value for Lerp & Slerp*/
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

        /*figures out which position keys to interpolate b/w and performs the interpolation
        and returns the translation matrix*/
        glm::mat4 InterpolatePosition(float animationTime);

        /*figures out which rotations keys to interpolate b/w and performs the interpolation
        and returns the rotation matrix*/
        glm::mat4 InterpolateRotation(float animationTime);

        /*figures out which scaling keys to interpolate b/w and performs the interpolation
        and returns the scale matrix*/
        glm::mat4 Bone::InterpolateScaling(float animationTime);
    };

    class Animation
    {
    public:

        friend LResourceManager;
        Animation(const Animation& anime);
        Animation(){}
        ~Animation(){}

        Bone* FindBone(const std::string& name);

        float GetTicksPerSecond() const { return m_TicksPerSecond; }
        float GetDuration() const { return m_Duration; }
        const NodeData& GetRootNode() const;
        std::unordered_map<std::string, BoneInfo>& GetBoneIDMap() {return m_BoneInfoMap;}

    private:

        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        NodeData m_RootNode;
        std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
    };

    class LModel;
    class Animator
    {
        friend LModel;
    public:

        Animator(){}
        Animator(Animation Animation);
        ~Animator();

        void Animator::UpdateAnimation(float dt);
        void Animator::PlayAnimation(Animation anime);

        const std::vector<glm::mat4>& GetFinalBoneMatrices() const {return m_FinalBoneMatrices;}

    private:

        void init();
        void Animator::CalculateBoneTransform(const NodeData* node, glm::mat4 parentTransform);
        std::vector<glm::mat4> m_FinalBoneMatrices;
        Animation m_CurrentAnimation;
        float m_CurrentTime;
        float m_DeltaTime;
        float speed = 0.00001f;
    };
}