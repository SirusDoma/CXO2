#include <O2/Loaders/UI/Components/OptionDialogLoader.hpp>
#include <O2/Loaders/UI/DialogLoader.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/Label.hpp>

OptionDialogLoader::OptionDialogLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> OptionDialogLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    auto metadata = DialogMetadata();

    metadata.SetResourceType(json.at("type").get<std::string>());
    ParseReferences(json["require"], metadata);
    DialogLoader::ParseDialog(json["attributes"], metadata);

    return std::make_unique<DialogMetadata>(metadata);
}

Gx::ResourcePtr<OptionDialog> OptionDialogLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const DialogMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto dialog = std::make_unique<OptionDialog>();
    dialog->SetName(context.Name);
    dialog->SetOrigin(spec->GetOrigin());
    dialog->SetPosition(spec->GetPosition());
    dialog->SetScale(spec->GetScale());
    dialog->SetRotation(spec->GetRotation());
    if (context.Texture)
    {
        dialog->SetTexture(*context.Texture);
        dialog->SetTexCoords(spec->GetTexCoords());
    }

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
