#ifndef GENODE_RESOURCE_DESERIALIZER_HPP
#define GENODE_RESOURCE_DESERIALIZER_HPP

#include <Genode/System/Primitives.hpp>

namespace Gx
{
	namespace priv
	{
		class BaseDeserializer
		{
		public:
			virtual ~BaseDeserializer() {}
		};
	}

	template<class T>
	class ResourceDeserializer : public priv::BaseDeserializer
	{
	public:
		virtual ~ResourceDeserializer() {}
		virtual T Deserialize(Uint8* data, Uint64 size) const = 0;
	};
}

#endif