#include "common.h"
#include <imm.h>
#include <Windows.h>

// 将当前程序的输入法设置为英文状态
void SetInputMethod(HWND hwnd, const wchar_t* method) {
    HIMC hImc = ImmGetContext(hwnd);
    if (hImc) {
        // 获取当前输入法状态
        DWORD dwConversion, dwSentence;
        ImmGetConversionStatus(hImc, &dwConversion, &dwSentence);

        // 清除所有输入法相关标志，强制设置为英文模式
        dwConversion &= ~(IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_CHARCODE);
        dwConversion |= IME_CMODE_ALPHANUMERIC;  // 设置字母数字模式

        // 应用新的输入法状态
        ImmSetConversionStatus(hImc, dwConversion, dwSentence);

        // 将输入法上下文与当前窗口关联
        ImmAssociateContextEx(hwnd, hImc, IACE_DEFAULT);

        ImmReleaseContext(hwnd, hImc);
    }
}