#include <O2/IO/Loaders/UI/NumberLoader.hpp>

#include <O2/IO/Metadata/UI/NumberMetadata.hpp>
#include <O2/IO/Loaders/TransformLoader.hpp>

NumberLoader::NumberLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> NumberLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    NumberMetadata metadata;

    metadata.SetType(json.at("type").get<std::string>());

    auto attributes = json.at("attributes");
    NumberLoader::ParseReferences(json["require"], metadata);
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    auto digitSize = attributes.find("digitSize");
    if (digitSize != attributes.end())
    {
        unsigned int w, h;
        digitSize->at("width").get_to(w);
        digitSize->at("height").get_to(h);
        metadata.SetDigitSize(sf::Vector2u(w, h));
    }

    auto frames = attributes.find("characters");
    if (frames != attributes.end())
    {
        for (auto [digit, frame] : frames->items())
        {
            unsigned int x, y, w, h;
            frame.at("x").get_to(x);
            frame.at("y").get_to(y);
            frame.at("width").get_to(w);
            frame.at("height").get_to(h);
            metadata.SetDigitFrame(std::stoi(digit), sf::IntRect(x, y, w, h));
        }
    }

    auto color = attributes.find("color");
    if (color != attributes.end())
    {
        unsigned int a, r, g, b;
        color->at("a").get_to(a);
        color->at("r").get_to(r);
        color->at("g").get_to(g);
        color->at("b").get_to(b);
        metadata.SetColor(sf::Color(r, g, b, a));
    }
    else
        metadata.SetColor(sf::Color::White);

    auto spacing = attributes.find("letterSpacing");
    if (spacing != attributes.end())
        metadata.SetLetterSpacing(spacing->get<float>());
    else
        metadata.SetLetterSpacing(0.f);

    return std::make_unique<NumberMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Number> NumberLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const NumberMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto number = std::make_unique<Gx::Number>();
    if (context.Texture)
        number->SetTexture(*context.Texture);

    number->SetName(context.Name);
    number->SetColor(spec->GetColor());

    if (spec->GetDigitSize() != sf::Vector2u() || spec->GetDigitFrames().size() > 0)
    {
        number->SetDigitsSize(spec->GetDigitSize());
        for (auto frame : spec->GetDigitFrames())
            number->SetDigitFrame(frame.first, frame.second);
    }
    else
        number->SetDigitsSize(sf::Vector2u(context.Texture->getSize().x / 10, context.Texture->getSize().y));

    number->SetLetterSpacing(spec->GetLetterSpacing());
    number->SetOrigin(spec->GetOrigin());
    number->SetPosition(spec->GetPosition());
    number->SetScale(spec->GetScale());
    number->SetRotation(spec->GetRotation());

    return number;
}
