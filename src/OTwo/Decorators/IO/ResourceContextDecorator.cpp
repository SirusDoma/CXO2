#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>

ResourceContextDecorator ResourceContextDecorator::Decorate(const Gx::ResourceContext& ctx)
{
    return ResourceContextDecorator(ctx);
}

ResourceContextDecorator::ResourceContextDecorator(const Gx::ResourceContext& ctx) :
    Gx::ResourceContext(ctx)
{
}
