#include <OTwo/Loaders/UI/DialogLoader.hpp>

#include <OTwo/Loaders/SpriteLoader.hpp>
#include <OTwo/Loaders/UI/ButtonLoader.hpp>
#include <OTwo/Loaders/UI/LabelLoader.hpp>

DialogLoader::DialogLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> DialogLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    auto metadata = DialogMetadata();

    metadata.ResourceType = json.at("type").get<std::string>();
    ParseReferences(json["require"], metadata);
    ParseDialog(json["attributes"], metadata);

    return std::make_unique<DialogMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Dialog> DialogLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const DialogMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto dialog = std::make_unique<Gx::Dialog>();
    dialog->SetName(context.Name);
    dialog->SetOrigin(spec->Origin);
    dialog->SetPosition(spec->Position);
    dialog->SetScale(spec->Scale);
    dialog->SetRotation(spec->Rotation);
    if (context.Texture)
    {
        dialog->SetTexture(*context.Texture);
        dialog->SetTexCoords(spec->TexCoords);
    }

    if (context.Resources)
    {
        auto resources    = context.Resources;
        auto labelLoader  = Gx::ResourceLoaderFactory::GetLoader<Gx::Label>();
        auto buttonLoader = Gx::ResourceLoaderFactory::GetLoader<Gx::Button>();

        auto labelMetadata = spec->PromptLabelMetadata;
        if (labelLoader)
            dialog->SetLabel(labelLoader->Load(labelMetadata, resources->ResolveContext(labelMetadata)).release());

        auto acceptButtonMetadata = spec->AcceptButtonMetadata;
        auto cancelButtonMetadata = spec->CancelButtonMetadata;
        if (buttonLoader)
        {
            dialog->SetAcceptButton(buttonLoader->Load(acceptButtonMetadata, resources->ResolveContext(acceptButtonMetadata)).release());
            dialog->SetCancelButton(buttonLoader->Load(cancelButtonMetadata, resources->ResolveContext(cancelButtonMetadata)).release());
        }
    }

    return dialog;
}

void DialogLoader::ParseDialog(Json attributes, DialogMetadata &metadata)
{
    SpriteLoader::ParseSprite(attributes, metadata);
    if (attributes.contains("label"))
    {
        auto labelNode = attributes.at("label");
        auto labelMetadata = LabelMetadata();
        ParseReferences(labelNode["require"], labelMetadata);

        LabelLoader::ParseLabel(labelNode["attributes"], labelMetadata);
        metadata.PromptLabelMetadata = labelMetadata;
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

            metadata.AcceptButtonMetadata = acceptMetadata;
        }

        if (buttons.contains("cancel"))
        {
            auto cancelNode = buttons.at("cancel");
            auto cancelMetadata = ButtonMetadata();

            ParseReferences(cancelNode["require"], cancelMetadata);
            ButtonLoader::ParseButton(cancelNode["attributes"], stateMap, cancelMetadata);

            metadata.CancelButtonMetadata = cancelMetadata;
        }
    }
}
