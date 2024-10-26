#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/UI/CursorMetadata.hpp>

class CursorLoader : public ResourceLoader<Gx::Cursor>
{
public :
    CursorLoader() = default;

    Gx::ResourcePtr<Gx::Cursor> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::Cursor> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
