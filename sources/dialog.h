#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <memory>
// ダイアログリザルト
enum class DialogResult
{
	Yes,
	No,
	OK,
	Cancel
};

//// ダイアログ
//class Dialog
//{
//public:
//	// [ファイルを開く]ダイアログボックスを表示
//	static DialogResult OpenFileName(char* filepath, int size, const char* filter = nullptr, const char* title = nullptr, HWND hWnd = NULL, bool multiSelect = false);
//	static DialogResult OpenFileName(TCHAR* filepath, int size, const char* filter = nullptr, const char* title = nullptr, HWND hWnd = NULL, bool multiSelect = false);
//
	// [ファイルを保存]ダイアログボックスを表示
//	static DialogResult SaveFileName(char* filepath, int size, const char* filter = nullptr, const char* title = nullptr, const char* ext = nullptr, HWND hWnd = NULL);
//};
std::unique_ptr<wchar_t, decltype(CoTaskMemFree)*> file_select(HWND hwndOwner)
{
	std::unique_ptr<wchar_t, decltype(CoTaskMemFree)*> retval{ nullptr, CoTaskMemFree };
	IFileDialog* dlg;
	auto hr = CoCreateInstance(__uuidof(FileOpenDialog), nullptr, CLSCTX_INPROC_SERVER, __uuidof(dlg), (void**)&dlg);
	if (SUCCEEDED(hr)) {
		if (SUCCEEDED(hr = dlg->SetOptions(NULL))) {
			if ((hr = dlg->Show(hwndOwner)) == S_OK) {
				IShellItem* item;
				if (SUCCEEDED(hr = dlg->GetResult(&item))) {
					LPWSTR pszName;
					if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &pszName)))
						retval.reset(pszName);
				}
			}
		}
		dlg->Release();
	}
	return retval;
}
