namespace Gx
{
//    template<typename R>
//    R *Mixer::Create(const std::string &source)
//    {
//        static_assert(std::is_base_of<sf::SoundSource, R>::value, "Parameter must be a sf::SoundSource");
//
//        if (!m_resources)
//            return nullptr;
//
////        auto resource = m_resources->AddFromFile<R>(source);
////        auto deleter  = resource.get_deleter();
////
////        m_sources.push_back(ResourcePtr<sf::SoundSource>{dynamic_cast<sf::SoundSource*>(resource.release()), [deleter] (auto sound) { deleter(dynamic_cast<R*>(sound)); }});
//        return &m_resources->AddFromFile<R>(source);
//    }
}
