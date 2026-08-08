#include <Extensions/TetrisExtension.hpp>
#include <Log.hpp>

#include <CXO2/States/State.hpp>

#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <algorithm>

namespace O2CX
{
    namespace
    {
        constexpr float CellSize = 24.f;
        constexpr float Padding  = 1.f;

        const std::array<sf::Color, 7> Palette =
        {
            sf::Color(64, 224, 208),
            sf::Color(240, 208, 64),
            sf::Color(176, 96, 224),
            sf::Color(96, 208, 96),
            sf::Color(224, 80, 80),
            sf::Color(80, 128, 224),
            sf::Color(240, 160, 64),
        };

        const sf::Color WellColor    = sf::Color(15, 15, 25);
        const sf::Color GameOverColor = sf::Color(90, 90, 100);

        void AddQuad(sf::VertexArray& vertices, const float x, const float y, const float w, const float h, const sf::Color color)
        {
            vertices.append(sf::Vertex{{x, y}, color});
            vertices.append(sf::Vertex{{x + w, y}, color});
            vertices.append(sf::Vertex{{x + w, y + h}, color});
            vertices.append(sf::Vertex{{x, y}, color});
            vertices.append(sf::Vertex{{x + w, y + h}, color});
            vertices.append(sf::Vertex{{x, y + h}, color});
        }
    }

    int TetrisExtension::SizeOf(const int type)
    {
        static constexpr std::array<int, 7> sizes = {4, 2, 3, 3, 3, 3, 3};
        return sizes[type];
    }

    std::array<TetrisExtension::Cell, 4> TetrisExtension::CellsOf(const int type, const int rotation)
    {
        static const std::array<std::array<Cell, 4>, 7> bases =
        {{
            {{ {0, 1}, {1, 1}, {2, 1}, {3, 1} }},
            {{ {0, 0}, {1, 0}, {0, 1}, {1, 1} }},
            {{ {1, 0}, {0, 1}, {1, 1}, {2, 1} }},
            {{ {1, 0}, {2, 0}, {0, 1}, {1, 1} }},
            {{ {0, 0}, {1, 0}, {1, 1}, {2, 1} }},
            {{ {0, 0}, {0, 1}, {1, 1}, {2, 1} }},
            {{ {2, 0}, {0, 1}, {1, 1}, {2, 1} }},
        }};

        const int size = SizeOf(type);
        auto cells     = bases[type];
        for (int r = 0; r < rotation; r++)
        {
            for (auto& cell : cells)
            {
                const int nx = size - 1 - cell.Y;
                cell.Y = cell.X;
                cell.X = nx;
            }
        }

        return cells;
    }

    bool TetrisExtension::Collides(const int type, const int rotation, const int px, const int py) const
    {
        for (const auto& cell : CellsOf(type, rotation))
        {
            const int x = px + cell.X;
            const int y = py + cell.Y;

            if (x < 0 || x >= Columns || y >= Rows)
                return true;

            if (y >= 0 && m_board[y][x] >= 0)
                return true;
        }

        return false;
    }

    void TetrisExtension::Reset()
    {
        for (auto& row : m_board)
            row.fill(-1);

        m_gameOver  = false;
        m_dropTimer = 0.f;
        m_next      = Gx::Randomizer::Randomize(0, 6);
        Spawn();
    }

    void TetrisExtension::Spawn()
    {
        m_type     = m_next;
        m_next     = Gx::Randomizer::Randomize(0, 6);
        m_rotation = 0;
        m_x        = (Columns - SizeOf(m_type)) / 2;
        m_y        = 0;

        if (Collides(m_type, m_rotation, m_x, m_y))
            m_gameOver = true;
    }

    void TetrisExtension::Lock()
    {
        for (const auto& cell : CellsOf(m_type, m_rotation))
        {
            const int x = m_x + cell.X;
            const int y = m_y + cell.Y;

            if (y >= 0 && y < Rows && x >= 0 && x < Columns)
                m_board[y][x] = m_type;
        }
    }

    void TetrisExtension::ClearLines()
    {
        int target = Rows - 1;
        for (int source = Rows - 1; source >= 0; source--)
        {
            const bool full = std::all_of(m_board[source].begin(), m_board[source].end(), [] (const int v) { return v >= 0; });
            if (full)
                continue;

            m_board[target] = m_board[source];
            target--;
        }

        for (int row = target; row >= 0; row--)
            m_board[row].fill(-1);
    }

    void TetrisExtension::Drop()
    {
        if (!Collides(m_type, m_rotation, m_x, m_y + 1))
        {
            m_y++;
            return;
        }

        Lock();
        ClearLines();
        Spawn();
    }

    bool TetrisExtension::Initialize()
    {
        Reset();
        Log("TetrisExtension: planet replaced with tetris");

        return false;
    }

    void TetrisExtension::Update(const sf::Time& delta)
    {
        if (m_gameOver)
            return;

        m_dropTimer += delta.asSeconds();
        if (m_dropTimer < m_dropInterval)
            return;

        m_dropTimer = 0.f;
        Drop();
    }

    bool TetrisExtension::Input(const sf::Event& ev)
    {
        const auto key = ev.getIf<sf::Event::KeyPressed>();
        if (!key)
            return true;

        using Key = sf::Keyboard::Key;

        if (m_gameOver)
        {
            if (key->code == Key::R)
                Reset();

            return true;
        }

        switch (key->code)
        {
            case Key::Left:
                if (!Collides(m_type, m_rotation, m_x - 1, m_y))
                    m_x--;
                break;

            case Key::Right:
                if (!Collides(m_type, m_rotation, m_x + 1, m_y))
                    m_x++;
                break;

            case Key::Down:
                if (!Collides(m_type, m_rotation, m_x, m_y + 1))
                {
                    m_y++;
                    m_dropTimer = 0.f;
                }
                break;

            case Key::Up:
            {
                const int rotation = (m_rotation + 1) % 4;
                if (!Collides(m_type, rotation, m_x, m_y))
                    m_rotation = rotation;
                break;
            }

            case Key::Space:
                while (!Collides(m_type, m_rotation, m_x, m_y + 1))
                    m_y++;
                Lock();
                ClearLines();
                Spawn();
                break;

            default:
                break;
        }

        return true;
    }

    Gx::RenderStates TetrisExtension::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        const auto center = GetState().GetView().getCenter();
        const float wellW = Columns * CellSize;
        const float wellH = Rows * CellSize;
        const float ox    = center.x - wellW / 2.f;
        const float oy    = center.y - wellH / 2.f;

        auto vertices = sf::VertexArray(sf::PrimitiveType::Triangles);
        AddQuad(vertices, ox, oy, wellW, wellH, WellColor);

        const auto block = [&] (const int col, const int row, const sf::Color color)
        {
            AddQuad(vertices, ox + col * CellSize + Padding, oy + row * CellSize + Padding, CellSize - Padding * 2.f, CellSize - Padding * 2.f, color);
        };

        for (int row = 0; row < Rows; row++)
        {
            for (int col = 0; col < Columns; col++)
            {
                if (m_board[row][col] >= 0)
                    block(col, row, m_gameOver ? GameOverColor : Palette[m_board[row][col]]);
            }
        }

        if (!m_gameOver)
        {
            for (const auto& cell : CellsOf(m_type, m_rotation))
            {
                const int col = m_x + cell.X;
                const int row = m_y + cell.Y;
                if (row >= 0)
                    block(col, row, Palette[m_type]);
            }
        }

        surface.Render(vertices, states);
        return states;
    }
}
