#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Cx
{
    class SoundLoader : public ResourceLoader<sf::Sound>
    {
    public :
        SoundLoader() = default;

        Gx::ResourcePtr<sf::Sound> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<sf::Sound> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<sf::Sound> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
    };
}
