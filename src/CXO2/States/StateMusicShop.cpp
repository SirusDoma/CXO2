#include <CXO2/States/StateMusicShop.hpp>

#include <CXO2/States/StateRoom.hpp>
#include <CXO2/States/StatePayment.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/CartContext.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>
#include <CXO2/IO/TextureCompiler.hpp>

#include <Genode/IO/FileSystem.hpp>

#include <CXO2/Utilities/StringFormatter.hpp>
#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/MusicShop.hpp>
#include <CXO2/Constants/Messages/MusicShop.hpp>

#include <Genode/Graphics/Animation.hpp>
#include <CXO2/UI/Button.hpp>
#include <CXO2/UI/Gauge.hpp>
#include <CXO2/UI/Image.hpp>
#include <CXO2/UI/Label.hpp>
#include <CXO2/UI/List.hpp>
#include <CXO2/UI/BitmapNumber.hpp>
#include <CXO2/UI/ToggleButton.hpp>

#include <SFML/Audio/Music.hpp>

#include <fmt/format.h>

#include <algorithm>

namespace Cx
{
    using namespace Constants::Identifiers;

    StateMusicShop::StateMusicShop(Gx::AudioMixer& mixer, SessionContext& session, CartContext& cart, ItemFactory& items, MusicDownloaderService& downloader) :
        m_mixer(mixer),
        m_session(session),
        m_cart(cart),
        m_items(items),
        m_downloader(downloader),
        m_cartCurrentPage(0),
        m_musicCurrentPage(0),
        m_shopCurrentPage(0),
        m_selector(nullptr)
    {
    }

