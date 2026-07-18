#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>
#include <CXO2/UI/Waiting/SpeedButton.hpp>

#include <CXO2/States/State.hpp>
#include <CXO2/Models/Game.hpp>

#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartLoader.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/RoomContext.hpp>

#include <CXO2/Utilities/StringFormatter.hpp>
#include <CXO2/StringTable/Identifiers/Cache.hpp>
#include <CXO2/StringTable/Identifiers/Sound.hpp>
#include <CXO2/StringTable/Identifiers/SelectMusic.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/ToggleButton.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/List.hpp>

#include <magic_enum/magic_enum.hpp>
#include <cmath>
#include <iterator>
#include <limits>
#include <unordered_set>

namespace Cx
{
    using namespace StringTable::Identifiers;

    SelectMusicDialog::SelectMusicDialog(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, SessionContext& session, RoomContext& room) :
        m_coverID(0),
        m_speed(0),
        m_mixer(mixer),
        m_resources(resources),
        m_session(session),
        m_room(room),
        m_music()
    {
    }

    void SelectMusicDialog::Initialize()
    {
        Gx::Dialog::Initialize();

        if (m_initialized)
        {
            InvalidateMusicList();
            return;
        }

        m_page = 0;
        m_musicList = m_session.GetInstalledMusic();
        FilterMusic();

        m_random     = m_room.GetRandomLevel();
        m_difficulty = m_room.GetDifficulty();
        m_speed      = m_room.GetSpeed();
        m_speedMode  = m_room.GetSpeedMode();

        if (!m_filteredList.empty())
        {
            if (m_room.GetMusic().ID != 0)
                m_music = m_room.GetMusic();
            else
                m_music = m_filteredList[m_filteredList.size() - 1];
        }

        if (auto leftButton = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_LEFT))
            leftButton->SetClickCallback([this] (auto& sender, auto& ev) { OnLeftButtonClicked(sender, ev); });

