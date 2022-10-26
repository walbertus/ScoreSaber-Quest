#pragma once

#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/GameNoteController_Pool.hpp"
#include "System/IDisposable.hpp"
#include "Tweening/TimeTweeningManager.hpp"
#include "Tweening/Tween.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Vector3.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/ITickable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#define INTERFACES                                                    \
    {                                                                 \
        classof(Zenject::ITickable*), classof(::System::IDisposable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::UI, SpectateAreaController, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::UI", INTERFACES, 0, nullptr,
                                    DECLARE_INSTANCE_FIELD_DEFAULT(int, _colorID, UnityEngine::Shader::PropertyToID("Color"));
                                    DECLARE_INSTANCE_FIELD(Tweening::TimeTweeningManager*, _timeTweeningManager);
                                    DECLARE_INSTANCE_FIELD(GameNoteController::Pool*, _gameNoteControllerPool);
                                    // DidUpdatePlayerSpectatorPose

                                    DECLARE_INSTANCE_FIELD(GameNoteController*, _activeNote);
                                    DECLARE_INSTANCE_FIELD(UnityEngine::Quaternion, _initialQuaternion);
                                    DECLARE_INSTANCE_FIELD(Tweening::Tween*, _movementTween);
                                    DECLARE_INSTANCE_FIELD(Tweening::Tween*, _statusTween);
                                    DECLARE_INSTANCE_FIELD(bool, _despawned);
                                    DECLARE_CTOR(ctor, Zenject::DiContainer* diContainer, Tweening::TimeTweeningManager* timeTweeningManager);

                                    DECLARE_INSTANCE_METHOD(void, AnimateTo, StringW poseID);
                                    DECLARE_INSTANCE_METHOD(void, JumpToCallback, StringW poseID);
                                    DECLARE_INSTANCE_METHOD(void, Dismiss);

                                    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());

                                    DECLARE_INSTANCE_METHOD(void, UpdateNoteScale, UnityEngine::Vector3 scale);
                                    DECLARE_INSTANCE_METHOD(void, DespawnActiveNote);
                                    DECLARE_INSTANCE_METHOD(UnityEngine::Vector3, TryGetPose, StringW poseID);

                                    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());)

#undef INTERFACES
