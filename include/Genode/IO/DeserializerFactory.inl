#include <Genode/IO/Deserializers/TextureDeserializer.hpp>
#include <Genode/IO/Deserializers/FontDeserializer.hpp>

namespace
{
	void EnsureDefaultDeserializersRegistered()
	{
		static bool registered = false;
		if (!registered)
		{
			Gx::DeserializerFactory::Register<sf::Texture>(new Gx::priv::TextureDeserializer());
			Gx::DeserializerFactory::Register<sf::Font>(new Gx::priv::FontDeserializer());

			registered = true;
		}
	}
}

namespace Gx
{
	template<class T>
	inline static void DeserializerFactory::Register(ResourceDeserializer<T>* deserializer)
	{
		Remove<T>();

		std::type_index type = typeid(T);
		m_deserializers[type] = deserializer;
	}
	
	template<class T>
	inline bool DeserializerFactory::Remove()
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
	inline ResourceDeserializer<T>* DeserializerFactory::GetDeserializer()
	{
		EnsureDefaultDeserializersRegistered();

		std::type_index type = typeid(T);
		auto iterator = m_deserializers.find(type);
		if (iterator != m_deserializers.end())
			return dynamic_cast<ResourceDeserializer<T>*>(iterator->second);

		return nullptr;
	}
}