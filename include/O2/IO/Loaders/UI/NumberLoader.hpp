#ifndef NUMBER_LOADER_HPP
#define NUMBER_LOADER_HPP

#include <O2/IO/Loaders/O2JamResourceLoader.hpp>
#include <Genode/UI/Number.hpp>

class NumberLoader : public O2JamResourceLoader<Gx::Number>
{
public :
    NumberLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Number> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

};


#endif