#include <O2/IO/Metadata/DialogMetadataLoader.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/UI/Label.hpp>

#include <O2/IO/Metadata/SpriteMetadataLoader.hpp>
#include <O2/IO/Metadata/ButtonMetadataLoader.hpp>
#include <O2/IO/Metadata/LabelMetadata.hpp>
#include <O2/IO/Metadata/LabelMetadataLoader.hpp>

DialogMetadataLoader::DialogMetadataLoader()
{
}

Gx::ResourceMetadata* DialogMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    DialogMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto attributes = json.at("attributes");
    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    SpriteMetadataLoader::Parse(attributes, &metadata);
    LabelMetadataLoader::Parse(attributes.at("prompt"), &metadata.PromptLabel);
    if (attributes.contains("buttons"))
    {
        auto buttons = attributes.at("buttons");
        std::unordered_map<std::string, Gx::Button::State> stateMap = {
            {"normal", Gx::Button::State::Normal},
            {"hover",  Gx::Button::State::Hover},
            {"click",  Gx::Button::State::Active},
        };

        auto accept = buttons.find("accept");
        if (accept != buttons.end())
        {
            resources = accept->at("resources");
            for (auto resource : resources.items())
                metadata.AcceptButton.ResourceReferences[resource.key()] = resource.value();

            ButtonMetadataLoader::Parse(accept->at("attributes"), stateMap, &metadata.AcceptButton);
        }

        auto cancel = buttons.find("cancel");
        if (cancel != buttons.end())
        {
            resources = cancel->at("resources");
            for (auto resource : resources.items())
                metadata.CancelButton.ResourceReferences[resource.key()] = resource.value();

            ButtonMetadataLoader::Parse(cancel->at("attributes"), stateMap, &metadata.CancelButton);
        }
    }

    return new DialogMetadata(metadata);
}

Gx::Dialog* DialogMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<DialogMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto dialog = new Gx::Dialog(context.Texture);
    dialog->SetName(spec->Name);
    dialog->SetOrigin(spec->Origin);
    dialog->SetPosition(spec->Position);
    dialog->SetScale(spec->Scale);
    dialog->SetRotation(spec->Rotation);

    dialog->SetLabel(Gx::ResourceManager::Instance()->Create<Gx::Label>(&spec->PromptLabel));
    dialog->SetAcceptButton(Gx::ResourceManager::Instance()->Create<Gx::Button>(&spec->AcceptButton));
    dialog->SetCancelButton(Gx::ResourceManager::Instance()->Create<Gx::Button>(&spec->CancelButton));

    return dialog;
}