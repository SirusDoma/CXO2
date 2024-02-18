#include <OTwo/Loaders/UI/NumberLoader.hpp>

#include <OTwo/Metadata/UI/NumberMetadata.hpp>
#include <OTwo/Loaders/TransformLoader.hpp>

NumberLoader::NumberLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> NumberLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    NumberMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();

    auto attributes = json.at("attributes");
    NumberLoader::ParseReferences(json["require"], metadata);
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    auto digitSize = attributes.find("digitSize");
    if (digitSize != attributes.end())
    {
        unsigned int w, h;
        digitSize->at("width").get_to(w);
        digitSize->at("height").get_to(h);
        metadata.DigitSize = sf::Vector2u(w, h);
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
            metadata.DigitFrames[std::stoi(digit)] = sf::IntRect(x, y, w, h);
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
        metadata.Color = sf::Color(r, g, b, a);
    }
    else
        metadata.Color = sf::Color::White;

    auto spacing = attributes.find("letterSpacing");
    if (spacing != attributes.end())
        metadata.LetterSpacing = spacing->get<float>();
    else
        metadata.LetterSpacing = 0.f;

    auto digitCount = attributes.find("digitCount");
    if (digitCount != attributes.end())
        metadata.DigitCount = digitCount->get<unsigned int>();
    else
        metadata.DigitCount = 1;

    auto value = attributes.find("value");
    if (value != attributes.end())
        metadata.Value = value->get<unsigned int>();
    else
        metadata.Value = 0;

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
    number->SetColor(spec->Color);

    if (spec->DigitSize != sf::Vector2u() || spec->DigitFrames.size() > 0)
    {
        number->SetDigitsSize(spec->DigitSize);
        for (auto frame : spec->DigitFrames)
            number->SetDigitFrame(frame.first, frame.second);
    }
    else
        number->SetDigitsSize(sf::Vector2u(context.Texture->getSize().x / 10, context.Texture->getSize().y));

    number->SetLetterSpacing(spec->LetterSpacing);
    number->SetDigitCount(spec->DigitCount);
    number->SetValue(spec->Value);

    number->SetOrigin(spec->Origin);
    number->SetPosition(spec->Position);
    number->SetScale(spec->Scale);
    number->SetRotation(spec->Rotation);

    return number;
}
