/*
 * Copyright (c) 2018 Isetta
 */
#include "InputLevel.h"

#include "Core/Config/Config.h"
#include "Graphics/CameraComponent.h"
#include "InputTestComponent.h"
#include "Scene/Entity.h"

using namespace Isetta;

void InputLevel::Load() {
  Entity* cameraEntity{Entity::Instantiate("Camera")};
  cameraEntity->SetTransform(Math::Vector3{0, 5, 10}, Math::Vector3{-15, 0, 0},
                             Math::Vector3::one);

  cameraEntity->AddComponent<CameraComponent>();
  // See InputTestComponent for details
  cameraEntity->AddComponent<InputTestComponent>();
}
