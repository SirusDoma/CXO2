#pragma once

#include <CXO2/IO/ResourceLoader.hpp>

#include <Genode/UI/Dialog.hpp>
#include <CXO2/Metadata/UI/DialogMetadata.hpp>

namespace Cx
{
    class DialogLoader : public ResourceLoader<Gx::Dialog>
    {
    public :
        DialogLoader() = default;

        Gx::ResourcePtr<Gx::Dialog> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<Gx::Dialog> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
