#include "rosewood/graphics/renderer.h"

#include <ostream>

#include "rosewood/graphics/material.h"

using rosewood::graphics::Renderer;

void Renderer::print_debug_info(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "- Renderer " << this << "\n";
    _material->print_debug_info(os, indent + 1);
}
