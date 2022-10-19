#pragma once

#include "Data/Private/ReplayFile.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/BasicBeatmapObjectManager.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/MemoryPoolContainer_1.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include <map>

// std::map<GoodCutScoringElement*, float> _scoringStartInfo;

#define INTERFACES                   \
    {                                \
        classof(Zenject::ITickable*) \
    }

___DECLARE_TYPE_WRAPPER_INHERITANCE(ScoreSaber::ReplaySystem::Playback, NotePlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "ScoreSaber::ReplaySystem::Playback", INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(int, _lastIndex);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::SaberManager*, _saberManager);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::AudioTimeSyncController*, _audioTimeSyncController);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::BasicBeatmapObjectManager*, _basicBeatmapObjectManager);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::MemoryPoolContainer_1<GlobalNamespace::GameNoteController*>*, _gameNotePool);
                                    DECLARE_PRIVATE_FIELD(GlobalNamespace::MemoryPoolContainer_1<GlobalNamespace::BombNoteController*>*, _bombNotePool);

                                    DECLARE_CTOR(ctor, GlobalNamespace::SaberManager* saberManager, GlobalNamespace::AudioTimeSyncController* audioTimeSyncController, GlobalNamespace::BasicBeatmapObjectManager* basicBeatmapObjectManager);
                                    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
                                    vector<Data::Private::NoteEvent> _sortedNoteEvents;
                                    std::map<GlobalNamespace::NoteCutInfo, ScoreSaber::Data::Private::NoteEvent> _recognizedNoteCutInfos;
                                    bool ProcessEvent(Data::Private::NoteEvent activeEvent);
                                    bool HandleEvent(Data::Private::NoteEvent activeEvent, GlobalNamespace::NoteController* noteController);
                                    bool DoesNoteMatchID(Data::Private::NoteID id, GlobalNamespace::NoteData* noteData);
                                    void TimeUpdate(float newTime);

)

#undef INTERFACES
