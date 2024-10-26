#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <SFML/Audio/Music.hpp>

class MusicLoader : public ResourceLoader<sf::Music>
{
public :
    MusicLoader() = default;

    Gx::ResourcePtr<sf::Music> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<sf::Music> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<sf::Music> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

    bool IsStreaming() const override;
};
