#include "stdafx.h"
// clang-format off
#include <windows.h>
// clang-format on
#include <imm.h>
#include <iostream>
#include <string>

// 补齐系统宏
#ifndef IMC_GETOPENSTATUS
#define IMC_GETOPENSTATUS 0x0005
#endif

#ifndef IMC_SETOPENSTATUS
#define IMC_SETOPENSTATUS 0x0006
#endif

int main(int argc, const char *argv[]) {
  HWND hwnd = GetForegroundWindow();
  if (!hwnd)
    return 0;

  DWORD threadId = GetWindowThreadProcessId(hwnd, NULL);

  if (argc == 1) {
    // 获取当前系统键盘布局 (LCID，例如英文是 1033，中文是 2052)
    HKL hkl = GetKeyboardLayout(threadId);
    unsigned int layoutId = (unsigned int)(uintptr_t)hkl & 0x0000FFFF;
    std::cout << layoutId;

    // 获取当前输入法的内部中英状态
    HWND hImeWnd = ImmGetDefaultIMEWnd(hwnd);
    if (hImeWnd) {
      LRESULT status =
          SendMessage(hImeWnd, WM_IME_CONTROL, IMC_GETOPENSTATUS, 0);
      // 输出复合状态，例如 "2052:1" (中文布局，中文模式) 或 "2052:0"
      // (中文布局，英文模式)
      std::cout << ":" << status;
    }
    std::cout << std::endl;
    return 0;
  } else if (argc == 2) {
    std::string arg = argv[1];
    std::string layoutStr = arg;
    std::string statusStr = "";

    // 解析传入的复合状态码 (以 ":" 为分隔符)
    size_t colonPos = arg.find(':');
    if (colonPos != std::string::npos) {
      layoutStr = arg.substr(0, colonPos);
      statusStr = arg.substr(colonPos + 1);
    }

    // 1. 恢复键盘布局
    // 使用 SendMessage 确保同步执行，以便后续状态设置生效
    unsigned int targetLayoutId = std::stoul(layoutStr);
    SendMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0,
                (LPARAM)(uintptr_t)targetLayoutId);

    // 2. 如果之前保存了具体的输入法状态，则进一步恢复该状态
    if (!statusStr.empty()) {
      // 给 Windows 20毫秒的喘息时间来完成底层 Layout 上下文的加载
      // 如果不加这个极短的延迟，立刻设置新加载输入法的状态可能会被系统忽略
      Sleep(20);
      HWND hImeWnd = ImmGetDefaultIMEWnd(hwnd);
      if (hImeWnd) {
        LRESULT targetStatus = std::stoi(statusStr);
        SendMessage(hImeWnd, WM_IME_CONTROL, IMC_SETOPENSTATUS, targetStatus);
      }
    }
    return 0;
  }
  return 0;
}
