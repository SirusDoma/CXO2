#ifndef SOUND_LOADER_HPP
#define SOUND_LOADER_HPP

#include <SFML/Audio/Sound.hpp>
#include <O2/Loaders/O2JamResourceLoader.hpp>

class SoundLoader : public O2JamResourceLoader<sf::Sound>
{
public :
    SoundLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<sf::Sound> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif