/*
 * Copyright (c) 2018 Isetta
 */
#include "SkeletonLevel.h"

#include "Components/Editor/EditorComponent.h"
#include "Components/Editor/FrameReporter.h"
#include "Components/FlyController.h"
#include "Components/GridComponent.h"
#include "Core/Config/Config.h"
#include "Custom/EscapeExit.h"
#include "Custom/IsettaCore.h"
#include "Events/Events.h"
#include "Graphics/CameraComponent.h"
#include "Graphics/LightComponent.h"

namespace Isetta {
using LightProperty = LightComponent::Property;
using CameraProperty = CameraComponent::Property;

void SkeletonLevel::OnLevelLoad() {
  Entity *cameraEntity{AddEntity("Camera")};
  CameraComponent *camComp =
      cameraEntity->AddComponent<CameraComponent, true>("Camera");
  cameraEntity->SetTransform(Math::Vector3{0, 5, 10}, Math::Vector3{-15, 0, 0},
                             Math::Vector3::one);
  cameraEntity->AddComponent<FlyController>();

  Entity *lightEntity{AddEntity("Light")};
  LightComponent *lightComp = lightEntity->AddComponent<LightComponent, true>(
      "materials/light.material.xml", "LIGHT_1");
  lightEntity->SetTransform(Math::Vector3{0, 200, 600}, Math::Vector3::zero,
                            Math::Vector3::one);

  Entity *grid{ADD_ENTITY("Grid")};
  grid->AddComponent<GridComponent>();
  grid->AddComponent<EditorComponent>();

  Entity *zombie{AddEntity("Zombie")};
  MeshComponent *mesh =
      zombie->AddComponent<MeshComponent>("Zombie/Zombie.scene.xml");
  mesh->SetActive(false);
  AnimationComponent *animation =
      zombie->AddComponent<AnimationComponent>(mesh);
  animation->AddAnimation("Zombie/Zombie.anim", 0, "", false);
  zombie->SetTransform(Math::Vector3::zero, Math::Vector3::zero,
                       Math::Vector3::one * 0.01f);
  Entity *zombie2{AddEntity("Zombie")};
  MeshComponent *mesh2 =
      zombie2->AddComponent<MeshComponent>("Zombie/Zombie.scene.xml");
  AnimationComponent *animation2 =
      zombie2->AddComponent<AnimationComponent>(mesh2);
  animation2->AddAnimation("Zombie/Zombie.anim", 0, "", false);
  zombie2->SetTransform({1, 0, 0}, Math::Vector3::zero,
                        Math::Vector3::one * 0.01f);

  Entity *head{AddEntity("Head")};
  head->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  head->transform->SetParent(zombie->transform);
  head->transform->SetWorldScale({0.1, 0.1, 0.1});
  Entity *hips{AddEntity("Hips")};
  hips->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  hips->transform->SetParent(zombie->transform);
  hips->transform->SetWorldScale({0.1, 0.1, 0.1});
  Entity *leftShoulder{AddEntity("LeftShoulder")};
  leftShoulder->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  leftShoulder->transform->SetParent(zombie->transform);
  leftShoulder->transform->SetWorldScale({0.1, 0.1, 0.1});
  Entity *rightShoulder{AddEntity("RightShoulder")};
  rightShoulder->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  rightShoulder->transform->SetParent(zombie->transform);
  rightShoulder->transform->SetWorldScale({0.1, 0.1, 0.1});
  Entity *leftHand{AddEntity("LeftHand")};
  leftHand->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  leftHand->transform->SetParent(zombie->transform);
  leftHand->transform->SetWorldScale({0.1, 0.1, 0.1});
  Entity *rightHand{AddEntity("RightHand")};
  rightHand->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  rightHand->transform->SetParent(zombie->transform);
  rightHand->transform->SetWorldScale({0.1, 0.1, 0.1});
  Entity *leftFoot{AddEntity("LeftFoot")};
  leftFoot->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  leftFoot->transform->SetParent(zombie->transform);
  leftFoot->transform->SetWorldScale({0.1, 0.1, 0.1});
  Entity *rightFoot{AddEntity("RightFoot")};
  rightFoot->AddComponent<MeshComponent>("primitive/cube.scene.xml");
  rightFoot->transform->SetParent(zombie->transform);
  rightFoot->transform->SetWorldScale({0.1, 0.1, 0.1});

  Events::Instance().RegisterEventListener(
      "CustomUpdate", [=](const EventObject &object) {
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("Head");
          head->transform->SetWorldPos(pos);
          head->transform->SetWorldRot(rot);
        }
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("Hips");
          hips->transform->SetWorldPos(pos);
          hips->transform->SetWorldRot(rot);
        }
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("LeftShoulder");
          leftShoulder->transform->SetWorldPos(pos);
          leftShoulder->transform->SetWorldRot(rot);
        }
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("RightShoulder");
          rightShoulder->transform->SetWorldPos(pos);
          rightShoulder->transform->SetWorldRot(rot);
        }
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("LeftHand");
          leftHand->transform->SetWorldPos(pos);
          leftHand->transform->SetWorldRot(rot);
        }
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("RightHand");
          rightHand->transform->SetWorldPos(pos);
          rightHand->transform->SetWorldRot(rot);
        }
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("LeftFoot");
          leftFoot->transform->SetWorldPos(pos);
          leftFoot->transform->SetWorldRot(rot);
        }
        {
          auto [pos, rot] = mesh->GetJointWorldTransform("RightFoot");
          rightFoot->transform->SetWorldPos(pos);
          rightFoot->transform->SetWorldRot(rot);
        }

        Events::Instance().RaiseQueuedEvent(
            EventObject("CustomUpdate", Time::GetTimeFrame() + 1,
                        EventPriority::MEDIUM, {}));
      });
  Events::Instance().RaiseQueuedEvent(EventObject(
      "CustomUpdate", Time::GetTimeFrame() + 1, EventPriority::MEDIUM, {}));
  // cube->transform->SetLocalPos(2.f * Math::Vector3::up);
}
}  // namespace Isetta