#include <CXO2/IO/Loaders/UI/SelectMusicDialogLoader.hpp>
#include <CXO2/IO/Loaders/UI/DialogLoader.hpp>

namespace Cx
{
    Gx::ResourcePtr<SelectMusicDialog> SelectMusicDialogLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = SelectMusicDialogMetadata();
        if (!DialogLoader::ParseMetadata(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            if (const auto colors = it->find("colors"); colors != it->end())
            {
                const auto parseColor = [&colors] (const char* key, sf::Color& target)
                {
                    if (const auto color = colors->find(key); color != colors->end())
                    {
                        unsigned int a, r, g, b;
                        color->at("a").get_to(a);
                        color->at("r").get_to(r);
                        color->at("g").get_to(g);
                        color->at("b").get_to(b);
                        target = sf::Color(r, g, b, a);
                    }
                };

                parseColor("random", metadata.Colors.Random);
                parseColor("notice", metadata.Colors.Notice);
                parseColor("disabled", metadata.Colors.Disabled);
                parseColor("warning", metadata.Colors.Warning);
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<SelectMusicDialog> SelectMusicDialogLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const SelectMusicDialogMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto dialog = Instantiate(context);
        DialogLoader::Apply(*dialog, *metadata, context);

        dialog->SetRandomColor(metadata->Colors.Random);
        dialog->SetNoticeColor(metadata->Colors.Notice);
        dialog->SetDisabledColor(metadata->Colors.Disabled);
        dialog->SetWarningColor(metadata->Colors.Warning);

        return dialog;
    }
}
