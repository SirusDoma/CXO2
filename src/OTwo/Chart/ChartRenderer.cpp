#include <OTwo/Chart/ChartRenderer.hpp>

ChartRenderer::ChartRenderer(Gx::Node &node, const std::initializer_list<Chart::ChannelType> instantiables) :
    m_container(&node),
    m_instantiables(instantiables),
    m_templates()
{
}

void ChartRenderer::Initialize(const GameContext &context)
{
    m_context = &context;

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

