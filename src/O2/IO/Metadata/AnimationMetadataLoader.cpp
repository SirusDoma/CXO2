#include <O2/IO/Metadata/AnimationMetadataLoader.hpp>

#include <Genode/IO/ResourceLoaderFactory.hpp>

AnimationMetadataLoader::AnimationMetadataLoader()
{
}

Gx::ResourceMetadata* AnimationMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    AnimationMetadata definition;

    json.at("type").get_to(definition.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        definition.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    attributes.at("isLoop").get_to(definition.IsLoop);

    unsigned int duration;
    attributes.at("duration").get_to(duration);
    definition.Duration = sf::milliseconds(duration);

    auto color     = attributes.at("color");
    auto texCoords = attributes.at("texCoords");

    unsigned int a, r, g, b;
    color.at("a").get_to(a);
    color.at("r").get_to(r);
    color.at("g").get_to(g);
    color.at("b").get_to(b);
    definition.Color = sf::Color(r, g, b, a);

    unsigned int x, y, w, h;
    texCoords.at("x").get_to(x);
    texCoords.at("y").get_to(y);
    texCoords.at("width").get_to(w);
    texCoords.at("height").get_to(h);
    definition.TexCoords = sf::IntRect(x, y, w, h);

    auto frames = attributes.at("frames");
    definition.Frames = std::vector<sf::IntRect>();
    for (auto frame : frames)
    {
        unsigned int width, height;
        frame.at("x").get_to(x);
        frame.at("y").get_to(y);
        frame.at("width").get_to(width);
        frame.at("height").get_to(height);

        definition.Frames.push_back(sf::IntRect(x, y, width, height));
    }

    return new AnimationMetadata(definition);
}

Gx::Animation* AnimationMetadataLoader::Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<AnimationMetadata*>(definition);
    if (!spec || !context.Texture)
        return nullptr;

    auto animation = new Gx::Animation();
    animation->SetLoop(spec->IsLoop);
    animation->SetDuration(spec->Duration);

    auto loader = Gx::ResourceLoaderFactory::GetMetadataLoader<Gx::Sprite>();
    animation->SetSprite(loader->Create(definition, context));

    for (auto frame : spec->Frames)
        animation->AddFrames({ frame });

    return animation;
}