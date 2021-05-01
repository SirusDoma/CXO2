#include <O2/Loaders/UI/Components/ChannelBoardLoader.hpp>

#include <O2/Metadata/UI/Components/ChannelBoardMetadata.hpp>
#include <O2/Loaders/UI/ImageLoader.hpp>

#include <O2/States/Components/Planet/ChannelBoard.hpp>

ChannelBoardLoader::ChannelBoardLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ChannelBoardLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ChannelBoardMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();
    auto attributes = json.at("attributes");

    ParseReferences(json["require"], metadata);
    ImageLoader::ParseImage(attributes, metadata);

    metadata.ChannelCount = ChannelBoard::CHANNEL_LIST_PER_PAGE;
    if (!attributes["channelCount"].empty())
        metadata.ChannelCount = attributes["channelCount"].get<unsigned int>();

    return std::make_unique<ChannelBoardMetadata>(metadata);
}

Gx::ResourcePtr<ChannelBoard> ChannelBoardLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ChannelBoardMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto channelBoard = std::make_unique<ChannelBoard>();
    channelBoard->SetChannelsPerPage(spec->ChannelCount);
    channelBoard->SetName(context.Name);
    channelBoard->SetColor(spec->Color);

    if (spec->Frames.size() > 0)
    {
        for (auto frame : spec->Frames)
            channelBoard->AddFrame(frame.first, frame.second);
    }
    else
        channelBoard->SetTexCoords(spec->TexCoords);

    if (context.Texture)
        channelBoard->SetTexture(*context.Texture);

    channelBoard->SetOrigin(spec->Origin);
    channelBoard->SetPosition(spec->Position);
    channelBoard->SetScale(spec->Scale);
    channelBoard->SetRotation(spec->Rotation);

    channelBoard->SetChannelTabButton(context.Resources->Resolve<Gx::Button>("Interface/Metadata/State/Planet/ChannelBoard/Btn_ChannelTab.json"));
    channelBoard->SetNoticeTabButton(context.Resources->Resolve<Gx::Button>("Interface/Metadata/State/Planet/ChannelBoard/Btn_NoticeTab.json"));
    channelBoard->SetChannelCategory(context.Resources->Resolve<Gx::Image>("Interface/Metadata/State/Planet/ChannelBoard/ChannelCategory.json"));
    channelBoard->SetChannelList(context.Resources->Resolve<Gx::List>("Interface/Metadata/State/Planet/ChannelBoard/ChannelList.json"));
    channelBoard->SetChannelEnterButton(context.Resources->Resolve<Gx::Button>("Interface/Metadata/State/Planet/ChannelBoard/Btn_ChannelEnter.json"));
    channelBoard->SetNavigateLeftButton(context.Resources->Resolve<Gx::Button>("Interface/Metadata/State/Planet/ChannelBoard/Btn_ChannelLeft.json"));
    channelBoard->SetNavigateRightButton(context.Resources->Resolve<Gx::Button>("Interface/Metadata/State/Planet/ChannelBoard/Btn_ChannelRight.json"));
    channelBoard->SetNotice(context.Resources->Resolve<Gx::Image>("Interface/Metadata/State/Planet/ChannelBoard/Notice.json"));
    channelBoard->SetCurrentPageNumber(context.Resources->Resolve<Gx::Number>("Interface/Metadata/State/Planet/ChannelBoard/CurrentPageNumber.json"));
    channelBoard->SetMaxPageNumber(context.Resources->Resolve<Gx::Number>("Interface/Metadata/State/Planet/ChannelBoard/MaxPageNumber.json"));

    for (int i = 0; i < spec->ChannelCount; i++)
    {
        auto channelButton = context.Resources->Resolve<ChannelButton>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel.json");
        channelButton->SetChannelNumber(i + 1);
        channelBoard->AddChannelButton(std::move(channelButton));
    }

    return channelBoard;
}
