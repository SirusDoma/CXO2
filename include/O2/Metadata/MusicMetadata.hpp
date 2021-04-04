#ifndef O2JAM_MUSIC_METADATA_HPP
#define O2JAM_MUSIC_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

class MusicMetadata : public Gx::ResourceMetadata
{
public:
    MusicMetadata() {}
    virtual ~MusicMetadata() {}

public:
    const std::string& GetSource() const { return m_source; }
    void SetSource(const std::string& source) { m_source = source; }

    bool IsLoop() const { return m_loop; }
    void SetLoop(bool loop) { m_loop = loop; }

private:
    std::string m_source;
    bool m_loop;
};

#endif