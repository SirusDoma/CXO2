#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/LabelMetadata.hpp>

namespace Cx
{
    class LabelLoader : public ResourceLoader<Label>
    {
    public :
        LabelLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Label> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Label> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

        static bool ParseMetadata(const Gx::Json& attributes, LabelMetadata& metadata, const Gx::ResourceContext& context);
    };
}
