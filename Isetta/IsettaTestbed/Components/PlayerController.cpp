/*
 * Copyright (c) 2018 Isetta
 */
#include "PlayerController.h"
#include "Bullet.h"
#include "Custom/IsettaCore.h"
#include "Graphics/MeshComponent.h"

namespace Isetta {
PlayerController* PlayerController::instance;

void PlayerController::OnEnable() {
  instance = this;
  shootAudio.SetAudioClip("gunshot.aiff");
  bullets.reserve(poolSize);
  for (int i = 0; i < poolSize; i++) {
    Entity* bullet{ADD_ENTITY(Util::StrFormat("Bullet (%d)", i))};
    auto bulletComp = bullet->AddComponent<Bullet, true>();
    bullet->SetActive(false);
    bullets.push_back(bullet);
  }
}

void PlayerController::Start() {
  animationComp = entity->GetComponent<AnimationComponent>();
}

void PlayerController::Update() {
  if (Input::IsKeyPressed(KeyCode::L)) {
    auto light =
        LevelManager::Instance().currentLevel->GetEntityByName("Light");
    if (light != nullptr) {
      Entity::Destroy(light);
    }
  }

  float dt = Time::GetDeltaTime();
  Math::Vector3 lookDir;
  Math::Vector3 movement{};
  if (Input::IsKeyPressed(KeyCode::W)) {
    movement += Math::Vector3::back;
  }

  if (Input::IsKeyPressed(KeyCode::S)) {
    movement += Math::Vector3::forward;
  }
  if (Input::IsKeyPressed(KeyCode::A)) {
    movement += Math::Vector3::right;
  }
  if (Input::IsKeyPressed(KeyCode::D)) {
    movement += Math::Vector3::left;
  }

  if (movement.Magnitude() > 0) {
    if (!isMoving) {
      isMoving = true;
      animationComp->TransitToAnimationState(1, 0.2f);
    }
    GetTransform().TranslateWorld(movement.Normalized() * moveSpeed * dt);
  } else {
    if (isMoving) {
      isMoving = false;
      animationComp->TransitToAnimationState(0, 0.2f);
    }
  }
  if (Input::IsKeyPressed(KeyCode::UP_ARROW)) {
    lookDir += Math::Vector3::back;
  }

  if (Input::IsKeyPressed(KeyCode::DOWN_ARROW)) {
    lookDir += Math::Vector3::forward;
  }

  if (Input::IsKeyPressed(KeyCode::LEFT_ARROW)) {
    lookDir += Math::Vector3::right;
  }

  if (Input::IsKeyPressed(KeyCode::RIGHT_ARROW)) {
    lookDir += Math::Vector3::left;
  }

  if (lookDir.Magnitude() >= 1.f) {
    lookDir.Normalize();
    GetTransform().LookAt(GetTransform().GetWorldPos() + lookDir);
    cooldown -= dt;
    if (cooldown <= 0.f) {
      Shoot();
      cooldown = shootInterval;
    }
  } else {
    cooldown = 0.f;
  }
}

void PlayerController::GuiUpdate() {
  float base = 50;
  float interval = 20;
  GUI::SliderFloat(RectTransform{Math::Rect{-200, base, 300, 100},
                                 GUI::Pivot::TopRight, GUI::Pivot::TopRight},
                   "Player Speed", &moveSpeed, 0, 30);

  GUI::SliderFloat(RectTransform{Math::Rect{-200, base + interval, 300, 100},
                                 GUI::Pivot::TopRight, GUI::Pivot::TopRight},
                   "Player Scale", &scale, 0, 0.1f, 1);
  GetTransform().SetLocalScale(scale * Math::Vector3::one);

  GUI::SliderFloat(
      RectTransform{Math::Rect{-200, base + interval * 2, 300, 100},
                    GUI::Pivot::TopRight, GUI::Pivot::TopRight},
      "Shoot Interval", &shootInterval, 0, .5f, 1);

  GUI::SliderFloat(
      RectTransform{Math::Rect{-200, base + interval * 3, 300, 100},
                    GUI::Pivot::TopRight, GUI::Pivot::TopRight},
      "Bullet Speed", &Bullet::flySpeed, 0, 100.f, 1);
}

PlayerController* PlayerController::Instance() { return instance; }

void PlayerController::Shoot() {
  shootAudio.Play(false, .75f);
  Entity* bullet = nullptr;

  for (Size i = 0; i < bullets.size(); ++i) {
    if (!bullets[i]->GetActive()) {
      bullet = bullets[i];
      break;
    }
  }

  bullet->SetActive(true);
  if (bullet != nullptr) {
    bullet->GetComponent<Bullet>()->Initialize(
        GetTransform().GetWorldPos() + GetTransform().GetForward() * 0.7 -
            GetTransform().GetLeft() * 0.1 + GetTransform().GetUp() * 1.5,
        GetTransform().GetForward());
  }
  // bulletComp->Initialize(GetTransform().GetWorldPos(),
  // GetTransform().GetForward());
}

}  // namespace Isetta
