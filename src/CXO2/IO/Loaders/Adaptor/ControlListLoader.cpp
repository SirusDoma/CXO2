#include <deque>
#include <CXO2/IO/Loaders/Adaptor/ControlListLoader.hpp>
#include <CXO2/Metadata/Legacy/ControlList.hpp>
#include <CXO2/IO/Loaders/Adaptor/BoundLoader.hpp>
#include <CXO2/IO/Loaders/Adaptor/O2JamSpriteLoader.hpp>

#include <Genode/IO/FileSystem.hpp>

#include <SFML/System/MemoryInputStream.hpp>

#include <fmt/format.h>
#include <sstream>

namespace Cx
{
    namespace
    {
        std::optional<std::string> ReadLine(std::istringstream& source)
        {
            std::string line;
            if (!std::getline(source, line))
                return std::nullopt;

            return Gx::StringHelper::Trim(line).toAnsiString();
        }

        std::uint32_t ParseNextIntegerParam(std::istringstream& stream, const bool optional = false)
        {
            std::string param;
            stream >> param;
            if (param.empty())
            {
                if (!optional)
                    throw Gx::ResourceLoadException(std::string(), "Invalid ControlList: Invalid number parameters.");

                return 0;
            }

            return std::stoul(param, nullptr, 16);
        }

        ControlList::Control ParseControl(std::istringstream& stream, const std::string& token)
        {
            const std::uint32_t id = ParseNextIntegerParam(stream);

            auto fn = std::string();
            stream >> fn;

            return ControlList::Control(token, id, Gx::StringHelper::Unquote(fn).toAnsiString());
        }
    }

    ControlListLoader::ControlListLoader(const bool mapBounds) :
        m_mapBounds(mapBounds)
    {
    }

    Gx::ResourcePtr<ControlList> ControlListLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException(ctx.GetID(), fileName.string());

