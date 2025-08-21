#pragma once
#include <OTwo/Metadata/ResourceMetadata.hpp>

struct ControlList : public ResourceMetadata
{
    struct Bound
    {
        std::uint32_t X1;
        std::uint32_t Y1;
        std::uint32_t X2;
        std::uint32_t Y2;

        std::uint32_t GetWidth() const { return X2 - X1; }
        std::uint32_t GetHeight() const { return Y2 - Y1; }
    };

    class Control
    {
    public:
        enum class Type
        {
            State        = 0x00,
            Toggle       = 0x01,
            Image        = 0x10,
            ScrollBar    = 0x02,
            Button       = 0x20,
            Input        = 0x30,
            Interactable = 0x50,
            SetItem      = 0x60,
            Avatar       = 0x70
        };

        Control() = default;
        Control(const std::string& name, const std::uint32_t attribute, const std::string& param = std::string()) :
            m_name(name),
            m_attribute(attribute),
            m_param(param)
        {
        }

        const std::string& GetName() const { return m_name; }
        std::uint32_t GetAttribute() const { return m_attribute; }
        const std::string& GetParam() const { return m_param; }

        std::uint8_t GetStateID() const { return (m_attribute >> 24) & 0xFF; }
        Type GetType() const { return static_cast<Type>((m_attribute >> 16) & 0xFF); }
        std::uint8_t GetGroupID() const { return (m_attribute >> 8) & 0xFF; }
        std::uint8_t GetNodeID() const { return m_attribute & 0xFF; }

    private:
        std::string   m_name;
        std::uint32_t m_attribute;
        std::string   m_param;
    };

    class State
    {
    public:
        explicit State(const std::string& name) :
            m_name(name),
            m_base()
        {
        }

        const std::string& GetName() const
        {
            return m_name;
        }

        Control GetBase() const
        {
            return m_base;
        }

        void SetBase(const Control& identifier)
        {
            m_base = identifier;
        }

        void Add(const Control& child)
        {
            if (m_controls.find(child.GetAttribute()) != m_controls.end())
                throw Gx::ArgumentException("Child is already exists");

            m_controls[child.GetAttribute()] = child;
        }

        void AddBound(const std::uint32_t id, const Bound& bound)
        {
            m_bounds[id] = bound;
        }

        const std::unordered_map<std::uint32_t, Control>& GetChildren() const
        {
            return m_controls;
        }

        const std::unordered_map<std::uint32_t, Bound>& GetBounds() const
        {
            return m_bounds;
        }

        std::size_t GetChildCount() const
        {
            return m_controls.size();
        }

    private:
        std::string m_name;
        Control m_base;

        std::unordered_map<std::uint32_t, Bound> m_bounds;
        std::unordered_map<std::uint32_t, Control> m_controls;
    };

    std::vector<State> States;
};


