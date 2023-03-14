#pragma once

#include <any>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "traits.h"

// TODO add a deserializer in case I ever need it
namespace siren::json
{

    template<typename Class, typename T>
    struct PropertyImpl {
        constexpr PropertyImpl(T Class::*a_member, const char* a_name)
            : member{a_member}, name{a_name}
        {
        }

        using Type = T;
        T Class::*member;
        const char* name;
    };

    template<typename Class, typename T>
    constexpr auto property(T Class::*member, const char* name)
    {
        return PropertyImpl<Class, T>{member, name};
    }

    template<typename Class, typename Func, size_t i = 0>
    constexpr void for_each(Func&& func)
    {
        constexpr auto property = std::get<i>(Class::properties());

        func(property, i);

        if constexpr (i + 1 != std::tuple_size<decltype(Class::properties())>{})
        {
            for_each<Class, Func, i + 1>(std::forward<Func>(func));
        }
    }

    template<typename Class>
    class Json
    {

    public:
        Json() = default;

        std::any& operator[](const std::string& name)
        {
            return m_data[name];
        }

        std::any operator[](const std::string& name) const
        {
            return m_data.at(name);
        }

        struct Compare {
            template<typename Pair>
            bool operator()(const Pair& lhs, const Pair& rhs) const
            {
                if (lhs.first != rhs.first)
                {
                    return false;
                }

                bool is_equal = true;
                for_each<Class>([&](auto property, size_t i) {
                    if (property.name == lhs.first && property.name == rhs.first)
                    {
                        using PropertyType =
                            std::remove_reference_t<decltype(std::declval<Class>().*(property.member))>;

                        auto left_payload = std::any_cast<PropertyType>(lhs.second);
                        auto right_payload = std::any_cast<PropertyType>(rhs.second);

                        if (lhs.first != rhs.first || left_payload != right_payload)
                        {
                            is_equal = false;
                        }
                    }
                });

                return is_equal;
            }
        };

        friend bool operator==(const Json& lhs, const Json& rhs)
        {
            return std::equal(lhs.get_data().begin(), lhs.get_data().end(),
                              rhs.get_data().begin(), Compare());
        }

    private:
        [[nodiscard]] std::unordered_map<std::string, std::any> get_data() const
        {
            return m_data;
        }

    private:
        std::unordered_map<std::string, std::any> m_data;
    };

    namespace detail
    {
        template<typename T>
        std::string t_string(const T& value)
        {
            if constexpr (std::is_convertible_v<T, std::string>)
            {
                return value;
            }
            else
            {
                return std::to_string(value);
            }
        }

        template<typename T>
        std::string QUOTE_VAL(T key)
        {
            return '"' + t_string(key) + '"';
        }

        template<typename T>
        std::string QUOTE_KEY(T key)
        {
            return QUOTE_VAL(std::forward<T>(key)) + ":";
        }

        template<typename Key, typename Val>
        void handle_map(std::string& str_repr, Key local_key, const Val& local_val);

        template<typename Key, typename Val>
        void handle_vector(std::string& str_repr, Key key, const Val& local_val)
        {
            std::string str_key = t_string(key);
            if (!str_key.empty())
            {
                str_repr += QUOTE_KEY(str_key);
            }
            str_repr += '[';

            size_t entry = 0;
            for (auto v : local_val)
            {
                if (entry != 0)
                {
                    str_repr += ',';
                }

                if constexpr (is_map<decltype(v)>::value)
                {
                    handle_map(str_repr, std::string{}, v);
                }

                if constexpr (is_vector<decltype(v)>::value)
                {
                    handle_vector(str_repr, std::string{}, v);
                }

                if constexpr (is_string_type<decltype(v)>::value)
                {
                    str_repr += QUOTE_VAL(v);
                }
                else if constexpr (std::is_fundamental<decltype(v)>::value)
                {
                    str_repr += t_string(v);
                }
                entry++;
            }
            str_repr += "]";
        }

        template<typename Key, typename Val>
        void handle_map(std::string& str_repr, Key local_key, const Val& local_val)
        {
            std::string str_key = t_string(local_key);
            if (!str_key.empty())
            {
                str_repr += QUOTE_KEY(local_key);
            }
            str_repr += '{';

            std::string map_sep;
            size_t entry = 0;
            for (auto [k, v] : local_val)
            {
                str_repr += map_sep;
                if (entry == 0)
                {
                    map_sep = ',';
                }

                if constexpr (is_map<decltype(v)>::value)
                {
                    handle_map(str_repr, k, v);
                }

                else if constexpr (is_vector<decltype(v)>::value)
                {
                    handle_vector(str_repr, k, v);
                }

                else if constexpr (is_string_type<decltype(v)>::value)
                {
                    str_repr += QUOTE_KEY(k);
                    str_repr += QUOTE_VAL(v);
                }

                else if constexpr (std::is_fundamental<decltype(v)>::value)
                {
                    str_repr += QUOTE_KEY(k);
                    str_repr += t_string(v);
                }
                entry++;
            }
            str_repr += "}";
        }
    }


    template<typename Class>
    std::string dumps(const Json<Class>& json)
    {
        std::string str_repr{"{"};
        std::string sep;

        for_each<Class>([&](auto property, size_t i) {
            using PropertyType = std::remove_reference_t<decltype(std::declval<Class>().*(property.member))>;

            std::string key = std::string(property.name);
            auto val = std::any_cast<PropertyType>(json[key]);

            str_repr += sep;
            if (i == 0)
            {
                sep = ',';
            }

            if constexpr (is_map<PropertyType>::value)
            {
                detail::handle_map(str_repr, key, val);
            }

            else if constexpr (is_vector<PropertyType>::value)
            {
                detail::handle_vector(str_repr, key, val);
            }

            else if constexpr (std::is_fundamental_v<PropertyType>)
            {
                str_repr += detail::QUOTE_KEY(key);
                str_repr += detail::t_string(val);
            }

            else if constexpr (is_string_type<PropertyType>::value)
            {
                str_repr += detail::QUOTE_KEY(key);
                str_repr += detail::QUOTE_VAL(val);
            }
            else
            {
                std::cerr << "This version of the serializer only supports "
                             "fundamental types, strings, vectors and maps."
                          << std::endl;
            }
        });
        str_repr += "}";
        return str_repr;
    }

    template<typename Class>
    Json<Class> to_json(const Class& obj)
    {
        Json<Class> json;
        for_each<Class>([&](auto& property, size_t i) {
            json[property.name] = obj.*(property.member);
        });
        return json;
    }

}// namespace siren::json