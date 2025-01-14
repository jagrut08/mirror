/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef MIRROR_SEQUENCE_HPP
#define MIRROR_SEQUENCE_HPP

#include "primitives.hpp"

namespace mirror {

/// @brief Template implementing an unpacked sequence of individual metaobjects,
/// @ingroup metaobjects
/// @see is_metaobject_sequence
/// @see reflects_metaobject_sequence
/// @see wrapped_metaobject
template <__metaobject_id... M>
struct unpacked_metaobject_sequence {};

/// @brief Indicates if the argument is a sequence of metaobjects.
/// @ingroup classification
/// @see reflects_object_sequence
/// @see is_empty
/// @see get_size
template <__metaobject_id M>
consteval auto is_object_sequence(wrapped_metaobject<M>) noexcept -> bool {
    return __metaobject_is_meta_object_sequence(M);
}

template <__metaobject_id... M>
consteval auto is_object_sequence(unpacked_metaobject_sequence<M...>) noexcept
  -> bool {
    return true;
}

template <typename X>
consteval auto is_object_sequence(const X&) noexcept -> bool {
    return false;
}

template <typename X>
concept metaobject_sequence = is_object_sequence(X{});

template <__metaobject_id... M>
consteval auto is_empty(unpacked_metaobject_sequence<M...>) noexcept -> bool {
    return sizeof...(M) == 0Z;
}

template <__metaobject_id... M>
consteval auto get_size(unpacked_metaobject_sequence<M...>) noexcept -> size_t {
    return sizeof...(M);
}

/// @brief Unpacks a sequence metaobject into sequence of separate metaobject ids.
/// @ingroup sequence_operations
/// @see reflects_object_sequence
/// @see expand
template <__metaobject_id M>
constexpr auto unpack(wrapped_metaobject<M>) noexcept
  -> __unpack_metaobject_seq<unpacked_metaobject_sequence, M> requires(
    __metaobject_is_meta_object_sequence(M)) {
    return {};
}

template <__metaobject_id... M>
constexpr auto unpack(unpacked_metaobject_sequence<M...> ms) noexcept {
    return ms;
}

/// @brief Makes an unpacked_metaobject_sequence from individual metaobjects.
/// @ingroup sequence_operations
/// @see is_object_sequence
template <__metaobject_id... M>
constexpr auto make_sequence(wrapped_metaobject<M>...) noexcept
  -> unpacked_metaobject_sequence<M...> {
    return {};
}

// count if
template <__metaobject_id... M, typename F>
constexpr auto
count_if(unpacked_metaobject_sequence<M...>, F predicate) noexcept -> size_t {
    return (0Z + ... + (predicate(wrapped_metaobject<M>{}) ? 1Z : 0Z));
}

template <__metaobject_id M, typename F>
constexpr auto count_if(wrapped_metaobject<M> mo, F predicate) noexcept
  -> size_t requires(__metaobject_is_meta_object_sequence(M)) {
    return count_if(unpack(mo), predicate);
}

// transform
template <__metaobject_id... M, typename F>
constexpr auto
transform(unpacked_metaobject_sequence<M...>, F function) noexcept {
    return unpacked_metaobject_sequence<unwrap(
      function(wrapped_metaobject<M>{}))...>{};
}

template <__metaobject_id M, typename F>
constexpr auto transform(wrapped_metaobject<M> mo, F function) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    return transform(unpack(mo), function);
}

// transform types
template <
  template <typename...>
  class Container,
  template <typename>
  class Transform,
  __metaobject_id... M>
constexpr auto
store_transformed_types(unpacked_metaobject_sequence<M...>) noexcept {
    return Container<typename Transform<__unrefltype(M)>::type...>{};
}

template <
  template <typename...>
  class Container,
  template <typename>
  class Transform,
  __metaobject_id M>
constexpr auto store_transformed_types(wrapped_metaobject<M> mo) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    return store_transformed_types(unpack(mo));
}

template <template <typename> class Transform, __metaobject_id... M>
constexpr auto transform_types(unpacked_metaobject_sequence<M...> ms) noexcept {
    return store_transformed_types<type_list, Transform>(ms);
}

template <template <typename> class Transform, __metaobject_id M>
constexpr auto transform_types(wrapped_metaobject<M> mo) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    return store_transformed_types<type_list, Transform>(unpack(mo));
}

// extract types
template <__metaobject_id... M>
constexpr auto extract_types(unpacked_metaobject_sequence<M...> ms) noexcept {
    return transform_types<std::type_identity>(ms);
}

template <__metaobject_id M>
constexpr auto extract_types(wrapped_metaobject<M> mo) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    return transform_types<std::type_identity>(unpack(mo));
}

// for each
template <__metaobject_id... M, typename F>
constexpr void
for_each(unpacked_metaobject_sequence<M...>, F function) noexcept {
    (void)(..., function(wrapped_metaobject<M>{}));
}

template <__metaobject_id M, typename F>
constexpr void for_each(wrapped_metaobject<M> mo, F function) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    return for_each(unpack(mo), std::move(function));
}

// for each with iteration info
class for_each_iteration_info {
private:
    std::size_t _index;
    std::size_t _count;

public:
    constexpr for_each_iteration_info(
      std::size_t index,
      std::size_t count) noexcept
      : _index{index}
      , _count{count} {}

    constexpr auto is_first() const noexcept -> bool {
        return _index == 0Z;
    }

    constexpr auto is_last() const noexcept -> bool {
        return _index + 1Z >= _count;
    }

    constexpr auto index() const noexcept -> std::size_t {
        return _index;
    }

