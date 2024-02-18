#ifndef O2JAM_BUTTON_LOADER_HPP
#define O2JAM_BUTTON_LOADER_HPP

#include <Genode/UI/Button.hpp>

#include <OTwo/Loaders/O2JamResourceLoader.hpp>
#include <OTwo/Metadata/UI/ButtonMetadata.hpp>

class ButtonLoader : public O2JamResourceLoader<Gx::Button>
{
public :
    ButtonLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Button> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

    static void ParseButton(Json attributes, std::unordered_map<std::string, Gx::Button::State> stateMap, ButtonMetadata& metadata);
};

#endif