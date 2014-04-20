#ifndef __ROSEWOOD_CORE_ENTITY_H__
#define __ROSEWOOD_CORE_ENTITY_H__

#include <vector>

#include "rosewood/data-structures/stable_vector.h"
#include "rosewood/core/component_array.h"

namespace rosewood { namespace core {

    typedef unsigned int EntityId;

    class EntityManager;

    struct Entity {
        EntityManager *owner;
        EntityId eid;

        void destroy();
        bool is_valid() const;

        template<typename TComp, typename... TArgs>
        TComp *add_component(TArgs... args);

        template<typename... TComps>
        void add_components();

        template<typename TComp>
        TComp *component();

        template<typename TComp>
        void remove_component();

        template<typename TComp>
        void if_component(const std::function<void(TComp*)> &handler) {
            auto comp = component<TComp>();
            if (comp) handler(comp);
        }
    };

    inline constexpr Entity nil_entity() { return Entity{nullptr, 0}; }

    inline bool operator==(const Entity &e1, const Entity &e2) {
        return e1.owner == e2.owner && e1.eid == e2.eid;
    }

    inline bool operator!=(const Entity &e1, const Entity &e2) {
        return !(e1 == e2);
    }

    class EntityManager {
    public:
        EntityManager();

        Entity create_entity();
        void destroy_entity(Entity e);

        template<typename TComp, typename... TComps>
        Entity create_entity();

        bool is_valid(Entity e) const;
        size_t entity_count() const;

        template<typename TComp, typename... TArgs>
        TComp *add_component(Entity entity, TArgs... args);

        template<typename TComp>
        void add_components(Entity entity);

        template<typename TComp, typename TComp2, typename... TComps>
        void add_components(Entity entity);

        template<typename TComp>
        TComp *component(Entity entity);

        template<typename TComp>
        void remove_component(Entity entity);

        template<typename TComp>
        ComponentArrayView<TComp> components();

        template<typename... TComps, typename F>
        void for_components(const F &func);

    private:
        EntityId _max_eid;
        std::vector<EntityId> _free_list;
        data_structures::StableVector<ComponentArray> _components;

        template<typename TComp>
        void ensure_component_index_available(size_t index);
    };

    template<typename TComp, typename... TArgs>
    TComp *Entity::add_component(TArgs... args) {
        return owner->add_component<TComp>(*this, std::forward<TArgs>(args)...);
    }

    template<typename TComp>
    TComp *Entity::component() {
        return owner->component<TComp>(*this);
    }

    template<typename... TComps>
    void Entity::add_components() {
        owner->add_components<TComps...>(*this);
    }

    template<typename TComp>
    void Entity::remove_component() {
        owner->remove_component<TComp>(*this);
    }

    template<typename TComp, typename... TComps>
    Entity EntityManager::create_entity() {
        Entity e = create_entity();
        add_components<TComp, TComps...>(e);
        return e;
    }

    template<typename TComp, typename... TArgs>
    TComp *EntityManager::add_component(Entity entity, TArgs... args) {
        auto index = TComp::register_type();
        ensure_component_index_available<TComp>(index);

        auto &comp_array = _components[index];
        return comp_array.template create<TComp>(entity.eid, entity, std::forward<TArgs>(args)...);
    };

    template<typename TComp>
    void EntityManager::add_components(Entity entity) {
        add_component<TComp>(entity);
    }

    template<typename TComp1, typename TComp2, typename... TComps>
    void EntityManager::add_components(Entity entity) {
        add_component<TComp1>(entity);
        add_components<TComp2, TComps...>(entity);
    }

    template<typename TComp>
    TComp *EntityManager::component(Entity entity) {
        auto index = TComp::register_type();
        if (index >= _components.size() || entity.eid >= _components[index].template size<TComp>()) {
            return nullptr;
        }

        auto &comp_array = _components[index];
        return comp_array.template at<TComp>(entity.eid);
    }

    template<typename TComp>
    void EntityManager::remove_component(Entity entity) {
        if (!component<TComp>(entity)) {
            return;
        }

        auto index = TComp::register_type();
        auto &comp_array = _components[index];
        comp_array.template remove<TComp>(entity.eid);
    }

    template<typename TComp>
    ComponentArrayView<TComp> EntityManager::components() {
        auto index = TComp::register_type();
        return ComponentArrayView<TComp>(&_components[index]);
    }

    template<typename T>
    bool all_not_null(T *ptr) {
        return ptr != nullptr;
    }

    template<typename TFirst, typename TSecond, typename... TRest>
    bool all_not_null(TFirst *first, TSecond *second, TRest*... rest) {
        if (!first) {
            return false;
        }
        return all_not_null<TSecond, TRest...>(second, rest...);
    }

    template<typename... TComps, typename F>
    void EntityManager::for_components(const F &func) {
        for (EntityId i = 0; i <= _max_eid; ++i) {
            Entity e{this, i};

            if (is_valid(e) && all_not_null(component<TComps>(e)...)) {
                func(component<TComps>(e)...);
            }
        }
    }

    template<typename TComp>
    void EntityManager::ensure_component_index_available(size_t index) {
        if (index >= _components.size()) {
            _components.resize(index + 1);
        }
    }

} }

namespace std {
    template<> struct hash<rosewood::core::Entity> {
        size_t operator()(const rosewood::core::Entity &entity) const {
            return hash<rosewood::core::EntityId>()(entity.eid);
        }
    };
}

#endif
