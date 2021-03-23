#include <O2/IO/Loaders/AnimationLoader.hpp>

#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <O2/IO/Loaders/TransformLoader.hpp>
#include <O2/IO/Loaders/SpriteLoader.hpp>

AnimationMetadataLoader::AnimationMetadataLoader()
{
}

Gx::ResourceMetadata* AnimationMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    AnimationMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    SpriteMetadataLoader::Parse(attributes, &metadata);

    unsigned int duration;
    attributes.at("duration").get_to(duration);
    attributes.at("isLoop").get_to(metadata.IsLoop);
    metadata.Duration = sf::milliseconds(duration);

    auto frames = attributes.at("frames");
    metadata.Frames = std::vector<sf::IntRect>();
    for (auto frame : frames)
    {
        unsigned int x, y, width, height;
        frame.at("x").get_to(x);
        frame.at("y").get_to(y);
        frame.at("width").get_to(width);
        frame.at("height").get_to(height);

        metadata.Frames.push_back(sf::IntRect(x, y, width, height));
    }

    return new AnimationMetadata(metadata);
}

Gx::Animation* AnimationMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<AnimationMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto animation = new Gx::Animation();
    animation->SetName(spec->Name);
    animation->SetLoop(spec->IsLoop);
    animation->SetDuration(spec->Duration);

    auto loader = Gx::ResourceLoaderFactory::GetMetadataLoader<Gx::Sprite>();
    if (loader)
        animation->SetSprite(loader->Create(metadata, context));

    for (auto frame : spec->Frames)
        animation->AddFrame(frame);

    return animation;
}