#include "systems/destroy_tagged.h"

#include "components/destroy_tag.h"
#include "registries.h"

namespace systems
{

void destroy_tagged()
{
    auto view = registries::audio.view<components::destroy_tag>();
    registries::audio.destroy(view.begin(), view.end());

    view = registries::core.view<components::destroy_tag>();
    registries::core.destroy(view.begin(), view.end());
}

}
