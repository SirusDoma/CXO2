#pragma once

template<typename T>
std::enable_if_t<std::is_base_of_v<State, T>, void>
SceneDirectorDecorator::Register(const std::string& fileName)
{
    m_director->Register(Gx::SceneDeserializer<T>([&, fileName] (const Gx::ResourceContext& context = Gx::ResourceContext::Default)
    {
        const auto stateLoader = Gx::ResourceLoaderFactory::CreateLoader<T>();
        if (!stateLoader)
            throw Gx::Exception("Failed to create state loader");

        auto state = stateLoader->LoadFromFile(fileName, context);
        if (state == nullptr)
            throw Gx::Exception("Failed to load state data");

        return state;
    }));
}

template<typename T>
std::enable_if_t<std::is_base_of_v<State, T>, void>
SceneDirectorDecorator::Present(const Gx::ResourceContext& context) const
{
    m_director->Present<T>(context);
}
