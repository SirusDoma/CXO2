#pragma once

#include <Genode/IO/ResourceLoader.hpp>

#include <SFML/Audio/SoundBuffer.hpp>

namespace Cx
{
    class SoundBufferLoader final : public Gx::ResourceLoader<sf::SoundBuffer>
    {
    public:
        SoundBufferLoader() = default;

        [[nodiscard]] Gx::ResourcePtr<sf::SoundBuffer> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<sf::SoundBuffer> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<sf::SoundBuffer> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
    };
}
