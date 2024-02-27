#include "Util/SoftParent.hpp"

DEFINE_TYPE(Cinema, SoftParent);

namespace Cinema 
{
    void SoftParent::Update()
    {
        if(!parent)
        {
            return;
        }
        
        transform->SetPositionAndRotation(parent->position, parent->rotation);
        UnityEngine::Vector3 side = UnityEngine::Vector3::op_Multiply(parent->right, posOffset.x);
        UnityEngine::Vector3 forward = UnityEngine::Vector3::op_Multiply(parent->forward, posOffset.z);
        UnityEngine::Vector3 total = UnityEngine::Vector3::op_Addition(side, forward);
        total = UnityEngine::Vector3(total.x, posOffset.y, total.z);
        transform->position = UnityEngine::Vector3::op_Subtraction(transform->position, total);
        transform->rotation = UnityEngine::Quaternion::op_Multiply(transform->rotation, UnityEngine::Quaternion::Inverse(rotOffset));
    }

    void SoftParent::AssignParent(UnityEngine::Transform* newParent)
    {
        parent = newParent;
        if(!parent)
        {
            return;
        }

        posOffset = UnityEngine::Vector3::op_Subtraction(parent->position, transform->position);
        rotOffset = UnityEngine::Quaternion::op_Multiply(parent->rotation, UnityEngine::Quaternion::Inverse(transform->rotation));
    }
}