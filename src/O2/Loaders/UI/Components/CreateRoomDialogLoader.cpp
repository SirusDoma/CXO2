#include <O2/Loaders/UI/Components/CreateRoomDialogLoader.hpp>
#include <O2/Loaders/UI/DialogLoader.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/Label.hpp>

CreateRoomDialogLoader::CreateRoomDialogLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> CreateRoomDialogLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    auto metadata = DialogMetadata();

    metadata.ResourceType = json.at("type").get<std::string>();
    ParseReferences(json["require"], metadata);
    DialogLoader::ParseDialog(json["attributes"], metadata);

    return std::make_unique<DialogMetadata>(metadata);
}

Gx::ResourcePtr<CreateRoomDialog> CreateRoomDialogLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const DialogMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto dialog = std::make_unique<CreateRoomDialog>();
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
        auto resources = context.Resources;

        dialog->SetTitleTextBox(resources->Resolve<Gx::TextBox>("Interface/Metadata/Dialog/CreateRoom/TitleTextBox.json"));
        dialog->SetPasswordTextBox(resources->Resolve<Gx::TextBox>("Interface/Metadata/Dialog/CreateRoom/PasswordTextBox.json"));

        dialog->SetJamModeButton(resources->Resolve<Gx::RadioButton>("Interface/Metadata/Dialog/CreateRoom/Btn_JamMode.json"));
        dialog->SetVsModeButton(resources->Resolve<Gx::RadioButton>("Interface/Metadata/Dialog/CreateRoom/Btn_VsMode.json"));
        dialog->SetSingleModeButton(resources->Resolve<Gx::RadioButton>("Interface/Metadata/Dialog/CreateRoom/Btn_SingleMode.json"));

        dialog->SetJamAnimation(resources->Resolve<Gx::Animation>("Interface/Metadata/Dialog/CreateRoom/JamModeAnimation.json"));
        dialog->SetVsModeAnimation(resources->Resolve<Gx::Animation>("Interface/Metadata/Dialog/CreateRoom/VsModeAnimation.json"));
        dialog->SetSingleModeAnimation(resources->Resolve<Gx::Animation>("Interface/Metadata/Dialog/CreateRoom/SingleModeAnimation.json"));

        dialog->SetLevelLimitCheckBox(resources->Resolve<Gx::CheckBox>("Interface/Metadata/Dialog/CreateRoom/LevelLimitCheckBox.json"));
        dialog->SetMinLevelLimitTextBox(resources->Resolve<Gx::TextBox>("Interface/Metadata/Dialog/CreateRoom/MinLevelLimitTextBox.json"));
        dialog->SetMaxLevelLimitTextBox(resources->Resolve<Gx::TextBox>("Interface/Metadata/Dialog/CreateRoom/MaxLevelLimitTextBox.json"));

        auto buttonLoader = Gx::ResourceLoaderFactory::GetLoader<Gx::Button>();
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
