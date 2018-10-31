/*
 * Copyright (c) 2018 Isetta
 */
#pragma once
#include <queue>
#include <vector>
#include "Core/IsettaAlias.h"
#include "EventObject.h"
#include "SID/sid.h"

namespace Isetta {
using CallbackPair = std::pair<U16, Action<EventObject>>;
class ISETTA_API Events {
 public:
  static Events& Instance() {
    static Events instance{};
    return instance;
  }

  void RaiseQueuedEvent(const EventObject& eventObject);
  void RaiseImmediateEvent(const EventObject& eventObject);

  U16 RegisterEventListener(std::string_view eventName,
                            const Action<EventObject>& callback);
  void UnregisterEventListener(std::string_view eventName,
                               U16 eventListenerHandle);

 private:
  Events() = default;
  std::priority_queue<EventObject, std::vector<EventObject>,
                      std::greater<EventObject>>
      eventQueue;
  std::unordered_map<StringId, std::vector<CallbackPair>> callbackMap;

  void Update();

  static U16 totalListeners;

  friend class EngineLoop;
};
}  // namespace Isetta