#include <CXO2/IO/Loaders/Graphics/ShapeLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/Graphics/ShapeMetadata.hpp>

#include <Genode/Graphics/Shapes/Circle.hpp>
#include <Genode/Graphics/Shapes/Polygon.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>
#include <Genode/Graphics/Shapes/RoundedRectangle.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    Gx::ResourcePtr<Gx::Shape> ShapeLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = std::make_unique<ShapeMetadata>();

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            auto type = attributes.find("type");
            if (type == attributes.end())
                return Instantiate(context);

            if (auto parsed = magic_enum::enum_cast<ShapeMetadata::Type>(type->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                metadata->ShapeType = parsed.value();
            else
                return Instantiate(context);

            if (metadata->ShapeType == ShapeMetadata::Type::Circle)
            {
                metadata = std::make_unique<CircleMetadata>();
                metadata->ShapeType = ShapeMetadata::Type::Circle;

                auto circle = dynamic_cast<CircleMetadata*>(metadata.get());

                auto radius = attributes.find("radius");
                if (type != attributes.end())
                    circle->Radius = radius->get<float>();
                else
                    circle->Radius = 0;

                if (auto pointCount = attributes.find("pointCount"); pointCount != attributes.end())
                    circle->PointCount = pointCount->get<unsigned int>();
                else
                    circle->PointCount = 30;
            }
            else if (metadata->ShapeType == ShapeMetadata::Type::Polygon)
            {
                metadata = std::make_unique<PolygonMetadata>();
                metadata->ShapeType = ShapeMetadata::Type::Polygon;

                auto polygon = dynamic_cast<PolygonMetadata*>(metadata.get());
                if (auto pointCount = attributes.find("pointCount"); pointCount != attributes.end())
                    polygon->PointCount = pointCount->get<unsigned int>();
                else
                    polygon->PointCount = 30;

                if (auto points = attributes.find("points"); points != attributes.end())
                {
                    for (auto& point : points->items())
                    {
                        polygon->Points.push_back({
                            point.value().at("x").get<float>(),
                            point.value().at("y").get<float>(),
                        });
                    }
                }
            }
            else if (metadata->ShapeType == ShapeMetadata::Type::Rectangle)
            {
                metadata = std::make_unique<RectangleMetadata>();
                metadata->ShapeType = ShapeMetadata::Type::Rectangle;

                auto rectangle = dynamic_cast<RectangleMetadata*>(metadata.get());
                if (auto width = attributes.find("width"); width != attributes.end())
                    rectangle->Width = width->get<unsigned int>();
                else
                    rectangle->Width = 0;

                if (auto height = attributes.find("height"); height != attributes.end())
                    rectangle->Height = height->get<unsigned int>();
                else
                    rectangle->Height = 0;
            }
            else if (metadata->ShapeType == ShapeMetadata::Type::RoundedRectangle)
            {
                metadata = std::make_unique<RoundedRectangleMetadata>();
                metadata->ShapeType = ShapeMetadata::Type::RoundedRectangle;

                auto rectangle = dynamic_cast<RoundedRectangleMetadata*>(metadata.get());
                if (auto width = attributes.find("width"); width != attributes.end())
                    rectangle->Width = width->get<unsigned int>();
                else
                    rectangle->Width = 0;

                if (auto height = attributes.find("height"); height != attributes.end())
                    rectangle->Height = height->get<unsigned int>();
                else
                    rectangle->Height = 0;

                auto radius = attributes.find("cornerRadius");
                if (type != attributes.end())
                    rectangle->CornerRadius = radius->get<float>();
                else
                    rectangle->CornerRadius = 1.f;

                if (auto pointCount = attributes.find("cornerPointCount"); pointCount != attributes.end())
                    rectangle->CornerPointCount = pointCount->get<unsigned int>();
                else
                    rectangle->CornerPointCount = 30;
            }
            else
                return Instantiate(context);

            if (!MetadataLoader::Parse(json, *metadata, context))
                return Instantiate(context);

            if (auto transform = attributes.find("transform"); transform != attributes.end())
                TransformLoader::ParseMetadata(transform.value(), *metadata, context);

            if (auto colorMap = attributes.find("colorMap"); colorMap != attributes.end())
            {
                unsigned int i = 0;
                for (auto& color : colorMap->items())
                {
                    unsigned int a, r, g, b;
                    color.value().at("a").get_to(a);
                    color.value().at("r").get_to(r);
                    color.value().at("g").get_to(g);
                    color.value().at("b").get_to(b);

                    metadata->ColorMap[i++] = sf::Color(r, g, b, a);
                }
            }

            if (auto color = attributes.find("color"); color != attributes.end())
            {
                unsigned int a, r, g, b;
                color->at("a").get_to(a);
                color->at("r").get_to(r);
                color->at("g").get_to(g);
                color->at("b").get_to(b);
                metadata->Color = sf::Color(r, g, b, a);
            }
            else
                metadata->Color = sf::Color::Black;

            if (auto outlineThickness = attributes.find("outlineThickness"); outlineThickness != attributes.end())
                metadata->OutlineThickness = outlineThickness->get<float>();
            else
                metadata->OutlineThickness = 0.f;

            if (auto outlineColor = attributes.find("outlineColor"); outlineColor != attributes.end())
            {
                unsigned int a, r, g, b;
                outlineColor->at("a").get_to(a);
                outlineColor->at("r").get_to(r);
                outlineColor->at("g").get_to(g);
                outlineColor->at("b").get_to(b);
                metadata->OutlineColor = sf::Color(r, g, b, a);
            }
            else
                metadata->OutlineColor = sf::Color::Transparent;

            if (auto texCoords  = attributes.find("texCoords"); texCoords != attributes.end())
            {
                unsigned int x, y, w, h;
                texCoords->at("x").get_to(x);
                texCoords->at("y").get_to(y);
                texCoords->at("width").get_to(w);
                texCoords->at("height").get_to(h);
                metadata->TexCoords = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
            }
        }

        return LoadFromMetadata(*metadata, context);
    }

    Gx::ResourcePtr<Gx::Shape> ShapeLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ShapeMetadata*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException("The specified metadata is incompatible");
    
        std::unique_ptr<Gx::Shape> shape;
        if (metadata->ShapeType == ShapeMetadata::Type::Circle)
        {
            const auto circle = dynamic_cast<const CircleMetadata*>(&meta);
            shape = std::make_unique<Gx::Circle>(circle->Radius, circle->PointCount);

        }
        else if (metadata->ShapeType == ShapeMetadata::Type::Polygon)
        {
            const auto polyMeta = dynamic_cast<const PolygonMetadata*>(&meta);
            auto polygon = std::make_unique<Gx::Polygon>(polyMeta->PointCount);
            for (std::size_t i = 0; i < polyMeta->Points.size(); i++)
                polygon->SetPoint(i, polyMeta->Points[i]);

            shape = std::move(polygon);
        }
        else if (metadata->ShapeType == ShapeMetadata::Type::Rectangle)
        {
            const auto rectangle = dynamic_cast<const RectangleMetadata*>(&meta);
            shape = std::make_unique<Gx::Rectangle>(sf::Vector2f(rectangle->Width, rectangle->Height));
        }
        else if (metadata->ShapeType == ShapeMetadata::Type::RoundedRectangle)
        {
            const auto rectangle = dynamic_cast<const RoundedRectangleMetadata*>(&meta);
            shape = std::make_unique<Gx::RoundedRectangle>(sf::Vector2f(rectangle->Width, rectangle->Height), rectangle->CornerRadius, rectangle->CornerPointCount);
        }

        shape->SetTexCoords(metadata->TexCoords);

        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
            shape->SetTexture(*texture);

        for (auto& [i, color] : metadata->ColorMap)
            shape->SetColor(i, color);

        if (metadata->Position != sf::Vector2f())
        {
            shape->SetPosition(metadata->Position);
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            shape->SetPosition({
                static_cast<float>(bound->position.x),
                static_cast<float>(bound->position.y),
            });
        }

        shape->SetColor(metadata->Color);
        shape->SetOutlineThickness(metadata->OutlineThickness);
        shape->SetOutlineColor(metadata->OutlineColor);
        shape->SetOrigin(metadata->Origin);
        shape->SetScale(metadata->Scale);
        shape->SetRotation(metadata->Rotation);

        auto container = ObjectContainer::Decorate(shape.get());
        LoadChildren(container, meta, context);

        return shape;
    }
}
