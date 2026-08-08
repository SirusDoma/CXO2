#pragma once

#include <SFML/System/String.hpp>

#include <cstddef>
#include <cstdint>
#include <string>

namespace Cx
{
    enum class Currency : std::uint8_t
    {
        None = 0,
        Gem  = 1,
        Cash = 2
    };

    enum class CartItemType
    {
        Equipment,
        EquipmentSet,
        Music
    };

    enum class ShopCategory
    {
        Special,
        Fashion,
        Accessory,
        Beauty,
        Instrument
    };

    enum class MusicSortKey
    {
        New,
        Genre,
        Title,
        Level,
        Bpm,
        Price,
        Time,
        Status
    };

    enum class MusicDownloadStatus
    {
        Idle,
        Connecting,
        Downloading,
        Completed,
        Failed
    };

    enum class MusicDownloadError
    {
        None,
        ConnectionFailed,
        DownloadFailed,
        InsufficientDiskSpace,
        Cancelled
    };

    struct MusicDownloadProgress
    {
        MusicDownloadStatus Status = MusicDownloadStatus::Idle;
        MusicDownloadError Error   = MusicDownloadError::None;

        std::uint16_t MusicID = 0;
        sf::String  MusicTitle;
        std::string FileName;

        std::size_t QueueIndex = 0;
        std::size_t QueueCount = 0;

        std::uint64_t FileBytesRead  = 0;
        std::uint64_t FileSize       = 0;
        std::uint64_t TotalBytesRead = 0;
        std::uint64_t TotalSize      = 0;
        std::uint64_t BytesPerSecond = 0;

        float GetFilePercent() const
        {
            return FileSize > 0 ? static_cast<float>(FileBytesRead) / static_cast<float>(FileSize) * 100.f : 0.f;
        }

        float GetTotalPercent() const
        {
            return TotalSize > 0 ? static_cast<float>(TotalBytesRead) / static_cast<float>(TotalSize) * 100.f : 0.f;
        }
    };
}
