#ifndef O2JAM_INSTRUMENT_SELECTOR_LOADER_HPP
#define O2JAM_INSTRUMENT_SELECTOR_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Waiting/InstrumentSelector.hpp>

class InstrumentSelectorLoader : public ResourceLoader<InstrumentSelector>
{
public :
    InstrumentSelectorLoader() = default;

    Gx::ResourcePtr<InstrumentSelector> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<InstrumentSelector> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