    void StateMusicShop::Initialize()
    {
        State::Initialize();

        const auto bgm       = Instantiate<sf::Music>(Sound::BGM::BG_MUSIC_SHOP);

        const auto currentGem = Instantiate<BitmapNumber>(Resource::MusicShop::IDC_NUMBER_GEM);
        currentGem->SetValue(m_session.GetWallet().Gem);

        const auto currentCash = Instantiate<BitmapNumber>(Resource::MusicShop::IDC_NUMBER_CASH);
        currentCash->SetValue(m_session.GetWallet().Cash);

        const auto shopContainer     = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_SHOP);
        const auto musicContainer    = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_MUSIC);
        const auto downloadContainer = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_DOWNLOAD);
        const auto cartContainer     = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_CART);
        const auto downloadTabButton = Instantiate<Cx::Button>(Resource::MusicShop::IDC_BUTTON_DOWNLOAD_TAB);
        const auto cartTabButton     = Instantiate<Cx::Button>(Resource::MusicShop::IDC_BUTTON_CART_TAB);

        const auto showAllButton     = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::IDC_BUTTON_SHOW_ALL);
        const auto showBuyableButton = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::IDC_BUTTON_SHOW_BUYABLE);

        showAllButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShowAllButtonClicked(sender, ev); });
        showBuyableButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShowBuyableButtonClicked(sender, ev); });

        const auto shopList           = shopContainer->FindChild<List>(Resource::MusicShop::Shop::IDC_LIST_SHOP);
        const auto shopPrevPageButton = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::Shop::IDC_BUTTON_LEFT);
        const auto shopNextPageButton = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::Shop::IDC_BUTTON_RIGHT);

        shopPrevPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShopPrevPageButtonClicked(sender, ev); });
        shopNextPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnShopNextPageButtonClicked(sender, ev); });
        shopList->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnShopListScrolled(sender, ev); });

        const auto shopSortContainer = shopContainer->FindChild<Cx::UiContainer>(Resource::MusicShop::Shop::Sort::IDC_CONTAINER);
        const auto registerShopSort  = [this, shopSortContainer] (const char* id, const MusicSortKey key)
        {
            const auto button = shopSortContainer->FindChild<Cx::Button>(id);

            m_sortKeys[button] = key;
            button->SetClickCallback([this] (auto& sender, auto& ev) { OnShopSortButtonClicked(sender, ev); });
        };

        registerShopSort(Resource::MusicShop::Shop::Sort::IDC_BUTTON_NEW,   MusicSortKey::New);
        registerShopSort(Resource::MusicShop::Shop::Sort::IDC_BUTTON_GENRE, MusicSortKey::Genre);
        registerShopSort(Resource::MusicShop::Shop::Sort::IDC_BUTTON_TITLE, MusicSortKey::Title);
        registerShopSort(Resource::MusicShop::Shop::Sort::IDC_BUTTON_LEVEL, MusicSortKey::Level);
        registerShopSort(Resource::MusicShop::Shop::Sort::IDC_BUTTON_BPM,   MusicSortKey::Bpm);
        registerShopSort(Resource::MusicShop::Shop::Sort::IDC_BUTTON_PRICE, MusicSortKey::Price);

        downloadTabButton->SetClickCallback([this] (auto& sender, auto& ev) { OnDownloadTabButtonClicked(sender, ev); });
        cartTabButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartTabButtonClicked(sender, ev); });

        const auto musicList = musicContainer->FindChild<List>(Resource::MusicShop::IDC_LIST_MUSIC);
        for (const auto child : musicList->GetChildren())
        {
            const auto item = dynamic_cast<Cx::UiContainer*>(child);
            if (!item)
                continue;

            if (const auto selector = item->FindChild<Image>(Resource::MusicShop::MusicItem::IDC_IMAGE_SELECTOR))
            {
                selector->SetVisible(false);
                item->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnMusicItemFocusChanged(sender, ev); });

                if (const auto toggleButton = item->FindChild<Cx::ToggleButton>(Resource::MusicShop::MusicItem::IDC_TOGGLE_SELECT))
                {
                    toggleButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMusicItemToggleClicked(sender, ev); });
                    toggleButton->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnMusicItemToggleFocusChanged(sender, ev); });
                    item->SetClickCallback([this] (auto& sender, auto& ev) { OnMusicItemClicked(sender, ev); });
                }
            }
        }

        const auto musicPrevPageButton = musicContainer->FindChild<Cx::Button>(Resource::MusicShop::Music::IDC_BUTTON_LEFT);
        const auto musicNextPageButton = musicContainer->FindChild<Cx::Button>(Resource::MusicShop::Music::IDC_BUTTON_RIGHT);
        const auto downloadButton      = musicContainer->FindChild<Cx::Button>(Resource::MusicShop::Music::IDC_BUTTON_DOWNLOAD);

        musicPrevPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMusicPrevPageButtonClicked(sender, ev); });
        musicNextPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMusicNextPageButtonClicked(sender, ev); });
        musicList->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnMusicListScrolled(sender, ev); });
        downloadButton->SetClickCallback([this] (auto& sender, auto& ev) { OnDownloadButtonClicked(sender, ev); });

        const auto musicSortContainer = musicContainer->FindChild<Cx::UiContainer>(Resource::MusicShop::Music::Sort::IDC_CONTAINER);
        const auto registerMusicSort  = [this, musicSortContainer] (const char* id, const MusicSortKey key)
        {
            const auto button = musicSortContainer->FindChild<Cx::Button>(id);

            m_sortKeys[button] = key;
            button->SetClickCallback([this] (auto& sender, auto& ev) { OnMusicSortButtonClicked(sender, ev); });
        };

        registerMusicSort(Resource::MusicShop::Music::Sort::IDC_BUTTON_GENRE,  MusicSortKey::Genre);
        registerMusicSort(Resource::MusicShop::Music::Sort::IDC_BUTTON_TITLE,  MusicSortKey::Title);
        registerMusicSort(Resource::MusicShop::Music::Sort::IDC_BUTTON_LEVEL,  MusicSortKey::Level);
        registerMusicSort(Resource::MusicShop::Music::Sort::IDC_BUTTON_TIME,   MusicSortKey::Time);
        registerMusicSort(Resource::MusicShop::Music::Sort::IDC_BUTTON_STATUS, MusicSortKey::Status);

        const auto cancelButton = downloadContainer->FindChild<Cx::Button>(Resource::MusicShop::Download::IDC_BUTTON_CANCEL);
        cancelButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCancelButtonClicked(sender, ev); });

        m_downloader.SetDownloadStartedCallback([this] (const auto musicID) { OnDownloadStarted(musicID); });
        m_downloader.SetRenamingCallback([this] (const auto musicID) { OnDownloadRenaming(musicID); });
        m_downloader.SetQueueCompletedCallback([this] { OnQueueCompleted(); });
        m_downloader.SetErrorCallback([this] (const auto error) { OnDownloadFailed(error); });
        m_downloader.SetProgressCallback([this] (const auto& progress) { OnDownloadProgressed(progress); });

        const auto buyButton = cartContainer->FindChild<Cx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_BUY);
        buyButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartBuyButtonClicked(sender, ev); });

        const auto giftButton = cartContainer->FindChild<Cx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_GIFT);
        giftButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartGiftButtonClicked(sender, ev); });

        const auto cartList           = cartContainer->FindChild<List>(Resource::MusicShop::Cart::IDC_LIST_CART);
        const auto cartPrevPageButton = cartContainer->FindChild<Cx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_LEFT);
        const auto cartNextPageButton = cartContainer->FindChild<Cx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_RIGHT);

        cartPrevPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartPrevPageButtonClicked(sender, ev); });
        cartNextPageButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartNextPageButtonClicked(sender, ev); });
        cartList->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnCartListScrolled(sender, ev); });

        const auto backButton = Instantiate<Cx::Button>(Resource::MusicShop::IDC_BUTTON_BACK);
        backButton->SetClickCallback([this] (auto& sender, auto& ev) { OnBackButtonClicked(sender, ev); });

        showBuyableButton->PerformClick();
        downloadTabButton->PerformClick();
        InvalidateCart();
        InvalidateMusicList();
        InvalidateShopList();
        InvalidateDownloadPanel();

        bgm->setLooping(true);
        m_mixer.Play(*bgm, Sound::Channel::BGM);
    }

    void StateMusicShop::Finalize()
    {
        m_downloader.SetDownloadStartedCallback(nullptr);
        m_downloader.SetDownloadCompletedCallback(nullptr);
        m_downloader.SetRenamingCallback(nullptr);
        m_downloader.SetQueueCompletedCallback(nullptr);
        m_downloader.SetErrorCallback(nullptr);
        m_downloader.SetProgressCallback(nullptr);

        State::Finalize();
    }

    void StateMusicShop::SelectMusicFilter(const bool showAll)
    {
        const auto shopContainer     = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_SHOP);
        const auto showAllButton     = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::IDC_BUTTON_SHOW_ALL);
        const auto showBuyableButton = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::IDC_BUTTON_SHOW_BUYABLE);

        showAllButton->SetEnabled(!showAll);
        showAllButton->SetVisible(!showAll);

        showBuyableButton->SetEnabled(showAll);
        showBuyableButton->SetVisible(showAll);
    }

    void StateMusicShop::SelectShopTab(const bool download)
    {
        const auto downloadContainer = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_DOWNLOAD);
        const auto cartContainer     = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_CART);

        downloadContainer->SetEnabled(download);
        downloadContainer->SetVisible(download);

        cartContainer->SetEnabled(!download);
        cartContainer->SetVisible(!download);
    }

    void StateMusicShop::SetDownloadStatus(const sf::String& status)
    {
        const auto container = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_DOWNLOAD);
        if (const auto label = container->FindChild<Label>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_STATUS))
            label->SetString(status);
    }

    void StateMusicShop::InvalidateMusicList()
    {

        const auto container = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_MUSIC);
        const auto musicList = container->FindChild<List>(Resource::MusicShop::IDC_LIST_MUSIC);
        const auto slots     = musicList->GetChildren();

        m_musicList.clear();
        for (const auto& entry : m_session.GetMusicList())
        {
            if (entry.Status != MusicStatus::Unacquired)
                m_musicList.push_back(entry);
        }

        SortMusicList(m_musicList, m_musicSortKey, m_musicSortAscending);

        const auto maxPage = static_cast<unsigned int>(std::ceil(static_cast<float>(m_musicList.size()) / static_cast<float>(slots.size())));
        m_musicCurrentPage = maxPage > 0 && m_musicCurrentPage >= maxPage ? maxPage - 1 : m_musicCurrentPage;

        const auto progress = m_downloader.GetProgress();
        m_musicItemIndices.clear();

        for (std::size_t i = 0, j = m_musicCurrentPage * slots.size(); i < slots.size(); i++)
        {
            const auto slot = dynamic_cast<Cx::UiContainer*>(slots[i]);
            if (!slot)
                continue;

            if (j >= m_musicList.size())
            {
                slot->SetEnabled(false);
                slot->SetVisible(false);
                continue;
            }

            const auto& entry = m_musicList[j];
            m_musicItemIndices[slot] = j++;

            slot->SetEnabled(true);
            slot->SetVisible(true);

            if (const auto genre = slot->FindChild<Label>(Resource::MusicShop::MusicItem::IDC_TEXT_GENRE))
                genre->SetString(entry.Genre);

            if (const auto title = slot->FindChild<Label>(Resource::MusicShop::MusicItem::IDC_TEXT_TITLE))
            {
                title->SetString(entry.Title);
            }

            if (const auto level = slot->FindChild<Label>(Resource::MusicShop::MusicItem::IDC_TEXT_LEVEL))
            {
                auto levels = entry.Levels;
                level->SetString(fmt::format(Constants::Messages::MusicShop::MusicList::LEVELS,
                    levels[Difficulty::EX], levels[Difficulty::NX], levels[Difficulty::HX]));
            }

            if (const auto time = slot->FindChild<Label>(Resource::MusicShop::MusicItem::IDC_TEXT_TIME))
            {
                auto durations      = entry.Durations;
                const auto seconds  = static_cast<int>(durations[Difficulty::EX].asSeconds());
                time->SetString(fmt::format(Constants::Messages::MusicShop::MusicList::DURATION, seconds / 60, seconds % 60));
            }

            const bool downloading = m_downloader.IsDownloading() && progress.MusicID == entry.ID;
            if (const auto status = slot->FindChild<Image>(Resource::MusicShop::MusicItem::IDC_IMAGE_STATUS))
            {
                if (downloading)
                    status->SetFrame("Downloading");
                else
                    status->SetFrame(entry.Status == MusicStatus::Playable ? "Downloaded" : "NotDownloaded");
            }

            if (const auto toggleButton = slot->FindChild<Cx::ToggleButton>(Resource::MusicShop::MusicItem::IDC_TOGGLE_SELECT))
            {
                const bool downloadable = entry.Status != MusicStatus::Playable;

                toggleButton->SetCheckedState(m_downloader.Contains(static_cast<std::uint16_t>(entry.ID)));
                toggleButton->SetEnabled(downloadable);
                toggleButton->SetVisible(downloadable);
            }
        }

        const auto currentPage = container->FindChild<BitmapNumber>(Resource::MusicShop::Music::IDC_NUMBER_CURRENT_PAGE);
        const auto totalPage   = container->FindChild<BitmapNumber>(Resource::MusicShop::Music::IDC_NUMBER_MAX_PAGE);

        currentPage->SetValue(maxPage > 0 ? m_musicCurrentPage + 1 : 0);
        totalPage->SetValue(maxPage);
    }

    void StateMusicShop::InvalidateShopList()
    {

        const auto container = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_SHOP);
        const auto shopList  = container->FindChild<List>(Resource::MusicShop::Shop::IDC_LIST_SHOP);
        const auto slots     = shopList->GetChildren();

        m_shopList.clear();
        for (const auto& entry : m_session.GetNonPlayableMusicList())
        {
            if (entry.Status == MusicStatus::Unacquired)
                m_shopList.push_back(entry);
        }

        SortMusicList(m_shopList, m_shopSortKey, m_shopSortAscending);

        const auto maxPage = static_cast<unsigned int>(std::ceil(static_cast<float>(m_shopList.size()) / static_cast<float>(slots.size())));
        m_shopCurrentPage  = maxPage > 0 && m_shopCurrentPage >= maxPage ? maxPage - 1 : m_shopCurrentPage;

        for (std::size_t i = 0, j = m_shopCurrentPage * slots.size(); i < slots.size(); i++)
        {
            const auto slot = dynamic_cast<Image*>(slots[i]);
            if (!slot)
                continue;

            if (j >= m_shopList.size())
            {
                slot->SetEnabled(false);
                slot->SetVisible(false);
                continue;
            }

            const auto& entry = m_shopList[j++];

            slot->SetEnabled(true);
            slot->SetVisible(true);

            if (const auto title = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_TITLE))
            {
                title->SetString(entry.Title);
            }

            if (const auto artist = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_ARTIST))
            {
                artist->SetString(entry.Artist);
            }

            if (const auto editor = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_EDITOR))
            {
                editor->SetString(entry.NoteDesigner);
            }

            if (const auto genre = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_GENRE))
                genre->SetString(entry.Genre);

            if (const auto bpm = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_BPM))
                bpm->SetString(std::to_string(static_cast<int>(entry.BPM)));

            if (const auto level = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_LEVEL))
            {
                auto levels = entry.Levels;
                level->SetString(fmt::format(Constants::Messages::MusicShop::ShopList::LEVELS,
                    levels[Difficulty::EX], levels[Difficulty::NX], levels[Difficulty::HX]));
            }

            if (const auto time = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_TIME))
            {
                auto durations     = entry.Durations;
                const auto seconds = static_cast<int>(durations[Difficulty::EX].asSeconds());
                time->SetString(fmt::format(Constants::Messages::MusicShop::ShopList::DURATION, seconds / 60, seconds % 60));
            }

            if (const auto price = slot->FindChild<Label>(Resource::MusicShop::Shop::Item::IDC_TEXT_GEM_PRICE))
            {
                const auto gem = entry.Prices.find(Currency::Gem);
                price->SetString(gem != entry.Prices.end() ? std::to_string(gem->second) : "Free");
            }

            if (const auto badge = slot->FindChild<Image>(Resource::MusicShop::Shop::Item::IDC_IMAGE_NEW))
                badge->SetVisible(entry.New);

            if (const auto thumbnail = slot->FindChild<Image>(Resource::MusicShop::Shop::Item::IDC_IMAGE_THUMBNAIL))
            {
                auto sprite = static_cast<SpriteSheet*>(nullptr);
                if (const auto source = fmt::format("SMI_{}.ojs", entry.ID); Gx::FileSystem::Contains(source))
                {
                    try
                    {
                        sprite = &GetResources().AddFromFile<SpriteSheet>(source);
                    }
                    catch (...)
                    {
                    }
                }

                if (sprite && !sprite->TexCoords.empty())
                {
                    thumbnail->SetTexture(sprite->GetTexture(), true);
                    thumbnail->SetTexCoords(sprite->TexCoords.front());
                    thumbnail->SetVisible(true);
                }
                else
                    thumbnail->SetVisible(false);
            }

            if (const auto buyButton = slot->FindChild<Cx::Button>(Resource::MusicShop::Shop::Item::IDC_BUTTON_BUY))
            {
                buyButton->SetClickCallback([this, entry] (auto& sender, auto& ev)
                {
                    OnBuyButtonClicked(sender, ev, entry);
                });
            }
        }

        const auto currentPage = container->FindChild<BitmapNumber>(Resource::MusicShop::Shop::IDC_NUMBER_CURRENT_PAGE);
        const auto totalPage   = container->FindChild<BitmapNumber>(Resource::MusicShop::Shop::IDC_NUMBER_MAX_PAGE);

        currentPage->SetValue(maxPage > 0 ? m_shopCurrentPage + 1 : 0);
        totalPage->SetValue(maxPage);
    }

    void StateMusicShop::InvalidateDownloadPanel()
    {

        const auto container  = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_DOWNLOAD);
        const auto music      = container->FindChild<Label>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_MUSIC);
        const auto speed      = container->FindChild<Label>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_SPEED);
        const auto musicTime  = container->FindChild<Label>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_MUSIC_TIME);
        const auto totalTime  = container->FindChild<Label>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_TOTAL_TIME);
        const auto count      = container->FindChild<Label>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_COUNT);
        const auto size       = container->FindChild<Label>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_SIZE);
        const auto musicGauge = container->FindChild<Gauge>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_MUSIC_GAUGE);
        const auto totalGauge = container->FindChild<Gauge>(Resource::MusicShop::Download::IDC_TEXT_DOWNLOAD_TOTAL_GAUGE);
        const auto indicator  = container->FindChild<Gx::Animation>(Resource::MusicShop::Download::IDC_ANIMATION_INDICATOR);

        const auto progress   = m_downloader.GetProgress();
        const bool downloading = m_downloader.IsDownloading();

        indicator->SetVisible(downloading);
        musicGauge->SetValue(downloading ? progress.GetFilePercent() : 0.f);
        totalGauge->SetValue(downloading ? progress.GetTotalPercent() : 0.f);

        size->SetString(sf::String());
        if (!downloading)
        {
            music->SetString(sf::String());
            speed->SetString(sf::String());
            musicTime->SetString(sf::String());
            totalTime->SetString(sf::String());
            count->SetString(sf::String());

            return;
        }

        music->SetString(progress.MusicTitle);
        count->SetString(fmt::format(Constants::Messages::MusicShop::Download::PROGRESS_COUNT,
            progress.QueueIndex + 1, progress.QueueCount));

        if (progress.BytesPerSecond == 0)
            return;

        const auto format = [] (const std::uint64_t remaining, const std::uint64_t rate)
        {
            const auto seconds = static_cast<int>(remaining / rate);
            return fmt::format(Constants::Messages::MusicShop::Download::PROGRESS_ELAPSED,
                seconds / 3600, seconds % 3600 / 60, seconds % 60);
        };

        speed->SetString(fmt::format(Constants::Messages::MusicShop::Download::PROGRESS_PERCENT,
            static_cast<float>(progress.BytesPerSecond) / 1024.f));

        musicTime->SetString(format(progress.FileSize - progress.FileBytesRead, progress.BytesPerSecond));
        totalTime->SetString(format(progress.TotalSize - progress.TotalBytesRead, progress.BytesPerSecond));
    }

    void StateMusicShop::InvalidateCart()
    {
        const auto container = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_CART);
        const auto cartList  = container->FindChild<List>(Resource::MusicShop::Cart::IDC_LIST_CART);
        const auto slots     = cartList->GetChildren();
        const auto cartItems = m_cart.GetItems();
        const int maxPage    = static_cast<unsigned int>(std::ceil(static_cast<float>(cartItems.size()) / static_cast<float>(slots.size())));

        m_cartCurrentPage = m_cartCurrentPage >= maxPage ? maxPage - 1 : m_cartCurrentPage;
        const auto& itemData = m_items.GetItemData();
        const auto& setInfoData = m_items.GetSetInfoData();

        unsigned int gem = 0, cash = 0;
        const auto calculateItem = [&] (const unsigned int itemID)
        {
            if (const auto it = itemData.Items.find(itemID); it != itemData.Items.end())
            {
                // Item or Set with one single costume item
                auto metadata = it->second;
                if (const auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                    gem += gemPrice->second;
                else if (const auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                    cash += cashPrice->second;
            }
        };

        for (auto item : cartItems)
        {
            if (item.Type == CartItemType::EquipmentSet)
            {
                const auto& sets = setInfoData.Sets;
                if (auto set = sets.find(item.ID); set != sets.end())
                {
                    for (const auto itemID : set->second.ItemsIDs)
                    {
                        calculateItem(itemID);

                        if (set->second.Discounts.empty())
                            continue;

                        auto discounts = set->second.Discounts.find(itemID);
                        if (discounts == set->second.Discounts.end())
                            continue;

                        if (auto gemDiscount = discounts->second.find(Currency::Gem); gemDiscount != discounts->second.end())
                            gem = gem < gemDiscount->second ? 0 : gem - gemDiscount->second;
                        else if (auto cashDiscount = discounts->second.find(Currency::Cash); cashDiscount != discounts->second.end())
                            cash = cash < cashDiscount->second ? 0 : cash - cashDiscount->second;
                    }
                }
            }
            else if (item.Type == CartItemType::Equipment)
                calculateItem(item.ID);

            // TODO: Music
        }

        for (std::size_t i = 0, j = m_cartCurrentPage * slots.size(); i < slots.size(); i++)
        {
            const auto slot = dynamic_cast<Cx::UiContainer*>(slots[i]);
            if (!slot)
                continue;

            if (j >= cartItems.size())
            {
                slot->SetEnabled(false);
                slot->SetVisible(false);
                continue;
            }

            slot->SetEnabled(true);
            slot->SetVisible(true);

            auto item               = cartItems[j++];
            const auto id           = slot->FindChild<Label>(Resource::MusicShop::Cart::Item::IDC_TEXT_NUMBER);
            const auto name         = slot->FindChild<Label>(Resource::MusicShop::Cart::Item::IDC_TEXT_NAME);
            const auto type         = slot->FindChild<Image>(Resource::MusicShop::Cart::Item::IDC_IMAGE_ITEM_TYPE);
            const auto price        = slot->FindChild<Label>(Resource::MusicShop::Cart::Item::IDC_TEXT_PRICE);
            const auto deleteButton = slot->FindChild<Cx::Button>(Resource::MusicShop::Cart::Item::IDC_BUTTON_DELETE);

            id->SetString(std::to_string(j));
            if (item.Type == CartItemType::EquipmentSet)
            {
                const auto& sets = setInfoData.Sets;
                if (auto set = sets.find(item.ID); set != sets.end())
                {
                    name->SetString(set->second.Name);
                    unsigned int setPriceGem  = 0;
                    unsigned int setPriceCash = 0;

                    for (const auto itemID : set->second.ItemsIDs)
                    {
                        if (auto it = itemData.Items.find(itemID); it != itemData.Items.end())
                        {
                            auto metadata = it->second;
                            if (auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                                setPriceGem += gemPrice->second;
                            else if (auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                                setPriceCash += cashPrice->second;

                            if (set->second.Discounts.empty())
                                continue;

                            auto discounts = set->second.Discounts.find(itemID);
                            if (discounts == set->second.Discounts.end())
                                continue;

                            if (auto gemDiscount = discounts->second.find(Currency::Gem); gemDiscount != discounts->second.end())
                                setPriceGem = setPriceGem < gemDiscount->second ? 0 : setPriceGem - gemDiscount->second;
                            else if (auto cashDiscount = discounts->second.find(Currency::Cash); cashDiscount != discounts->second.end())
                                setPriceCash = setPriceCash < cashDiscount->second ? 0 : setPriceCash - cashDiscount->second;
                        }
                    }

                    if (setPriceGem > 0)
                        price->SetString(fmt::format("{}G", setPriceGem));
                    else
                        price->SetString(fmt::format("{}M", setPriceCash));
                }

                type->SetFrame("EquipmentSet");
            }
            else if (item.Type == CartItemType::Equipment)
            {
                if (auto it = itemData.Items.find(item.ID); it != itemData.Items.end())
                {
                    auto metadata = it->second;
                    name->SetString(metadata.Name);
                    if (auto gemPrice = metadata.Prices.find(Currency::Gem); gemPrice != metadata.Prices.end())
                        price->SetString(fmt::format("{}G", gemPrice->second));
                    else if (auto cashPrice = metadata.Prices.find(Currency::Cash); cashPrice != metadata.Prices.end())
                        price->SetString(fmt::format("{}M", cashPrice->second));
                }

                type->SetFrame("Equipment");
            }
            else if (item.Type == CartItemType::Music)
            {
                type->SetFrame("Music");
            }

            m_cartDeleteButtonIndices[deleteButton] = j - 1;
            deleteButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCartItemDeleteButtonClicked(sender, ev); });
        }

        const auto currentPage = container->FindChild<BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_CURRENT_PAGE);
        const auto totalPage   = container->FindChild<BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_MAX_PAGE);
        const auto totalGem    = container->FindChild<BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_TOTAL_GEM);
        const auto totalCash   = container->FindChild<BitmapNumber>(Resource::MusicShop::Cart::IDC_NUMBER_TOTAL_CASH);

        currentPage->SetValue(maxPage > 0 ? m_cartCurrentPage + 1 : 0);
        totalPage->SetValue(maxPage);
        totalGem->SetValue(gem);
        totalCash->SetValue(cash);
    }

    void StateMusicShop::OnShowAllButtonClicked(Control& sender, Control::Event& ev)
    {
        SelectMusicFilter(true);
    }

    void StateMusicShop::OnShowBuyableButtonClicked(Control& sender, Control::Event& ev)
    {
        SelectMusicFilter(false);
    }

    void StateMusicShop::OnDownloadTabButtonClicked(Control& sender, Control::Event& ev)
    {
        SelectShopTab(true);
    }

    void StateMusicShop::OnCartTabButtonClicked(Control& sender, Control::Event& ev)
    {
        SelectShopTab(false);
    }

    void StateMusicShop::OnMusicItemFocusChanged(Control& sender, Control::Event& ev)
    {
        const auto selector = sender.FindChild<Image>(Resource::MusicShop::MusicItem::IDC_IMAGE_SELECTOR);

        selector->SetVisible(sender.IsFocused());
        if (const auto toggleButton = sender.FindChild<Cx::ToggleButton>(Resource::MusicShop::MusicItem::IDC_TOGGLE_SELECT); sender.IsFocused() && toggleButton)
            toggleButton->SetFocus(sender.IsFocused());
    }

    void StateMusicShop::OnMusicItemToggleClicked(Control& sender, Control::Event& ev)
    {
        ev.Handled = true; // Use item click callback
    }

    void StateMusicShop::OnMusicItemToggleFocusChanged(Control& sender, Control::Event& ev)
    {
        const auto item = sender.GetParent<Cx::UiContainer>();
        ev.State = item->IsFocused() ? Control::State::Hover : Control::State::Normal;
    }

    void StateMusicShop::OnMusicItemClicked(Control& sender, Control::Event& ev)
    {
        const auto index = m_musicItemIndices.find(&sender);
        if (index == m_musicItemIndices.end())
            return;

        const auto& entry = m_musicList[index->second];
        if (entry.Status == MusicStatus::Playable || m_downloader.IsDownloading())
            return;

        const auto toggleButton = sender.FindChild<Cx::ToggleButton>(Resource::MusicShop::MusicItem::IDC_TOGGLE_SELECT);
        const auto musicID      = static_cast<std::uint16_t>(entry.ID);

        if (m_downloader.Contains(musicID))
        {
            m_downloader.Remove(musicID);
            toggleButton->SetCheckedState(false);
        }
        else
        {
            m_downloader.Enqueue(musicID);
            toggleButton->SetCheckedState(true);
        }
    }

    void StateMusicShop::OnMusicPrevPageButtonClicked(Control& sender, Control::Event& ev)
    {
        if (m_musicCurrentPage > 0)
        {
            m_musicCurrentPage--;
            InvalidateMusicList();
        }
    }

    void StateMusicShop::OnMusicNextPageButtonClicked(Control& sender, Control::Event& ev)
    {
        m_musicCurrentPage++;
        InvalidateMusicList();
    }

    void StateMusicShop::OnMusicListScrolled(Control& sender, Control::Event& ev)
    {
        const auto musicContainer      = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_MUSIC);
        const auto musicPrevPageButton = musicContainer->FindChild<Cx::Button>(Resource::MusicShop::Music::IDC_BUTTON_LEFT);
        const auto musicNextPageButton = musicContainer->FindChild<Cx::Button>(Resource::MusicShop::Music::IDC_BUTTON_RIGHT);

        if (ev.Delta > 0)
            musicNextPageButton->PerformClick();
        else
            musicPrevPageButton->PerformClick();
    }

    void StateMusicShop::OnShopPrevPageButtonClicked(Control& sender, Control::Event& ev)
    {
        if (m_shopCurrentPage > 0)
        {
            m_shopCurrentPage--;
            InvalidateShopList();
        }
    }

    void StateMusicShop::OnShopNextPageButtonClicked(Control& sender, Control::Event& ev)
    {
        m_shopCurrentPage++;
        InvalidateShopList();
    }

    void StateMusicShop::OnShopListScrolled(Control& sender, Control::Event& ev)
    {
        const auto shopContainer      = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_SHOP);
        const auto shopPrevPageButton = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::Shop::IDC_BUTTON_LEFT);
        const auto shopNextPageButton = shopContainer->FindChild<Cx::Button>(Resource::MusicShop::Shop::IDC_BUTTON_RIGHT);

        if (ev.Delta > 0)
            shopNextPageButton->PerformClick();
        else
            shopPrevPageButton->PerformClick();
    }

    void StateMusicShop::SortMusicList(std::vector<ChartMetadata>& list, const MusicSortKey key, const bool ascending) const
    {
        const auto level = [] (const ChartMetadata& entry)
        {
            const auto it = entry.Levels.find(Difficulty::EX);
            return it != entry.Levels.end() ? it->second : 0u;
        };

        const auto duration = [] (const ChartMetadata& entry)
        {
            const auto it = entry.Durations.find(Difficulty::EX);
            return it != entry.Durations.end() ? it->second : sf::Time::Zero;
        };

        const auto price = [] (const ChartMetadata& entry)
        {
            const auto gem = entry.Prices.find(Currency::Gem);
            return gem != entry.Prices.end() ? gem->second : 0u;
        };

        std::stable_sort(list.begin(), list.end(), [&] (const ChartMetadata& a, const ChartMetadata& b)
        {
            const auto& lhs = ascending ? a : b;
            const auto& rhs = ascending ? b : a;

            switch (key)
            {
                case MusicSortKey::New:    return lhs.New && !rhs.New;
                case MusicSortKey::Genre:  return lhs.Genre < rhs.Genre;
                case MusicSortKey::Title:  return lhs.Title < rhs.Title;
                case MusicSortKey::Level:  return level(lhs) < level(rhs);
                case MusicSortKey::Bpm:    return lhs.BPM < rhs.BPM;
                case MusicSortKey::Price:  return price(lhs) < price(rhs);
                case MusicSortKey::Time:   return duration(lhs) < duration(rhs);
                case MusicSortKey::Status: return static_cast<int>(lhs.Status) < static_cast<int>(rhs.Status);
            }

            return false;
        });
    }

    void StateMusicShop::OnShopSortButtonClicked(Control& sender, Control::Event& ev)
    {
        const auto key = m_sortKeys.find(&sender);
        if (key == m_sortKeys.end())
            return;

        auto sortKey   = key->second;
        auto ascending = m_shopSortKey == sortKey ? !m_shopSortAscending : true;

        m_shopSortAscending = ascending;
        m_shopSortKey       = sortKey;

        InvalidateShopList();
    }

    void StateMusicShop::OnMusicSortButtonClicked(Control& sender, Control::Event& ev)
    {
        const auto key = m_sortKeys.find(&sender);
        if (key == m_sortKeys.end())
            return;

        auto sortKey   = key->second;
        auto ascending = m_musicSortKey == sortKey ? !m_musicSortAscending : true;

        m_musicSortAscending = ascending;
        m_musicSortKey       = sortKey;

        InvalidateMusicList();
    }

    void StateMusicShop::OnDownloadButtonClicked(Control& sender, Control::Event& ev)
    {
        if (m_downloader.IsDownloading() || m_downloader.GetQueueCount() == 0)
            return;

        SetDownloadStatus(Constants::Messages::MusicShop::Download::INITIALIZING);
        m_downloader.StartDownload();

        SelectShopTab(true);
        InvalidateMusicList();
    }

    void StateMusicShop::OnCancelButtonClicked(Control& sender, Control::Event& ev)
    {
        if (!m_downloader.IsDownloading())
            return;

        ShowDialog(Constants::Messages::MusicShop::DOWNLOAD_CANCEL_CONFIRM, DialogStyle::YesNo, [this] (const bool answer)
        {
            OnCancelDialogAnswered(answer);
        });
    }

    void StateMusicShop::OnCancelDialogAnswered(const bool answer)
    {
        if (!answer)
            return;

        m_downloader.Cancel();
    }

    void StateMusicShop::OnDownloadStarted(std::uint16_t musicID)
    {

        SetDownloadStatus(Constants::Messages::MusicShop::Download::DOWNLOADING);
        InvalidateMusicList();
    }

    void StateMusicShop::OnDownloadRenaming(std::uint16_t musicID)
    {

        SetDownloadStatus(Constants::Messages::MusicShop::Download::RENAMING);
    }

    void StateMusicShop::OnDownloadProgressed(const MusicDownloadProgress& progress)
    {

        if (progress.Status == MusicDownloadStatus::Connecting)
            SetDownloadStatus(Constants::Messages::MusicShop::Download::CONNECTING);

        InvalidateDownloadPanel();
    }

    void StateMusicShop::OnDownloadCompleted(std::uint16_t musicID)
    {

        SetDownloadStatus(Constants::Messages::MusicShop::Download::COMPLETED);
        auto _ = m_session.GetMusicList(true);

        InvalidateMusicList();
    }

    void StateMusicShop::OnQueueCompleted()
    {

        auto _ = m_session.GetMusicList(true);

        InvalidateMusicList();
    }

    void StateMusicShop::OnDownloadFailed(const MusicDownloadError error)
    {

        switch (error)
        {
            case MusicDownloadError::ConnectionFailed:
                SetDownloadStatus(Constants::Messages::MusicShop::Download::CONNECT_FAILED);
                ShowDialog(Constants::Messages::MusicShop::Download::CONNECT_FAILED_NOTICE, DialogStyle::Information);
                break;

            case MusicDownloadError::InsufficientDiskSpace:
                SetDownloadStatus(Constants::Messages::MusicShop::Download::DISK_FULL);
                ShowDialog(Constants::Messages::MusicShop::Download::DISK_FULL_NOTICE, DialogStyle::Information);
                break;

            case MusicDownloadError::DownloadFailed:
                SetDownloadStatus(Constants::Messages::MusicShop::Download::FILE_NOT_FOUND);
                ShowDialog(Constants::Messages::MusicShop::Download::FAILED_NOTICE, DialogStyle::Information);
                break;

            default:
                SetDownloadStatus(sf::String());
                break;
        }

        const auto _ = m_session.GetMusicList(true);
        InvalidateMusicList();
    }

    void StateMusicShop::OnBuyButtonClicked(Control& sender, Control::Event& ev, const ChartMetadata& entry)
    {
        auto music = entry;

        if (music.Prices.empty())
        {
            ShowDialog(Constants::Messages::MusicShop::ALREADY_FREE, DialogStyle::Information);
        }
        else
        {
            const auto gem = music.Prices.find(Currency::Gem);
            const auto prompt = fmt::format(
                Constants::Messages::MusicShop::Purchase::PROMPT,
                Constants::Messages::MusicShop::Purchase::TITLE,
                music.Title,
                Constants::Messages::MusicShop::Purchase::PRICE,
                sf::String(std::to_string(gem != music.Prices.end() ? gem->second : 0)),
                Constants::Messages::MusicShop::Purchase::CURRENCY,
                Constants::Messages::MusicShop::Purchase::CONFIRM
            );

            ShowDialog(prompt, DialogStyle::YesNo, [this] (const bool)
            {
                ShowDialog("Music shop purchase is currently not available.", DialogStyle::Information);
            });
        }
    }

    void StateMusicShop::OnCartBuyButtonClicked(Control& sender, Control::Event& ev)
    {
        if (m_cart.GetItems().size() == 0)
        {
            ShowDialog("Shopping bag is empty", DialogStyle::Information);
            return;
        }

        ShowDialog("Would you like to move\nto the transaction window?", DialogStyle::YesNo, [=] (const bool answer)
        {
            OnCartBuyDialogAnswered(answer);
        });
    }

    void StateMusicShop::OnCartBuyDialogAnswered(const bool answer)
    {
        if (answer)
        {
            m_cart.SetCheckoutType(CartContext::CheckoutType::Music);
            m_mixer.Play(*Instantiate<sf::Sound>(Sound::Effects::EF_02), Sound::Channel::SFX);
            GetDirector().Present<StatePayment>();
        }
        else
            m_mixer.Play(*Instantiate<sf::Sound>(Sound::Effects::EF_03), Sound::Channel::SFX);
    }

    void StateMusicShop::OnCartGiftButtonClicked(Control& sender, Control::Event& ev)
    {
        if (m_cart.GetItems().size() == 0)
        {
            ShowDialog("Shopping bag is empty", DialogStyle::Information);
            return;
        }

        ShowDialog("Gift is currently not available", DialogStyle::Information);
    }

    void StateMusicShop::OnCartPrevPageButtonClicked(Control& sender, Control::Event& ev)
    {
        if (m_cartCurrentPage > 0)
        {
            m_cartCurrentPage--;
            InvalidateCart();
        }
    }

    void StateMusicShop::OnCartNextPageButtonClicked(Control& sender, Control::Event& ev)
    {
        m_cartCurrentPage++;
        InvalidateCart();
    }

    void StateMusicShop::OnCartListScrolled(Control& sender, Control::Event& ev)
    {
        const auto cartContainer      = Instantiate<Cx::UiContainer>(Resource::MusicShop::IDC_CONTAINER_CART);
        const auto cartPrevPageButton = cartContainer->FindChild<Cx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_LEFT);
        const auto cartNextPageButton = cartContainer->FindChild<Cx::Button>(Resource::MusicShop::Cart::IDC_BUTTON_RIGHT);

        if (ev.Delta > 0)
            cartNextPageButton->PerformClick();
        else
            cartPrevPageButton->PerformClick();
    }

    void StateMusicShop::OnCartItemDeleteButtonClicked(Control& sender, Control::Event& ev)
    {
        m_cart.Remove(m_cartDeleteButtonIndices.at(&sender));
        InvalidateCart();
    }

    void StateMusicShop::OnBackButtonClicked(Control& sender, Control::Event& ev)
    {
        if (const auto sfx = Find<sf::Sound>(Sound::Effects::EF_35))
            m_mixer.Play(*sfx, Sound::Channel::SFX);

        GetDirector().Dismiss<StateRoom>();
    }
}
