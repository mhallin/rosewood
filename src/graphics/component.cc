#include "rosewood/graphics/component.h"

#include <ostream>

using rosewood::graphics::Component;

Component::Component(SceneObject *owner) : _owner(owner) { }

Component::~Component() { }

void Component::forward_update() { }
void Component::backward_update() { }

void Component::print_debug_info(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "- Unknown Component " << this << "\n";
}
