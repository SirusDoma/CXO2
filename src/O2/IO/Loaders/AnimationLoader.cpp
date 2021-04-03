#include <O2/IO/Loaders/AnimationLoader.hpp>

#include <O2/IO/Metadata/AnimationMetadata.hpp>
#include <O2/IO/Loaders/TransformLoader.hpp>
#include <O2/IO/Loaders/SpriteLoader.hpp>

AnimationLoader::AnimationLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> AnimationLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    AnimationMetadata metadata;

    ParseReferences(json["require"], metadata);
    SpriteLoader::ParseSprite(json["attributes"], metadata);

    auto attributes = json.at("attributes");
    metadata.SetDuration(sf::milliseconds(attributes.at("duration").get<unsigned int>()));
    metadata.SetLoop(attributes.at("isLoop").get<bool>());

    auto frames = attributes.at("frames");
    for (auto frame : frames)
    {
        unsigned int x, y, width, height;
        frame.at("x").get_to(x);
        frame.at("y").get_to(y);
        frame.at("width").get_to(width);
        frame.at("height").get_to(height);

        metadata.AddFrame(sf::IntRect(x, y, width, height));
    }

    return std::make_unique<AnimationMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Animation> AnimationLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const AnimationMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto animation = std::make_unique<Gx::Animation>();
    animation->SetName(context.Name);
    animation->SetLoop(spec->isLoop());
    animation->SetDuration(spec->GetDuration());

    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::Sprite>();
    if (loader)
        animation->SetSprite(std::move(loader->Load(metadata, context)).release());

    for (auto frame : spec->GetFrames())
        animation->AddFrame(frame);

    return animation;
}
