#include <OTwo/States/Components/Dialogs/SelectMusicDialog.hpp>

#include <Genode/UI.hpp>

#include <cmath>
#include <Genode/Utilities/StringHelper.hpp>

SelectMusicDialog::SelectMusicDialog(const Gx::Dialog &copy) :
    Gx::Dialog(copy),
    Gx::UiContainer(copy),
    Gx::Node(copy),
    m_initialized(false),
    m_page(0),
    m_musicList(),
    m_displayList()
{
}

void SelectMusicDialog::Initialize()
{
    if (m_initialized)
        return;

    Gx::Dialog::Initialize();

    // Rewire callbacks due to copy constructor
    if (auto acceptButton = GetAcceptButton(); acceptButton)
        SetAcceptButton(*acceptButton);

    if (auto cancelButton = GetCancelButton(); cancelButton)
        SetCancelButton(*cancelButton);

    m_page = 0;
    m_musicList.push_back(ChartMetadata
    {
        .ID           = 495,
        .Title        = "Earth Quake",
        .Artist       = "KAZE.o2SE",
        .NoteDesigner = "KAZE.o2SE",
        .BPM          = 150.0f,
        .LevelEx      = 2,
        .LevelNx      = 15,
        .LevelHx      = 36,
        .NoteCountEx  = 431,
        .NoteCountNx  = 1343,
        .NoteCountHx  = 2821,
        .Duration     = sf::seconds(195)
    });

    m_musicList.push_back(ChartMetadata
    {
        .ID           = 394,
        .Title        = "Red Sign",
        .Artist       = "Kevin.o2SE",
        .NoteDesigner = "Kevin.o2SE",
        .BPM          = 180.0f,
        .LevelEx      = 2,
        .LevelNx      = 15,
        .LevelHx      = 34,
        .NoteCountEx  = 284,
        .NoteCountNx  = 898,
        .NoteCountHx  = 2266,
        .Duration     = sf::seconds(185)
    });

    m_musicList.push_back(ChartMetadata
    {
        .ID           = 521,
        .Title        = "Monster Express",
        .Artist       = "BeautifulDay",
        .NoteDesigner = "NoteFactory",
        .BPM          = 140.0f,
        .LevelEx      = 4,
        .LevelNx      = 14,
        .LevelHx      = 19,
        .NoteCountEx  = 321,
        .NoteCountNx  = 727,
        .NoteCountHx  = 969,
        .Duration     = sf::seconds(108)
    });

    for (auto& metadata : m_musicList)
        m_displayList.push_back(&metadata);

    if (!m_displayList.empty())
        m_music = m_displayList[0];

    if (auto list = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR"); list)
    {
        auto children = list->GetChildren();
        for (int i = 0; i < children.size(); i++)
        {
            auto button = dynamic_cast<Gx::RadioButton*>(children[i]);
            if (!button)
                continue;

            button->SetCheckStateChangeCallback([this, i] (auto sender)
            {
                if (!sender->IsChecked())
                    return;

                m_music = m_displayList[i];
                if (auto infoList = FindChild<Gx::List>("IDC_LIST_MUSIC_INFO"); infoList)
                {
                    std::vector<std::string> info =
                    {
                        "Title: " + m_music->Title,
                        "Artist: " + m_music->Artist,
                        "Note Designer: " + m_music->NoteDesigner,
                        "Total Notes: " + std::to_string(m_music->NoteCountHx),
                        "BPM: " + Gx::StringHelper::ToString(m_music->BPM, 2)
                    };

                    auto children = infoList->GetChildren();
                    for (int x = 0; x < children.size(); x++)
                    {
                        auto label = dynamic_cast<Gx::Label*>(children[x]);
                        if (!label)
                            continue;

                        if (x < info.size())
                            label->SetString(info[x]);
                        else
                            label->SetString(std::string());

                    }
                }
            });
        }
    }

    if (auto genreSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_GENRE_SELECTOR"); genreSelector)
    {
        if (auto allButton = genreSelector->FindChild<Gx::RadioButton>("IDC_RADIO_GENRE_ALL"); allButton)
            allButton->SetCheckedState(true);
    }

    if (auto levelSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_DIFFICULTY_SELECTOR"); levelSelector)
    {
        if (auto exButton = levelSelector->FindChild<Gx::RadioButton>("IDC_RADIO_NOTE_EX"); exButton)
            exButton->SetCheckedState(true);
    }

    if (auto speedSelector = FindChild<Gx::UiContainer>("IDC_CONTAINER_SPEED_SELECTOR"); speedSelector)
    {
        if (auto speedButton = speedSelector->FindChild<Gx::RadioButton>("IDC_RADIO_SPEED_10"); speedButton)
            speedButton->SetCheckedState(true);
    }


    Invalidate();
}

void SelectMusicDialog::Invalidate()
{
    Dialog::Invalidate();

    auto selectList = FindChild<Gx::List>("IDC_LIST_MUSIC_SELECTOR");
    auto selectListChildren = selectList->GetChildren();

    unsigned int itemListCount = selectList->GetVerticalCount() + selectList->GetHorizontalCount();
    int maxPage = ceil(static_cast<float>(m_displayList.size()) / static_cast<float>(itemListCount));

    if (auto page = FindChild<Gx::Label>("IDC_TEXT_MUSIC_PAGE"); page)
        page->SetString("[" + Gx::StringHelper::ToString(static_cast<int>(m_page) + 1, 2) + "/" + Gx::StringHelper::ToString(maxPage, 2) + "]");

    for (int i = static_cast<int>(m_page * itemListCount); i < selectListChildren.size(); i++)
    {
        auto button = dynamic_cast<Gx::RadioButton*>(selectListChildren[i]);
        if (!button)
            continue;

        if (i >= m_displayList.size())
        {
            button->SetEnabled(false);
            button->SetVisible(false);

            continue;
        }

        auto metadata = m_displayList[i];
        if (auto title = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TITLE"); title)
            title->SetString(metadata->Title);

        if (auto level = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_LEVEL"); level)
            level->SetString(std::to_string(metadata->LevelHx));

        if (auto duration = button->FindChild<Gx::Label>("IDC_TEXT_MUSIC_TIME"); duration)
        {
            float seconds = metadata->Duration.asSeconds();
            int minute    = floor(seconds / 60);
            int remainder = static_cast<int>(seconds) % 60;

            duration->SetString("[" + std::to_string(minute) + ":" + Gx::StringHelper::ToString(remainder, 2) + "]");
        }

        button->SetCheckedState(m_music && m_music->ID == metadata->ID);
        button->SetEnabled(true);
        button->SetVisible(true);
    }
}

