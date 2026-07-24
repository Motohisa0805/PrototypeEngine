#include "MaterialSettingsItem.h"

filesystem::path MaterialSettingsItem::mCurrentMatPath = "";

void MaterialSettingsItem::DrawMatSettings(const filesystem::path& matPath) 
{
    if (mCurrentMatPath != matPath)
    {

        mCurrentMatPath = matPath;
    }
}
