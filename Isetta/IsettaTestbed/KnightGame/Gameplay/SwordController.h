/*
 * Copyright (c) 2018 Isetta
 */
#pragma once
#include "Scene/Component.h"

using namespace Isetta;

namespace Isetta {
class MeshComponent;
}

namespace KnightGame {
BEGIN_COMPONENT(SwordController, Component, true)
private:
MeshComponent* const knightMesh;
class KnightController* const knight;

public:
SwordController(MeshComponent* knightMesh, class KnightController* const knight)
    : knightMesh{knightMesh}, knight{knight} {}

void Update() override;
};  // namespace KnightGame
}  // namespace KnightGame

using namespace KnightGame;
REGISTER_COMPONENT(SwordController, Component, true)