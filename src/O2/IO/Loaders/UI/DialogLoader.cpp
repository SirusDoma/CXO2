#include <O2/IO/Loaders/UI/DialogLoader.hpp>

#include <O2/IO/Loaders/SpriteLoader.hpp>
#include <O2/IO/Loaders/UI/ButtonLoader.hpp>
#include <O2/IO/Loaders/UI/LabelLoader.hpp>

#include <O2/IO/Metadata/UI/DialogMetadata.hpp>

DialogLoader::DialogLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> DialogLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    auto metadata = DialogMetadata();

    metadata.SetType(json.at("type").get<std::string>());
    ParseReferences(json["require"], metadata);

    auto attributes = json.at("attributes");
    SpriteLoader::ParseSprite(attributes, metadata);
    if (attributes.contains("label"))
    {
        auto labelNode = attributes.at("label");
        auto labelMetadata = LabelMetadata();
        ParseReferences(labelNode["require"], labelMetadata);

        LabelLoader::ParseLabel(labelNode["attributes"], labelMetadata);
        metadata.SetPromptLabel(labelMetadata);
    }

    if (attributes.contains("buttons"))
    {
        auto buttons = attributes.at("buttons");
        std::unordered_map<std::string, Gx::Button::State> stateMap = {
            {"normal", Gx::Button::State::Normal},
            {"hover",  Gx::Button::State::Hover},
            {"click",  Gx::Button::State::Active},
        };

        if (buttons.contains("accept"))
        {
            auto acceptNode = buttons.at("accept");
            auto acceptMetadata = ButtonMetadata();

            ParseReferences(acceptNode["require"], acceptMetadata);
            ButtonLoader::ParseButton(acceptNode["attributes"], stateMap, acceptMetadata);

            metadata.SetAcceptButton(acceptMetadata);
        }

        if (buttons.contains("cancel"))
        {
            auto cancelNode = buttons.at("cancel");
            auto cancelMetadata = ButtonMetadata();

            ParseReferences(cancelNode["require"], cancelMetadata);
            ButtonLoader::ParseButton(cancelNode["attributes"], stateMap, cancelMetadata);

            metadata.SetCancelButton(cancelMetadata);
        }
    }

    return std::make_unique<DialogMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Dialog> DialogLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const DialogMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto dialog = std::make_unique<Gx::Dialog>();
    dialog->SetName(context.Name);
    dialog->SetOrigin(spec->GetOrigin());
    dialog->SetPosition(spec->GetPosition());
    dialog->SetScale(spec->GetScale());
    dialog->SetRotation(spec->GetRotation());
    if (context.Texture)
        dialog->SetTexture(*context.Texture);

    if (context.Resources)
    {
        auto resources    = context.Resources;
        auto labelLoader  = Gx::ResourceLoaderFactory::GetLoader<Gx::Label>();
        auto buttonLoader = Gx::ResourceLoaderFactory::GetLoader<Gx::Button>();

        auto labelMetadata = spec->GetPromptLabel();
        if (labelLoader)
            dialog->SetLabel(labelLoader->Load(labelMetadata, resources->ResolveContext(labelMetadata)).release());

        auto acceptButtonMetadata = spec->GetAcceptButton();
        auto cancelButtonMetadata = spec->GetCancelButton();
        if (buttonLoader)
        {
            dialog->SetAcceptButton(buttonLoader->Load(acceptButtonMetadata, resources->ResolveContext(acceptButtonMetadata)).release());
            dialog->SetCancelButton(buttonLoader->Load(cancelButtonMetadata, resources->ResolveContext(cancelButtonMetadata)).release());
        }
    }

    return dialog;
}
