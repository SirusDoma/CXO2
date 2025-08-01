#pragma once

#include <OTwo/Services/Service.hpp>

#include <functional>

enum class EquipmentType : std::uint8_t;

struct CharacterInfo;

struct EquipItemRequest;
struct EquipItemResponse;

class NetworkException;
class CharacterService : public Service
{
public:
    virtual void GetCharacterInfo(std::function<void(const CharacterInfo&)> callback) const = 0;

    virtual void Equip(
        const EquipItemRequest& request,
        std::function<void(const EquipItemResponse&)> callback,
        std::function<void(const NetworkException&)> errorCallback
    ) const = 0;
};

class NetworkAdapter;
class CharacterOnlineService : public CharacterService
{
public:
    explicit CharacterOnlineService(NetworkAdapter& adapter);

    void GetCharacterInfo(std::function<void(const CharacterInfo&)> callback) const override;

    void Equip(
        const EquipItemRequest& request,
        std::function<void(const EquipItemResponse&)> callback,
        std::function<void(const NetworkException&)> errorCallback
    ) const override;

private:
    NetworkAdapter& m_adapter;
};

class CharacterOfflineService : CharacterService
{
public:
    void GetCharacterInfo(std::function<void(const CharacterInfo&)> callback) const override;

    void Equip(
        const EquipItemRequest& request,
        std::function<void(const EquipItemResponse&)> callback,
        std::function<void(const NetworkException&)> errorCallback
    ) const override;
};