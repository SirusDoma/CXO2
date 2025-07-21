#pragma once
#include <Genode/IO/ResourceLoader.hpp>
#include <OTwo/Metadata/Legacy/ControlList.hpp>

class BoundLoader : public Gx::ResourceLoader<std::vector<ControlList::Bound>>
{
public:
    Gx::ResourcePtr<std::vector<ControlList::Bound>> LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<std::vector<ControlList::Bound>> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
    Gx::ResourcePtr<std::vector<ControlList::Bound>> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
};


