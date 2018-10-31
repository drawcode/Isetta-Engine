/*
 * Copyright (c) 2018 Isetta
 */
#include "Components/Console.h"

#include <utility>
#include "imgui/imgui.h"

#include "Core/Config/Config.h"
#include "Core/Debug/Logger.h"
#include "Core/Math/Vector2.h"
#include "Core/Time/Time.h"
#include "Graphics/GUI.h"
#include "Input/KeyCode.h"
#include "Util.h"

namespace Isetta {
std::vector<std::string_view> Console::cfgCmds;
std::unordered_map<std::string_view, Action<Console* const, std::string_view>>
    Console::userCmds;
std::list<Console*> Console::consolesOpen;

Console::Console(std::string title, bool isOpen)
    : title{title}, isOpen{isOpen} {
  cfgCmds = Config::Instance().GetCommands();
  AddCommand("commands", [](Console* const console, std::string_view) {
    console->AddLog("CMD\\> Config Commands:\n");
    for (const auto& cmd : cfgCmds)
      console->AddLog("- " + std::string{cmd.data()} + "\n");
    console->AddLog("CMD\\> User Commands:\n");
    for (const auto& cmd : userCmds)
      console->AddLog("- " + std::string{cmd.first.data()} + "\n");
  });

  verbosityColor[static_cast<int>(Debug::Verbosity::Info)] = Color::white;
  verbosityColor[static_cast<int>(Debug::Verbosity::Warning)] = Color::orange;
  verbosityColor[static_cast<int>(Debug::Verbosity::Error)] = Color::red;
}

int Console::CommandCallbackStub(InputTextCallbackData* data) {
  Console* console = static_cast<Console*>(data->UserData);
  return console->CommandCallback(data);
}
int Console::CommandCallback(InputTextCallbackData* data) {
  switch ((GUI::InputTextFlags)data->EventFlag) {
    case GUI::InputTextFlags::CallbackCompletion: {
      const char* wordEnd = data->Buf + data->CursorPos;
      const char* wordStart = wordEnd;
      while (wordStart > data->Buf) {
        const char c = wordStart[-1];
        if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
        wordStart--;
      }

      int len = wordEnd - wordStart;
      std::string_view word{wordStart, (size_t)len};
      std::vector<std::string_view> cfgCandidates, userCandidates;
      for (const auto& cmd : cfgCmds) {
        if (cmd.compare(0, len, word) == 0) cfgCandidates.push_back(cmd);
      }
      for (const auto& cmd : userCmds) {
        if (cmd.first.compare(0, len, word) == 0)
          userCandidates.push_back(cmd.first);
      }
      if (cfgCandidates.size() + userCandidates.size() == 0) {
        AddLog(Util::StrFormat("CMD\\> No match for \"%.*s\"!\n",
                               static_cast<int>(wordEnd - wordStart),
                               wordStart));
      } else if (cfgCandidates.size() == 1) {
        data->DeleteChars(static_cast<int>(wordStart - data->Buf),
                          static_cast<int>(wordEnd - wordStart));
        data->InsertChars(data->CursorPos, cfgCandidates[0].data());
        data->InsertChars(data->CursorPos, "=");
      } else if (userCandidates.size() == 1) {
        data->DeleteChars(static_cast<int>(wordStart - data->Buf),
                          static_cast<int>(wordEnd - wordStart));
        data->InsertChars(data->CursorPos, userCandidates[0].data());
        data->InsertChars(data->CursorPos, "|");
      } else {
        int matchLen = len;
        if (cfgCandidates.size() > 0)
          word = cfgCandidates[0];
        else if (userCandidates.size() > 0)
          word = userCandidates[0];
        for (;;) {
          int c = 0;
          bool allMatch = true;
          std::string_view word;
          c = word[matchLen];
          for (const auto& cmd : cfgCandidates) {
            if (c == 0 || c != cmd[matchLen]) {
              allMatch = false;
              break;
            }
          }
          if (!allMatch) break;
          for (const auto& cmd : userCandidates) {
            if (c == 0 || c != cmd[matchLen]) {
              allMatch = false;
              break;
            }
          }
          if (!allMatch) break;
          matchLen++;
        }
        if (matchLen > 0) {
          data->DeleteChars(static_cast<int>(wordStart - data->Buf),
                            static_cast<int>(wordEnd - wordStart));
          data->InsertChars(data->CursorPos, word.data(),
                            word.data() + matchLen);
          AddLog("CMD\\> Config Commands:\n");
          for (const auto& cmd : cfgCandidates)
            AddLog("- " + std::string{cmd.data()} + "\n");
          AddLog("CMD\\> User Commands:\n");
          for (const auto& cmd : userCandidates)
            AddLog("- " + std::string{cmd.data()} + "\n");
        }
      }
      break;
    }
    case GUI::InputTextFlags::CallbackHistory: {
      const int prevHistoryPos = historyPos;
      if (data->EventKey == ImGuiKey_UpArrow) {
        if (historyPos == -1)
          historyPos = history.size() - 1;
        else if (historyPos > 0)
          historyPos--;
      } else if (data->EventKey == ImGuiKey_DownArrow) {
        if (historyPos != -1)
          if (++historyPos >= history.size()) historyPos = -1;
      }

      if (prevHistoryPos != historyPos) {
        std::string historyStr = (historyPos >= 0) ? history[historyPos] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, historyStr.c_str());
      }

      break;
    }
  }
  return 0;
}

