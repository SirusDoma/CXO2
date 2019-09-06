#ifndef GENODE_DESERIALIZER_FACTORY_HPP
#define GENODE_DESERIALIZER_FACTORY_HPP

#include <Genode/IO/ResourceDeserializer.hpp>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <algorithm>

namespace Gx
{
	class DeserializerFactory
	{
	public:
		template<class T>
		static void Register(ResourceDeserializer<T>* deserializer);

		template<class T>
		static bool Remove();

		template<class T>
		static ResourceDeserializer<T>* GetDeserializer();

	private:
		typedef std::unordered_map<std::type_index, priv::BaseDeserializer*> DeserializerMap;
		static DeserializerMap m_deserializers;
	};
}

#include <Genode/IO/DeserializerFactory.inl>
#endif
