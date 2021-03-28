#ifndef REPEATER_LOADER_HPP
#define REPEATER_LOADER_HPP

#include <O2/IO/Loaders/O2JamResourceLoader.hpp>
#include <Genode/UI/Repeater.hpp>

class RepeaterLoader : public O2JamResourceLoader<Gx::Repeater>
{
public :
    RepeaterLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Repeater> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

};

#endif