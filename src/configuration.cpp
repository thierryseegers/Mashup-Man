#include "configuration.h"

#include "tomlpp.h"
#include "utility.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace configuration
{

using values_t = toml::table;

template<typename T>
struct value_proxy<T>::impl_t
{
    toml::node_view<const toml::node> const table;
    std::optional<T> option;
};

template<typename T>
value_proxy<T>::value_proxy(impl_t impl)
    : impl{std::make_unique<impl_t>(impl)}
{}

template<typename T>
value_proxy<T>::~value_proxy() = default;

template<typename T>
value_proxy<T>::operator T() const
{
    return impl->option ? impl->table.template value_or<T>(std::forward<T>(*impl->option)) :
                          *impl->table.template value<T>();
}

template<typename T>
value_proxy<T> value_proxy<T>::operator[](std::string_view key) const
{
    return {value_proxy<T>::impl_t{.table = impl->table[key]}};
}

template
class value_proxy<int>;

template
class value_proxy<float>;

template<typename T>
value_proxy<T> value()
{
    auto const& values = utility::single::instance<configuration::values_t>();
    return {typename value_proxy<T>::impl_t{
                .table = static_cast<toml::node_view<const toml::node>>(values)}};
}

template<typename T>
value_proxy<T> value_or(T const& option_)
{
    auto const& values = utility::single::instance<configuration::values_t>();
    return {typename value_proxy<T>::impl_t{
                .table = static_cast<toml::node_view<const toml::node>>(values),
                .option = option_}};
}

template
value_proxy<int> value<int>();

template
value_proxy<float> value<float>();

template
value_proxy<int> value_or<int>(int const&);

template
value_proxy<float> value_or<float>(float const&);

void initialize(
    std::filesystem::path const& config_file)
{
    static bool initialized = false;

    if(!initialized)
    {
        utility::single::mutable_instance<values_t>() = toml::parse_file(config_file.native());
        initialized = true;
    }
    else
    {
        throw std::logic_error("re-initialization of configuration");
    }
}

struct info_t info;

std::ostream& operator<<(
    std::ostream& o, info_t const&)
{
    return o << utility::single::instance<configuration::values_t>();
}

}