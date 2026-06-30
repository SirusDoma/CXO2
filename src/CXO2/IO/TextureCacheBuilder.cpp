#include <CXO2/IO/TextureCacheBuilder.hpp>
#include <CXO2/IO/Loaders/Adaptor/O2JamSpriteLoader.hpp>
#include <CXO2/IO/Loaders/Adaptor/ControlListLoader.hpp>

#include <CXO2/Metadata/Legacy/ControlList.hpp>
#include <CXO2/Metadata/Avatar/ItemData.hpp>
#include <CXO2/Metadata/Avatar/SetInfoData.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <fmt/format.h>
#include <future>

namespace Cx
{
    TextureCacheBuilder::TextureCacheBuilder(Gx::Archive& archive, Gx::ResourceManager& resources) :
        m_archive(archive),
        m_resources(resources)
    {
    }

    void TextureCacheBuilder::BuildCache() const
    {
        if (m_archive.Contains("ControlList_Interface.txt"))
            BuildInterfaceCache();
        else if (m_archive.Contains("ControlList_Playing.txt"))
            BuildPlayingCache();
        else if (!m_archive.Scan("itemdata*.dat", false).empty())
            BuildItemCache();
        else
            throw Gx::NotSupportedException("Specified archive is not supported for cache");
    }

    void TextureCacheBuilder::BuildInterfaceCache() const
    {
        if (!m_archive.Contains("ControlList_Interface.txt"))
            throw new Gx::InvalidOperationException("Cannot cache interface archive without 'ControlList_Interface.txt'");

        if (!Gx::FileSystem::IsMounted(m_archive))
            throw new Gx::InvalidOperationException("Cannot cache interface without mounting to the file system");

        BuildControlsCache("ControlList_Interface.txt");
    }

    void TextureCacheBuilder::BuildPlayingCache() const
    {
        if (!m_archive.Contains("ControlList_Playing.txt"))
            throw new Gx::InvalidOperationException("Cannot cache playing archive without 'ControlList_Playing.txt'");

        if (!Gx::FileSystem::IsMounted(m_archive))
            throw new Gx::InvalidOperationException("Cannot cache playing interface without mounting to the file system");

        BuildControlsCache("ControlList_Playing.txt", false);
    }

    void TextureCacheBuilder::BuildItemCache() const
    {
        if (!Gx::FileSystem::IsMounted(m_archive))
            throw new Gx::InvalidOperationException("Cannot cache avatar interface without mounting to the file system");

        const auto itemDataFileName = m_archive.Scan("itemdata*.dat", false).front()->GetName();
        if (itemDataFileName.empty())
            throw new Gx::InvalidOperationException("Cannot cache avatar archive without 'ItemData.dat'");

        BuildTextures();

        m_resources.AddFromFile<ItemData>(itemDataFileName);
        if (const auto scan = m_archive.Scan("setinfodata.*", false); !scan.empty())
        {
            const auto setInfoDataFileName = scan.front()->GetName();
            m_resources.AddFromFile<SetInfoData>(setInfoDataFileName);
        }
    }

    void TextureCacheBuilder::BuildTextures() const
    {
        auto tasks = std::vector<std::future<std::pair<std::string, Gx::ResourcePtr<SpriteSheet>>>>();
        for (const auto& item : m_archive.GetFileEntries())
        {
            bool supported = false;
            for (const auto& format : { ".ojs", ".oji", ".ojt", ".oja" })
            {
                if (Gx::StringHelper::EndsWith(item->GetName(), format))
                {
                    supported = true;
                    break;
                }
            }

            if (!supported)
                continue;

            const auto& fileName = item->GetName();
            tasks.push_back(std::async(std::launch::async, [&, fileName]
            {
                const auto loader = O2JamSpriteLoader();
                return std::make_pair(fileName, loader.LoadFromFile(fileName, Gx::ResourceContext(fileName, m_resources)));
            }));
        }

        for (auto& task : tasks)
        {
            auto result = std::move(task.get());
            result.second->GetTexture();

            m_resources.Store(result.first, std::move(result.second));
        }
    }

    void TextureCacheBuilder::BuildControlsCache(const std::string& controlListName, const bool mapBounds) const
    {
        BuildTextures();

        const auto controls = m_resources.AddFromDeserializer<ControlList>(controlListName, [&, this]
        {
            const auto ctx = Gx::ResourceContext(controlListName, m_resources);
            const auto loader = ControlListLoader(mapBounds);

            return loader.LoadFromFile(controlListName, ctx);
        });

        for (const auto& state : controls.States)
        {
            for (const auto& [id, bound] : state.GetBounds())
            {
                auto texCoords = std::make_unique<sf::IntRect>(
                    sf::Vector2{
                        static_cast<int>(bound.X1),
                        static_cast<int>(bound.Y1)
                    },
                    sf::Vector2{
                        static_cast<int>(bound.X2 - bound.X1),
                        static_cast<int>(bound.Y2 - bound.Y1)
                    }
                );

                m_resources.Store<sf::IntRect>(fmt::format("{:#08X}", id), std::move(texCoords));
            }

            for (auto [id, control] : state.GetChildren())
            {
                const auto& fileName = control.GetParam();
                if (fileName.empty() || (!Gx::FileSystem::Contains(fileName) && !Gx::StringHelper::StartsWith(fileName, "_")))
                    continue;

                m_resources.Store<ControlList::Control>(fmt::format("{:#08X}", id), control);
            }
        }
    }
}
