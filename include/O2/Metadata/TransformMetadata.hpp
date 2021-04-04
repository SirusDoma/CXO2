#ifndef O2JAM_TRANSFORM_METADATA_HPP
#define O2JAM_TRANSFORM_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>

class TransformMetadata : public Gx::ResourceMetadata
{
public:
    TransformMetadata() {}
    virtual ~TransformMetadata() {}

    const sf::Vector2f& GetPosition() const         { return m_position; }
    void SetPosition(const sf::Vector2f& mPosition) { m_position = mPosition; }

    const sf::Vector2f& GetScale() const            { return m_scale; }
    void SetScale(const sf::Vector2f& mScale)       { m_scale = mScale; }

    float GetRotation() const                       { return m_rotation; }
    void SetRotation(float mRotation)               { m_rotation = mRotation; }

    const sf::Vector2f& GetOrigin() const           { return m_origin; }
    void SetOrigin(const sf::Vector2f& mOrigin)     { m_origin = mOrigin; }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_scale;
    float        m_rotation;
    sf::Vector2f m_origin;
};

#endif