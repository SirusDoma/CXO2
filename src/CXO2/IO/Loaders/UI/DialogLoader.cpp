#include <CXO2/IO/Loaders/UI/DialogLoader.hpp>
#include <CXO2/IO/Loaders/UI/ButtonLoader.hpp>
#include <CXO2/IO/Loaders/UI/LabelLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

#include <CXO2/Constants/Identifiers/Cache.hpp>

#include <CXO2/UI/Dialogs/OptionDialog.hpp>
#include <CXO2/UI/Dialogs/CreateRoomDialog.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Services/WaitingService.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    void DialogLoader::OnRegistered(const std::string& id, const Builder& builder)
    {
        ResourceLoader<Gx::Dialog>::OnRegistered(id, builder);

        Gx::ResourceLoaderFactory::Map<Gx::Dialog,
            OptionDialog,
            CreateRoomDialog
        >();

        Gx::ResourceLoaderFactory::Map<Gx::Node,
            OptionDialog,
            CreateRoomDialog
        >();
    }

    Gx::ResourcePtr<Gx::Dialog> DialogLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = DialogMetadata();
        if (!ParseMetadata(json, metadata, context))
            return Instantiate(context);

        return LoadFromMetadata(metadata, context);
    }

    bool DialogLoader::ParseMetadata(const Gx::Json& json, DialogMetadata& metadata, const Gx::ResourceContext& context)
    {
        if (!MetadataLoader::Parse(json, metadata, context))
            return false;

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
                return false;

            if (const auto label = attributes.find("label"); label != attributes.end())
            {
                MetadataLoader::Parse(label.value(), metadata.PromptLabelMetadata, context);
                if (const auto attr = label.value().find("attributes"); attr != label.value().end())
                    LabelLoader::ParseMetadata(attr.value(), metadata.PromptLabelMetadata, Gx::ResourceContext::Rebind(context, fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_TEXT_PROMPT)));
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
                        name = fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_BUTTON_ACCEPT);
                        target = &metadata.AcceptButtonMetadata;
                    }
                    else
                    {
                        name = fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_BUTTON_CANCEL);
                        target = &metadata.CancelButtonMetadata;
                    }

                    MetadataLoader::Parse(button.value(), *target, Gx::ResourceContext::Rebind(context, name));
                    if (const auto attr = button.value().find("attributes"); attr != button.value().end())
                        ButtonLoader::ParseMetadata(attr.value(), *target, Gx::ResourceContext::Rebind(context, name));
                }
            }
        }

        return true;
    }

    void DialogLoader::Apply(Gx::Dialog& dialog, const DialogMetadata& metadata, const Gx::ResourceContext& context)
    {
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(metadata); texture)
        {
            dialog.SetTexture(*texture);
            dialog.SetTexCoords(metadata.TexCoords);
        }
        else if (const auto sheet = ctx.Require<SpriteSheet>(metadata))
        {
            dialog.SetTexture(sheet->GetTexture());
            if (metadata.TexCoords != sf::IntRect())
                dialog.SetTexCoords(metadata.TexCoords);
            else if (!sheet->TexCoords.empty())
                dialog.SetTexCoords(sheet->TexCoords[0]);
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(metadata))
        {
            if (metadata.TexCoords != sf::IntRect())
                dialog.SetTexCoords(metadata.TexCoords);
            else
                dialog.SetTexCoords(*bound);
        }

        auto container = SceneComposer::Compose(dialog);
        LoadChildren(container, metadata, context);

        const auto labelLoader = LabelLoader();
        const auto buttonLoader = ButtonLoader();

        if (auto label = labelLoader.LoadFromMetadata(metadata.PromptLabelMetadata, Gx::ResourceContext::Rebind(context, fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_TEXT_PROMPT))); label)
            dialog.SetLabel(context.Store(fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_TEXT_PROMPT), std::move(label)));

        if (auto accept = buttonLoader.LoadFromMetadata(metadata.AcceptButtonMetadata, Gx::ResourceContext::Rebind(context, fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_BUTTON_ACCEPT))); accept)
            dialog.SetAcceptButton(context.Store(fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_BUTTON_ACCEPT), std::move(accept)));

        if (auto cancel = buttonLoader.LoadFromMetadata(metadata.CancelButtonMetadata, Gx::ResourceContext::Rebind(context, fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_BUTTON_CANCEL))); cancel)
            dialog.SetCancelButton(context.Store(fmt::format("{}/{}", context.GetID(), Resource::Cache::Dialog::IDC_BUTTON_CANCEL), std::move(cancel)));

        dialog.SetName(metadata.Name);
        dialog.SetOrigin(metadata.Origin);
        dialog.SetPosition(metadata.Position.value_or(sf::Vector2f()));
        dialog.SetScale(metadata.Scale);
        dialog.SetRotation(metadata.Rotation);
    }

    Gx::ResourcePtr<Gx::Dialog> DialogLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const DialogMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto dialog = Instantiate(context);
        Apply(*dialog, *metadata, context);

        return dialog;
    }
}
