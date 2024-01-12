#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <memory>
// �_�C�A���O���U���g
enum class DialogResult
{
	Yes,
	No,
	OK,
	Cancel
};

//// �_�C�A���O
//class Dialog
//{
//public:
//	// [�t�@�C�����J��]�_�C�A���O�{�b�N�X��\��
//	static DialogResult OpenFileName(char* filepath, int size, const char* filter = nullptr, const char* title = nullptr, HWND hWnd = NULL, bool multiSelect = false);
//	static DialogResult OpenFileName(TCHAR* filepath, int size, const char* filter = nullptr, const char* title = nullptr, HWND hWnd = NULL, bool multiSelect = false);
//
	// [�t�@�C����ۑ�]�_�C�A���O�{�b�N�X��\��
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