void Console::OnEnable() {
  Logger::outputCallback =
      std::bind(&Console::AddLog, this, std::placeholders::_1);
  consolesOpen.push_back(this);
  LOG_INFO("info");
  LOG_WARNING("warning");
  LOG_INFO(Debug::Channel::Gameplay, "info");
  LOG_INFO(Debug::Channel::Memory, "info");
}
void Console::OnDisable() {
  consolesOpen.remove(this);
  if (consolesOpen.size() == 0) Logger::outputCallback = NULL;
}
void Console::GuiUpdate() {
  GUI::Window(
      rectTransform, title,
      [&]() {
        float width = 60, height = 20, padding = 5;
        RectTransform rect{{padding, 10, width, height}};
        if (GUI::Button(rect, "Clear")) Clear();
        rect.rect.x += width + padding;
        bool copy = GUI::Button(rect, "Copy");
        rect.rect.x += width + padding;

        static bool displayVerbosity[static_cast<int>(Debug::Verbosity::All)];
        for (int verbosity = static_cast<int>(Debug::Verbosity::All) - 1;
             verbosity > static_cast<int>(Debug::Verbosity::Off); verbosity--) {
          displayVerbosity[verbosity] =
              Logger::verbosityMask.test(verbosity - 1);
          std::string label =
              Debug::ToString(static_cast<Debug::Verbosity>(verbosity));
          GUI::Toggle(rect, label, &displayVerbosity[verbosity]);
          Logger::verbosityMask.set(verbosity - 1, displayVerbosity[verbosity]);

          if (verbosity == static_cast<int>(Debug::Verbosity::All) - 1)
            displayVerbosity[0] = true;
          else
            displayVerbosity[0] =
                displayVerbosity[0] &&
                displayVerbosity[verbosity] == displayVerbosity[verbosity + 1];

          rect.rect.x +=
              padding + 0.4f * width + ImGui::CalcTextSize(label.c_str()).x;
        }
        if (displayVerbosity[0]) {
          displayVerbosity[0] = displayVerbosity[1];
          Logger::verbosityMask = displayVerbosity[0] ? ~0 : 0;
        }

        static bool displayChannel[static_cast<int>(Debug::Channel::All) + 1];
        int allChannel = static_cast<int>(Debug::Channel::All);
        // ALL
        displayChannel[allChannel] = Logger::channelMask.all();

        // OTHERS
        displayChannel[allChannel] = true;
        for (int channel = 0; channel < static_cast<int>(Debug::Channel::All);
             channel++) {
          displayChannel[channel] = Logger::channelMask.test(channel);
        }
        GUI::ButtonDropDown(rect, "Channel", Math::Vector2{30, 20}, [&]() {
          RectTransform rect{{10, 10, 20, 20}};

          // ALL
          displayChannel[allChannel] = Logger::channelMask.all();
          std::string label = Debug::ToString(Debug::Channel::All);
          GUI::Toggle(rect, label, &displayChannel[allChannel]);
          if (displayChannel[allChannel]) Logger::channelMask = ~0;
          rect.rect.y += height + padding;

          // OTHERS
          displayChannel[allChannel] = true;
          for (int channel = 0; channel < static_cast<int>(Debug::Channel::All);
               channel++) {
            displayChannel[channel] = Logger::channelMask.test(channel);
            std::string label =
                Debug::ToString(static_cast<Debug::Channel>(channel));
            GUI::Toggle(rect, label, &displayChannel[channel]);
            Logger::channelMask.set(channel, displayChannel[channel]);

            displayChannel[allChannel] =
                displayChannel[allChannel] && displayChannel[channel];

            rect.rect.y += height + padding;
          }
          if (displayChannel[allChannel]) Logger::channelMask = ~0;
        });

        rect.anchor = GUI::Pivot::TopRight;
        rect.rect.x = -2.5 * width - 2 * padding;
        GUI::Toggle(rect, "Scroll To Bottom", &scrollToBottom);
        ImGui::Separator();
        rect.anchor = GUI::Pivot::TopLeft;
        rect.rect.SetPosition(padding, rect.rect.y + height + padding);

        const float footerHeightReserve = ImGui::GetStyle().ItemSpacing.y +
                                          ImGui::GetFrameHeightWithSpacing();
        rect.rect.SetSize(0, -footerHeightReserve);
        RectTransform lineRect{{padding, padding, height, width}};
        GUI::Child(
            rect, "scrolling",
            [&]() {
              if (copy) ImGui::LogToClipboard();

              // TextBuffer buf;
              for (const auto& line : log) {
                int textVerbosity = -1, textChannel = -1;
                for (int verbosity =
                         static_cast<int>(Debug::Verbosity::Off) + 1;
                     verbosity < static_cast<int>(Debug::Verbosity::All);
                     verbosity++) {
                  if (displayVerbosity[verbosity] &&
                      line.find("[" +
                                Debug::ToString(
                                    static_cast<Debug::Verbosity>(verbosity)) +
                                "]") != std::string::npos) {
                    textVerbosity = verbosity;
                    break;
                  }
                }
                for (int channel = 0;
                     channel < static_cast<int>(Debug::Channel::All);
                     channel++) {
                  if (displayChannel[channel] &&
                      line.find("[" +
                                Debug::ToString(
                                    static_cast<Debug::Channel>(channel)) +
                                "]") != std::string::npos) {
                    textChannel = channel;
                    break;
                  }
                }
                if ((displayVerbosity[0] || textVerbosity != -1) &&
                    (displayChannel[allChannel] || textChannel != -1)) {
                  GUI::Text(lineRect, line.data(),
                            GUI::TextStyle{verbosityColor[textVerbosity]});
                  lineRect.rect.y += height;
                } else if (textVerbosity == -1 && textChannel == -1) {
                  GUI::Text(lineRect, line.data(),
                            GUI::TextStyle{defaultTextColor});
                  lineRect.rect.y += height;
                }
              }
              if (scrollToBottom) ImGui::SetScrollHere(1.0f);
            },
            false, GUI::WindowFlags::HorizontalScrollbar);
        ImGui::Separator();
        rect.anchor = GUI::Pivot::BotLeft;
        rect.rect.SetPosition(padding, -5 * padding);
        rect.rect.SetSize(4 * width, height);
        static char command[256];
        if (GUI::InputText(rect, "Command", command, 256, {},
                           GUI::InputTextFlags::EnterReturnsTrue |
                               GUI::InputTextFlags::CallbackCompletion |
                               GUI::InputTextFlags::CallbackHistory,
                           &CommandCallbackStub, static_cast<void*>(this))) {
          std::string cmdStr{command};
          history.push_back(cmdStr);
          historyPos = -1;
          AddLog("CMD\\> " + cmdStr);
          // TODO(Jacob) this signifies a user command?
          int userPos = cmdStr.find(USER_DELIM);
          if (userPos != std::string::npos) {
            std::string key = cmdStr.substr(0, userPos);
            std::string_view value =
                cmdStr.substr(userPos + 1, cmdStr.length());
            auto it = userCmds.find(key);
            if (it != userCmds.end()) it->second(this, value);
          } else {
            int equalPos = cmdStr.find(CFG_DELIM);
            if (equalPos != std::string::npos) {
              std::string key = cmdStr.substr(0, equalPos);
              std::string_view value =
                  cmdStr.substr(equalPos + 1, cmdStr.length());
              Config::Instance().SetVal(key, value.data());
            }
            lineRect.rect.y += height;
          }
          *command = '\0';
        }
      },
      &isOpen, GUI::WindowStyle{Math::Rect{600, 150, 1000, 900}});
}  // namespace Isetta
void Console::Clear() { log.clear(); }
void Console::AddLog(const std::string_view& format) {
  log.push_back(format.data());
  // scrollToBottom = true;
}
void Console::AddCommand(
    const std::string_view& cmd,
    const Action<Console* const, std::string_view>& callback) {
  userCmds.insert(std::make_pair(cmd, callback));
}
}  // namespace Isetta