#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/SelectMusicDialogMetadata.hpp>
#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>

namespace Cx
{
    class SelectMusicDialogLoader : public ResourceLoader<SelectMusicDialog>
    {
    public :
        SelectMusicDialogLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<SelectMusicDialog> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<SelectMusicDialog> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
