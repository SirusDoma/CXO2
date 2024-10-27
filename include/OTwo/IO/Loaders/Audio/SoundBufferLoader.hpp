#pragma once

#include <Genode/IO/ResourceLoader.hpp>

#include <SFML/Audio/SoundBuffer.hpp>

class SoundBufferLoader final : public Gx::ResourceLoader<sf::SoundBuffer>
{
public:
    SoundBufferLoader() = default;

    Gx::ResourcePtr<sf::SoundBuffer> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<sf::SoundBuffer> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<sf::SoundBuffer> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
};
