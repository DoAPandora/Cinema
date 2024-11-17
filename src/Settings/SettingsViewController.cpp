#include "Settings/SettingsViewController.hpp"
#include "Screen/PlaybackController.hpp"
#include "ModConfig.hpp"
#include "VideoMenu/VideoMenu.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"

#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/Texture2D.hpp"

#include <cstddef>
#include <string>

DEFINE_TYPE(Cinema, SettingsViewController);

namespace Cinema
{
    void SettingsViewController::SetSettingsTexture()
    {
        auto texture = UnityEngine::Texture2D::New_ctor(2, 2);
        UnityEngine::ImageConversion::LoadImage(texture, IncludedAssets::beat_saber_logo_landscape_png);
        PlaybackController::get_instance()->videoPlayer->SetStaticTexture(texture);
    }

    void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if(firstActivation)
        {
            BSML::parse_and_construct(IncludedAssets::Settings_bsml, transform, this);
        }

        if(!getModConfig().enabled.GetValue())
        {
            return;
        }

        PlaybackController::get_instance()->StopPlayback();
        PlaybackController::get_instance()->videoPlayer->FadeIn(FADE_DURATION);
        SetSettingsTexture();

        if(!getModConfig().transparencyEnabled.GetValue())
        {
            PlaybackController::get_instance()->videoPlayer->ShowScreenBody();
        }
    }

    void SettingsViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        PlaybackController::get_instance()->videoPlayer->FadeOut(FADE_DURATION);
        PlaybackController::get_instance()->videoPlayer->SetDefaultMenuPlacement();
    }

    bool SettingsViewController::get_showVideo()
    {
        return getModConfig().enabled.GetValue();
    }

    void SettingsViewController::set_showVideo(bool value)
    {
        if(value)
        {
            SetSettingsTexture();
            PlaybackController::get_instance()->videoPlayer->FadeIn(FADE_DURATION);
        } else
        {
            PlaybackController::get_instance()->videoPlayer->FadeOut(FADE_DURATION);
            VideoMenu::get_instance()->HandleDidSelectLevel(nullptr);
        }
        getModConfig().enabled.SetValue(value);
    }

    bool SettingsViewController::get_overrideEnvironment()
    {
        return getModConfig().overrideEnvironment.GetValue();
    }

    void SettingsViewController::set_overrideEnvironment(bool value)
    {
        getModConfig().overrideEnvironment.SetValue(value);
    }

    bool SettingsViewController::get_enable360Rotation()
    {
        return getModConfig().enable360Rotation.GetValue();
    }

    void SettingsViewController::set_enable360Rotation(bool value)
    {
        getModConfig().enable360Rotation.SetValue(value);
    }

    int SettingsViewController::get_bloomIntensity()
    {
        return getModConfig().bloomIntensity.GetValue();
    }

    void SettingsViewController::set_bloomIntensity(int value)
    {
        getModConfig().bloomIntensity.SetValue(value);
    }

    int SettingsViewController::get_cornerRoundness()
    {
        return getModConfig().cornerRoundness.GetValue() * 100;
    }

    void SettingsViewController::set_cornerRoundness(int value)
    {
        getModConfig().cornerRoundness.SetValue((float)value / 100.0f);
        PlaybackController::get_instance()->videoPlayer->screenController->SetVigenette();
    }

    bool SettingsViewController::get_curvedScreen()
    {
        return getModConfig().curvedScreen.GetValue();
    }

    void SettingsViewController::set_curvedScreen(bool value)
    {
        getModConfig().curvedScreen.SetValue(value);
        if(getModConfig().enabled.GetValue())
        {
            SetSettingsTexture();
        }
    }

    bool SettingsViewController::get_transparencyEnabled()
    {
        return getModConfig().transparencyEnabled.GetValue();
    }

    void SettingsViewController::set_transparencyEnabled(bool value)
    {
        getModConfig().transparencyEnabled.SetValue(value);
        if(value)
        {
            PlaybackController::get_instance()->videoPlayer->HideScreenBody();
        } else
        {
            PlaybackController::get_instance()->videoPlayer->ShowScreenBody();
        }
    }

    bool SettingsViewController::get_colorBlendingEnabled()
    {
        return getModConfig().colorBlendingEnabled.GetValue();
    }

    void SettingsViewController::set_colorBlendingEnabled(bool value)
    {
        getModConfig().colorBlendingEnabled.SetValue(value);
        PlaybackController::get_instance()->videoPlayer->screenController->EnableColorBlending(value);
    }

    bool SettingsViewController::get_coverEnabled()
    {
        return getModConfig().coverEnabled.GetValue();
    }

    void SettingsViewController::set_coverEnabled(bool value)
    {
        getModConfig().coverEnabled.SetValue(value);
    }

    StringW SettingsViewController::get_qualityMode()
    {
        return std::to_string(getModConfig().qualityMode.GetValue());
    }

    void SettingsViewController::set_qualityMode(StringW value)
    {
        getModConfig().qualityMode.SetValue(std::stoi(static_cast<std::string>(value).substr(0, value->Length - 1)));
    }
}