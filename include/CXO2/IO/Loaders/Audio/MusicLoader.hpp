#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <SFML/Audio/Music.hpp>

namespace Cx
{
    class MusicLoader : public ResourceLoader<sf::Music>
    {
    public :
        MusicLoader() = default;

        static void OnRegistered(const std::string& id, const Builder& builder);

        [[nodiscard]] Gx::ResourcePtr<sf::Music> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<sf::Music> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        [[nodiscard]] Gx::ResourcePtr<sf::Music> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;

        [[nodiscard]] bool IsStreaming() const override;
    };
}
