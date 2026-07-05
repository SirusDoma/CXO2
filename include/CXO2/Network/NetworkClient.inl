#pragma once

namespace Cx
{
    template<typename T>
    std::enable_if_t<std::is_unsigned_v<T> && !std::is_same_v<T, bool>, void>
    NetworkClient::UseDefaultPrefix()
    {
        if (m_status != sf::Socket::Status::Disconnected)
            throw Gx::InvalidOperationException();

        m_prefix = Packet::Prefix::Of<T>();
    }
}
