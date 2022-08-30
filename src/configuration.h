#pragma once

#include <filesystem>
#include <iosfwd>
#include <memory>
#include <string_view>

namespace configuration
{

template<typename T>
class value_proxy;

// Retrive a configuration value.
template<typename T>
value_proxy<T> value();

// Retrive a configuration value or return a default value if not existent.
template<typename T>
value_proxy<T> value_or(
    T const&);

template<typename T>
class value_proxy
{
public:
    ~value_proxy();

    operator T() const;

    value_proxy<T> operator[](
        std::string_view key) const;

    // https://stackoverflow.com/a/8915746/1300177
    value_proxy<T> operator[](
        const char *key)
    {
        return (*this)[std::string_view{key}];
    }

private:
    friend value_proxy<T> value<T>();
    friend value_proxy<T> value_or<T>(T const&);

    struct impl_t;
    std::unique_ptr<impl_t> impl;

    value_proxy(
        impl_t);
};

void initialize(
    std::filesystem::path const& config_file);

extern struct info_t {} info;

std::ostream& operator<<(
    std::ostream&, info_t const&);

}
