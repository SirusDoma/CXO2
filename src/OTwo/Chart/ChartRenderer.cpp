#include <OTwo/Chart/ChartRenderer.hpp>

ChartRenderer::ChartRenderer(State &state, const std::initializer_list<Chart::Channel> instantiables) :
    m_parent(&state),
    m_instantiables(instantiables),
    m_context(),
    m_templates()
{
}

void ChartRenderer::Initialize(const GameContext &context)
{
    m_context = &context;

    // Load note templates
    for (auto channel : m_instantiables)
    {
        const int key = static_cast<Gx::Uint16>(channel) - 1;
        if (key <= 0 || key >= 8)
            continue;

        m_templates[channel] = {};
        m_templates[channel][Chart::NoteType::Normal] = {
            m_parent->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_1"),
            m_parent->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_NORMAL" + std::to_string(key) + "_2"),
        };
        m_templates[channel][Chart::NoteType::Hold] = {
            m_parent->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_1"),
            m_parent->Find<Gx::Animation>("STATE_PLAYING/IDC_ANIMATION_NOTE_LONG" + std::to_string(key) + "_2"),
        };
    }

    // TODO:
    // 1. Get Chart and Events
    // 2. Get Note Templates from "Require"
    // 3. Spawn (Both Circle and Square) Notes from the Templates to Note Container

    // TODO:
    // Add "viewport" attributes in Playing.json asset to determine play area (e.g Size <width?>, 480px)
    // X and Y coordinate should infer from the Note min X and min Y position
}

void ChartRenderer::Update(double delta)
{
}

