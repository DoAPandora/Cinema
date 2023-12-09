#include "main.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/CustomBeatmapLevel.hpp"

#include "Util/Collections.hpp"

using namespace GlobalNamespace;

MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self)
{
    StandardLevelDetailView_RefreshContent(self);

    auto cast = il2cpp_utils::try_cast<CustomBeatmapLevel>(self->level);
    auto level = cast.has_value() ? cast.value() : nullptr;
    if (!level)
    {
        return;
    }

    auto songData = Collections::RetrieveExtraSongData(static_cast<std::string>(level->levelID));
    if (!songData.has_value())
    {
        DEBUG("Song Data was empty");
        return;
    }

    auto difficultyData = Collections::RetrieveDifficultyData(self->selectedDifficultyBeatmap);
    if (!difficultyData.has_value())
    {
        DEBUG("Difficulty Data was empty");
        return;
    }

    if (!difficultyData->HasCinemaRequirement())
    {
        return;
    }

}