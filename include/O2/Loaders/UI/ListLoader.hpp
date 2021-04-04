#ifndef REPEATER_LOADER_HPP
#define REPEATER_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>
#include <Genode/UI/List.hpp>

class ListLoader : public O2JamResourceLoader<Gx::List>
{
public :
    ListLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::List> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

};

#endif