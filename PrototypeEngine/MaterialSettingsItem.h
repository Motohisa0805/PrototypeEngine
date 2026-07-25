#pragma once
#include "Typedefs.h"
#include "Material.h"

class MaterialSettingsItem
{
private:
    static filesystem::path mCurrentMatPath;

    static Material* mCurrentMaterial;

public:
    static void DrawMatSettings(const filesystem::path& matPath);
};
