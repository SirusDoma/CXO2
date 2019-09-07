#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/Loaders/FontLoader.hpp>

namespace
{
	void EnsureDefaultDeserializersRegistered()
	{
		static bool registered = false;
		if (!registered)
		{
			Gx::LoaderFactory::Register<sf::Texture>(new Gx::priv::TextureLoader());
			Gx::LoaderFactory::Register<sf::Font>(new Gx::priv::FontLoader());

			registered = true;
		}
	}
}

namespace Gx
{
	template<class T>
	inline static void LoaderFactory::Register(ResourceLoader<T>* deserializer)
	{
		Remove<T>();

		std::type_index type = typeid(T);
		m_deserializers[type] = deserializer;
	}
	
	template<class T>
	inline bool LoaderFactory::Remove()
	{
		std::type_index type = typeid(T);
		auto iterator = m_deserializers.find(type);
		if (iterator != m_deserializers.end())
		{
			delete iterator->second;
			m_deserializers.erase(iterator);

			return true;
		}

		return false;
	}
	
	template<class T>
	inline ResourceLoader<T>* LoaderFactory::GetLoader()
	{
		EnsureDefaultDeserializersRegistered();

		std::type_index type = typeid(T);
		auto iterator = m_deserializers.find(type);
		if (iterator != m_deserializers.end())
			return dynamic_cast<ResourceLoader<T>*>(iterator->second);

		return nullptr;
	}
}