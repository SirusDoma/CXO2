template<typename T>
void SceneDirectorDecorator::Register(const std::string &fileName)
{
    static_assert(std::is_base_of_v<State, T>, "Parameter must be a State");

    const Gx::SceneDirector::SceneFactory deserializer = [&, fileName] (const Gx::ResourceContext &context)
    {
        const auto stateLoader = Gx::ResourceLoaderFactory::CreateLoader<T>();
        if (!stateLoader)
            throw Gx::Exception("Failed to create state loader");

        auto state = stateLoader->LoadFromFile(fileName, context);
        if (state == nullptr)
            throw Gx::Exception("Failed to load state data");

        return Gx::ResourcePtr<Gx::Scene>(state.release(), [deleter = state.get_deleter()] (auto ptr) {
            deleter(dynamic_cast<T*>(ptr));
        });
    };

    m_director->Register<T>(deserializer);
}

template<typename T>
void SceneDirectorDecorator::Register(State &state)
{
    static_assert(std::is_base_of_v<State, T>, "Parameter must be a State");

   m_director->Register<T>(std::make_unique<T>(state));
}

template<typename T>
void SceneDirectorDecorator::Present(const Gx::ResourceContext &context) const
{
    m_director->Present<T>(context);
}
