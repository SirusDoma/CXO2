template<typename T>
void SceneDirectorDecorator::Register(const std::string &fileName)
{
    static_assert(std::is_base_of_v<State, T>, "Parameter must be a State.");

    auto deserializer = std::function<std::unique_ptr<Gx::Scene>()>([&, fileName]
    {
        auto stateLoader = Gx::ResourceLoaderFactory::GetLoader<State>();
        if (!stateLoader)
            throw Gx::Exception("Failed to load state data");

        auto state = stateLoader->LoadFromFile(fileName, Gx::ResourceContext(typeid(T).name()));
        if (state == nullptr)
            throw Gx::Exception("Failed to load state data");

        return std::make_unique<T>(*state.get());
    });


    m_director->Register<T>(deserializer);
}

template<typename T>
void SceneDirectorDecorator::Register(State &state)
{
    static_assert(std::is_base_of_v<State, T>, "Parameter must be a State.");

   m_director->Register<T>(std::make_unique<T>(state));
}

template<typename T>
void SceneDirectorDecorator::Present()
{
    m_director->Present<T>();
}
