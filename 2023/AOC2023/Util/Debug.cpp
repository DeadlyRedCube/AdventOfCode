#include "Util/Core.h"

#include <stdio.h>
#include <stdarg.h>

#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace NS_Debug
{
  DialogResult AssertTaskDialog(const wchar_t *title, const char *file, int line, bool allButtons, const char *message)
  {
    FixedArray<wchar_t, 2048> content;
    FixedArray<wchar_t, 2048> messageBuffer;
    const char *fileNoDir = strrchr(file, '\\');
    if (fileNoDir == nullptr)
    {
      fileNoDir = file;
    }
    swprintf_s(content.Elements(), content.Count(), L"Failure detected in \"%S\", line %d", fileNoDir, line);

    TASKDIALOGCONFIG config;
    ZeroMemory(&config, sizeof(config));
    config.cbSize = sizeof(config);
    config.hwndParent = NULL;

    config.pszWindowTitle = title;
    config.pszContent = content.Elements();
    config.dwFlags = TDF_EXPANDED_BY_DEFAULT;

    swprintf_s(messageBuffer.Elements(), messageBuffer.Count(), L"%S", message);

    config.pszExpandedInformation = messageBuffer.Elements();

    TASKDIALOG_BUTTON buttons[3];
    buttons[0].nButtonID = u32(DialogResult::Break);
    buttons[0].pszButtonText = L"Break";
    buttons[1].nButtonID = u32(DialogResult::Ignore);
    buttons[1].pszButtonText = L"Ignore";
    buttons[2].nButtonID = u32(DialogResult::IgnoreAlways);
    buttons[2].pszButtonText = L"Ignore Always";

    if (allButtons)
    {
      config.cButtons = 3;
      config.pszMainIcon = TD_WARNING_ICON;
    }
    else
    {
      config.cButtons = 1;
      config.pszMainIcon = TD_ERROR_ICON;
    }

    config.pButtons = buttons;


    config.nDefaultButton = 2;

    int result, radioResult;
    BOOL verif;
    TaskDialogIndirect(&config, &result, &radioResult, &verif);

    return DialogResult(result);
  }

  void Trace(const char * message)
  {
	  OutputDebugStringA(message);
  }
}
