#include "main.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"

#include "Util/Collections.hpp"
#include "Util/Events.hpp"
#include "Screen/PlaybackController.hpp"

#include "songcore/shared/SongLoader/CustomBeatmapLevel.hpp"

using namespace GlobalNamespace;

MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self)
{
    StandardLevelDetailView_RefreshContent(self);

    if (Cinema::PlaybackController::get_instance()->videoConfig == nullptr)
    {
        return;
    }

    auto cast = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(self->_beatmapLevel);
    auto level = cast.value_or(nullptr);
    if (!level)
    {
        return;
    }

    auto songData = Collections::RetrieveExtraSongData(static_cast<std::string>(level->levelID));
    if (!songData.has_value())
    {
        return;
    }

    auto difficultyData = Collections::RetrieveDifficultyData(self->beatmapKey);
    if (!difficultyData.has_value())
    {
        return;
    }

    Cinema::Events::SetExtraSongData(songData.value(), difficultyData.value());

    if (!difficultyData->HasCinemaRequirement())
    {
        return;
    }
}