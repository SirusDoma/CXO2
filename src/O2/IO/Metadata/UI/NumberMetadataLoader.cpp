#include <O2/IO/Metadata/UI/NumberMetadataLoader.hpp>

#include <O2/IO/Metadata/TransformMetadataLoader.hpp>

NumberMetadataLoader::NumberMetadataLoader()
{
}

Gx::ResourceMetadata* NumberMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    NumberMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    TransformMetadataLoader::Parse(attributes["transform"], &metadata);

    auto digitSize = attributes.find("digitSize");
    if (digitSize != attributes.end())
    {
        unsigned int w, h;
        digitSize->at("width").get_to(w);
        digitSize->at("height").get_to(h);
        metadata.DigitSize = sf::Vector2u(w, h);
    }

    metadata.DigitFrames = std::unordered_map<unsigned int, sf::IntRect>();
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
        spacing->get_to(metadata.LetterSpacing);
    else
        metadata.LetterSpacing = 0;

    return new NumberMetadata(metadata);
}

Gx::Number* NumberMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<NumberMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto number = new Gx::Number(context.Texture);
    number->SetName(spec->Name);
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
    number->SetOrigin(spec->Origin);
    number->SetPosition(spec->Position);
    number->SetScale(spec->Scale);
    number->SetRotation(spec->Rotation);

    return number;
}
