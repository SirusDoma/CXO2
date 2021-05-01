#include <O2/Loaders/Dialogs/OptionDialogLoader.hpp>
#include <O2/Loaders/UI/DialogLoader.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/Label.hpp>

OptionDialogLoader::OptionDialogLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> OptionDialogLoader::LoadMetadata(const void *data, std::size_t size) const
{
    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::Dialog>();
    if (!loader)
        return nullptr;

    return loader->LoadMetadata(data, size);
}

Gx::ResourcePtr<OptionDialog> OptionDialogLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const DialogMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto dialog = std::make_unique<OptionDialog>();
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
