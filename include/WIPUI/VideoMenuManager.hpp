#pragma once

#include "custom-types/shared/macros.hpp"
#include "bsml/shared/macros.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

DECLARE_CLASS_CODEGEN(Cinema, VideoMenuManager, Il2CppObject,

    DECLARE_INSTANCE_METHOD(bool, DownloadVideo, std::string_view url, std::function<void(float)> status);
    DECLARE_INSTANCE_METHOD(bool, DownloadCurrentVideo);
    
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, videoTitleText);

    DECLARE_BSML_PROPERTY(bool, doesCurrentSongUseCinema);
    DECLARE_BSML_PROPERTY(bool, isCurrentSongDownloaded);

    DECLARE_CTOR(ctor);
    
    public:
        static VideoMenuManager *get_instance();
    private:
        static SafePtr<VideoMenuManager> instance;
        
        bool doesCurrentSongUseCinema;
        bool isCurrentSongDownloaded;
)