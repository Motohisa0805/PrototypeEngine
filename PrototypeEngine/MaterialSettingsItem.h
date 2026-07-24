#pragma once
#include "Typedefs.h"

class MaterialSettingsItem
{
private:
    static filesystem::path mCurrentMatPath;

public:
    static void DrawMatSettings(const filesystem::path& matPath);
};
