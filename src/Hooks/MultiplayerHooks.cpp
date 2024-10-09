#define BS_HOOK_MATCH_UNSAFE
#include "main.hpp"
#include "Hooks/MultiplayerHooks.hpp"
#include "Util/Events.hpp"

#include "GlobalNamespace/MultiplayerLobbyConnectionController.hpp"
#include "GlobalNamespace/MultiplayerController.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"

#include "System/Collections/Generic/IReadOnlyCollection_1.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"

#include <optional>

using namespace GlobalNamespace;

static std::optional<MultiplayerLobbyConnectionController::LobbyConnectionType> connectionType;

bool Cinema::MultiplayerHooks::IsMultiplayer()
{
    return connectionType.has_value() && connectionType.value() != MultiplayerLobbyConnectionController::LobbyConnectionType::None;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerLobbyConnectionController_set_connectionType, &MultiplayerLobbyConnectionController::set_connectionType, void, MultiplayerLobbyConnectionController* self, MultiplayerLobbyConnectionController::LobbyConnectionType type)
{
    connectionType = self->get_connectionType();
    if(Cinema::MultiplayerHooks::IsMultiplayer())
    {
        return;
    }
    Cinema::Events::SetSelectedLevel(nullptr);

    MultiplayerLobbyConnectionController_set_connectionType(self, type);
}

static UnityW<MultiplayerPlayersManager> playersManager;

int Cinema::MultiplayerHooks::PlayerCount()
{
    return playersManager.isAlive() ? playersManager->get_allActiveAtGameStartPlayers()->i___System__Collections__Generic__IReadOnlyCollection_1_T_()->get_Count() : 0;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerController_StartSceneLoadSync, &MultiplayerController::StartSceneLoadSync, void, MultiplayerController* self)
{
    playersManager = self->_playersManager;
    Cinema::Events::SetSelectedLevel(nullptr);

    MultiplayerController_StartSceneLoadSync(self);
}

MAKE_AUTO_HOOK_MATCH(MultiplayerController_EndGameplay, &MultiplayerController::EndGameplay, void, MultiplayerController* self, GlobalNamespace::MultiplayerLevelCompletionResults *localPlayerResults, System::Collections::Generic::Dictionary_2<StringW, GlobalNamespace::MultiplayerLevelCompletionResults*> *otherPlayerResults)
{
    Cinema::Events::SetSelectedLevel(nullptr);

    MultiplayerController_EndGameplay(self, localPlayerResults, otherPlayerResults);
}