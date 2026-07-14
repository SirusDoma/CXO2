#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>

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
            Invalidate();
            return;
        }

        m_page = 0;
        m_musicList = m_session.GetInstalledMusic();

        for (auto& metadata : m_musicList)
            m_displayList.push_back(metadata);

        m_random     = m_room.GetRandomLevel();
        m_difficulty = m_room.GetDifficulty();
        m_speed      = m_room.GetSpeed();

        if (!m_displayList.empty())
        {
            if (m_room.GetMusic().ID != 0)
                m_music = m_room.GetMusic();
            else
                m_music = m_displayList[m_displayList.size() - 1];
        }

        auto leftButton = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_LEFT);
        if (leftButton)
        {
            leftButton->SetClickCallback([this] (auto& sender, auto& ev) { OnLeftButtonClicked(sender, ev); });
        }

        auto rightButton = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_RIGHT);
        if (rightButton)
        {
            rightButton->SetClickCallback([this] (auto& sender, auto& ev) { OnRightButtonClicked(sender, ev); });
        }

        if (auto list = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR); list)
        {
            auto children = list->GetChildren();
            for (std::size_t i = 0; i < children.size(); i++)
            {
                auto button = dynamic_cast<Gx::RadioButton*>(children[i]);
                if (!button)
                    continue;

                if (auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                    activeHighlighter->SetVisible(false);

                if (auto focusHighlighter = button->FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_HIGHLIGHT); focusHighlighter)
                    focusHighlighter->SetVisible(false);

                button->SetFocusChangedCallback([this] (auto& sender, auto& ev) { OnMusicButtonFocusChanged(sender, ev); });

                m_musicButtonIndices[button] = i;
                button->SetCheckStateChangeCallback([this] (auto& sender) { OnMusicButtonCheckChanged(sender); });
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
                button->SetCheckStateChangeCallback([this] (auto& sender) { OnGenreButtonCheckChanged(sender); });
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
                button->SetCheckStateChangeCallback([this] (auto& sender) { OnRandomLevelButtonCheckChanged(sender); });
            }
        }

        if (auto levelSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_DIFFICULTY_SELECTOR); levelSelector)
        {
            if (auto exButton = levelSelector->FindChild<Gx::RadioButton>(Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_EX); exButton)
            {
                exButton->SetCheckedState(true);
                exButton->SetCheckStateChangeCallback([this] (auto& sender) { OnExButtonCheckChanged(sender); });
            }

            if (auto nxButton = levelSelector->FindChild<Gx::RadioButton>(Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_NX); nxButton)
            {
                nxButton->SetCheckStateChangeCallback([this] (auto& sender) { OnNxButtonCheckChanged(sender); });
            }

            if (auto hxButton = levelSelector->FindChild<Gx::RadioButton>(Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_HX); hxButton)
            {
                hxButton->SetCheckStateChangeCallback([this] (auto& sender) { OnHxButtonCheckChanged(sender); });
            }
        }

        // TODO: Create a special `SpeedButton` to declare the speed properties
        if (auto speedSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_SPEED_SELECTOR); speedSelector)
        {
            for (auto child : speedSelector->GetChildren())
            {
                auto button = dynamic_cast<Gx::RadioButton*>(child);
                if (!button)
                    continue;

                auto name = button->GetName();
                if (auto index = name.find_last_of('/'); index != -1)
                    name = name.substr(index + 1);

                auto prefix = std::string("IDC_RADIO_SPEED_");
                if (name.compare(0, prefix.size(), prefix) != 0)
                    continue;

                auto speedName = name.substr(prefix.size());
                float speed = 0.f;

                if (speedName == "XR")
                    speed = XrSpeed;
                else if (speedName == "3D")
                    speed = TdSpeed;
                else if (speedName.size() == 2)
                    speed = std::stof(std::string(1, speedName[0]) + "." + std::string(1, speedName[1]));

                if (speed != XrSpeed && speed != TdSpeed)
                {
                    bool supported = false;
                    for (float s : SupportedHiSpeeds)
                    {
                        if (speed == s)
                        {
                            supported = true;
                            break;
                        }
                    }

                    if (!supported)
                        continue;
                }

                if (speed == m_speed)
                    button->SetCheckedState(true);

                m_speedButtonValues[button] = speed;
                button->SetCheckStateChangeCallback([this] (auto& sender) { OnSpeedButtonCheckChanged(sender); });
            }
        }

        Sort(m_room.GetMusicSortMode(), m_room.GetMusicSortOrder());
        m_initialized = true;
    }

    void SelectMusicDialog::OnLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (m_random != static_cast<LevelCategory>(0) || m_page == 0)
            return;

        m_page--;
        m_music = ChartMetadata{};
        Invalidate();
    }

    void SelectMusicDialog::OnRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto musicSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        if (!musicSelector)
            return;

        const unsigned int itemListCount = musicSelector->GetChildrenCount();
        const unsigned int maxPage = std::ceil(static_cast<float>(m_displayList.size()) / static_cast<float>(itemListCount));
        if (m_random != static_cast<LevelCategory>(0) || m_page == maxPage - 1)
            return;

        m_page++;
        m_music = ChartMetadata{};
        Invalidate();
    }

    void SelectMusicDialog::OnMusicButtonFocusChanged(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (auto focusHighlighter = sender.template FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_HIGHLIGHT); focusHighlighter)
            focusHighlighter->SetVisible(sender.IsEnabled() && ev.State != State::Normal);
    }

    void SelectMusicDialog::OnMusicButtonCheckChanged(Gx::RadioButton& sender)
    {
        const auto listSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        if (!sender.IsChecked())
        {
            if (auto activeHighlighter = sender.template FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                activeHighlighter->SetVisible(false);

            return;
        }

        if (auto activeHighlighter = sender.template FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
            activeHighlighter->SetVisible(true);

        const unsigned int itemListCount = listSelector->GetChildrenCount();
        const auto i = m_musicButtonIndices.at(&sender);
        const auto music = m_displayList[i + static_cast<int>(m_page * itemListCount)];
        if (m_music.Source == music.Source)
            return;

        m_music = music;

        Invalidate();
    }

    void SelectMusicDialog::OnMusicSelectorScrolled(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto leftButton  = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_LEFT);
        const auto rightButton = FindChild<Gx::Button>(Resource::SelectMusic::IDC_BUTTON_RIGHT);

        if (leftButton && ev.Delta > 0)
            leftButton->PerformClick();
        else if (rightButton && ev.Delta < 0)
            rightButton->PerformClick();
    }

    void SelectMusicDialog::ToggleSort(const MusicSortMode mode)
    {
        Sort(mode, m_sort != mode || m_order != MusicSortOrder::Ascending ? MusicSortOrder::Ascending : MusicSortOrder::Descending);
    }

    void SelectMusicDialog::OnSortNewButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::ID);
    }

    void SelectMusicDialog::OnSortTitleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::Title);
    }

    void SelectMusicDialog::OnSortLevelButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::Level);
    }

    void SelectMusicDialog::OnSortDurationButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        ToggleSort(MusicSortMode::Duration);
    }

    void SelectMusicDialog::OnGenreButtonCheckChanged(Gx::RadioButton& sender)
    {
        if (!sender.IsChecked())
            return;

        if (m_random != static_cast<LevelCategory>(0))
        {
            sender.SetCheckedState(false);
            return;
        }

        m_genre = m_genreButtonValues.at(&sender);
        m_music = ChartMetadata{};
        m_page  = 0;
        m_displayList.clear();
        for (auto& metadata : m_musicList)
        {
            auto musicGenre = magic_enum::enum_cast<Genre>(metadata.Genre.toAnsiString())
                .value_or(Genre::Etc);

            if (!m_genre.has_value() || musicGenre == m_genre)
                m_displayList.push_back(metadata);
        }

        if (m_sort.has_value() && m_order.has_value())
            Sort(m_sort.value(), m_order.value());
        else
            Invalidate();
    }

    void SelectMusicDialog::OnRandomLevelButtonCheckChanged(Gx::ToggleButton& sender)
    {
        const auto lv = m_randomLevelButtonValues.at(&sender);
        if (sender.IsChecked())
            m_random = static_cast<LevelCategory>(static_cast<int>(m_random) | static_cast<int>(lv));
        else
            m_random = static_cast<LevelCategory>(static_cast<int>(m_random) & ~static_cast<int>(lv));

        if (const auto genreSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_GENRE_SELECTOR); genreSelector)
        {
            std::unordered_map<std::string, Genre> genreMap = {
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_ALL,         static_cast<Genre>(-1)},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_BALLAD,      Genre::Ballad},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_ROCK,        Genre::Rock},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_DANCE,       Genre::Dance},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_TECHNO,      Genre::Techno},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_HIPHOP,      Genre::HipHop},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_SOUL,        Genre::Soul},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_JAZZ,        Genre::Jazz},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_FUNK,        Genre::Funk},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_CLASSICAL,   Genre::Classical},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_TRADITIONAL, Genre::Traditional},
                {Resource::SelectMusic::Genre::IDC_RADIO_GENRE_ETC,         Genre::Etc},
            };

            for (auto [key, genre]: genreMap)
            {
                if (genre != m_genre)
                    continue;

                if (const auto genreButton = genreSelector->FindChild<Gx::RadioButton>(key); genreButton)
                {
                    genreButton->SetCheckedState(m_random == static_cast<LevelCategory>(0));
                    break;
                }
            }
        }

        if (const auto levelSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_DIFFICULTY_SELECTOR); levelSelector)
        {
            std::unordered_map<std::string, Difficulty> diffMap = {
                {Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_EX, Difficulty::EX},
                {Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_NX, Difficulty::NX},
                {Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_HX, Difficulty::HX},
            };

            for (auto [key, diff]: diffMap)
            {
                if (diff != m_difficulty)
                    continue;

                if (const auto diffButton = levelSelector->FindChild<Gx::RadioButton>(key); diffButton)
                {
                    diffButton->SetCheckedState(m_random == static_cast<LevelCategory>(0));
                    break;
                }
            }
        }

        Invalidate();
    }

    void SelectMusicDialog::SelectDifficulty(Gx::RadioButton& sender, const Difficulty difficulty)
    {
        if (!sender.IsChecked())
            return;

        if (m_random != static_cast<LevelCategory>(0))
        {
            sender.SetCheckedState(false);
            return;
        }

        m_difficulty = difficulty;
        if (m_sort.has_value() && m_order.has_value())
            Sort(m_sort.value(), m_order.value());
        else
            Invalidate();
    }

    void SelectMusicDialog::OnExButtonCheckChanged(Gx::RadioButton& sender)
    {
        SelectDifficulty(sender, Difficulty::EX);
    }

    void SelectMusicDialog::OnNxButtonCheckChanged(Gx::RadioButton& sender)
    {
        SelectDifficulty(sender, Difficulty::NX);
    }

    void SelectMusicDialog::OnHxButtonCheckChanged(Gx::RadioButton& sender)
    {
        SelectDifficulty(sender, Difficulty::HX);
    }

    void SelectMusicDialog::OnSpeedButtonCheckChanged(Gx::RadioButton& sender)
    {
        if (!sender.IsChecked())
            return;

        m_speed = m_speedButtonValues.at(&sender);
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

                            Invalidate();
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
                const unsigned int maxPage = std::ceil(static_cast<float>(m_displayList.size()) / static_cast<float>(children.size()));
                for (std::size_t i = 0; i < children.size(); i++)
                {
                    const auto button = dynamic_cast<Gx::RadioButton*>(children[i]);
                    if (!button)
                        continue;

                    if (button->IsChecked())
                    {
                        if (i == children.size() - 1 && m_page < maxPage - 1)
                        {
                            m_page++;
                            m_music = ChartMetadata{};
                            Invalidate();
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
            if (m_page == 0)
                return;

            m_page--;
            m_music = ChartMetadata{};
            Invalidate();
        }
        else if (ev.code == sf::Keyboard::Key::Right)
        {
            if (const auto list = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR); list)
            {
                const auto children = list->GetChildren();
                const unsigned int maxPage = std::ceil(static_cast<float>(m_displayList.size()) / static_cast<float>(children.size()));

                if (m_page == maxPage - 1)
                    return;
            }


            m_page++;
            m_music = ChartMetadata{};
            Invalidate();
        }
    }

    void SelectMusicDialog::OnPresented(Parent& parent, const Gx::PresentationContext& context)
    {
        Dialog::OnPresented(parent, context);

        // TODO: Do not rescan, use fs to watch music folder
        bool rescan = false; // !m_initialized;

        Initialize();

        m_musicList = m_session.GetInstalledMusic(rescan);
        m_displayList.clear();
        for (auto& metadata : m_musicList)
        {
            auto genre = magic_enum::enum_cast<Genre>(metadata.Genre.toAnsiString());
            if (!genre.has_value())
                genre = Genre::Etc;

            if (!m_genre.has_value() || genre == m_genre)
                m_displayList.push_back(metadata);
        }

        if (!m_room.GetMusic().Source.empty())
            m_music = m_room.GetMusic();
        else
            m_music = m_musicList[m_musicList.size() - 1];

        m_random = m_room.GetRandomLevel();
        m_difficulty = m_room.GetDifficulty();
        if (const auto levelSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_DIFFICULTY_SELECTOR); levelSelector)
        {
            std::unordered_map<std::string, Difficulty> diffMap = {
                {Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_EX, Difficulty::EX},
                {Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_NX, Difficulty::NX},
                {Resource::SelectMusic::Difficulty::IDC_RADIO_NOTE_HX, Difficulty::HX},
            };

            for (auto [key, diff]: diffMap)
            {
                if (const auto button = levelSelector->FindChild<Gx::RadioButton>(key); button)
                    button->SetCheckedState(diff == m_difficulty && m_random == static_cast<LevelCategory>(0));
            }
        }

        m_speed = m_room.GetSpeed();
        if (const auto speedSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_SPEED_SELECTOR); speedSelector)
        {
            for (const auto child : speedSelector->GetChildren())
            {
                const auto button = dynamic_cast<Gx::RadioButton*>(child);
                if (!button)
                    continue;

                auto name = button->GetName();
                if (const auto index = name.find_last_of('/'); index != -1)
                    name = name.substr(index + 1);

                auto prefix = std::string("IDC_RADIO_SPEED_");
                if (name.compare(0, prefix.size(), prefix) != 0)
                    continue;

                auto speedName = name.substr(prefix.size());
                float speed = 0.f;

                if (speedName == "XR")
                    speed = XrSpeed;
                else if (speedName == "3D")
                    speed = TdSpeed;
                else if (speedName.size() == 2)
                    speed = std::stof(std::string(1, speedName[0]) + "." + std::string(1, speedName[1]));

                if (speed != XrSpeed && speed != TdSpeed)
                {
                    bool supported = false;
                    for (const float s : SupportedHiSpeeds)
                    {
                        if (speed == s)
                        {
                            supported = true;
                            break;
                        }
                    }

                    if (!supported)
                        continue;
                }

                button->SetCheckedState(speed == m_speed);
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

                if (static_cast<int>(m_random) & static_cast<int>(level))
                    button->SetCheckedState(true);
                else
                    button->SetCheckedState(false);
            }
        }


        Sort(m_room.GetMusicSortMode(), m_room.GetMusicSortOrder());
    }

    void SelectMusicDialog::OnAccepted()
    {
        // Edge case: No music in selected genre
        if (m_music.Source.empty() && m_random == static_cast<LevelCategory>(0))
            return;

        // Edge case: No music match with random categories
        if (m_randomMusicCount == 0 && m_random != static_cast<LevelCategory>(0))
            return;

        Dialog::OnAccepted();

        m_room.SetMusic(m_music);
        m_room.SetRandomLevel(m_random);
        m_room.SetMusicSortMode(m_sort.value_or(MusicSortMode::ID));
        m_room.SetMusicSortOrder(m_order.value_or(MusicSortOrder::None));
        m_room.SetDifficulty(m_difficulty);
        m_room.SetSpeed(m_speed);

        auto& sfx = m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_02);
        m_mixer.Play(sfx, Sound::Channel::SFX);
    }

    void SelectMusicDialog::OnCancelled()
    {
        Dialog::OnCancelled();

        auto& sfx = m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_03);
        m_mixer.Play(sfx, Sound::Channel::SFX);
    }

    ChartMetadata SelectMusicDialog::GetSelectedMusic() const
    {
        return m_music;
    }

    LevelCategory SelectMusicDialog::GetSelectedRandomLevels() const
    {
        return m_random;
    }

    Difficulty SelectMusicDialog::GetSelectedDifficulty() const
    {
        return m_difficulty;
    }

    Genre SelectMusicDialog::GetSelectedGenre() const
    {
        return m_genre.value_or(Genre::Etc);
    }

    float SelectMusicDialog::GetSelectedSpeed() const
    {
        return m_speed;
    }

    void SelectMusicDialog::Sort(const MusicSortMode sort, const MusicSortOrder order)
    {
        m_sort  = sort;
        m_order = order;

        switch (sort)
        {
            case MusicSortMode::ID:
                std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto& a, auto& b)
                {
                    if (m_order == MusicSortOrder::Ascending)
                        return a.ID < b.ID;

                    return a.ID > b.ID;
                });
                break;
            case MusicSortMode::Title:
                std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto& a, auto& b)
                {
                    if (m_order == MusicSortOrder::Ascending)
                        return std::string(a.Title) < std::string(b.Title);

                    return std::string(a.Title) > std::string(b.Title);
                });
                break;
            case MusicSortMode::Level:
                std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto& a, auto& b)
                {
                    auto x = a.Levels[m_difficulty];
                    auto y = b.Levels[m_difficulty];

                    if (m_order == MusicSortOrder::Ascending)
                        return x < y;

                    return x > y;
                });
                break;
            case MusicSortMode::Duration:
                std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto& a, auto& b)
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

        Invalidate();
    }

    void SelectMusicDialog::Invalidate()
    {
        // TODO: Prevent unnecessary invalidate
        Dialog::Invalidate();

        auto musicSelector = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_SELECTOR);
        if (!musicSelector)
            return;

        auto elements = musicSelector->GetChildren();
        unsigned int itemListCount = musicSelector->GetChildrenCount();
        unsigned int maxPage = std::ceil(static_cast<float>(m_displayList.size()) / static_cast<float>(itemListCount));

        if (!m_music.Source.empty())
        {
            unsigned int i = 0;
            for (auto m : m_displayList)
            {
                if (m.Source == m_music.Source)
                    break;

                i++;
            }

            m_page = std::floor(static_cast<float>(i) / static_cast<float>(itemListCount));
        }

        m_page = static_cast<unsigned int>(std::min(maxPage - 1, m_page));
        auto pager = FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_PAGE);
        if (pager)
        {
            pager->SetString(fmt::format("[{0:02}/{1:02}]", static_cast<int>(m_page) + 1, static_cast<int>(maxPage)));
            pager->SetVisible(true);
        }

        if (m_random != static_cast<LevelCategory>(0))
        {
            m_music = ChartMetadata{};
            if (pager)
                pager->SetVisible(false);

            std::unordered_set<unsigned int> scanned;
            m_randomMusicCount = 0;
            for (std::size_t r = 1; r < elements.size(); r++)
            {
                if (auto button = dynamic_cast<Gx::RadioButton*>(elements[r]); button)
                {
                    auto lv = static_cast<LevelCategory>(1 << (r - 1));
                    bool isRandomActivated = static_cast<int>(m_random) & static_cast<int>(lv);

                    button->SetCheckedState(false);
                    button->SetEnabled(false);

                    if (auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
                    {
                        button->SetVisible(isRandomActivated);
                        if (auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                            activeHighlighter->SetVisible(false);

                        Gx::Label* infoLabel = nullptr;
                        if (auto infoList = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_INFO); infoList && r - 1 < infoList->GetChildrenCount())
                            infoLabel = dynamic_cast<Gx::Label*>(infoList->GetChildren()[r - 1]);

                        if (isRandomActivated)
                        {
                            title->SetColor(sf::Color(200, 155, 55));
                            switch(lv)
                            {
                                case LevelCategory::Level1:
                                    title->SetString("LEVEL 1 - 5");
                                    if (infoLabel)
                                        infoLabel->SetString("LEVEL 1 - 5");

                                    m_randomMusicCount += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (ChartMetadata& m)
                                    {
                                        const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                        const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff)
                                        {
                                            return m.Levels[diff] <= 5;
                                        });

                                        if (result)
                                        {
                                            if (auto [_, inserted] = scanned.insert(m.ID); !inserted)
                                                return false;
                                        }

                                        return result;
                                    });

                                    break;
                                case LevelCategory::Level2:
                                    title->SetString("LEVEL 5 - 9");
                                    if (infoLabel)
                                        infoLabel->SetString("LEVEL 5 - 9");

                                    m_randomMusicCount += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (ChartMetadata& m)
                                    {
                                        const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                        const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff)
                                        {
                                            const int level = m.Levels[diff];
                                            return level > 5 && level <= 9;
                                        });

                                        if (result)
                                        {
                                            if (auto [_, inserted] = scanned.insert(m.ID); !inserted)
                                                return false;
                                        }

                                        return result;
                                    });

                                    break;
                                case LevelCategory::Level3:
                                    title->SetString("LEVEL 9 - 13");
                                    if (infoLabel)
                                        infoLabel->SetString("LEVEL 9 - 13");

                                    m_randomMusicCount += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (ChartMetadata& m)
                                    {
                                        const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                        const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff)
                                        {
                                            const int level = m.Levels[diff];
                                            return level > 9 && level <= 13;
                                        });

                                        if (result)
                                        {
                                            if (auto [_, inserted] = scanned.insert(m.ID); !inserted)
                                                return false;
                                        }

                                        return result;
                                    });

                                    break;
                                case LevelCategory::Level4:
                                    title->SetString("LEVEL 13 higher");
                                    if (infoLabel)
                                        infoLabel->SetString("LEVEL 13 higher");

                                    m_randomMusicCount += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (ChartMetadata& m)
                                    {
                                        const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                        const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff)
                                        {
                                            return m.Levels[diff] >= 13;
                                        });

                                        if (result)
                                        {
                                            if (auto [_, inserted] = scanned.insert(m.ID); !inserted)
                                                return false;
                                        }

                                        return result;
                                    });

                                    break;
                            }
                        }
                        else
                        {
                            title->SetString(std::string());
                            if (infoLabel)
                                infoLabel->SetString(std::string());
                        }
                    }

                    if (auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
                        level->SetString(std::string());

                    if (auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
                        duration->SetString(std::string());
                }
            }

            if (!elements.empty())
            {
                if (auto button = dynamic_cast<Gx::RadioButton*>(elements[0]); button)
                {
                    if (auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter)
                        activeHighlighter->SetVisible(false);

                    button->SetCheckedState(false);
                    button->SetEnabled(false);
                    button->SetVisible(true);
                    if (auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
                    {
                        title->SetColor(sf::Color(200, 155, 55));
                        title->SetString(fmt::format("'Random' is selected (Total: {0:02}/{1:02})",
                            static_cast<int>(m_randomMusicCount),
                            static_cast<int>(m_musicList.size())
                        ));
                    }

                    if (auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
                        level->SetString(std::string());

                    if (auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
                        duration->SetString(std::string());
                }
            }

            if (auto genreSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_GENRE_SELECTOR); genreSelector)
            {
                for (auto child : genreSelector->GetChildren())
                {
                    if (auto button = dynamic_cast<Gx::RadioButton*>(child); button)
                        button->SetCheckedState(false);
                }
            }

            if (auto levelSelector = FindChild<Gx::UiContainer>(Resource::SelectMusic::IDC_CONTAINER_DIFFICULTY_SELECTOR); levelSelector)
            {
                for (auto child : levelSelector->GetChildren())
                {
                    if (auto button = dynamic_cast<Gx::RadioButton*>(child); button)
                        button->SetCheckedState(false);
                }
            }

            if (auto thumbnail = FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_THUMBNAIL))
                thumbnail->SetVisible(false);

            return;
        }

        for (int i = static_cast<int>(elements.size()) - 1; i >= 0; i--)
        {
            auto button = dynamic_cast<Gx::RadioButton*>(elements[i]);
            if (!button)
                continue;

            const auto refTitle = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE);
            static auto originalTextColor = refTitle != nullptr ? refTitle->GetColor() : sf::Color(25, 25, 25);

            int index = i + static_cast<int>(m_page * itemListCount);
            if (index >= m_displayList.size())
            {
                button->SetCheckedState(false);
                button->SetEnabled(false);
                button->SetVisible(false);

                if (i == 0 && m_displayList.empty())
                {
                    m_music = ChartMetadata{};
                    if (auto infoList = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_INFO); infoList)
                    {
                        for (auto child : infoList->GetChildren())
                        {
                            if (auto label = dynamic_cast<Gx::Label*>(child); label)
                                label->SetString(std::string());
                        }
                    }

                    if (pager)
                        pager->SetVisible(false);

                    button->SetVisible(true);
                    if (auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
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

                    if (auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
                        level->SetString(std::string());

                    if (auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
                        duration->SetString(std::string());
                }

                continue;
            }

            if (i == 0 && m_music.Source.empty())
                m_music = m_displayList[index];

            const auto& availableMusicIDs = m_room.GetAvailableMusicIDs();
            auto textColor = originalTextColor;
            if (auto title = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TITLE); title)
            {
                static sf::Color originalColor = title->GetColor();

                if (availableMusicIDs.find(static_cast<std::uint32_t>(m_displayList[index].ID)) == availableMusicIDs.end())
                    title->SetColor(sf::Color(241, 195, 10));
                else
                    title->SetColor(originalTextColor);

                textColor = title->GetColor();

                auto name = m_displayList[index].Title;
                title->SetString(name);
                title->Truncate(150);

                if (title->GetString() != name)
                    title->SetString(fmt::format(L"{}..", title->GetString()));
            }

            if (auto level = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_LEVEL); level)
            {
                level->SetColor(textColor);
                level->SetString(std::to_string(m_displayList[index].Levels[m_difficulty]));
            }

            if (auto duration = button->FindChild<Gx::Label>(Resource::SelectMusic::IDC_TEXT_MUSIC_TIME); duration)
            {
                float seconds = m_displayList[index].Durations[m_difficulty].asSeconds();
                int minute    = std::floor(seconds / 60);
                int remainder = static_cast<int>(seconds) % 60;

                duration->SetColor(textColor);
                duration->SetString(fmt::format("[{}:{:02}]", minute, remainder));
            }

            button->SetCheckedState(m_music.Source == m_displayList[index].Source);
            button->SetEnabled(true);
            button->SetVisible(true);

            if (auto activeHighlighter = button->FindChild<Gx::Shape>(Resource::SelectMusic::IDC_IMAGE_MUSIC_ACTIVE); activeHighlighter && m_music.Source == m_displayList[index].Source)
                activeHighlighter->SetVisible(true);
        }

        if (m_music.Source.empty())
            return;

        auto currentMetadata = m_music;
        if (auto infoList = FindChild<Gx::List>(Resource::SelectMusic::IDC_LIST_MUSIC_INFO); infoList)
        {
            std::vector info =
            {
                fmt::format(L"Title: {}", currentMetadata.Title),
                fmt::format(L"Artist: {}", currentMetadata.Artist),
                fmt::format(L"Note Designer: {}",  currentMetadata.NoteDesigner),
                fmt::format(L"Total Notes: {}", currentMetadata.NoteCounts[m_difficulty]),
                fmt::format(L"BPM: {:.2f}", m_music.BPM)
            };

            auto children = infoList->GetChildren();
            for (std::size_t x = 0; x < children.size(); x++)
            {
                auto label = dynamic_cast<Gx::Label*>(children[x]);
                if (!label)
                    continue;

                if (!m_music.Source.empty() && x < info.size())
                    label->SetString(info[x]);
                else
                    label->SetString(std::string());
            }
        }

        if (auto thumbnail = FindChild<Gx::Image>(Resource::SelectMusic::IDC_IMAGE_MUSIC_THUMBNAIL))
        {
            if (m_coverID != m_music.ID)
            {
                m_coverID  = m_music.ID;
                if (auto image = O2JamChartLoader::LoadThumbnail(m_music.Source, Gx::ResourceContext::Default))
                {
                    m_thumbnail = std::make_unique<sf::Texture>();
                    if (m_thumbnail->loadFromImage(*image))
                    {
                        m_thumbnail->setSmooth(true);
                        thumbnail->SetVisible(true);
                        thumbnail->SetTexture(*m_thumbnail, true);
                    }
                }
                else if (auto cover = O2JamChartLoader::LoadCoverArt(m_music.Source, Gx::ResourceContext::Default))
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
    }
}
