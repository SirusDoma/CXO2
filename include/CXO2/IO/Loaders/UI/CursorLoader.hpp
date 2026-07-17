#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/UI/CursorMetadata.hpp>

namespace Cx
{
    class CursorLoader : public ResourceLoader<Gx::Cursor>
    {
    public :
        CursorLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<Gx::Cursor> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::Cursor> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
