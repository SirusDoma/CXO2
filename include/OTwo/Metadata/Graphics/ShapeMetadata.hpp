#ifndef O2JAM_SHAPE_METADATA_HPP
#define O2JAM_SHAPE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct ShapeMetadata : TransformMetadata
{
    enum class Type
    {
        Circle,
        Polygon,
        Rectangle,
        RoundedRectangle
    };

    ~ShapeMetadata() override = default;

    Type        ShapeType;
    sf::Color   Color;
    sf::IntRect TexCoords;
    float       OutlineThickness;
    sf::Color   OutlineColor;
    std::unordered_map<unsigned int, sf::Color> ColorMap;
};

struct CircleMetadata : ShapeMetadata
{
    ~CircleMetadata() override = default;

    float Radius;
    unsigned int PointCount;
};

struct PolygonMetadata : ShapeMetadata
{
    ~PolygonMetadata() override = default;

    unsigned int PointCount;
    std::vector<sf::Vector2f> Points;
};

struct RectangleMetadata : ShapeMetadata
{
    ~RectangleMetadata() override = default;

    unsigned int Width;
    unsigned int Height;
};

struct RoundedRectangleMetadata : ShapeMetadata
{
    ~RoundedRectangleMetadata() override = default;

    unsigned int Width;
    unsigned int Height;
    unsigned int CornerPointCount;
    float CornerRadius;
};

#endif