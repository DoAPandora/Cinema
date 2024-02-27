#include "main.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/CustomBeatmapLevel.hpp"

#include "Util/Collections.hpp"
#include "Util/Events.hpp"
#include "Screen/PlaybackController.hpp"

using namespace GlobalNamespace;

MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self)
{
    StandardLevelDetailView_RefreshContent(self);

    if (Cinema::PlaybackController::get_instance()->videoConfig == nullptr)
    {
        return;
    }

    auto cast = il2cpp_utils::try_cast<CustomBeatmapLevel>(self->_level);
    auto level = cast.has_value() ? cast.value() : nullptr;
    if (!level)
    {
        return;
    }

    auto songData = Collections::RetrieveExtraSongData(static_cast<std::string>(level->levelID));
    if (!songData.has_value())
    {
        return;
    }

    auto difficultyData = Collections::RetrieveDifficultyData(self->selectedDifficultyBeatmap);
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