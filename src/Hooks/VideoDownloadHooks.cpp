#include "main.hpp"
#include "Hooks/Hooks.hpp"

#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"

#include "UI/VideoMenuManager.hpp"
#include "Video/VideoLoader.hpp"

#include "pinkcore/shared/LevelDetailAPI.hpp"
            
MAKE_HOOK_MATCH(StandardLevelDetailView_SetContent, &GlobalNamespace::StandardLevelDetailView::SetContent, void, GlobalNamespace::StandardLevelDetailView* self, ::GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty defaultDifficulty, GlobalNamespace::BeatmapCharacteristicSO* defaultBeatmapCharacteristic, GlobalNamespace::PlayerData* playerData)
{
	StandardLevelDetailView_SetContent(self, level, defaultDifficulty, defaultBeatmapCharacteristic, playerData);

    auto mapData = PinkCore::API::GetCurrentMapData();
    auto menu = Cinema::VideoMenuManager::get_instance();

    bool useCinema = std::find(mapData.currentSuggestions.begin(), mapData.currentSuggestions.end(), "Cinema") != mapData.currentSuggestions.end();

    menu->doesCurrentSongUseCinema = useCinema && mapData.isCustom;

    if(useCinema)
    {
        auto config = Cinema::VideoLoader::GetConfigForLevel(level->i_IPreviewBeatmapLevel());
        menu->currentVideoConfig = config.value();
    }
    menu->UpdateMenu();
}

void Cinema::Hooks::InstallVideoDownloadHooks() {
   INSTALL_HOOK(getLogger(), StandardLevelDetailView_SetContent);
}