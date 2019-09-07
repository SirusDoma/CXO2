#ifndef GENODE_DESERIALIZER_FACTORY_HPP
#define GENODE_DESERIALIZER_FACTORY_HPP

#include <Genode/IO/ResourceLoader.hpp>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <algorithm>

namespace Gx
{
	class LoaderFactory
	{
	public:
		template<class T>
		static void Register(ResourceLoader<T>* deserializer);

		template<class T>
		static bool Remove();

		template<class T>
		static ResourceLoader<T>* GetLoader();

	private:
		typedef std::unordered_map<std::type_index, priv::BaseLoader*> DeserializerMap;
		static DeserializerMap m_deserializers;
	};
}

#include <Genode/IO/LoaderFactory.inl>
#endif
