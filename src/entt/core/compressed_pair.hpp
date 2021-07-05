#ifndef ENTT_CORE_COMPRESSED_PAIR_HPP
#define ENTT_CORE_COMPRESSED_PAIR_HPP


#include <tuple>
#include <type_traits>
#include <utility>
#include "../config/config.h"
#include "type_traits.hpp"


namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename Type, std::size_t, typename = void>
struct compressed_pair_element {
    template<bool Dummy = true, typename = std::enable_if_t<Dummy && std::is_default_constructible_v<Type>>>
    compressed_pair_element()
        : value{}
    {}

    template<typename Args, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Args>, compressed_pair_element>>>
    compressed_pair_element(Args &&args)
        : value{std::forward<Args>(args)}
    {}

    template<typename... Args, std::size_t... Index>
    compressed_pair_element(std::tuple<Args...> args, std::index_sequence<Index...>)
        : value{std::get<Index>(args)...}
    {}

    [[nodiscard]] Type & get() ENTT_NOEXCEPT {
        return value;
    }

    [[nodiscard]] const Type & get() const ENTT_NOEXCEPT {
        return value;
    }

private:
    Type value;
};


template<typename Type, std::size_t Tag>
struct compressed_pair_element<Type, Tag, std::enable_if_t<is_ebco_eligible_v<Type>>>
    : Type
{
    template<bool Dummy = true, typename = std::enable_if_t<Dummy && std::is_default_constructible_v<Type>>>
    compressed_pair_element()
        : Type{}
    {}

    template<typename Args, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Args>, compressed_pair_element>>>
    compressed_pair_element(Args &&args)
        : Type{std::forward<Args>(args)}
    {}

    template<typename... Args, std::size_t... Index>
    compressed_pair_element(std::tuple<Args...> args, std::index_sequence<Index...>)
        : Type{std::get<Index>(args)...}
    {}

    [[nodiscard]] Type & get() ENTT_NOEXCEPT {
        return *this;
    }

    [[nodiscard]] const Type & get() const ENTT_NOEXCEPT {
        return *this;
    }
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief A compressed pair.
 *
 * A pair that exploits the _Empty Base Class Optimization_ (or _EBCO_) to
 * reduce its final size to a minimum.
 *
 * @tparam First The type of the first element that the pair stores.
 * @tparam Second The type of the second element that the pair stores.
 */
template<typename First, typename Second>
class compressed_pair final:
    internal::compressed_pair_element<First, 0u>,
    internal::compressed_pair_element<Second, 1u>
{
    using first_base = internal::compressed_pair_element<First, 0u>;
    using second_base = internal::compressed_pair_element<Second, 1u>;

public:
    /**
     * @brief Default constructor, conditionally enabled.
     *
     * This constructor is only available when the types that the pair stores
     * are both at least default constructible.
     *
     * @tparam Dummy Dummy template parameter used for internal purposes.
     */
    template<bool Dummy = true, typename = std::enable_if_t<Dummy && std::is_default_constructible_v<First> && std::is_default_constructible_v<Second>>>
    constexpr compressed_pair()
        : first_base{},
          second_base{}
    {}

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    constexpr compressed_pair(const compressed_pair &other) = default;

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    constexpr compressed_pair(compressed_pair &&other) ENTT_NOEXCEPT = default;

    /**
     * @brief Constructs a pair from its values.
     * @tparam Arg Type of value to use to initialize the first element.
     * @tparam Other Type of value to use to initialize the second element.
     * @param arg Value to use to initialize the first element.
     * @param other Value to use to initialize the second element.
     */
    template<typename Arg, typename Other>
    constexpr compressed_pair(Arg &&arg, Other &&other)
        : first_base{std::forward<Arg>(arg)},
          second_base{std::forward<Other>(other)}
    {}

    /**
     * @brief Constructs a pair by forwarding the arguments to its parts.
     * @tparam Args Types of arguments to use to initialize the first element.
     * @tparam Other Types of arguments to use to initialize the second element.
     * @param args Arguments to use to initialize the first element.
     * @param other Arguments to use to initialize the second element.
     */
    template<typename... Args, typename... Other>
    constexpr compressed_pair(std::piecewise_construct_t, std::tuple<Args...> args, std::tuple<Other...> other)
        : first_base{std::move(args), std::index_sequence_for<Args...>{}},
          second_base{std::move(other), std::index_sequence_for<Other...>{}}
    {}

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This compressed pair object.
     */
    constexpr compressed_pair & operator=(const compressed_pair &other) = default;

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This compressed pair object.
     */
    constexpr compressed_pair & operator=(compressed_pair &&other) ENTT_NOEXCEPT = default;

    /**
     * @brief Returns the first element that a pair stores.
     * @return The first element that a pair stores.
     */
    [[nodiscard]] First & first() ENTT_NOEXCEPT {
        return static_cast<first_base &>(*this).get();
    }

    /*! @copydoc first */
    [[nodiscard]] const First & first() const ENTT_NOEXCEPT {
        return static_cast<const first_base &>(*this).get();
    }

    /**
     * @brief Returns the second element that a pair stores.
     * @return The second element that a pair stores.
     */
    [[nodiscard]] Second & second() ENTT_NOEXCEPT {
        return static_cast<second_base &>(*this).get();
    }

    /*! @copydoc second */
    [[nodiscard]] const Second & second() const ENTT_NOEXCEPT {
        return static_cast<const second_base &>(*this).get();
    }

    /**
     * @brief Swaps two compressed pair objects.
     * @param other The compressed pair to swap with.
     */
    void swap(compressed_pair &other) {
        using std::swap;
        swap(first(), other.first());
        swap(second(), other.second());
    }
};


/**
 * @brief Deduction guide.
 * @tparam Type Type of value to use to initialize the first element.
 * @tparam Other Type of value to use to initialize the second element.
 */
template<typename Type, typename Other>
compressed_pair(Type &&, Other &&)
-> compressed_pair<std::decay_t<Type>, std::decay_t<Other>>;


/**
 * @brief Swaps two compressed pair objects.
 * @tparam First The type of the first element that the pairs store.
 * @tparam Second The type of the second element that the pairs store.
 * @param lhs A valid compressed pair object.
 * @param rhs A valid compressed pair object.
 */
template<typename First, typename Second>
inline void swap(compressed_pair<First, Second> &lhs, compressed_pair<First, Second> &rhs) {
    lhs.swap(rhs);
}


}


#endif