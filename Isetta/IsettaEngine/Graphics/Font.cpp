/*
 * Copyright (c) 2018 Isetta
 */
#include "Font.h"

#include "Core/Config/Config.h"
#include "Graphics/GUIModule.h"
#include "imgui_internal.h"

namespace Isetta {
GUIModule* Font::guiModule;
std::unordered_map<StringId,
                   std::pair<std::string, std::unordered_map<float, Font*>>>
    Font::fonts;

Font* Font::GetDefaultFont() {
  return reinterpret_cast<Font*>(ImGui::GetDefaultFont());
}
void Font::AddDefaultFont(const std::string_view& fontName, float size) {
  Font* font = GetFont(fontName, size);
  if (font)
    ImGui::GetIO().Fonts->AddFontDefault(
        reinterpret_cast<ImFont*>(font)->ConfigData);
  else
    LOG_WARNING(Debug::Channel::GUI,
                "Font::AddDefaultFont => Cannot find font by name: %s",
                fontName.data());
}
void Font::AddDefaultFont(Font* const font) {
  ImGui::GetIO().FontDefault = reinterpret_cast<ImFont*>(font);
}
Font* Font::GetFont(const std::string_view fontName, float size) {
  const auto fontList = fonts.find(SID(fontName.data()));
  if (fontList != fonts.end()) {
    Font* font;
    auto fontSizeMap = fontList->second.second;
    const auto find = fontSizeMap.find(size);
    if (find == fontSizeMap.end()) {
      std::string_view filepath = fontList->second.first;
      font = reinterpret_cast<Font*>(
          ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath.data(), size));
      fontSizeMap.insert({size, font});
    } else {
      font = find->second;
    }
    return font;
  }
  return nullptr;
}

void Font::AddFontFromFile(const std::string& filename,
                           std::initializer_list<float> fontSizes,
                           const std::string_view& fontName) {
  const auto fontList = fonts.find(SID(fontName.data()));
  const char* filepath =
      std::string{CONFIG_VAL(resourcePath) + "\\" + filename}.c_str();
  std::unordered_map<float, Font*> fontSizeMap;
  if (fontList == fonts.end()) {
    fonts.insert({SID(fontName.data()), {filepath, fontSizeMap}});
  } else {
    fontSizeMap = fontList->second.second;
  }
  for (const auto& size : fontSizes) {
    if (GetFont(fontName, size)) continue;
    auto font = reinterpret_cast<Font*>(
        ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath, size));
    fontSizeMap.insert({size, font});
  }
}

void Font::AddFontFromFile(const std::string& filename,
                           const std::string_view& fontName) {
  const auto fontList = fonts.find(SID(fontName.data()));
  const char* filepath =
      std::string{CONFIG_VAL(resourcePath) + "\\" + filename}.c_str();
  std::unordered_map<float, Font*> fontSizeMap;
  if (fontList == fonts.end()) {
    fonts.insert({SID(fontName.data()), {filepath, fontSizeMap}});
  }
}

Font* Font::AddFontFromFile(const std::string& filename, float fontSize,
                            const std::string_view& fontName) {
  const auto fontList = fonts.find(SID(fontName.data()));
  const char* filepath =
      std::string{CONFIG_VAL(resourcePath) + "\\" + filename}.c_str();
  Font* font;
  if (fontList == fonts.end()) {
    std::unordered_map<float, Font*> fontSizeMap;
    font = reinterpret_cast<Font*>(
        ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath, fontSize));
    fontSizeMap.insert({fontSize, font});
    fonts.insert({SID(fontName.data()), {filepath, fontSizeMap}});
  } else {
    auto& fontSizeMap = fontList->second.second;
    const auto findSize = fontSizeMap.find(fontSize);
    if (findSize == fontSizeMap.end()) {
      font = reinterpret_cast<Font*>(
          ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath, fontSize));
      fontSizeMap.insert({fontSize, font});
    } else {
      font = findSize->second;
    }
  }
  return font;
}

Font* Font::AddFontFromMemory(void* fontBuffer, float fontSize, float pixels,
                              const std::string_view& filename,
                              const std::string_view& fontName) {
  const auto fontList = fonts.find(SID(fontName.data()));
  Font* font;
  if (fontList == fonts.end()) {
    const std::string filepath =
        CONFIG_VAL(resourcePath) + "\\" + filename.data();
    std::unordered_map<float, Font*> fontSizeMap;
    font = reinterpret_cast<Font*>(ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
        fontBuffer, fontSize, pixels));
    fontSizeMap.insert({fontSize, font});
    fonts.insert({SID(fontName.data()), {filepath, fontSizeMap}});
  } else {
    auto& fontSizeMap = fontList->second.second;
    const auto findSize = fontSizeMap.find(fontSize);
    if (findSize == fontSizeMap.end()) {
      font = reinterpret_cast<Font*>(ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
          fontBuffer, fontSize, pixels));
      fontSizeMap.insert({fontSize, font});
    } else {
      font = findSize->second;
    }
  }
  return font;
}
void Font::PushFont(const std::string_view fontName, float fontSize) {
  Font* font = GetFont(fontName, fontSize);
  if (font) ImGui::PushFont(reinterpret_cast<ImFont*>(font));
}
void Font::PushFont(Font* const font) {
  ImGui::PushFont(reinterpret_cast<ImFont*>(font));
}
void Font::PopFont() { ImGui::PopFont(); }

}  // namespace Isetta