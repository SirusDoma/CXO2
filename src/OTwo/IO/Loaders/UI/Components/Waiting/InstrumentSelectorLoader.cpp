#include <OTwo/IO/Loaders/UI/Components/Waiting/InstrumentSelectorLoader.hpp>
#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>

Gx::ResourcePtr<InstrumentSelector> InstrumentSelectorLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    const auto baseLoader = UiContainerLoader();
    if (const auto container = baseLoader.LoadFromJson(json, context); container)
        return std::make_unique<InstrumentSelector>(std::move(*container));

    return nullptr;
}

Gx::ResourcePtr<InstrumentSelector> InstrumentSelectorLoader::LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    const auto baseLoader = UiContainerLoader();
    if (const auto container = baseLoader.LoadFromMetadata(metadata, context); container)
        return std::make_unique<InstrumentSelector>(std::move(*container));

    return nullptr;
}

