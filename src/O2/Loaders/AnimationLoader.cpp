#include <O2/Loaders/AnimationLoader.hpp>

#include <O2/Metadata/AnimationMetadata.hpp>
#include <O2/Loaders/TransformLoader.hpp>
#include <O2/Loaders/SpriteLoader.hpp>

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
    auto frames     = attributes["frames"];
    if (!frames.empty())
    {
        for (auto frame : frames)
        {
            SpriteMetadata frameMetadata;
            SpriteLoader::ParseSprite(frame, frameMetadata);

            metadata.Frames.push_back(Gx::Animation::Frame
            {
                frameMetadata.TexCoords,
                frameMetadata.Origin,
                frameMetadata.Position,
                frameMetadata.Rotation,
                frameMetadata.Scale
            });
        }
    }
    else
    {
        metadata.Frames.push_back(Gx::Animation::Frame
        {
            metadata.TexCoords,
            metadata.Origin,
            metadata.Position,
            metadata.Rotation,
            metadata.Scale
        });
    }

    metadata.IsLoop = !attributes["isLoop"].empty() && attributes["isLoop"].get<bool>();
    if (!attributes["duration"].empty())
        metadata.Duration = sf::milliseconds(attributes["duration"].get<unsigned int>());
    else
        metadata.Duration = sf::milliseconds(metadata.Frames.size() * 65);

    return std::make_unique<AnimationMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Animation> AnimationLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const AnimationMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto animation = std::make_unique<Gx::Animation>();
    animation->SetName(context.Name);
    animation->SetLoop(spec->IsLoop);
    animation->SetDuration(spec->Duration);

    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::Sprite>();
    if (loader)
        animation->SetSprite(std::move(loader->Load(metadata, context)).release());

    for (auto frame : spec->Frames)
        animation->AddFrame(frame);

    return animation;
}