        auto& inputStream = *stream.get();
        return LoadFromStream(inputStream, ctx);
    }

    Gx::ResourcePtr<ControlList> ControlListLoader::LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto stream = sf::MemoryInputStream(data, size);
        return LoadFromStream(stream, ctx);
    }

    Gx::ResourcePtr<ControlList> ControlListLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
    {
        const auto size = stream.getSize().value_or(0) - stream.tell().value_or(0);
        auto source = std::string();
        source.resize(size);

        if (!stream.read(source.data(), source.size()).has_value())
            throw Gx::ResourceLoadException(ctx.GetID(), ctx.GetID());


        return Parse(std::istringstream(source.data()), m_mapBounds, ctx);
    }

    Gx::ResourcePtr<ControlList> ControlListLoader::Parse(std::istringstream source, bool mapBounds, const Gx::ResourceContext& ctx)
    {
        auto result = std::make_unique<ControlList>();
        while (true)
        {
            auto readLine = ReadLine(source);
            if (!readLine.has_value())
                break;

            auto line = readLine.value();
            if (Gx::StringHelper::StartsWith(line, "//"))
                continue;

            auto stream = std::istringstream(line.data());
            auto token = std::string();
            stream >> token;

            if (token.empty())
                continue;

            if (Gx::StringHelper::StartsWith(token, "NUMBER_OF_"))
            {
                if (token == "NUMBER_OF_STATE")
                {
                    std::uint32_t stateCount = ParseNextIntegerParam(stream);
                    if (stateCount == 0)
                        return {};
                }
                else if (token == "NUMBER_OF_DIALOG")
                {
                    std::uint32_t dialogCount = ParseNextIntegerParam(stream);
                    if (dialogCount == 0)
                        return result;
                }
                else
                    throw Gx::ResourceLoadException(ctx.GetID(), "Invalid count token: " + token);

                continue;
            }

            std::uint32_t count    = ParseNextIntegerParam(stream);
            std::uint32_t setCount = ParseNextIntegerParam(stream, true);

            if (count == 0)
                continue;

            result->States.push_back(ControlList::State(token));
            auto& state = result->States.back();

            bool started = false;
            int baseId = 0;

            auto bounds = std::deque<ControlList::Bound>();
            int boundCount = 0;

            while (true)
            {
                // Has extra 2 objects:
                // - BOUND
                // - IDC_STATE_NAME (base attribute number)

                readLine = ReadLine(source);
                if (!readLine.has_value())
                    throw Gx::ResourceLoadException(ctx.GetID(), "Invalid ControlList: Expected '}' token. Got: EOF");

                line = readLine.value();
                if (line.empty() || Gx::StringHelper::StartsWith(line, "//"))
                    continue;

                if (!started)
                {
                    if (!Gx::StringHelper::StartsWith(line, "{"))
                        throw Gx::ResourceLoadException(ctx.GetID(), "Invalid ControlList: Expected '{' token. Got: " + token);

                    started = true;
                    continue;
                }

                if (Gx::StringHelper::StartsWith(line, "}"))
                    break;

                stream = std::istringstream(line.data());
                stream >> token;

                if (token == "BOUND")
                {
                    if (!bounds.empty())
                        throw Gx::ResourceLoadException(ctx.GetID(), "Invalid ControlList: Duplicate 'BOUND's");

                    std::string fileName;
                    stream >> fileName;

                    fileName = Gx::StringHelper::Unquote(fileName).toAnsiString();
                    if (!Gx::FileSystem::Contains(fileName))
                        continue;

                    auto boundLoader = BoundLoader();
                    auto bnd = boundLoader.LoadFromFile(fileName, ctx);
                    if (bnd == nullptr)
                        throw Gx::ResourceLoadException(ctx.GetID(), "Invalid ControlList: Invalid number of 'BOUND' element");

                    for (const auto& b : *bnd)
                        bounds.push_back(b);

                    boundCount = bounds.size();
                }
                else if (token == "SET")
                {
                    std::uint32_t setItemCount = ParseNextIntegerParam(stream);
                    std::uint32_t setId        = ParseNextIntegerParam(stream);

                    bool setStarted = false;
                    while (true)
                    {
                        readLine = ReadLine(source);
                        if (!readLine.has_value())
                            throw Gx::ResourceLoadException(ctx.GetID(), "Invalid ControlList: Expected '}' token. Got: EOF");

                        line = readLine.value();
                        if (Gx::StringHelper::StartsWith(line, "//"))
                            continue;

                        if (!setStarted)
                        {
                            if (!Gx::StringHelper::StartsWith(line, "{"))
                                throw Gx::ResourceLoadException(ctx.GetID(), "Invalid ControlList: Expected '{' token. Got: " + token);

                            setStarted = true;
                            continue;
                        }

                        if (line.empty())
                            continue;

                        if (Gx::StringHelper::StartsWith(line, "}"))
                            break;

                        stream = std::istringstream(line.data());
                        stream >> token;

                        if (token == "SET")
                            throw Gx::ResourceLoadException(ctx.GetID(), "Invalid ControlList: Nested 'SET' is not supported");

                        auto control = ParseControl(stream, token);
                        if (!bounds.empty())
                        {
                            state.AddBound(control.GetAttribute(), bounds.front());
                            bounds.pop_front();
                        }
                        else
                        {
                            state.AddBound(control.GetAttribute(), ControlList::Bound{0, 0, 0, 0});
                        }

                        setItemCount++;
                        if (!control.GetParam().empty())
                            state.Add(std::move(control));
                    }
                }
                else
                {
                    if (baseId == 0)
                    {
                        baseId = ParseNextIntegerParam(stream);
                        state.SetBase(ControlList::Control(token, baseId));

                        if (!mapBounds)
                        {
                            int idc = baseId;
                            while (!bounds.empty())
                            {
                                state.AddBound(++idc, bounds.front());
                                bounds.pop_front();
                            }
                        }

                        continue;
                    }

                    auto control = ParseControl(stream, token);
                    if (!bounds.empty())
                    {
                        state.AddBound(control.GetAttribute(), bounds.front());
                        bounds.pop_front();
                    }
                    else
                    {
                        state.AddBound(control.GetAttribute(), ControlList::Bound{0, 0, 0, 0});
                    }

                    if (!control.GetParam().empty())
                        state.Add(std::move(control));
                }
            }

            if (!bounds.empty())
            {
                int idc = baseId + (boundCount - bounds.size());
                while (!bounds.empty())
                {
                    state.AddBound(++idc, bounds.front());
                    bounds.pop_front();
                }
            }
        }

        return result;
    }
}
