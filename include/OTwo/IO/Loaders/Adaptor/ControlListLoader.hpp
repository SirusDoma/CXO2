#pragma once
#include <Genode/IO/ResourceLoader.hpp>

struct ControlList;

class ControlListLoader : public Gx::ResourceLoader<ControlList>
{
public:
    ControlListLoader() = default;
    explicit ControlListLoader(bool mapBounds);

    Gx::ResourcePtr<ControlList> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<ControlList> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<ControlList> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;

private:
    static Gx::ResourcePtr<ControlList> Parse(std::istringstream source, bool mapBounds, const Gx::ResourceContext& ctx);

    bool m_mapBounds = true;
};
