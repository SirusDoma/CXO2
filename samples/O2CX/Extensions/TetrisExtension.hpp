#pragma once

#include <CXO2/States/StateExtension.hpp>

#include <array>

namespace O2CX
{
    class TetrisExtension : public Cx::StateExtension
    {
    protected:
        bool Initialize() override;
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        bool Input(const sf::Event& ev) override;

    private:
        static constexpr int Columns = 10;
        static constexpr int Rows    = 20;

        struct Cell { int X, Y; };

        static int SizeOf(int type);
        static std::array<Cell, 4> CellsOf(int type, int rotation);

        bool Collides(int type, int rotation, int px, int py) const;
        void Reset();
        void Spawn();
        void Lock();
        void ClearLines();
        void Drop();

        std::array<std::array<int, Columns>, Rows> m_board{};
        int   m_type{0};
        int   m_rotation{0};
        int   m_x{0};
        int   m_y{0};
        int   m_next{0};
        float m_dropTimer{0.f};
        float m_dropInterval{0.5f};
        bool  m_gameOver{false};
    };
}
