#include <OTwo/IO/ResourceContextDecorator.hpp>

const ResourceContextDecorator ResourceContextDecorator::Decorate(const Gx::ResourceContext &ctx)
{
    return ResourceContextDecorator(ctx);
}

ResourceContextDecorator::ResourceContextDecorator(const Gx::ResourceContext &ctx) :
    Gx::ResourceContext(ctx)
{
}
