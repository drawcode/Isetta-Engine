/*
 * Copyright (c) 2018 Isetta
 */
#include "AudioSource.h"

#include "AudioClip.h"
#include "AudioModule.h"
#include "FMOD/inc/fmod.hpp"
#include "Scene/Transform.h"

namespace Isetta {
AudioModule* AudioSource::audioModule;

AudioSource::AudioSource()
    : properties{0b100}, volume{1.f}, speed{1.f}, loopCount{-1} {}

AudioSource::AudioSource(std::string_view clipName)
    : properties{0b100}, volume{1.f}, speed{1.f}, loopCount{-1} {
  if (!clipName.empty()) clip = AudioClip::GetClip(clipName);
}

AudioSource::AudioSource(const std::bitset<3>& properties,
                         std::string_view clipName)
    : properties{properties}, volume{1.f}, speed{1.f}, loopCount{-1} {
  if (!clipName.empty()) clip = AudioClip::GetClip(clipName);
}

void AudioSource::Update() {
  if (fmodChannel && IsPlaying() && GetProperty(Property::IS_3D)) {
    const Math::Vector3 position = transform->GetWorldPos();
    const FMOD_VECTOR fmodPosition{position.x, position.y, position.z};
    fmodChannel->set3DAttributes(&fmodPosition, nullptr);
  }
}

void AudioSource::SetProperty(const Property prop, bool value) {
  switch (prop) {
    case Property::IS_3D:
      properties.set(static_cast<int>(Property::IS_3D), value);
      if (clip) {
        if (value)
          clip->fmodSound->setMode(FMOD_3D);
        else
          clip->fmodSound->setMode(FMOD_2D);
      }
      break;
    case Property::LOOP:
      properties.set(static_cast<int>(Property::LOOP), value);
      break;
    case Property::IS_MUTE:
      properties.set(static_cast<int>(Property::IS_MUTE), value);
      if (fmodChannel) AudioModule::CheckStatus(fmodChannel->setMute(value));
      break;
  };
}

bool AudioSource::GetProperty(const Property prop) const {
  switch (prop) {
    case Property::IS_3D:
      return properties.test(static_cast<int>(Property::IS_3D));
    case Property::LOOP:
      return properties.test(static_cast<int>(Property::LOOP));
    case Property::IS_MUTE:
      return properties.test(static_cast<int>(Property::IS_MUTE));
  };
}

void AudioSource::SetAudioClip(const std::string_view audioClip) {
  this->clip = AudioClip::GetClip(audioClip);
}

void AudioSource::Play() {
  if (isSoundValid()) {
    audioModule->Play(this);
  }
}

void AudioSource::Pause() const {
  if (isChannelValid()) {
    AudioModule::CheckStatus(fmodChannel->setPaused(true));
  }
}

void AudioSource::Continue() const {
  if (isChannelValid()) {
    AudioModule::CheckStatus(fmodChannel->setPaused(false));
  }
}

void AudioSource::Stop() const {
  if (isChannelValid() && IsPlaying()) {
    AudioModule::CheckStatus(fmodChannel->stop());
  }
}

void AudioSource::SetVolume(const float inVolume) {
  volume = Math::Util::Clamp01(inVolume);
  if (fmodChannel) AudioModule::CheckStatus(fmodChannel->setVolume(volume));
}

void AudioSource::Mute(bool mute) {
  SetProperty(Property::IS_MUTE, mute);
  if (fmodChannel) AudioModule::CheckStatus(fmodChannel->setMute(mute));
}

// void AudioSource::SetSpeed(const float inSpeed) {
//  speed = inSpeed;
//  if (clip) AudioModule::CheckStatus(clip->fmodSound->setMusicSpeed(speed));
//}
// float AudioSource::GetSpeed() const { return speed; }

bool AudioSource::IsPlaying() const {
  if (fmodChannel) {
    bool isPlaying, isPaused;
    audioModule->CheckStatus(fmodChannel->isPlaying(&isPlaying));
    audioModule->CheckStatus(fmodChannel->getPaused(&isPaused));
    return isPlaying && isPaused;
  }
  return false;
}

bool AudioSource::IsPaused() const {
  if (fmodChannel) {
    bool isPaused;
    audioModule->CheckStatus(fmodChannel->getPaused(&isPaused));
    return isPaused;
  }
  return false;
}

bool AudioSource::IsStarted() const {
  if (fmodChannel) {
    bool isPlaying;
    audioModule->CheckStatus(fmodChannel->isPlaying(&isPlaying));
    return isPlaying;
  }
  return false;
}

void AudioSource::LoopFor(int count) {
  loopCount = count;
  SetProperty(Property::LOOP, count > 0);
  if (clip->fmodSound)
    AudioModule::CheckStatus(clip->fmodSound->setLoopCount(count));
}

bool AudioSource::isChannelValid() const {
  if (!fmodChannel) {
    throw std::exception{
        "AudioSource::isChannelValid => There is no sound playing on this "
        "AudioSource"};
  }

  return true;
}

bool AudioSource::isSoundValid() const {
  if (!clip) {
    throw std::exception{
        "AudioSource::isSoundValid => Audio clip for this audio source is not "
        "found!"};
  }

  return true;
}
}  // namespace Isetta
