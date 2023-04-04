#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "pch.hpp"

#include "window.hpp"
#include "scene_object.hpp"
#include "model.hpp"
#include "light.hpp"

namespace Controller {
void InitGUI(Window &window);

void TransformGUI(std::shared_ptr<SceneObject> object);

void ModelAttributeGUI(std::shared_ptr<Model> model);

void LightAttributeGUI(std::shared_ptr<Light> light);

} // namespace Controller

#endif
