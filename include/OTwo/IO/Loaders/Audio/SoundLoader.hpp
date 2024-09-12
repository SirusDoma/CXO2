#ifndef O2JAM_SOUND_LOADER_HPP
#define O2JAM_SOUND_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <SFML/Audio/Sound.hpp>

class SoundLoader : public ResourceLoader<sf::Sound>
{
public :
    SoundLoader() = default;

    Gx::ResourcePtr<sf::Sound> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<sf::Sound> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<sf::Sound> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};

#endif