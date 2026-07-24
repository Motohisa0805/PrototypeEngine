#pragma once
#include "Typedefs.h"
#include "FBXImportSettings.h"


class ImportSettingsItem
{
public:

    enum class ImportTab
    {
        Model,
        Rig,
        Animation,
        Materials
    };

private:
    static filesystem::path     mCurrentFBXPath;

    static AllImportSettings    mCurrentAllImportSettings;

    static ImportTab            mCurrentTab;

    static void                 DrawModelSettings();

    static void                 DrawRigSettings();
    
    static void                 DrawAnimationSettings();
    
    static void                 DrawMaterialsSettings();

public:
    static void                 DrawFBXImportSettings(const filesystem::path& fbxPath);
};
