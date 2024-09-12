#include <OTwo/IO/Loaders/UI/DialogLoader.hpp>
#include <OTwo/IO/Loaders/UI/ButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::Dialog> DialogLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    auto metadata = DialogMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (const auto label = attributes.find("label"); label != attributes.end())
    {
        MetadataLoader::Parse(label.value(), metadata.PromptLabelMetadata, context);
        LabelLoader::ParseMetadata(label.value().at("attributes"), metadata.PromptLabelMetadata, Gx::ResourceContext::Rebind(context.GetID() + "/IDC_TEXT_PROMPT", context));
    }

    if (const auto buttons = attributes.find("buttons"); buttons != attributes.end())
    {
        for (std::string type : {"accept", "cancel"})
        {
            auto button = buttons->find(type);
            if (button == buttons->end())
                continue;

            ButtonMetadata* target = nullptr;
            std::string name;
            if (type == "accept")
            {
                name = context.GetID() + "/IDC_BUTTON_ACCEPT";
                target = &metadata.AcceptButtonMetadata;
            }
            else
            {
                name = context.GetID() + "/IDC_BUTTON_CANCEL";
                target = &metadata.CancelButtonMetadata;
            }

            MetadataLoader::Parse(button.value(), *target, Gx::ResourceContext::Rebind(name, context));
            ButtonLoader::ParseMetadata(button.value().at("attributes"), *target, Gx::ResourceContext::Rebind(name, context));
        }
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Dialog> DialogLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const DialogMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");
    
    auto dialog = Create();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        dialog->SetTexture(*texture);

    auto container = ObjectContainer::Decorate(dialog.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }

    const auto labelLoader = LabelLoader();
    const auto buttonLoader = ButtonLoader();

    if (auto label = labelLoader.LoadFromMetadata(metadata->PromptLabelMetadata, Gx::ResourceContext::Rebind(context.GetID() + "/IDC_TEXT_PROMPT", context)); label)
        dialog->SetLabel(context.Store(context.GetID() + "/IDC_TEXT_PROMPT", std::move(label)));

    if (auto accept = buttonLoader.LoadFromMetadata(metadata->AcceptButtonMetadata, Gx::ResourceContext::Rebind(context.GetID() + "/IDC_BUTTON_ACCEPT", context)); accept)
        dialog->SetAcceptButton(context.Store(context.GetID() + "/IDC_BUTTON_ACCEPT", std::move(accept)));

    if (auto cancel = buttonLoader.LoadFromMetadata(metadata->CancelButtonMetadata, Gx::ResourceContext::Rebind(context.GetID() + "/IDC_BUTTON_CANCEL", context)); cancel)
        dialog->SetCancelButton(context.Store(context.GetID() + "/IDC_BUTTON_CANCEL", std::move(cancel)));

    dialog->SetName(metadata->Name);
    dialog->SetTexCoords(metadata->TexCoords);
    dialog->SetOrigin(metadata->Origin);
    dialog->SetPosition(metadata->Position);
    dialog->SetScale(metadata->Scale);
    dialog->SetRotation(metadata->Rotation);

    return dialog;
}
