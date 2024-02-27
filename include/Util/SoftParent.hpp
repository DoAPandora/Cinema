#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"

DECLARE_CLASS_CODEGEN(Cinema, SoftParent, UnityEngine::MonoBehaviour,

    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Transform>, parent);
    DECLARE_INSTANCE_FIELD(UnityEngine::Vector3, posOffset);
    DECLARE_INSTANCE_FIELD(UnityEngine::Quaternion, rotOffset);

    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, AssignParent, UnityEngine::Transform* newParent);
)