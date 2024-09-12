#include <OTwo/UI/Dialogs/SelectMusicDialog.hpp>

#include <OTwo/Models/Game.hpp>

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/MusicSelectionContext.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <magic_enum.hpp>
#include <cmath>
#include <unordered_set>
#include <OTwo/States/State.hpp>

SelectMusicDialog::SelectMusicDialog(Gx::Mixer& mixer, Gx::ResourceManager& resources, SessionContext& session, MusicSelectionContext& selection) :
    m_coverID(0),
    m_speed(0),
    m_mixer(mixer),
    m_resources(resources),
    m_session(session),
    m_selection(selection),
    m_music()
{
}

void SelectMusicDialog::Initialize()
{
    Gx::Dialog::Initialize();

    if (m_initialized)
        return;

    m_page = 0;
    m_musicList = m_session.GetInstalledMusic();

    for (auto& metadata : m_musicList)
        m_displayList.push_back(metadata);

    m_random = m_selection.GetRandomLevel();
    m_difficulty = m_selection.GetDifficulty();
    m_speed = m_selection.GetSpeed();
    if (!m_displayList.empty())
    {
        if (m_selection.GetMetadata().ID != 0)
            m_music = m_selection.GetMetadata();
        else
            m_music = m_displayList[m_displayList.size() - 1];
    }

    auto leftButton = FindChild<Gx::Button>("IDC_BUTTON_LEFT");
    if (leftButton)
    {
        leftButton->SetClickCallback([this] (auto& sender, auto& ev)
        {
            if (m_random != static_cast<LevelCategory>(0) || m_page == 0)
                return;

            m_page--;
            m_music = ChartMetadata{};
            Invalidate();
        });
    }

    auto rightButton = FindChild<Gx::Button>("IDC_BUTTON_RIGHT");
    if (rightButton)
    {
        rightButton->SetClickCallback([this] (auto& sender, auto& ev)
        {
            const auto musicSelector = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR");
            if (!musicSelector)
                return;

            const unsigned int itemListCount = musicSelector->GetVerticalCount() * musicSelector->GetHorizontalCount();
            const unsigned int maxPage = std::ceil(static_cast<float>(m_displayList.size()) / static_cast<float>(itemListCount));
            if (m_random != static_cast<LevelCategory>(0) || m_page == maxPage - 1)
                return;

            m_page++;
            m_music = ChartMetadata{};
            Invalidate();
        });
    }

    if (auto list = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR"); list)
    {
        auto children = list->GetChildren();
        for (int i = 0; i < children.size(); i++)
        {
            auto button = dynamic_cast<Gx::RadioButton*>(children[i]);
            if (!button)
                continue;

            if (auto activeHighlighter = button->FindChild<Gx::Shape>("IDC_IMAGE_MUSIC_ACTIVE"); activeHighlighter)
                activeHighlighter->SetVisible(false);

            if (auto focusHighlighter = button->FindChild<Gx::Image>("IDC_IMAGE_MUSIC_HIGHLIGHT"); focusHighlighter)
                focusHighlighter->SetVisible(false);

            button->SetFocusChangedCallback([] (auto& sender, auto& ev)
            {
                if (auto focusHighlighter = sender.template FindChild<Gx::Image>("IDC_IMAGE_MUSIC_HIGHLIGHT"); focusHighlighter)
                    focusHighlighter->SetVisible(sender.IsEnabled() && ev.State != State::Normal);
            });

            button->SetCheckStateChangeCallback([this, i] (auto sender)
            {
                const auto list = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR");
                if (!sender->IsChecked())
                {
                    if (auto activeHighlighter = sender->template FindChild<Gx::Shape>("IDC_IMAGE_MUSIC_ACTIVE"); activeHighlighter)
                        activeHighlighter->SetVisible(false);

                    return;
                }

                if (auto activeHighlighter = sender->template FindChild<Gx::Shape>("IDC_IMAGE_MUSIC_ACTIVE"); activeHighlighter)
                    activeHighlighter->SetVisible(true);

                const unsigned int itemListCount = list->GetVerticalCount() * list->GetHorizontalCount();
                const auto music = m_displayList[i + static_cast<int>(m_page * itemListCount)];
                if (m_music.Source == music.Source)
                    return;

                m_music = music;

                Invalidate();
            });
        }

        list->SetScrollWheelCallback([=] (auto& sender, auto& ev)
        {
            if (leftButton && ev.Delta > 0)
                leftButton->PerformClick();
            else if (rightButton && ev.Delta < 0)
                rightButton->PerformClick();
        });
    }

    if (auto sortSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_SORT_SELECTOR"); sortSelector)
    {
        if (auto newButton = sortSelector->FindChild<Gx::Button>("IDC_BUTTON_SORT_NEW"); newButton)
        {
            newButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                Sort(MusicSortMode::ID, m_sort != MusicSortMode::ID || m_order != MusicSortOrder::Ascending ? MusicSortOrder::Ascending : MusicSortOrder::Descending);
            });
        }

        if (auto titleButton = sortSelector->FindChild<Gx::Button>("IDC_BUTTON_SORT_TITLE"); titleButton)
        {
            titleButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                Sort(MusicSortMode::Title, m_sort != MusicSortMode::Title || m_order != MusicSortOrder::Ascending ? MusicSortOrder::Ascending : MusicSortOrder::Descending);
            });
        }

        if (auto levelButton = sortSelector->FindChild<Gx::Button>("IDC_BUTTON_SORT_DIFF"); levelButton)
        {
            levelButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                Sort(MusicSortMode::Level, m_sort != MusicSortMode::Level || m_order != MusicSortOrder::Ascending ? MusicSortOrder::Ascending : MusicSortOrder::Descending);
            });
        }

        if (auto durationButton = sortSelector->FindChild<Gx::Button>("IDC_BUTTON_SORT_TIME"); durationButton)
        {
            durationButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                Sort(MusicSortMode::Duration, m_sort != MusicSortMode::Duration || m_order != MusicSortOrder::Ascending ? MusicSortOrder::Ascending : MusicSortOrder::Descending);
            });
        }
    }

    if (auto genreSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_GENRE_SELECTOR"); genreSelector)
    {
        if (auto allButton = genreSelector->FindChild<Gx::RadioButton>("IDC_RADIO_GENRE_ALL"); allButton)
            allButton->SetCheckedState(true);

        std::unordered_map<std::string, Genre> genreMap = {
            { "IDC_RADIO_GENRE_ALL", static_cast<Genre>(-1) },
            { "IDC_RADIO_GENRE_BALLAD", Genre::Ballad },
            { "IDC_RADIO_GENRE_ROCK", Genre::Rock },
            { "IDC_RADIO_GENRE_DANCE", Genre::Dance },
            { "IDC_RADIO_GENRE_TECHNO", Genre::Techno },
            { "IDC_RADIO_GENRE_HIPHOP", Genre::HipHop },
            { "IDC_RADIO_GENRE_SOUL", Genre::Soul },
            { "IDC_RADIO_GENRE_JAZZ", Genre::Jazz },
            { "IDC_RADIO_GENRE_FUNK", Genre::Funk },
            { "IDC_RADIO_GENRE_CLASSICAL", Genre::Classical },
            { "IDC_RADIO_GENRE_TRADITIONAL", Genre::Traditional },
            { "IDC_RADIO_GENRE_ETC", Genre::Etc },
        };

        for (auto [key, genre] : genreMap)
        {
            auto button = genreSelector->FindChild<Gx::RadioButton>(key);
            if (!button)
                continue;

            button->SetCheckStateChangeCallback([this, g = genre] (auto sender)
            {
                if (!sender->IsChecked())
                    return;

                if (m_random != static_cast<LevelCategory>(0))
                {
                    sender->SetCheckedState(false);
                    return;
                }

                m_genre = g;
                m_music = ChartMetadata{};
                m_page  = 0;
                m_displayList.clear();
                for (auto& metadata : m_musicList)
                {
                    if (m_genre == static_cast<Genre>(-1) || metadata.Genre == m_genre)
                        m_displayList.push_back(metadata);
                }

                Sort(m_sort, m_order);
            });
        }
    }

    if (auto randomSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_RANDOM_SELECTOR"); randomSelector)
    {
        std::unordered_map<std::string, LevelCategory> randomLevelMap = {
            { "IDC_CHECKBOX_RANDOM_DIFF_1", LevelCategory::Level1 },
            { "IDC_CHECKBOX_RANDOM_DIFF_2", LevelCategory::Level2 },
            { "IDC_CHECKBOX_RANDOM_DIFF_3", LevelCategory::Level3 },
            { "IDC_CHECKBOX_RANDOM_DIFF_4", LevelCategory::Level4 },
        };

        for (auto [id, level] : randomLevelMap)
        {
            auto button = randomSelector->FindChild<Gx::CheckBox>(id);
            if (!button)
                continue;

            button->SetCheckStateChangeCallback([this, lv = level] (auto sender)
            {
                if (sender->IsChecked())
                    m_random = static_cast<LevelCategory>(static_cast<int>(m_random) | static_cast<int>(lv));
                else
                    m_random = static_cast<LevelCategory>(static_cast<int>(m_random) & ~static_cast<int>(lv));

                if (const auto genreSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_GENRE_SELECTOR"); genreSelector)
                {
                    std::unordered_map<std::string, Genre> genreMap = {
                        {"IDC_RADIO_GENRE_ALL",         static_cast<Genre>(-1)},
                        {"IDC_RADIO_GENRE_BALLAD",      Genre::Ballad},
                        {"IDC_RADIO_GENRE_ROCK",        Genre::Rock},
                        {"IDC_RADIO_GENRE_DANCE",       Genre::Dance},
                        {"IDC_RADIO_GENRE_TECHNO",      Genre::Techno},
                        {"IDC_RADIO_GENRE_HIPHOP",      Genre::HipHop},
                        {"IDC_RADIO_GENRE_SOUL",        Genre::Soul},
                        {"IDC_RADIO_GENRE_JAZZ",        Genre::Jazz},
                        {"IDC_RADIO_GENRE_FUNK",        Genre::Funk},
                        {"IDC_RADIO_GENRE_CLASSICAL",   Genre::Classical},
                        {"IDC_RADIO_GENRE_TRADITIONAL", Genre::Traditional},
                        {"IDC_RADIO_GENRE_ETC",         Genre::Etc},
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

                if (const auto levelSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_DIFFICULTY_SELECTOR"); levelSelector)
                {
                    std::unordered_map<std::string, Difficulty> diffMap = {
                        {"IDC_RADIO_NOTE_EX", Difficulty::EX},
                        {"IDC_RADIO_NOTE_NX", Difficulty::NX},
                        {"IDC_RADIO_NOTE_HX", Difficulty::HX},
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
            });
        }
    }

    if (auto levelSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_DIFFICULTY_SELECTOR"); levelSelector)
    {
        if (auto exButton = levelSelector->FindChild<Gx::RadioButton>("IDC_RADIO_NOTE_EX"); exButton)
        {
            exButton->SetCheckedState(true);
            exButton->SetCheckStateChangeCallback([this] (auto sender)
            {
                if (!sender->IsChecked())
                    return;

                if (m_random != static_cast<LevelCategory>(0))
                {
                    sender->SetCheckedState(false);
                    return;
                }

                m_difficulty = Difficulty::EX;
                Sort(m_sort, m_order);
            });
        }

        if (auto nxButton = levelSelector->FindChild<Gx::RadioButton>("IDC_RADIO_NOTE_NX"); nxButton)
        {
            nxButton->SetCheckStateChangeCallback([this] (auto sender)
            {
                if (!sender->IsChecked())
                    return;

                if (m_random != static_cast<LevelCategory>(0))
                {
                    sender->SetCheckedState(false);
                    return;
                }

                m_difficulty = Difficulty::NX;
                Sort(m_sort, m_order);
            });
        }

        if (auto hxButton = levelSelector->FindChild<Gx::RadioButton>("IDC_RADIO_NOTE_HX"); hxButton)
        {
            hxButton->SetCheckStateChangeCallback([this] (auto sender)
            {
                if (!sender->IsChecked())
                    return;

                if (m_random != static_cast<LevelCategory>(0))
                {
                    sender->SetCheckedState(false);
                    return;
                }

                m_difficulty = Difficulty::HX;
                Sort(m_sort, m_order);
            });
        }
    }

    if (auto speedSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_SPEED_SELECTOR"); speedSelector)
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

            button->SetCheckStateChangeCallback([this, speed] (auto sender)
            {
                if (!sender->IsChecked())
                    return;

                m_speed = speed;
            });
        }
    }

    Sort(m_selection.GetSortMode(), m_selection.GetSortOrder());
    CacheMusicCover();
}

void SelectMusicDialog::OnKeyPressed(const sf::Event::KeyPressed& ev)
{
    Dialog::OnKeyPressed(ev);

    if (ev.code == sf::Keyboard::Key::Up)
    {
        if (const auto list = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR"); list)
        {
            const auto children = list->GetChildren();
            Gx::RadioButton* previous = nullptr;
            for (int i = 0; i < children.size(); i++)
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
                        for (int j = children.size() - 1; j > 0; j--)
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
        if (const auto list = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR"); list)
        {
            const auto children = list->GetChildren();
            const unsigned int maxPage = std::ceil(static_cast<float>(m_displayList.size()) / static_cast<float>(children.size()));
            for (int i = 0; i < children.size(); i++)
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
                        for (int j = i + 1; j < children.size(); j++)
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
        if (const auto list = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR"); list)
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

void SelectMusicDialog::OnShown(Gx::Scene& scene)
{
    Dialog::OnShown(scene);

    m_musicList = m_session.GetInstalledMusic(true);
    m_displayList.clear();
    for (auto& metadata : m_musicList)
    {
        if (m_genre == static_cast<Genre>(-1) || metadata.Genre == m_genre)
            m_displayList.push_back(metadata);
    }

    if (!m_selection.GetMetadata().Source.empty())
        m_music = m_selection.GetMetadata();
    else
        m_music = m_musicList[m_musicList.size() - 1];

    m_random = m_selection.GetRandomLevel();
    m_difficulty = m_selection.GetDifficulty();
    if (const auto levelSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_DIFFICULTY_SELECTOR"); levelSelector)
    {
        std::unordered_map<std::string, Difficulty> diffMap = {
            {"IDC_RADIO_NOTE_EX", Difficulty::EX},
            {"IDC_RADIO_NOTE_NX", Difficulty::NX},
            {"IDC_RADIO_NOTE_HX", Difficulty::HX},
        };

        for (auto [key, diff]: diffMap)
        {
            if (const auto button = levelSelector->FindChild<Gx::RadioButton>(key); button)
                button->SetCheckedState(diff == m_difficulty && m_random == static_cast<LevelCategory>(0));
        }
    }

    m_speed = m_selection.GetSpeed();
    if (const auto speedSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_SPEED_SELECTOR"); speedSelector)
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

    Sort(m_selection.GetSortMode(), m_selection.GetSortOrder());
}

void SelectMusicDialog::OnAccepted()
{
    // Edge case: No music in selected genre
    if (m_music.Source.empty() && m_random == static_cast<LevelCategory>(0))
        return;

    Dialog::OnAccepted();

    m_selection.SetMetadata(m_music);
    m_selection.SetRandomLevel(m_random);
    m_selection.SetSortMode(m_sort);
    m_selection.SetSortOrder(m_order);
    m_selection.SetDifficulty(m_difficulty);
    m_selection.SetSpeed(m_speed);
    CacheMusicCover();

    const auto sfx = &m_resources.AddFromFile<sf::Sound>("bgEffect/02");
    m_mixer.Play(sfx);
}

void SelectMusicDialog::OnCancelled()
{
    Dialog::OnCancelled();

    const auto sfx = &m_resources.AddFromFile<sf::Sound>("bgEffect/03");
    m_mixer.Play(sfx);
}

void SelectMusicDialog::CacheMusicCover(const bool refresh) const
{
    if (m_music.Source.empty())
        return;

    try
    {
        if (!refresh && m_resources.Find<sf::Image>("IDC_IMAGE_STATE_LOADING_COVER"))
            return;

        if (auto image = ChartLoader::LoadCoverArt(m_music, Gx::ResourceContext::Default); image)
            m_resources.Store<sf::Image>("IDC_IMAGE_STATE_LOADING_COVER", std::move(image), Gx::CacheMode::Update);
        else
            m_resources.Destroy<sf::Image>("IDC_IMAGE_STATE_LOADING_COVER");
    }
    catch (Gx::Exception)
    {
        m_resources.Destroy<sf::Image>("IDC_IMAGE_STATE_LOADING_COVER");
    }
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
    return m_genre;
}

float SelectMusicDialog::GetSelectedSpeed() const
{
    return m_speed;
}

void SelectMusicDialog::Sort(const MusicSortMode sort, const MusicSortOrder order)
{
    m_sort = sort;
    m_order = order;

    switch (sort)
    {
        case MusicSortMode::ID:
            std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto a, auto b)
            {
                if (m_order == MusicSortOrder::Ascending)
                    return a.ID < b.ID;

                return a.ID > b.ID;
            });
            break;
        case MusicSortMode::Title:
            std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto a, auto b)
            {
                if (m_order == MusicSortOrder::Ascending)
                    return std::string(a.Title) < std::string(b.Title);

                return std::string(a.Title) > std::string(b.Title);
            });
            break;
        case MusicSortMode::Level:
            std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto a, auto b)
            {
                auto x = a.ToChartMetadataView(m_difficulty);
                auto y = b.ToChartMetadataView(m_difficulty);

                if (m_order == MusicSortOrder::Ascending)
                    return x.Level < y.Level;

                return x.Level > y.Level;
            });
            break;
        case MusicSortMode::Duration:
            std::sort(m_displayList.begin(), m_displayList.end(), [this] (auto a, auto b)
            {
                auto x = a.ToChartMetadataView(m_difficulty);
                auto y = b.ToChartMetadataView(m_difficulty);

                if (m_order == MusicSortOrder::Ascending)
                    return x.Duration < y.Duration;

                return x.Duration > y.Duration;
            });
            break;
        default:
            break;
    }

    Invalidate();
}

void SelectMusicDialog::Invalidate()
{
    Dialog::Invalidate();

    auto musicSelector = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR");
    if (!musicSelector)
        return;

    auto elements = musicSelector->GetChildren();
    unsigned int itemListCount = musicSelector->GetVerticalCount() * musicSelector->GetHorizontalCount();
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
    auto pager = FindChild<Gx::Label>("IDC_TEXT_MUSIC_PAGE");
    if (pager)
    {
        pager->SetString("[" + Gx::StringHelper::ToString(static_cast<int>(m_page) + 1, 2) + "/" + Gx::StringHelper::ToString(static_cast<int>(maxPage), 2) + "]");
        pager->SetVisible(true);
    }

    if (m_random != static_cast<LevelCategory>(0))
    {
        m_music = ChartMetadata{};
        if (pager)
            pager->SetVisible(false);

        std::unordered_set<unsigned int> scanned;
        unsigned int used = 0;
        for (int r = 1; r < elements.size(); r++)
        {
            if (auto button = dynamic_cast<Gx::RadioButton*>(elements[r]); button)
            {
                auto lv = static_cast<LevelCategory>(1 << (r - 1));
                bool isRandomActivated = static_cast<int>(m_random) & static_cast<int>(lv);
                button->SetCheckedState(false);
                button->SetEnabled(false);

                if (auto title = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE"); title)
                {
                    button->SetVisible(isRandomActivated);
                    if (auto activeHighlighter = button->FindChild<Gx::Shape>("IDC_IMAGE_MUSIC_ACTIVE"); activeHighlighter)
                        activeHighlighter->SetVisible(false);

                    Gx::Label* infoLabel = nullptr;
                    if (auto infoList = FindChild<Gx::List>("IDC_LIST_MUSIC_INFO"); infoList && r - 1 < infoList->GetChildren().size())
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

                                used += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (const ChartMetadata& m)
                                {
                                    const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                    const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff) { return
                                                                            m.ToChartMetadataView(diff).Level <= 5; });

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

                                used += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (const ChartMetadata& m)
                                {
                                    const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                    const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff)
                                    {
                                        const int level = m.ToChartMetadataView(diff).Level;
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

                                used += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (const ChartMetadata& m)
                                {
                                    const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                    const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff)
                                    {
                                        const int level = m.ToChartMetadataView(diff).Level;
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

                                used += std::count_if(m_musicList.begin(), m_musicList.end(), [&scanned] (const ChartMetadata& m)
                                {
                                    const auto diffs = {Difficulty::EX, Difficulty::NX, Difficulty::HX};
                                    const bool result = std::any_of(diffs.begin(), diffs.end(), [&m] (auto diff) { return
                                                                            m.ToChartMetadataView(diff).Level > 13; });

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

                if (auto level = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_LEVEL"); level)
                    level->SetString(std::string());

                if (auto duration = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TIME"); duration)
                    duration->SetString(std::string());
            }
        }

        if (!elements.empty())
        {
            if (auto button = dynamic_cast<Gx::RadioButton*>(elements[0]); button)
            {
                if (auto activeHighlighter = button->FindChild<Gx::Shape>("IDC_IMAGE_MUSIC_ACTIVE"); activeHighlighter)
                    activeHighlighter->SetVisible(false);

                button->SetCheckedState(false);
                button->SetEnabled(false);
                button->SetVisible(true);
                if (auto title = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE"); title)
                {
                    title->SetColor(sf::Color(200, 155, 55));
                    title->SetString("'Random' is selected (Total: " +
                        Gx::StringHelper::ToString(static_cast<int>(used), 2) + "/" +
                        Gx::StringHelper::ToString(static_cast<int>(m_musicList.size()), 2) + ")"
                    );
                }

                if (auto level = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_LEVEL"); level)
                    level->SetString(std::string());

                if (auto duration = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TIME"); duration)
                    duration->SetString(std::string());
            }
        }

        if (auto genreSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_GENRE_SELECTOR"); genreSelector)
        {
            for (auto child : genreSelector->GetChildren())
            {
                if (auto button = dynamic_cast<Gx::RadioButton*>(child); button)
                    button->SetCheckedState(false);
            }
        }

        if (auto levelSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_DIFFICULTY_SELECTOR"); levelSelector)
        {
            for (auto child : levelSelector->GetChildren())
            {
                if (auto button = dynamic_cast<Gx::RadioButton*>(child); button)
                    button->SetCheckedState(false);
            }
        }

        return;
    }

    Gx::Label* lastTitle = nullptr;
    for (int i = static_cast<int>(elements.size()) - 1; i >= 0; i--)
    {
        auto button = dynamic_cast<Gx::RadioButton*>(elements[i]);
        if (!button)
            continue;

        int index = i + static_cast<int>(m_page * itemListCount);
        if (index >= m_displayList.size())
        {
            button->SetCheckedState(false);
            button->SetEnabled(false);
            button->SetVisible(false);

            if (auto title = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE"); title)
            {
                if (!lastTitle)
                    lastTitle = title;
            }

            if (i == 0 && m_displayList.empty())
            {
                m_music = ChartMetadata{};
                if (auto infoList = FindChild<Gx::List>("IDC_LIST_MUSIC_INFO"); infoList)
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
                if (auto title = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE"); title)
                {
                    if (!lastTitle)
                        lastTitle = title;

                    title->SetString(std::string(magic_enum::enum_name(m_genre)) + " is not available yet.");
                    title->SetColor(sf::Color(135, 200, 60));
                }

                if (auto level = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_LEVEL"); level)
                    level->SetString(std::string());

                if (auto duration = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TIME"); duration)
                    duration->SetString(std::string());
            }

            continue;
        }

        auto metadata = m_displayList[index].ToChartMetadataView(m_difficulty);
        if (i == 0 && m_music.Source.empty())
            m_music = m_displayList[index];

        if (auto title = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE"); title)
        {
            if (!lastTitle)
                lastTitle = title;
            else
                title->SetColor(lastTitle->GetColor());

            title->SetString(metadata.Title);
        }

        if (auto level = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_LEVEL"); level)
            level->SetString(std::to_string(metadata.Level));

        if (auto duration = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TIME"); duration)
        {
            float seconds = metadata.Duration.asSeconds();
            int minute    = std::floor(seconds / 60);
            int remainder = static_cast<int>(seconds) % 60;

            duration->SetString("[" + std::to_string(minute) + ":" + Gx::StringHelper::ToString(remainder, 2) + "]");
        }

        button->SetCheckedState(m_music.Source == m_displayList[index].Source);
        button->SetEnabled(true);
        button->SetVisible(true);
    }

    if (m_music.Source.empty())
        return;

    auto currentMetadata = m_music.ToChartMetadataView(m_difficulty);
    if (auto infoList = FindChild<Gx::List>("IDC_LIST_MUSIC_INFO"); infoList)
    {
        std::vector<std::string> info =
        {
            "Title: " + currentMetadata.Title,
            "Artist: " + currentMetadata.Artist,
            "Note Designer: " + currentMetadata.NoteDesigner,
            "Total Notes: " + std::to_string(currentMetadata.NoteCount),
            "BPM: " + Gx::StringHelper::ToString(m_music.BPM, 2)
        };

        auto children = infoList->GetChildren();
        for (int x = 0; x < children.size(); x++)
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

    if (auto thumbnail = FindChild<Gx::Image>("IDC_IMAGE_MUSIC_THUMBNAIL"); thumbnail && m_coverID != m_music.ID)
    {
        m_coverID  = m_music.ID;
        if (auto image = ChartLoader::LoadThumbnail(m_music, Gx::ResourceContext::Default))
        {
            m_thumbnail = std::make_unique<sf::Texture>();
            if (m_thumbnail->loadFromImage(*image))
            {
                thumbnail->SetVisible(true);
                thumbnail->SetTexture(*m_thumbnail);
            }
        }
        else
            thumbnail->SetVisible(false);
    }
}
