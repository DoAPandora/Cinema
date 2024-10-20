#pragma once

#include "custom-types/shared/macros.hpp"

#include "HMUI/ViewController.hpp"

#include "bsml/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Cinema, SettingsViewController, HMUI::ViewController,

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidDeactivate, &HMUI::ViewController::DidDeactivate, bool removedFromHierarchy, bool screenSystemDisabling);

    void SetSettingsTexture();

    DECLARE_BSML_PROPERTY(bool, showVideo);
    DECLARE_BSML_PROPERTY(bool, overrideEnvironment);
    DECLARE_BSML_PROPERTY(bool, enable360Rotation);
    DECLARE_BSML_PROPERTY(int, bloomIntensity);
    DECLARE_BSML_PROPERTY(int, cornerRoundness);
    DECLARE_BSML_PROPERTY(bool, curvedScreen);
    DECLARE_BSML_PROPERTY(bool, transparencyEnabled);
    DECLARE_BSML_PROPERTY(bool, colorBlendingEnabled);
    DECLARE_BSML_PROPERTY(bool, coverEnabled);
    DECLARE_BSML_PROPERTY(StringW, qualityMode);

    static inline const float FADE_DURATION = 0.2f;
)