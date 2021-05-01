#include <O2/Loaders/UI/Components/ChannelButtonLoader.hpp>

#include <Genode/IO/ResourceManager.hpp>

#include <O2/Metadata/UI/RadioButtonMetadata.hpp>
#include <O2/States/Components/Planet/ChannelButton.hpp>

ChannelButtonLoader::ChannelButtonLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ChannelButtonLoader::LoadMetadata(const void *data, std::size_t size) const
{
    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::RadioButton>();
    if (!loader)
        return nullptr;

    return loader->LoadMetadata(data, size);
}

Gx::ResourcePtr<ChannelButton> ChannelButtonLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const RadioButtonMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto channelButton = std::make_unique<ChannelButton>();
    if (context.Texture)
        channelButton->SetTexture(*context.Texture);

    channelButton->SetName(context.Name);
    channelButton->SetOrigin(spec->Origin);
    channelButton->SetPosition(spec->Position);
    channelButton->SetScale(spec->Scale);
    channelButton->SetRotation(spec->Rotation);

    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::Sprite>();
    if (loader)
    {
        for (auto[state, meta] : spec->States)
            channelButton->SetStateFrame(state, *loader->Load(meta, Gx::ResourceContext()));
    }

    channelButton->SetDefaultMetadata(const_cast<RadioButtonMetadata*>(spec));
    channelButton->SetHighMetadata(context.Resources->LoadMetadata<Gx::RadioButton>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/High.json"));
    channelButton->SetIntermediateMetadata(context.Resources->LoadMetadata<Gx::RadioButton>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/Intermediate.json"));
    channelButton->SetBeginnerMetadata(context.Resources->LoadMetadata<Gx::RadioButton>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json"));

    return channelButton;
}
