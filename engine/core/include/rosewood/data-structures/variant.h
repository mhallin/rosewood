#ifndef __ROSEWOOD_DATA_STRUCTURES_VARIANT_H__
#define __ROSEWOOD_DATA_STRUCTURES_VARIANT_H__

#include "rosewood/data-structures/metaprogramming.h"

namespace rosewood { namespace data_structures {

    template<typename... TMembers>
    class Variant {
    public:
        Variant();
        Variant(const Variant&);
        Variant &operator=(const Variant&);

        template<typename TSet> Variant(TSet &&t,
            typename std::enable_if<static_member<typename std::remove_const<typename std::remove_reference<TSet>::type>::type, TMembers...>::result>::type *v = 0);

        template<typename TSet, typename TValid = typename std::enable_if<static_member<typename std::decay<TSet>::type, TMembers...>::result>::type>
        Variant &operator=(TSet &&t);

        ~Variant();

        template<typename TGet> bool has() const;
        template<typename TGet> const TGet &get() const;
        template<typename TGet> TGet &get();

        static constexpr size_t kSize = static_max<SizeOf, TMembers...>::value;
        typedef typename static_max<AlignOf, TMembers...>::type alignment_type;

    private:
        union {
            char data[kSize];
            alignment_type alignment;
        };

        size_t type_index;
    };

    template<typename TGet, typename... TMembers>
    bool operator==(const Variant<TMembers...> &v, const TGet &g) {
        return v.template has<TGet>() && v.template get<TGet>() == g;
    }

    template<typename TGet, typename... TMembers>
    bool operator==(const TGet &g, const Variant<TMembers...> &v) {
        return v.template has<TGet>() && v.template get<TGet>() == g;
    }

    template<typename TGet, typename... TMembers>
    bool operator!=(const Variant<TMembers...> &v, const TGet &g) {
        return !(v == g);
    }

    template<typename TGet, typename... TMembers>
    bool operator!=(const TGet &g, const Variant<TMembers...> &v) {
        return !(v == g);
    }

    template<typename... TMembers>
    Variant<TMembers...>::Variant() {
        type_index = 0;
        placement_default_construct<TMembers...>(type_index, data);
    }

    template<typename... TMembers>
    Variant<TMembers...>::Variant(const Variant &other) {
        type_index = other.type_index;
        placement_copy_construct<TMembers...>(type_index, data, other.data);
    }

    template<typename... TMembers>
    Variant<TMembers...> &Variant<TMembers...>::operator=(const Variant<TMembers...> &other) {
        destruct<TMembers...>(type_index, data);
        type_index = other.type_index;
        placement_copy_construct<TMembers...>(type_index, data, other.data);
        return *this;
    }

    template<typename... TMembers>
    template<typename TSet>
    Variant<TMembers...>::Variant(TSet &&t,
        typename std::enable_if<static_member<typename std::remove_const<typename std::remove_reference<TSet>::type>::type, TMembers...>::result>::type*) {
        typedef typename std::remove_const<typename std::remove_reference<TSet>::type>::type TNoRef;
        static_assert(static_member<TNoRef, TMembers...>::result,
                      "invalid type in variant constructor");
        type_index = static_index_of<TNoRef, TMembers...>::result;
        new (data) TNoRef(std::forward<TSet>(t));
    }

    template<typename... TMembers>
    template<typename TSet, typename TValid>
    Variant<TMembers...> &Variant<TMembers...>::operator=(TSet &&t) {
        return (*this = Variant(t));
    }

    template<typename... TMembers>
    Variant<TMembers...>::~Variant() {
        destruct<TMembers...>(type_index, data);
    }

    template<typename... TMembers>
    template<typename TGet>
    bool Variant<TMembers...>::has() const {
        static_assert(static_member<TGet, TMembers...>::result,
                      "variant does not contain type");
        return type_index == static_index_of<TGet, TMembers...>::result;
    }

    template<typename... TMembers>
    template<typename TGet>
    const TGet &Variant<TMembers...>::get() const {
        static_assert(static_member<TGet, TMembers...>::result,
                      "variant does not contain type");
        return *reinterpret_cast<const TGet*>(data);
    }

    template<typename... TMembers>
    template<typename TGet>
    TGet &Variant<TMembers...>::get() {
        static_assert(static_member<TGet, TMembers...>::result,
                      "variant does not contain type");
        return *reinterpret_cast<TGet*>(data);
    }

    template<typename... TMembers>
    const size_t Variant<TMembers...>::kSize;

} }

#endif
