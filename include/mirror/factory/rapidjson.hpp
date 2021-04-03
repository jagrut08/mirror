/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef MIRROR_FACTORY_RAPIDJSON_HPP
#define MIRROR_FACTORY_RAPIDJSON_HPP

#include <cassert>
#include <tuple>
#include <vector>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/pointer.h>
#include <rapidjson/rapidjson.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

namespace mirror {
//------------------------------------------------------------------------------
struct rapidjson_factory_traits {
    class builder_unit;

    template <typename Product>
    class factory_unit;

    template <typename Product>
    class constructor_unit;

    template <typename T>
    class atomic_unit;

    template <typename T>
    class composite_unit;

    template <typename T>
    class copy_unit;

    struct construction_context {
        rapidjson::Value& value;
    };

    class builder_unit {};

    template <typename Product>
    class factory_unit {
    public:
        factory_unit(const builder_unit&) noexcept {}
        factory_unit(const composite_unit<Product>&) noexcept {}

        auto select_constructor(construction_context& ctx, const factory& fac)
          -> size_t {
            size_t result = _children.size();
            size_t count = 0;
            std::tuple<int, int> best_match{0, 0};
            for(size_t i = 0; i < _children.size(); ++i) {
                const auto match = _children[i]->match(ctx, fac.constructor(i));
                if(match > best_match) {
                    count = 1;
                    result = i;
                } else if(match == best_match) {
                    ++count;
                    result = i;
                }
            }
            return (count == 1) ? result : _children.size();
        }

    private:
        std::vector<constructor_unit<Product>*> _children;

        friend constructor_unit<Product>;
    };

    template <typename Product>
    class constructor_unit {
    public:
        constructor_unit(factory_unit<Product>& parent) noexcept {
            parent._children.emplace_back(this);
        }

        template <typename T>
        static auto type_match(const T*, construction_context&) noexcept
          -> std::tuple<int, int> {
            return {true, true};
        }

        static auto type_match(float*, construction_context& ctx) noexcept
          -> std::tuple<bool, bool> {
            return {
              ctx.value.IsDouble() || ctx.value.IsInt64() || ctx.value.IsInt(),
              ctx.value.IsDouble()};
        }

        auto match(construction_context& ctx, const factory_constructor& ctr)
          const noexcept -> std::tuple<int, int> {
            const std::tuple<int, int> no_match{-1, 0};
            int result = 0;
            int exact = 0;
            const size_t n = ctr.parameter_count();
            for(size_t i = 0; i < n; ++i) {
                auto& param = ctr.parameter(i);
                auto pos = ctx.value.FindMember(param.name().data());
                if(pos == ctx.value.MemberEnd()) {
                    return no_match;
                }
                const auto [match, exact_match] =
                  type_match(static_cast<Product*>(nullptr), ctx);
                if(!match) {
                    return no_match;
                }
                if(exact_match) {
                    ++exact;
                }
                ++result;
            }
            return {result, exact};
        }
    };

    template <typename T>
    static constexpr bool is_atomic =
      std::is_floating_point_v<T> || std::is_integral_v<T> ||
      std::is_same_v<T, std::string>;

    class constructor_info {
    public:
        constructor_info(
          const object_builder& builder,
          const factory_constructor& ctr) noexcept
          : _name{builder.name()}
          , _index{ctr.constructor_index()}
          , _is_default{ctr.is_default_constructor()}
          , _is_move{ctr.is_move_constructor()}
          , _is_copy{ctr.is_copy_constructor()} {}

        auto nested(construction_context& ctx) -> construction_context {
            if(!_name.empty()) {
                if(ctx.value.IsObject()) {
                    auto pos = ctx.value.FindMember(_name.c_str());
                    if(pos != ctx.value.MemberEnd()) {
                        return {pos->value};
                    }
                }
            }
            // TODO: Handle JSON arrays
            // TODO: Handle nullptr -> default constructor
            return ctx;
        }

    private:
        std::string _name;
        size_t _index;
        bool _is_default;
        bool _is_move;
        bool _is_copy;
    };

    template <typename T>
    class atomic_unit {
    public:
        template <typename P>
        atomic_unit(
          const constructor_unit<P>&,
          const object_builder& builder,
          const factory_constructor& ctr) noexcept
          : _info{builder, ctr} {}

        static void fetch(float& dest, const rapidjson::Value& v) {
            if(v.IsDouble()) {
                dest = static_cast<float>(v.GetDouble());
            } else if(v.IsInt64()) {
                dest = static_cast<float>(v.GetInt64());
            } else if(v.IsInt()) {
                dest = static_cast<float>(v.GetInt());
            }
        }

        auto
        get(construction_context& ctx, const factory_constructor_parameter&) {
            T result{};
            fetch(result, _info.nested(ctx).value);
            return result;
        }

    private:
        constructor_info _info;
    };

    template <typename T>
    class composite_unit {
    public:
        template <typename P>
        composite_unit(
          const constructor_unit<P>&,
          const object_builder& builder,
          const factory_constructor& ctr) noexcept
          : _info{builder, ctr}
          , _fac{*this, builder} {}

        auto
        get(construction_context& ctx, const factory_constructor_parameter&) {
            return _fac.construct(_info.nested(ctx));
        }

    private:
        constructor_info _info;
        built_factory_type<rapidjson_factory_traits, T> _fac;
    };

    template <typename T>
    class copy_unit {
    public:
        template <typename P>
        copy_unit(
          const constructor_unit<P>&,
          const object_builder& builder,
          const factory_constructor& ctr)
          : _info{builder, ctr} {}

        auto get(construction_context&, const factory_constructor_parameter&)
          -> T {
            return T{};
        }

    private:
        constructor_info _info;
    };
};
//------------------------------------------------------------------------------
} // namespace mirror

#endif