    constexpr auto count() const noexcept -> std::size_t {
        return _count;
    }
};

template <__metaobject_id M, typename F>
constexpr void for_each_info(wrapped_metaobject<M> mo, F function) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    std::size_t index{0};
    const auto count{__metaobject_get_size(M)};
    return for_each(unpack(mo), [&](auto me) {
        function(me, for_each_iteration_info(index++, count));
    });
}

// find if
template <typename F>
constexpr auto find_if(unpacked_metaobject_sequence<>, F) noexcept {
    return no_metaobject;
}

template <__metaobject_id M, __metaobject_id... Mt, typename F>
constexpr auto
find_if(unpacked_metaobject_sequence<M, Mt...>, F predicate) noexcept {
    if constexpr(predicate(wrapped_metaobject<M>{})) {
        return wrapped_metaobject<M>{};
    } else {
        return find_if(unpacked_metaobject_sequence<Mt...>{}, predicate);
    }
}

template <__metaobject_id M, typename F>
constexpr auto find_if(wrapped_metaobject<M> mo, F predicate) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    return find_if(unpack(mo), predicate);
}

// filter
template <__metaobject_id... M, typename F>
constexpr auto do_filter(
  unpacked_metaobject_sequence<M...>,
  unpacked_metaobject_sequence<>,
  F) noexcept -> unpacked_metaobject_sequence<M...> {
    return {};
}

template <
  __metaobject_id... Mh,
  __metaobject_id M,
  __metaobject_id... Mt,
  typename F>
constexpr auto do_filter(
  unpacked_metaobject_sequence<Mh...>,
  unpacked_metaobject_sequence<M, Mt...>,
  F predicate) noexcept {
    if constexpr(predicate(wrapped_metaobject<M>{})) {
        return do_filter(
          unpacked_metaobject_sequence<Mh..., M>{},
          unpacked_metaobject_sequence<Mt...>{},
          predicate);
    } else {
        return do_filter(
          unpacked_metaobject_sequence<Mh...>{},
          unpacked_metaobject_sequence<Mt...>{},
          predicate);
    }
}

template <__metaobject_id... M, typename F>
constexpr auto
filter(unpacked_metaobject_sequence<M...> seq, F predicate) noexcept {
    return do_filter(unpacked_metaobject_sequence<>{}, seq, predicate);
}

template <__metaobject_id M, typename F>
constexpr auto filter(wrapped_metaobject<M> mo, F predicate) noexcept
  requires(__metaobject_is_meta_object_sequence(M)) {
    return filter(unpack(mo), predicate);
}

// all of
template <__metaobject_id... M, typename F>
constexpr auto all_of(unpacked_metaobject_sequence<M...>, F predicate) noexcept
  -> bool {
    return (... && predicate(wrapped_metaobject<M>{}));
}

template <__metaobject_id M, typename F>
constexpr auto all_of(wrapped_metaobject<M> mo, F predicate) noexcept
  -> bool requires(__metaobject_is_meta_object_sequence(M)) {
    return all_of(unpack(mo), predicate);
}

// any_of
template <__metaobject_id... M, typename F>
constexpr auto any_of(unpacked_metaobject_sequence<M...>, F predicate) noexcept
  -> bool {
    return (... || predicate(wrapped_metaobject<M>{}));
}

template <__metaobject_id M, typename F>
constexpr auto any_of(wrapped_metaobject<M> mo, F predicate) noexcept
  -> bool requires(__metaobject_is_meta_object_sequence(M)) {
    return any_of(unpack(mo), predicate);
}

// none_of
template <__metaobject_id... M, typename F>
constexpr auto none_of(unpacked_metaobject_sequence<M...>, F predicate) noexcept
  -> bool {
    return !(... || predicate(wrapped_metaobject<M>{}));
}

template <__metaobject_id M, typename F>
constexpr auto none_of(wrapped_metaobject<M> mo, F predicate) noexcept
  -> bool requires(__metaobject_is_meta_object_sequence(M)) {
    return none_of(unpack(mo), predicate);
}

// select
template <typename T, __metaobject_id... M, typename F, typename... P>
constexpr auto select(
  unpacked_metaobject_sequence<M...>,
  F function,
  T fallback,
  P&&... param) noexcept -> T {
    (void)(..., function(fallback, wrapped_metaobject<M>{}, std::forward<P>(param)...));
    return fallback;
}

template <typename T, __metaobject_id M, typename F, typename... P>
constexpr auto
select(wrapped_metaobject<M> mo, F function, T fallback, P&&... param) noexcept
  -> T requires(__metaobject_is_meta_object_sequence(M)) {
    return select(
      unpack(mo),
      std::move(function),
      std::move(fallback),
      std::forward<P>(param)...);
}

// concat
template <__metaobject_id... L, __metaobject_id... R>
constexpr auto concat(
  unpacked_metaobject_sequence<L...>,
  unpacked_metaobject_sequence<R...>) noexcept
  -> unpacked_metaobject_sequence<L..., R...> {
    return {};
}

template <metaobject_sequence M>
constexpr auto concat(M ms) noexcept {
    return ms;
}

template <metaobject_sequence M, metaobject_sequence... Ms>
constexpr auto concat(M h, Ms... t) noexcept {
    return concat(unpack(h), concat(unpack(t)...));
}

// flatten
template <__metaobject_id... M>
constexpr auto flatten(unpacked_metaobject_sequence<M...>) requires(
  ...&& is_object_sequence(wrapped_metaobject<M>{})) {
    return concat(wrapped_metaobject<M>{}...);
}

} // namespace mirror

#endif // MIRROR_SEQUENCE_HPP
