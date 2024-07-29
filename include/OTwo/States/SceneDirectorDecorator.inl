template<typename T>
void SceneDirectorDecorator::Register(const std::string &fileName)
{
    static_assert(std::is_base_of_v<State, T>, "Parameter must be a State.");

    const auto deserializer = Gx::SceneDirector::SceneFactory([&, fileName] (const Gx::ResourceContext &context)
    {
        auto stateLoader   = Gx::ResourceLoaderFactory::GetLoader<T>();
        auto genericLoader = Gx::ResourceLoaderFactory::GetLoader<State>();

        if (!stateLoader && !genericLoader)
            throw Gx::Exception("Failed to find state loader");

        if (stateLoader)
        {
            auto state = stateLoader->LoadFromFile(fileName, context);
            if (state == nullptr)
                throw Gx::Exception("Failed to load state data");

            // TODO: Find a way to move the pointer and preserve the deleter?
            return std::unique_ptr<T>(state.release(), std::default_delete<T>());
        }
        else
        {
            const auto state = genericLoader->LoadFromFile(fileName, context);
            if (state == nullptr)
                throw Gx::Exception("Failed to load state data");

            return std::make_unique<T>(std::move(*state.get()));
        }
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
void SceneDirectorDecorator::Present(const Gx::ResourceContext &context) const
{
    m_director->Present<T>(context);
}
