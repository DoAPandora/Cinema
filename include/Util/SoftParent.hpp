#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"

DECLARE_CLASS_CODEGEN(Cinema, SoftParent, UnityEngine::MonoBehaviour) {

    UnityW<UnityEngine::Transform> parent;
    UnityEngine::Vector3 posOffset;
    UnityEngine::Quaternion rotOffset;

    DECLARE_INSTANCE_METHOD(void, Update);

    public:

    void AssignParent (UnityEngine::Transform* newParent);
};