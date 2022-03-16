
#include "UI/Other/ScoreSaberLeaderboardView.hpp"

#include "CustomTypes/Components/LeaderboardScoreInfoButtonHandler.hpp"
#include "Data/InternalLeaderboard.hpp"
#include "Data/Score.hpp"

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "GlobalNamespace/PlatformLeaderboardViewController.hpp"
#include "GlobalNamespace/PlatformLeaderboardsHandler.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_GetScoresCompletionHandler.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_GetScoresResult.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_LeaderboardScore.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"

#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/IconSegmentedControl.hpp"
#include "HMUI/IconSegmentedControl_DataItem.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/Screen.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

#include "Services/LeaderboardService.hpp"
#include "Services/PlayerService.hpp"

#include "Sprites.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UI/FlowCoordinators/ScoreSaberFlowCoordinator.hpp"
#include "UI/Other/Banner.hpp"
#include "UI/Other/PanelView.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "Utils/StringUtils.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "logging.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/QuestUI.hpp"

using namespace HMUI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;
using namespace ScoreSaber;
using namespace StringUtils;
using namespace ScoreSaber::CustomTypes;
using namespace ScoreSaber::UI::FlowCoordinators;
using namespace ScoreSaber::Services;

namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView
{
    ScoreSaber::UI::Other::PanelView* view;
    ScoreSaber::UI::Other::Banner* scoreSaberBanner;
    ScoreSaber::CustomTypes::Components::LeaderboardScoreInfoButtonHandler* leaderboardScoreInfoButtonHandler;

    PlatformLeaderboardViewController* _platformLeaderboardViewController;

    UnityEngine::UI::Button* _pageUpButton;
    UnityEngine::UI::Button* _pageDownButton;
    UnityEngine::UI::Button* _playButton;

    bool _activated = false;

    int _lastCell = 0;
    int _leaderboardPage = 1;
    bool _filterAroundCountry = false;
    std::string _currentLeaderboardRefreshId;

    void ResetPage()
    {
        _leaderboardPage = 1;
    }

    void DidActivate(PlatformLeaderboardViewController* self, bool firstActivation, bool addedToHeirarchy,
                     bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            StandardLevelDetailViewController* _standardLevelDetailViewController = ArrayUtil::First(Resources::FindObjectsOfTypeAll<StandardLevelDetailViewController*>());
            _playButton = _standardLevelDetailViewController->standardLevelDetailView->actionButton;

            _platformLeaderboardViewController = self;
            Sprite* globalLeaderboardIcon = self->globalLeaderboardIcon;
            Sprite* friendsLeaderboardIcon = self->friendsLeaderboardIcon;
            Sprite* aroundPlayerLeaderboardIcon = self->aroundPlayerLeaderboardIcon;
            Sprite* countryLeaderboardIcon = Base64ToSprite(country_base64);
            countryLeaderboardIcon->get_textureRect().set_size({64.0f, 64.0f});

            IconSegmentedControl* scopeSegmentedControl = self->scopeSegmentedControl;

            ::Array<IconSegmentedControl::DataItem*>* array = ::Array<IconSegmentedControl::DataItem*>::New({
                IconSegmentedControl::DataItem::New_ctor(globalLeaderboardIcon, StrToIl2cppStr("Global")),
                IconSegmentedControl::DataItem::New_ctor(aroundPlayerLeaderboardIcon, StrToIl2cppStr("Around You")),
                IconSegmentedControl::DataItem::New_ctor(friendsLeaderboardIcon, StrToIl2cppStr("Friends")),
                IconSegmentedControl::DataItem::New_ctor(countryLeaderboardIcon, StrToIl2cppStr("Country")),
            });

            scopeSegmentedControl->SetData(array);

            // Page Buttons

            if (!_pageUpButton)
            {
                _pageUpButton = QuestUI::BeatSaberUI::CreateUIButton(self->get_transform(), "", "SettingsButton", Vector2(-40.0f, 20.0f), Vector2(5.0f, 5.0f),
                                                                     [=]() {
                                                                         DirectionalButtonClicked(PageDirection::Up);
                                                                     });

                QuestUI::BeatSaberUI::SetButtonSprites(_pageUpButton, Base64ToSprite(carat_up_inactive_base64),
                                                       Base64ToSprite(carat_up_base64));

                RectTransform* rectTransform = reinterpret_cast<RectTransform*>(_pageUpButton->get_transform()->GetChild(0));
                rectTransform->set_sizeDelta({10.0f, 10.0f});
            }

            if (!_pageDownButton)
            {
                _pageDownButton = QuestUI::BeatSaberUI::CreateUIButton(self->get_transform(), "", "SettingsButton", Vector2(-40.0f, -20.0f), Vector2(5.0f, 5.0f),
                                                                       [=]() {
                                                                           DirectionalButtonClicked(PageDirection::Down);
                                                                       });

                QuestUI::BeatSaberUI::SetButtonSprites(_pageDownButton, Base64ToSprite(carat_down_inactive_base64),
                                                       Base64ToSprite(carat_down_base64));
                RectTransform* rectTransform = reinterpret_cast<RectTransform*>(_pageDownButton->get_transform()->GetChild(0));
                rectTransform->set_sizeDelta({10.0f, 10.0f});
            }

            // RedBrumbler top panel

            scoreSaberBanner = ::ScoreSaber::UI::Other::Banner::Create(self->get_transform());
            auto playerProfileModal = ::ScoreSaber::UI::Other::PlayerProfileModal::Create(self->get_transform());
            scoreSaberBanner->playerProfileModal = playerProfileModal;

            scoreSaberBanner->Prompt("Signing into ScoreSaber...", false, 5.0f, nullptr);
            auto newGo = GameObject::New_ctor();
            auto t = newGo->get_transform();
            t->get_transform()->SetParent(self->get_transform(), false);
            t->set_localScale({1, 1, 1});
            leaderboardScoreInfoButtonHandler = newGo->AddComponent<ScoreSaber::CustomTypes::Components::LeaderboardScoreInfoButtonHandler*>();
            leaderboardScoreInfoButtonHandler->Setup();
            leaderboardScoreInfoButtonHandler->scoreInfoModal->playerProfileModal = playerProfileModal;
            leaderboardScoreInfoButtonHandler = leaderboardScoreInfoButtonHandler;
            leaderboardScoreInfoButtonHandler->set_buttonCount(0);

            _activated = true;

            PlayerService::AuthenticateUser([&](PlayerService::LoginStatus loginStatus) {
                switch (loginStatus)
                {
                    case PlayerService::LoginStatus::Success: {
                        scoreSaberBanner->Prompt("<color=#89fc81>Successfully signed in to ScoreSaber</color>", false, 5.0f,
                                                 nullptr);
                        _platformLeaderboardViewController->Refresh(true, true);
                        break;
                    }
                    case PlayerService::LoginStatus::Error: {
                        scoreSaberBanner->Prompt("<color=#89fc81>Authentication failed</color>", false, 5.0f, nullptr);
                        break;
                    }
                }
            });
        }
    }

    void RefreshLeaderboard(IDifficultyBeatmap* difficultyBeatmap, LeaderboardTableView* tableView,
                            PlatformLeaderboardsModel::ScoresScope scope, LoadingControl* loadingControl,
                            std::string refreshId)
    {
        // UMBY: Check uploading
        if (!_activated)
        {
            return;
        }

        // _playButton->get_gameObject()->set_active(false);

        leaderboardScoreInfoButtonHandler->set_buttonCount(0);
        if (PlayerService::playerInfo.loginStatus == PlayerService::LoginStatus::Error)
        {
            SetErrorState(loadingControl, "ScoreSaber authentication failed, please restart Beat Saber", false);
            // _playButton->get_gameObject()->set_active(true);
            return;
        }

        if (PlayerService::playerInfo.loginStatus != PlayerService::LoginStatus::Success)
        {
            return;
        }

        // UMBY: Implement delay to prevent leaderboard spam
        // if (_currentLeaderboardRefreshId == refreshId)
        // {
        LeaderboardService::GetLeaderboardData(
            difficultyBeatmap, scope, _leaderboardPage,
            [=](Data::InternalLeaderboard internalLeaderboard) {
                QuestUI::MainThreadScheduler::Schedule([=]() {
                    if (internalLeaderboard.leaderboard.has_value())
                    {
                        int playerScoreIndex = GetPlayerScoreIndex(internalLeaderboard.leaderboard.value().scores);
                        if (internalLeaderboard.leaderboardItems->get_Count() != 0)
                        {
                            if (scope == PlatformLeaderboardsModel::ScoresScope::AroundPlayer && playerScoreIndex == -1)
                            {
                                SetErrorState(loadingControl, "You haven't set a score on this leaderboard", true);
                            }
                            else
                            {
                                tableView->SetScores(internalLeaderboard.leaderboardItems, playerScoreIndex);
                                loadingControl->ShowText(System::String::_get_Empty(), false);
                                loadingControl->Hide();
                                leaderboardScoreInfoButtonHandler->set_scoreCollection(internalLeaderboard.leaderboard.value().scores);
                                // UMBY: If upload daemon is uploading, disable panel view
                            }
                        }
                        else
                        {
                            if (_leaderboardPage > 1)
                            {
                                SetErrorState(loadingControl, "No scores on this page");
                            }
                            else
                            {
                                SetErrorState(loadingControl, "No scores on this leaderboard, be the first!");
                            }
                        }
                    }
                });
            },
            _filterAroundCountry);
        // }
    }

    int GetPlayerScoreIndex(std::vector<Data::Score> scores)
    {

        // INFO("Local playerId: %s", PlayerService::playerInfo.localPlayerData.id.c_str());

        for (int i = 0; i < scores.size(); i++)
        {
            // INFO("playerId on leaderboard: %s", scores[i].leaderboardPlayerInfo.id->c_str());
            if (scores[i].leaderboardPlayerInfo.id == PlayerService::playerInfo.localPlayerData.id)
            {
                return i;
            }
        }
        return -1;
    }

    void SetErrorState(LoadingControl* loadingControl, std::string errorText, bool showRefreshButton)
    {
        loadingControl->Hide();
        loadingControl->ShowText(il2cpp_utils::newcsstr(errorText), showRefreshButton);
        // UMBY: set play button active
    }

    void ChangeScope(bool filterAroundCountry)
    {
        if (_activated)
        {
            INFO("Resetting leaderboard page");
            _leaderboardPage = 1;
            _filterAroundCountry = filterAroundCountry;
            _platformLeaderboardViewController->Refresh(true, true);
            CheckPage();
        }
    }

    void CheckPage()
    {
        if (_leaderboardPage > 1)
        {
            _pageUpButton->set_interactable(true);
        }
        else
        {
            _pageUpButton->set_interactable(false);
        }
    }

    void DirectionalButtonClicked(PageDirection direction)
    {
        switch (direction)
        {
            case PageDirection::Up: {
                _leaderboardPage--;
                break;
            }
            case PageDirection::Down: {
                _leaderboardPage++;
                break;
            }
        }
        _platformLeaderboardViewController->Refresh(true, true);
        CheckPage();
    }
} // namespace ScoreSaber::UI::Other::ScoreSaberLeaderboardView