#include "rosewood/core/entity.h"

using rosewood::core::Entity;
using rosewood::core::EntityManager;

void Entity::destroy() {
    owner->destroy_entity(*this);
}

bool Entity::is_valid() const {
    return owner && owner->is_valid(*this);
}

EntityManager::EntityManager() : _max_eid(0) { }

Entity EntityManager::create_entity() {
    EntityId eid;

    if (_free_list.empty()) {
        eid = ++_max_eid;
    }
    else {
        eid = _free_list.back();
        _free_list.pop_back();
    }

    return Entity{this, eid};
}

void EntityManager::destroy_entity(Entity e) {
    unsigned int type_code = 0;
    for (auto &comp_array : _components) {
        comp_array.remove_dynamic(e.eid, type_code++);
    }

    if (e.eid == _max_eid) {
        --_max_eid;
    }
    else {
        _free_list.push_back(e.eid);
    }
}

bool EntityManager::is_valid(Entity e) const {
    return (e.eid <= _max_eid &&
            std::find(begin(_free_list), end(_free_list), e.eid) == end(_free_list));
}

size_t EntityManager::entity_count() const {
    return _max_eid - _free_list.size();
}
