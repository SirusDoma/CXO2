#pragma once

#include <SFML/Audio.hpp>

#include <unordered_set>
#include <string>

namespace Gx
{
    class SoundGroup
    {
    public:
        explicit SoundGroup(const std::string& name);

        const std::string& GetName() const;
        void SetName(const std::string& name);

        sf::SoundSource::Status GetStatus() const;

        void Play() const;
        void Resume() const;
        void Pause() const;
        void Stop() const;

        void SetPlayingOffset(sf::Time timeOffset) const;

        float GetVolume() const;
        void SetVolume(float volume);

        float GetPan() const;
        void SetPan(float pan);

        bool IsEnabled() const;
        void SetEnabled(bool enable);

        void Reset(bool stop = false);

        class Iterator
        {
        public:
            using IteratorHandle = std::unordered_set<sf::SoundSource*>::iterator;

            explicit Iterator(SoundGroup& soundGroup, bool end = false);

            IteratorHandle begin() const;
            IteratorHandle end() const;

            bool             operator!=(const Iterator& other) const;
            sf::SoundSource& operator*() const;
            Iterator&        operator++();

        private:
            SoundGroup& m_soundGroup;
            IteratorHandle m_iterator;
        };

    protected:
        friend class AudioMixer;

        sf::SoundSource& Play(sf::SoundSource& source);
        bool Remove(const sf::SoundSource& source);

    private:
        std::string m_name;
        float m_volume, m_pan;
        bool m_enabled;

        std::unordered_set<sf::SoundSource*> m_sources;
    };

    SoundGroup::Iterator begin(SoundGroup& soundGroup);
    SoundGroup::Iterator end(SoundGroup& soundGroup);
}
