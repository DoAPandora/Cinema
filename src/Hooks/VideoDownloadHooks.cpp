#include "main.hpp"
#include "hooks.hpp"
#include "VideoManager.hpp"

#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/FilteredBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
            
MAKE_HOOK_MATCH(StandardLevelDetailView_SetContent, &GlobalNamespace::StandardLevelDetailView::SetContent, void, GlobalNamespace::StandardLevelDetailView* self, ::GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty defaultDifficulty, GlobalNamespace::BeatmapCharacteristicSO* defaultBeatmapCharacteristic, GlobalNamespace::PlayerData* playerData)
{
	StandardLevelDetailView_SetContent(self, level, defaultDifficulty, defaultBeatmapCharacteristic, playerData);
    bool isCustom = self->level->i_IPreviewBeatmapLevel()->get_levelID()->StartsWith("custom_level_");

    std::string songpath = "";
    if(isCustom)
    {
        auto preview = level->i_IPreviewBeatmapLevel();
        GlobalNamespace::CustomPreviewBeatmapLevel* customLevel;
        if (auto filter = il2cpp_utils::try_cast<GlobalNamespace::FilteredBeatmapLevel>(level)) {
            customLevel = il2cpp_utils::cast<GlobalNamespace::CustomPreviewBeatmapLevel>(filter.value()->beatmapLevel);
        } else {
            customLevel = il2cpp_utils::cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level);
        }
        songpath = static_cast<std::string>(customLevel->customLevelPath);
    }
    Cinema::VideoManager::DidSelectLevel(isCustom, songpath);
    if(Cinema::VideoManager::GetShouldCreateScreen())
        Cinema::VideoManager::DownloadCurrentVideo();
}

void Cinema::InstallVideoDownloadHooks() {
   INSTALL_HOOK(getLogger(), StandardLevelDetailView_SetContent);
}