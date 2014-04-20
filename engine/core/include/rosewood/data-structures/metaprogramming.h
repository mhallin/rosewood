#ifndef __ROSEWOOD_DATA_STRUCTURES_METAPROGRAMMING_H__
#define __ROSEWOOD_DATA_STRUCTURES_METAPROGRAMMING_H__

#include <stddef.h>

#include <type_traits>

namespace rosewood { namespace data_structures {

    namespace internal {
        template<template <typename> class TPred, typename... TArgs>
        struct static_max_helper;

        template<typename... TArgs>
        struct static_member_helper;

        template<typename TElement, size_t index, typename... TArgs>
        struct static_index_of_helper;

        template<typename... TArgs>
        void destruct_helper(size_t n, void *data);

        template<typename... TArgs>
        void placement_copy_construct_helper(size_t n, void *dest, const void *source);

        template<typename... TArgs>
        void placement_default_construct_helper(size_t n, void *dest);
    }

    /// static_max<TPred<>, TArgs...> : finds the maximum element (and type)
    /// in a list. TPred<> is the function to determine the size of an element.
    template<template <typename> class TPred, typename... TArgs>
    struct static_max;

    template<template <typename> class TPred, typename TFirst, typename... TRest>
    struct static_max<TPred, TFirst, TRest...> {
        typedef decltype(TPred<TFirst>::value) result_type;
        typedef internal::static_max_helper<TPred, TFirst, TRest...> helper;

        static constexpr result_type value = helper::value;
        typedef typename helper::type type;
    };

    /// SizeOf<T> : Can be used with static_max above to find the largest
    /// type in a list
    template<typename T>
    struct SizeOf {
        static constexpr size_t value = sizeof(T);
    };

    /// AlignOf<T> : Can be used with static_max above to find the type
    /// with the largest alignment requirement in a list
    template<typename T>
    struct AlignOf {
        static constexpr size_t value = alignof(T);
    };


    /// static_member<TNeedle, THaystack...> : Find if an element is a member of
    /// a list.
    template<typename TNeedle, typename... THaystack>
    struct static_member {
        typedef internal::static_member_helper<TNeedle, THaystack...> helper;

        static constexpr bool result = helper::value;
    };


    /// static_index_of<TElement, TArgs...> : Find the index of an element
    template<typename TElement, typename... TArgs>
    struct static_index_of {
        typedef internal::static_index_of_helper<TElement, 0, TArgs...> helper;

        static constexpr size_t result = helper::value;
    };


    /// destruct<TArgs...>(int n, void *data) : Call the destructor for the
    /// n:th type on data
    template<typename... TArgs>
    void destruct(size_t n, void *data) {
        internal::destruct_helper<TArgs...>(n, data);
    }

    /// placement_copy_construct<TArgs...>(int n, void *dest, const void *source)
    /// Call the copy constructor for the n:th type on source saving to dest.
    template<typename... TArgs>
    void placement_copy_construct(size_t n, void *dest, const void *source) {
        internal::placement_copy_construct_helper<TArgs...>(n, dest, source);
    }

    /// placement_default_construct<TArgs...>(int n, void *dest) : Default
    /// constructs the n:th type at dest.
    template<typename... TArgs>
    void placement_default_construct(size_t n, void *dest) {
        internal::placement_default_construct_helper<TArgs...>(n, dest);
    }

    namespace internal {
        template<template <typename> class TPred, typename TAcc, typename TNext, typename... TRest>
        struct static_max_helper<TPred, TAcc, TNext, TRest...> {
            typedef std::conditional<(TPred<TAcc>::value > TPred<TNext>::value),
                                     static_max_helper<TPred, TAcc, TRest...>,
                                     static_max_helper<TPred, TNext, TRest...>>
                next_helper_cond;

            static constexpr decltype(TPred<TAcc>::value) value = next_helper_cond::type::value;
            typedef typename next_helper_cond::type::type type;
        };

        template<template <typename> class TPred, typename TSingle>
        struct static_max_helper<TPred, TSingle> {
            static constexpr decltype(TPred<TSingle>::value) value = TPred<TSingle>::value;
            typedef TSingle type;
        };

        template<typename TNeedle, typename TFirst, typename... THaystack>
        struct static_member_helper<TNeedle, TFirst, THaystack...> {
            typedef std::conditional<std::is_same<TNeedle, TFirst>::value,
                                     static_member_helper<TNeedle, TFirst>,
                                     static_member_helper<TNeedle, THaystack...>>
                next_helper_cond;

            static constexpr bool value = next_helper_cond::type::value;
        };

        template<typename TNeedle, typename TFirst>
        struct static_member_helper<TNeedle, TFirst> {
            static constexpr bool value = std::is_same<TNeedle, TFirst>::value;
        };

        template<typename TNeedle, size_t position, typename TFirst, typename... TRest>
        struct static_index_of_helper<TNeedle, position, TFirst, TRest...> {
            typedef std::conditional<std::is_same<TNeedle, TFirst>::value,
                                     static_index_of_helper<TNeedle, position>,
                                     static_index_of_helper<TNeedle, position + 1, TRest...>>
                next_helper_cond;

            static constexpr size_t value = next_helper_cond::type::value;
        };

        template<typename TNeedle, size_t position>
        struct static_index_of_helper<TNeedle, position> {
            static constexpr size_t value = position;
        };

        template<typename TFunc>
        struct at_index {
            TFunc func;
            template<size_t index, typename TFirst, typename TSecond, typename... TRest>
            void operator()(size_t n) {
                if (n == index) {
                    func.template operator()<TFirst>();
                }
                else {
                    this->operator()<index + 1, TSecond, TRest...>(n);
                }
            }

            template<size_t index, typename TSingle>
            void operator()(size_t n) {
                if (n == index) {
                    func.template operator()<TSingle>();
                }
            }
        };

        struct destruct {
            void *data;

            template<typename T>
            void operator()() { static_cast<T*>(data)->~T(); }
        };

        template<> inline void destruct::operator()<void>() { }

        template<typename... TArgs>
        void destruct_helper(size_t n, void *data) {
            destruct callback{data};
            at_index<destruct> runner{callback};
            runner.template operator()<0, TArgs...>(n);
        }

        struct placement_copy_construct {
            void *dest;
            const void *source;

            template<typename T>
            void operator()() { new(dest) T(*static_cast<const T*>(source)); }
        };

        template<>
        inline void placement_copy_construct::operator()<void>() { }

        template<typename... TArgs>
        void placement_copy_construct_helper(size_t n, void *dest, const void *source) {
            placement_copy_construct callback{dest, source};
            at_index<placement_copy_construct> runner{callback};
            runner.template operator()<0, TArgs...>(n);
        }

        struct placement_default_construct {
            void *dest;

            template<typename T>
            void operator()() { new(dest) T(); }
        };

        template<>
        inline void placement_default_construct::operator()<void>() { }

        template<typename... TArgs>
        void placement_default_construct_helper(size_t n, void *dest) {
            placement_default_construct callback{dest};
            at_index<placement_default_construct> runner{callback};
            runner.template operator()<0, TArgs...>(n);
        }
    }

} }

#endif
