#ifndef O2JAM_MUSIC_LIST_STATE_HPP
#define O2JAM_MUSIC_LIST_STATE_HPP

#include <OTwo/Metadata/Chart/O2ChartMetadata.hpp>
#include <OTwo/IO/Loaders/Chart/O2ChartMetadataLoader.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <vector>

class MusicListState
{
public:
    static const std::vector<O2ChartMetadata> &GetDefaultMusicList()
    {
        static bool loaded = false;
        if (!loaded)
        {
            auto metaLoader = O2ChartMetadataLoader();
            for (const auto &file : Gx::FileSystem::Scan("o2ma*.ojn"))
            {
                auto name = file.GetName();
                auto meta = metaLoader.LoadFromFile(file.GetName(), Gx::ResourceContext::Default);

                m_defaultList.push_back(*meta);
            }
        }

        return m_defaultList;
    };

private:
    inline static std::vector<O2ChartMetadata> m_defaultList;
};

#endif
