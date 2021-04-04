#ifndef MUSIC_LOADER_HPP
#define MUSIC_LOADER_HPP

#include <SFML/Audio/Music.hpp>
#include <O2/Loaders/O2JamResourceLoader.hpp>

class MusicLoader : public O2JamResourceLoader<sf::Music>
{
public :
    MusicLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<sf::Music> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif