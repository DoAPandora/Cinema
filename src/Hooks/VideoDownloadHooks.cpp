#include "main.hpp"
#include "hooks.hpp"

#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/FilteredBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"

#include "UI/VideoMenuManager.hpp"

#include "pinkcore/shared/LevelDetailAPI.hpp"
            
MAKE_HOOK_MATCH(StandardLevelDetailView_SetContent, &GlobalNamespace::StandardLevelDetailView::SetContent, void, GlobalNamespace::StandardLevelDetailView* self, ::GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty defaultDifficulty, GlobalNamespace::BeatmapCharacteristicSO* defaultBeatmapCharacteristic, GlobalNamespace::PlayerData* playerData)
{
	StandardLevelDetailView_SetContent(self, level, defaultDifficulty, defaultBeatmapCharacteristic, playerData);

    auto mapData = PinkCore::API::GetCurrentMapData();
    auto menu = Cinema::VideoMenuManager::get_instance();

    bool useCinema = std::find(mapData.currentSuggestions.begin(), mapData.currentSuggestions.end(), "Cinema") != mapData.currentSuggestions.end();

    menu->set_doesCurrentSongUseCinema(useCinema && mapData.isCustom);

    if(useCinema)
    {
        auto preview = level->i_IPreviewBeatmapLevel();
        GlobalNamespace::CustomPreviewBeatmapLevel* customLevel;
        if (auto filter = il2cpp_utils::try_cast<GlobalNamespace::FilteredBeatmapLevel>(level)) {
            customLevel = il2cpp_utils::cast<GlobalNamespace::CustomPreviewBeatmapLevel>(filter.value()->beatmapLevel);
        } else {
            customLevel = il2cpp_utils::cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level);
        }

        std::string songpath = static_cast<std::string>(customLevel->customLevelPath);
        Cinema::JSON::CinemaInfo info;
        ReadFromFile(songpath + "/cinema-video.json", info);

        if(info.videoFile.empty())
            info.videoFile = info.title + ".mp4";

        menu->set_currentLevelData(info);
    }
    menu->UpdateMenu();
}

void Cinema::Hooks::InstallVideoDownloadHooks() {
   INSTALL_HOOK(getLogger(), StandardLevelDetailView_SetContent);
}