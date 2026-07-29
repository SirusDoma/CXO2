#pragma once

#include <CXO2/Models/Planet.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Room.hpp>
#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/Metadata/Chart/O2JamMusicList.hpp>

#include <Genode/IO/Resource.hpp>

#include <SFML/System/String.hpp>

#include <optional>
#include <unordered_set>

namespace Cx
{
    struct CharacterInfoResponse;

    enum class MusicListMode
    {
        Legacy,
        Mixed,
        Free
    };

    class SessionContext
    {
    public:
        struct Item
        {
            // ReSharper disable once CppNonExplicitConvertingConstructor
            Item(const std::uint32_t id) : ID(id) {}

            // ReSharper disable once CppNonExplicitConversionOperator
            operator std::uint32_t() const { return ID;}

            std::uint32_t ID{};
            std::uint32_t Quantity{1};
        };

        struct RankStats
        {
            std::int32_t Rank{};
            std::int32_t Wins{};
            std::int32_t Loses{};
            std::int32_t Draws{};
        };

        struct Wallet
        {
            std::uint32_t Gem{};
            std::uint32_t Cash{}; // a.k.a point a.k.a ePoint a.k.a eP
        };

        using ItemList = std::vector<Item>;

        SessionContext() = default;
        explicit SessionContext(const std::string& token);

        const std::string& GetToken() const;

        sf::String GetName() const;
        void SetName(const sf::String& name);

        Gender GetGender() const;
        void SetGender(Gender gender);

        Role GetRole() const;
        void SetRole(Role role);

        std::int32_t GetLevel() const;
        void SetLevel(std::int32_t level);

        std::int32_t GetExperience() const;
        void SetExperience(std::int32_t experience);

        RankStats GetRankStats() const;
        void SetRankStats(const RankStats& rankStats);

        Wallet GetWallet() const;
        void SetWallet(const Wallet& wallet);

        Planet GetPlanet() const;
        void SetPlanet(Planet planet);

        MusicHall GetMusicHall() const;
        void SetMusicHall(MusicHall hall);

        unsigned int GetChannelID() const;
        void SetChannelID(unsigned int channelId);

        const EquipmentSet& GetEquippedItemIDs() const;
        const ItemList& GetInventory() const;
        const MusicList& GetMusicIDs() const;

        void SetMusicListMode(MusicListMode mode);
        void SetAcquiredMusicIDs(std::unordered_set<std::uint32_t> musicIDs);

        const std::vector<ChartMetadata>& GetMusicList(bool rescan = false) const;
        const std::vector<ChartMetadata>& GetInstalledMusic(bool rescan = false) const;
        const std::vector<ChartMetadata>& GetNonPlayableMusicList() const;

        void Equip(std::uint32_t itemID);
        void Unequip(std::uint32_t itemID);
        void SetEquipment(const EquipmentSet& equippedItemIDs);
        void SetInventoryItem(std::uint32_t slotID, const Item& item);
        void AddInventoryItem(std::uint32_t itemID);

        void UpdateFrom(const CharacterInfoResponse& response);

        void Load();
        void Save() const;

    private:
        void ScanMusic() const;

        std::string m_token;

        sf::String   m_name;
        Gender       m_gender{};
        Role         m_role{};
        std::int32_t m_level{};
        std::int32_t m_experience{};
        RankStats    m_rankStats{};
        Wallet       m_wallet{};

        EquipmentSet m_equippedItemIDs;
        ItemList     m_inventory;
        MusicList    m_musicIDs;

        Planet m_planet = Planet::O2Planet;
        MusicHall m_server;
        unsigned int m_channelID;

        MusicListMode m_musicListMode{};
        std::optional<std::unordered_set<std::uint32_t>> m_acquiredMusicIDs;

        mutable Gx::ResourcePtr<O2JamMusicList> m_musicList;
        mutable std::vector<ChartMetadata> m_installedMusicList;
        mutable std::vector<ChartMetadata> m_displayMusicList;
        mutable std::vector<ChartMetadata> m_nonPlayableMusicList;
        mutable bool m_musicScanned{};
        mutable bool m_musicListLoaded{};
    };
}