        if (auto rightButton = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_RIGHT))
            rightButton->SetClickCallback([this] (auto& sender, auto& ev) { OnRightButtonClicked(sender, ev); });

        if (auto list = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR); list)
        {
            auto children = list->GetChildren();
            for (std::size_t i = 0; i < children.size(); i++)
            {
                auto button = dynamic_cast<Gx::RadioButton*>(children[i]);
                if (!button)
                    continue;

                if (m_musicButtonIndices.empty())
                {
                    if (auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
                        m_titleColor = title->GetColor();
                }

                if (auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                    activeHighlighter->SetVisible(false);

                if (auto focusHighlighter = button->FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_HIGHLIGHT); focusHighlighter)
                    focusHighlighter->SetVisible(false);

                button->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnMusicButtonFocusChanged(sender, ev); });

                m_musicButtonIndices[button] = i;
                button->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnMusicButtonCheckChanged(sender, ev); });
            }

            list->SetScrollWheelCallback([this] (auto& sender, auto& ev) { OnMusicSelectorScrolled(sender, ev); });
        }

        if (auto sortSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_SORT_SELECTOR); sortSelector)
        {
            if (auto newButton = sortSelector->FindChild<Gx::Button>(Resource::SelectMusic::Sort::IDC_BUTTON_SORT_NEW); newButton)
            {
                newButton->SetClickCallback([this] (auto& sender, auto& ev) { OnSortNewButtonClicked(sender, ev); });
            }

            if (auto titleButton = sortSelector->FindChild<Gx::Button>(Resource::SelectMusic::Sort::IDC_BUTTON_SORT_TITLE); titleButton)
            {
                titleButton->SetClickCallback([this] (auto& sender, auto& ev) { OnSortTitleButtonClicked(sender, ev); });
            }

            if (auto levelButton = sortSelector->FindChild<Gx::Button>(Resource::SelectMusic::Sort::IDC_BUTTON_SORT_DIFF); levelButton)
            {
                levelButton->SetClickCallback([this] (auto& sender, auto& ev) { OnSortLevelButtonClicked(sender, ev); });
            }

            if (auto durationButton = sortSelector->FindChild<Gx::Button>(Resource::SelectMusic::Sort::IDC_BUTTON_SORT_TIME); durationButton)
            {
                durationButton->SetClickCallback([this] (auto& sender, auto& ev) { OnSortDurationButtonClicked(sender, ev); });
            }
        }

        if (auto genreSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_GENRE_SELECTOR); genreSelector)
        {
            if (auto allButton = genreSelector->FindChild<Gx::RadioButton>(Resource::SelectMusic::Genre::IDC_RADIO_GENRE_ALL); allButton)
                allButton->SetCheckedState(true);

            std::unordered_map<std::string, std::optional<Genre>> genreMap = {
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_ALL,         std::nullopt },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_BALLAD,      Genre::Ballad },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_ROCK,        Genre::Rock },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_DANCE,       Genre::Dance },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_TECHNO,      Genre::Techno },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_HIPHOP,      Genre::HipHop },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_SOUL,        Genre::Soul },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_JAZZ,        Genre::Jazz },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_FUNK,        Genre::Funk },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_CLASSICAL,   Genre::Classical },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_TRADITIONAL, Genre::Traditional },
                { Resource::SelectMusic::Genre::IDC_RADIO_GENRE_ETC,         Genre::Etc },
            };

            for (auto [key, genre] : genreMap)
            {
                auto button = genreSelector->FindChild<Gx::RadioButton>(key);
                if (!button)
                    continue;

                m_genreButtonValues[button] = genre;
                button->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnGenreButtonCheckChanged(sender, ev); });
            }
        }

        if (auto randomSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_RANDOM_SELECTOR); randomSelector)
        {
            std::unordered_map<std::string, LevelCategory> randomLevelMap = {
                { Resource::SelectMusic::Random::IDC_TOGGLE_RANDOM_DIFF_1, LevelCategory::Level1 },
                { Resource::SelectMusic::Random::IDC_TOGGLE_RANDOM_DIFF_2, LevelCategory::Level2 },
                { Resource::SelectMusic::Random::IDC_TOGGLE_RANDOM_DIFF_3, LevelCategory::Level3 },
                { Resource::SelectMusic::Random::IDC_TOGGLE_RANDOM_DIFF_4, LevelCategory::Level4 },
            };

            for (auto [id, level] : randomLevelMap)
            {
                auto button = randomSelector->FindChild<Gx::ToggleButton>(id);
                if (!button)
                    continue;

                m_randomLevelButtonValues[button] = level;
                button->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnRandomLevelButtonCheckChanged(sender, ev); });
            }
        }

        if (auto levelSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_DIFFICULTY_SELECTOR); levelSelector)
        {
            if (auto exButton = levelSelector->FindChild<Gx::RadioButton>(Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_EX); exButton)
            {
                exButton->SetCheckedState(true);
                m_difficultyButtonValues[exButton] = Difficulty::EX;
                exButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnExButtonCheckChanged(sender, ev); });
            }

            if (auto nxButton = levelSelector->FindChild<Gx::RadioButton>(Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_NX); nxButton)
            {
                m_difficultyButtonValues[nxButton] = Difficulty::NX;
                nxButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnNxButtonCheckChanged(sender, ev); });
            }

            if (auto hxButton = levelSelector->FindChild<Gx::RadioButton>(Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_HX); hxButton)
            {
                m_difficultyButtonValues[hxButton] = Difficulty::HX;
                hxButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnHxButtonCheckChanged(sender, ev); });
            }
        }

        if (auto speedSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_SPEED_SELECTOR); speedSelector)
        {
            for (auto child : speedSelector->GetChildren())
            {
                auto button = dynamic_cast<SpeedButton*>(child);
                if (!button)
                    continue;

                const float speed = button->GetSpeed();
                if (button->GetMode() == SpeedMode::HiSpeed && speed <= 0.f)
                    continue;

                if (button->GetMode() == m_speedMode && (m_speedMode != SpeedMode::HiSpeed || speed == m_speed))
                    button->SetCheckedState(true);

                m_speedButtons.push_back(button);
                button->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnSpeedButtonCheckChanged(sender, ev); });
            }
        }

        Sort(m_room.GetMusicSortMode(), m_room.GetMusicSortOrder());
        m_initialized = true;
    }

    ChartMetadata SelectMusicDialog::GetSelectedMusic() const
    {
        return m_music;
    }

    Difficulty SelectMusicDialog::GetSelectedDifficulty() const
    {
        return m_difficulty;
    }

    LevelCategory SelectMusicDialog::GetSelectedRandomLevels() const
    {
        return m_random;
    }

    bool SelectMusicDialog::IsRandomActive() const
    {
        return m_random != static_cast<LevelCategory>(0);
    }

    Genre SelectMusicDialog::GetSelectedGenre() const
    {
        return m_genre.value_or(Genre::Etc);
    }

    float SelectMusicDialog::GetSelectedSpeed() const
    {
        return m_speed;
    }

    SpeedMode SelectMusicDialog::GetSelectedSpeedMode() const
    {
        return m_speedMode;
    }

    void SelectMusicDialog::Sort(const MusicSortMode sort, const MusicSortOrder order)
    {
        m_sort  = sort;
        m_order = order;

        switch (sort)
        {
            case MusicSortMode::ID:
                std::sort(m_filteredList.begin(), m_filteredList.end(), [this] (auto& a, auto& b)
                {
                    if (m_order == MusicSortOrder::Ascending)
                        return a.ID < b.ID;

                    return a.ID > b.ID;
                });
                break;
            case MusicSortMode::Title:
                std::sort(m_filteredList.begin(), m_filteredList.end(), [this] (auto& a, auto& b)
                {
                    if (m_order == MusicSortOrder::Ascending)
                        return a.Title.toAnsiString() < b.Title.toAnsiString();

                    return a.Title.toAnsiString() > b.Title.toAnsiString();
                });
                break;
            case MusicSortMode::Level:
                std::sort(m_filteredList.begin(), m_filteredList.end(), [this] (auto& a, auto& b)
                {
                    auto x = a.Levels[m_difficulty];
                    auto y = b.Levels[m_difficulty];

                    if (m_order == MusicSortOrder::Ascending)
                        return x < y;

                    return x > y;
                });
                break;
            case MusicSortMode::Duration:
                std::sort(m_filteredList.begin(), m_filteredList.end(), [this] (auto& a, auto& b)
                {
                    auto x = a.Durations[m_difficulty];
                    auto y = b.Durations[m_difficulty];

                    if (m_order == MusicSortOrder::Ascending)
                        return x < y;

                    return x > y;
                });
                break;
            default:
                break;
        }

        InvalidateMusicList();
    }

    void SelectMusicDialog::FilterMusic()
    {
        m_filteredList.clear();
        for (auto& metadata : m_musicList)
        {
            auto musicGenre = magic_enum::enum_cast<Genre>(metadata.Genre.toAnsiString())
                .value_or(Genre::Etc);

            if (!m_genre.has_value() || musicGenre == m_genre)
                m_filteredList.push_back(metadata);
        }
    }

    unsigned int SelectMusicDialog::CountRandomMusic(const int min, const int max, std::unordered_set<unsigned int>& scanned)
    {
        return std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned, min, max] (ChartMetadata& m)
        {
            const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
            const bool result = std::any_of(diffs.begin(), diffs.end(), [&m, min, max] (auto diff)
            {
                const int level = m.Levels[diff];
                return level >= min && level <= max;
            });

            if (result)
            {
                if (auto [_, inserted] = scanned.insert(m.ID); !inserted)
                    return false;
            }

            return result;
        });
    }

    unsigned int SelectMusicDialog::GetMaxPage() const
    {
        const auto musicSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        if (!musicSelector)
            return 1;

        return std::ceil(static_cast<float>(m_filteredList.size()) / static_cast<float>(musicSelector->GetChildrenCount()));
    }

    unsigned int SelectMusicDialog::GetPage() const
    {
        return m_page;
    }

    void SelectMusicDialog::SetPage(const unsigned int page)
    {
        if (IsRandomActive())
            return;

        if (page >= GetMaxPage())
            return;

        m_page  = page;
        m_music = ChartMetadata{};
        InvalidateMusicList();
    }

    void SelectMusicDialog::ToggleSort(const MusicSortMode mode)
    {
        Sort(mode, m_sort != mode || m_order != MusicSortOrder::Ascending ? MusicSortOrder::Ascending : MusicSortOrder::Descending);
    }

    void SelectMusicDialog::SelectDifficulty(Gx::RadioButton& sender, const Difficulty difficulty)
    {
        if (!sender.IsChecked())
            return;

        if (IsRandomActive())
        {
            sender.SetCheckedState(false);
            return;
        }

        m_difficulty = difficulty;
        if (m_sort.has_value() && m_order.has_value())
            Sort(m_sort.value(), m_order.value());
        else
            InvalidateMusicList();
    }

    void SelectMusicDialog::InvalidateMusicList()
    {
        Invalidate();

        const auto musicSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        if (!musicSelector)
            return;

        const unsigned int itemListCount = musicSelector->GetChildrenCount();
        const unsigned int maxPage = GetMaxPage();

        if (!m_music.Source.empty())
        {
            unsigned int i = 0;
            for (auto m : m_filteredList)
            {
                if (m.Source == m_music.Source)
                    break;

                i++;
            }

            m_page = std::floor(static_cast<float>(i) / static_cast<float>(itemListCount));
        }

        m_page = static_cast<unsigned int>(std::min(maxPage - 1, m_page));
        const auto pager = FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_PAGE);
        if (pager)
        {
            pager->SetString(fmt::format("[{0:02}/{1:02}]", static_cast<int>(m_page) + 1, static_cast<int>(maxPage)));
            pager->SetVisible(true);
        }

        if (IsRandomActive())
        {
            m_music = ChartMetadata{};
            if (pager)
                pager->SetVisible(false);

            InvalidateRandomView();
        }
        else
        {
            InvalidateSelectors();
            if (m_music.Source.empty())
                return;

            InvalidateMusicInfo();
        }
    }

    void SelectMusicDialog::InvalidateRandomView()
    {
        struct LevelRange
        {
            const char* Label;
            int Min;
            int Max;
        };

        static constexpr LevelRange ranges[] = {
            { "LEVEL 1 - 5",     std::numeric_limits<int>::min(), 5 },
            { "LEVEL 5 - 9",     6,  9 },
            { "LEVEL 9 - 13",    10, 13 },
            { "LEVEL 13 higher", 13, std::numeric_limits<int>::max() },
        };

        const auto musicSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        const auto elements = musicSelector->GetChildren();

        std::unordered_set<unsigned int> scanned;
        m_randomMusicCount = 0;

        for (std::size_t r = 1; r < elements.size(); r++)
        {
            if (const auto button = dynamic_cast<Gx::RadioButton*>(elements[r]); button)
            {
                auto lv = static_cast<LevelCategory>(1 << (r - 1));
                const bool isRandomActivated = r - 1 < std::size(ranges) && (static_cast<int>(m_random) & static_cast<int>(lv)) != 0;

                button->SetCheckedState(false);
                button->SetEnabled(false);

                if (const auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
                {
                    button->SetVisible(isRandomActivated);
                    if (const auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                        activeHighlighter->SetVisible(false);

                    Gx::Label* infoLabel = nullptr;
                    if (const auto infoList = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_INFO); infoList && r - 1 < infoList->GetChildrenCount())
                        infoLabel = dynamic_cast<Gx::Label*>(infoList->GetChildren()[r - 1]);

                    if (isRandomActivated)
                    {
                        const auto& range = ranges[r - 1];

                        title->SetColor(sf::Color(200, 155, 55));
                        title->SetString(range.Label);
                        if (infoLabel)
                            infoLabel->SetString(range.Label);

                        m_randomMusicCount += CountRandomMusic(range.Min, range.Max, scanned);
                    }
                    else
                    {
                        title->SetString(sf::String());
                        if (infoLabel)
                            infoLabel->SetString(sf::String());
                    }
                }

                if (const auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
                    level->SetString(sf::String());

                if (const auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
                    duration->SetString(sf::String());
            }
        }

        if (!elements.empty())
        {
            if (const auto button = dynamic_cast<Gx::RadioButton*>(elements[0]); button)
            {
                if (const auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                    activeHighlighter->SetVisible(false);

                button->SetCheckedState(false);
                button->SetEnabled(false);
                button->SetVisible(true);
                if (const auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
                {
                    title->SetColor(sf::Color(200, 155, 55));
                    title->SetString(fmt::format("'Random' is selected (Total: {0:02}/{1:02})",
                        static_cast<int>(m_randomMusicCount),
                        static_cast<int>(m_musicList.size())
                    ));
                }

                if (const auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
                    level->SetString(std::string());

                if (const auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
                    duration->SetString(std::string());
            }
        }

        for (auto [button, genre] : m_genreButtonValues)
            button->SetCheckedState(false);

        for (auto [button, difficulty] : m_difficultyButtonValues)
            button->SetCheckedState(false);

        if (const auto thumbnail = FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_THUMBNAIL))
            thumbnail->SetVisible(false);
    }

    void SelectMusicDialog::InvalidateSelectors()
    {
        const auto musicSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        const auto elements      = musicSelector->GetChildren();

        const unsigned int itemListCount = musicSelector->GetChildrenCount();
        for (int i = static_cast<int>(elements.size()) - 1; i >= 0; i--)
        {
            const auto button = dynamic_cast<Gx::RadioButton*>(elements[i]);
            if (!button)
                continue;

            const int index = i + static_cast<int>(m_page * itemListCount);
            if (index >= m_filteredList.size())
            {
                button->SetCheckedState(false);
                button->SetEnabled(false);
                button->SetVisible(false);

                if (i == 0 && m_filteredList.empty())
                {
                    m_music = ChartMetadata{};
                    if (const auto infoList = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_INFO); infoList)
                    {
                        for (const auto child : infoList->GetChildren())
                        {
                            if (const auto label = dynamic_cast<Gx::Label*>(child); label)
                                label->SetString(std::string());
                        }
                    }

                    if (const auto pager = FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_PAGE); pager)
                        pager->SetVisible(false);

                    button->SetVisible(true);
                    if (const auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
                    {
                        if (m_genre.has_value())
                        {
                            title->SetString(fmt::format("{} is not available yet.", magic_enum::enum_name(m_genre.value())));
                            title->SetColor(sf::Color(135, 200, 60));
                        }
                        else
                        {
                            title->SetString("No installed music found.");
                            title->SetColor(sf::Color(135, 200, 60));
                        }
                    }

                    if (const auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
                        level->SetString(std::string());

                    if (const auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
                        duration->SetString(std::string());
                }

                continue;
            }

            if (i == 0 && m_music.Source.empty())
                m_music = m_filteredList[index];

            const auto& availableMusicIDs = m_room.GetAvailableMusicIDs();
            auto textColor = m_titleColor;
            if (const auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
            {
                if (availableMusicIDs.find(static_cast<std::uint32_t>(m_filteredList[index].ID)) == availableMusicIDs.end())
                    title->SetColor(sf::Color(241, 195, 10));
                else
                    title->SetColor(m_titleColor);

                textColor = title->GetColor();

                auto name = m_filteredList[index].Title;
                title->SetString(name);
                title->Truncate(150);

                if (title->GetString() != name)
                    title->SetString(fmt::format(L"{}..", title->GetString()));
            }

            if (const auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
            {
                level->SetColor(textColor);
                level->SetString(std::to_string(m_filteredList[index].Levels[m_difficulty]));
            }

            if (const auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
            {
                const float seconds = m_filteredList[index].Durations[m_difficulty].asSeconds();
                int minute    = std::floor(seconds / 60);
                int remainder = static_cast<int>(seconds) % 60;

                duration->SetColor(textColor);
                duration->SetString(fmt::format("[{}:{:02}]", minute, remainder));
            }

            button->SetCheckedState(m_music.Source == m_filteredList[index].Source);
            button->SetEnabled(true);
            button->SetVisible(true);

            if (const auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter && m_music.Source == m_filteredList[index].Source)
                activeHighlighter->SetVisible(true);
        }
    }

    void SelectMusicDialog::InvalidateMusicInfo()
    {
        auto currentMetadata = m_music;
        if (const auto infoList = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_INFO); infoList)
        {
            const std::vector info =
            {
                fmt::format(L"Title: {}", currentMetadata.Title),
                fmt::format(L"Artist: {}", currentMetadata.Artist),
                fmt::format(L"Note Designer: {}",  currentMetadata.NoteDesigner),
                fmt::format(L"Total Notes: {}", currentMetadata.NoteCounts[m_difficulty]),
                fmt::format(L"BPM: {:.2f}", m_music.BPM)
            };

            const auto children = infoList->GetChildren();
            for (std::size_t x = 0; x < children.size(); x++)
            {
                const auto label = dynamic_cast<Gx::Label*>(children[x]);
                if (!label)
                    continue;

                if (!m_music.Source.empty() && x < info.size())
                    label->SetString(info[x]);
                else
                    label->SetString(std::string());
            }
        }

        if (const auto thumbnail = FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_THUMBNAIL))
        {
            if (m_coverID == m_music.ID)
                return;

            m_coverID  = m_music.ID;
            if (const auto image = O2JamChartLoader::LoadThumbnail(m_music.Source, Gx::ResourceContext::Default))
            {
                m_thumbnail = std::make_unique<sf::Texture>();
                if (m_thumbnail->loadFromImage(*image))
                {
                    m_thumbnail->setSmooth(true);
                    thumbnail->SetVisible(true);
                    thumbnail->SetTexture(*m_thumbnail, true);
                }
            }
            else if (const auto cover = O2JamChartLoader::LoadCoverArt(m_music.Source, Gx::ResourceContext::Default))
            {
                m_thumbnail = std::make_unique<sf::Texture>();
                if (m_thumbnail->loadFromImage(*cover))
                {
                    m_thumbnail->setSmooth(true);
                    thumbnail->SetVisible(true);
                    thumbnail->SetTexture(*m_thumbnail, true);
                }
            }
            else
            {
                thumbnail->SetVisible(false);
            }
        }
    }

    void SelectMusicDialog::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        Dialog::OnKeyPressed(ev);

        if (ev.code == sf::Keyboard::Key::Up)
        {
            if (const auto list = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR); list)
            {
                const auto children = list->GetChildren();
                Gx::RadioButton* previous = nullptr;
                for (std::size_t i = 0; i < children.size(); i++)
                {
                    const auto button = dynamic_cast<Gx::RadioButton*>(children[i]);
                    if (!button)
                        continue;

                    if (button->IsChecked())
                    {
                        if (!previous && m_page != 0)
                        {
                            m_page--;
                            m_music = ChartMetadata{};
                            for (std::size_t j = children.size() - 1; j > 0; j--)
                            {
                                if (const auto next = dynamic_cast<Gx::RadioButton*>(children[j]); next)
                                {
                                    if (next->IsVisible() && next->IsEnabled())
                                        next->SetCheckedState(true);

                                    break;
                                }
                            }

                            InvalidateMusicList();
                            return;
                        }
                        else if (i > 0)
                        {
                            if (previous)
                                previous->SetCheckedState(true);

                            return;
                        }
                    }

                    previous = button;
                }
            }
        }
        else if (ev.code == sf::Keyboard::Key::Down)
        {
            if (const auto list = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR); list)
            {
                const auto children = list->GetChildren();
                for (std::size_t i = 0; i < children.size(); i++)
                {
                    const auto button = dynamic_cast<Gx::RadioButton*>(children[i]);
                    if (!button)
                        continue;

                    if (button->IsChecked())
                    {
                        if (i == children.size() - 1 && m_page < GetMaxPage() - 1)
                        {
                            SetPage(m_page + 1);
                            return;
                        }
                        else if (i < children.size())
                        {
                            for (std::size_t j = i + 1; j < children.size(); j++)
                            {
                                if (const auto next = dynamic_cast<Gx::RadioButton*>(children[j]); next)
                                {
                                    if (next->IsVisible() && next->IsEnabled())
                                        next->SetCheckedState(true);

                                    break;
                                }
                            }

                            return;
                        }
                    }
                }
            }
        }
        else if (ev.code == sf::Keyboard::Key::Left)
        {
            SetPage(m_page - 1);
        }
        else if (ev.code == sf::Keyboard::Key::Right)
        {
            SetPage(m_page + 1);
        }
    }

    void SelectMusicDialog::OnPresented(Parent& parent, const Gx::PresentationContext& context)
    {
        Dialog::OnPresented(parent, context);

        // TODO: Do not rescan, use fs to watch music folder
        const bool rescan = false; // !m_initialized;

        Initialize();

        m_musicList = m_session.GetInstalledMusic(rescan);
        FilterMusic();

        if (!m_room.GetMusic().Source.empty())
            m_music = m_room.GetMusic();
        else
            m_music = m_musicList[m_musicList.size() - 1];

        m_random = m_room.GetRandomLevel();
        m_difficulty = m_room.GetDifficulty();
        for (auto [button, difficulty] : m_difficultyButtonValues)
            button->SetCheckedState(difficulty == m_difficulty && !IsRandomActive());

        m_speed = m_room.GetSpeed();
        m_speedMode = m_room.GetSpeedMode();
        for (const auto button : m_speedButtons)
            button->SetCheckedState(button->GetMode() == m_speedMode && (m_speedMode != SpeedMode::HiSpeed || button->GetSpeed() == m_speed));

        for (auto [button, level] : m_randomLevelButtonValues)
            button->SetCheckedState((static_cast<int>(m_random) & static_cast<int>(level)) != 0);

        Sort(m_room.GetMusicSortMode(), m_room.GetMusicSortOrder());
    }

    void SelectMusicDialog::OnAccepted()
    {
        // Edge case: No music in selected genre
        if (m_music.Source.empty() && !IsRandomActive())
            return;

        // Edge case: No music match with random categories
        if (m_randomMusicCount == 0 && IsRandomActive())
            return;

        Dialog::OnAccepted();

        m_room.SetMusic(m_music);
        m_room.SetRandomLevel(m_random);
        m_room.SetMusicSortMode(m_sort.value_or(MusicSortMode::ID));
        m_room.SetMusicSortOrder(m_order.value_or(MusicSortOrder::None));
        m_room.SetDifficulty(m_difficulty);
        m_room.SetSpeed(m_speed);
        m_room.SetSpeedMode(m_speedMode);

        auto& sfx = m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_02);
        m_mixer.Play(sfx, Sound::Channel::SFX);
    }

    void SelectMusicDialog::OnCancelled()
    {
        Dialog::OnCancelled();

        auto& sfx = m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_03);
        m_mixer.Play(sfx, Sound::Channel::SFX);
    }

    void SelectMusicDialog::OnLeftButtonClicked(const Gx::Control& sender, const Gx::Control::Event& ev)
    {
        SetPage(m_page - 1);
    }

    void SelectMusicDialog::OnRightButtonClicked(const Gx::Control& sender, const Gx::Control::Event& ev)
    {
        SetPage(m_page + 1);
    }

    void SelectMusicDialog::OnMusicButtonFocusChanged(Gx::Control& sender, const Gx::Control::Event& ev)
    {
        if (const auto focusHighlighter = sender.template FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_HIGHLIGHT); focusHighlighter)
            focusHighlighter->SetVisible(sender.IsEnabled() && ev.State != State::Normal);
    }

    void SelectMusicDialog::OnMusicButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev)
    {
        const auto listSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        if (!sender.IsChecked())
        {
            if (const auto activeHighlighter = sender.template FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                activeHighlighter->SetVisible(false);

            return;
        }

        if (const auto activeHighlighter = sender.template FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
            activeHighlighter->SetVisible(true);

        const unsigned int itemListCount = listSelector->GetChildrenCount();
        const auto i = m_musicButtonIndices.at(&sender);
        const auto music = m_filteredList[i + static_cast<int>(m_page * itemListCount)];
        if (m_music.Source == music.Source)
            return;

        m_music = music;

        InvalidateMusicList();
    }

    void SelectMusicDialog::OnMusicSelectorScrolled(Gx::Control& sender, const Gx::Control::Event& ev)
    {
        const auto leftButton  = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_LEFT);
        const auto rightButton = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_RIGHT);

        if (leftButton && ev.Delta > 0)
            leftButton->PerformClick();
        else if (rightButton && ev.Delta < 0)
            rightButton->PerformClick();
    }

    void SelectMusicDialog::OnSortNewButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::ID);
    }

    void SelectMusicDialog::OnSortTitleButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::Title);
    }

    void SelectMusicDialog::OnSortLevelButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::Level);
    }

    void SelectMusicDialog::OnSortDurationButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::Duration);
    }

    void SelectMusicDialog::OnGenreButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev)
    {
        if (!sender.IsChecked())
            return;

        if (IsRandomActive())
        {
            sender.SetCheckedState(false);
            return;
        }

        m_genre = m_genreButtonValues.at(&sender);
        m_music = ChartMetadata{};
        m_page  = 0;
        FilterMusic();

        if (m_sort.has_value() && m_order.has_value())
            Sort(m_sort.value(), m_order.value());
        else
            InvalidateMusicList();
    }

    void SelectMusicDialog::OnRandomLevelButtonCheckChanged(Gx::ToggleButton& sender, const Gx::Control::Event& ev)
    {
        const auto lv = m_randomLevelButtonValues.at(&sender);
        if (sender.IsChecked())
            m_random = static_cast<LevelCategory>(static_cast<int>(m_random) | static_cast<int>(lv));
        else
            m_random = static_cast<LevelCategory>(static_cast<int>(m_random) & ~static_cast<int>(lv));

        for (auto [button, genre] : m_genreButtonValues)
        {
            if (genre != m_genre)
                continue;

            button->SetCheckedState(!IsRandomActive());
            break;
        }

        for (auto [button, difficulty] : m_difficultyButtonValues)
        {
            if (difficulty != m_difficulty)
                continue;

            button->SetCheckedState(!IsRandomActive());
            break;
        }

        InvalidateMusicList();
    }

    void SelectMusicDialog::OnExButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev)
    {
        SelectDifficulty(sender, Difficulty::EX);
    }

    void SelectMusicDialog::OnNxButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev)
    {
        SelectDifficulty(sender, Difficulty::NX);
    }

    void SelectMusicDialog::OnHxButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev)
    {
        SelectDifficulty(sender, Difficulty::HX);
    }

    void SelectMusicDialog::OnSpeedButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev)
    {
        if (!sender.IsChecked())
            return;

        if (const auto button = dynamic_cast<SpeedButton*>(&sender); button)
        {
            m_speed     = button->GetSpeed();
            m_speedMode = button->GetMode();
        }
    }
}
