/*
 * (C) 2025- TeraTerm Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* ui property page */

#include <stdio.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <assert.h>

#include "tttypes.h"
#include "dlglib.h"
#include "compat_win.h"
#include "helpid.h"
#include "asprintf.h"
#include "win32helper.h"
#include "tipwin2.h"

#include "ui_pp.h"
#include "ui_pp_res.h"

typedef struct {
	wchar_t* fullname;
	wchar_t* filename;
	wchar_t* language;
	wchar_t* date;
	wchar_t* contributor;
} LangInfo;

static const wchar_t* get_lang_folder()
{
	return (IsWindowsNTKernel()) ? L"lang_utf16le" : L"lang";
}

static void LangFree(LangInfo* infos, size_t count)
{
	size_t i;
	for (i = 0; i < count; i++) {
		LangInfo* p = infos + i;
		free(p->filename);
		free(p->fullname);
		free(p->language);
		free(p->date);
		free(p->contributor);
	}
	free(infos);
}

/**
 *	�t�@�C���������X�g����(�t�@�C�����̂�)
 *	infos�ɒǉ�����return����
 */
static LangInfo* LangAppendFileList(const wchar_t* folder, LangInfo* infos, size_t* infos_size)
{
	wchar_t* fullpath;
	HANDLE hFind;
	WIN32_FIND_DATAW fd;
	size_t count = *infos_size;

	aswprintf(&fullpath, L"%s\\*.lng", folder);
	hFind = FindFirstFileW(fullpath, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				LangInfo* p = (LangInfo*)realloc(infos, sizeof(LangInfo) * (count + 1));
				if (p != NULL) {
					infos = p;
					p = infos + count;
					count++;
					memset(p, 0, sizeof(*p));
					p->filename = _wcsdup(fd.cFileName);
					aswprintf(&p->fullname, L"%s\\%s", folder, fd.cFileName);
				}
			}
		} while (FindNextFileW(hFind, &fd));
		FindClose(hFind);
	}
	free(fullpath);

	*infos_size = count;
	return infos;
}

/**
 *	lng�t�@�C���� Info �Z�N�V������ǂݍ���
 */
static void LangRead(LangInfo* infos, size_t infos_size)
{
	size_t i;

	for (i = 0; i < infos_size; i++) {
		LangInfo* p = infos + i;
		const wchar_t* lng = p->fullname;
		wchar_t* s;
		hGetPrivateProfileStringW(L"Info", L"language", NULL, lng, &s);
		if (s[0] == 0) {
			free(s);
			p->language = _wcsdup(p->filename);
		}
		else {
			p->language = s;
		}
		hGetPrivateProfileStringW(L"Info", L"date", NULL, lng, &s);
		if (s[0] == 0) {
			free(s);
			p->date = _wcsdup(L"-");
		}
		else {
			p->date = s;
		}
		hGetPrivateProfileStringW(L"Info", L"contributor", NULL, lng, &s);
		if (s[0] == 0) {
			free(s);
			p->contributor = _wcsdup(L"-");
		}
		else {
			p->contributor = s;
		}
	}
}

static wchar_t *LangInfoText(const LangInfo *p)
{
	wchar_t *info;
	aswprintf(&info,
			  L"language\r\n"
			  L"  %s\r\n"
			  L"filename\r\n"
			  L"  %s\r\n"
			  L"date\r\n"
			  L"  %s\r\n"
			  L"contributor\r\n"
			  L"  %s",
			  p->language, p->filename, p->date, p->contributor);
	return info;
}

struct UIPPData {
	TTTSet *pts;
	const wchar_t *UILanguageFileW;
	DLGTEMPLATE *dlg_templ;
	TComVar *pcv;
	LangInfo* lng_infos;
	size_t lng_size;
	size_t selected_lang;	// �I�΂�Ă���lng�t�@�C���ԍ�
	TipWin2 *tipwin2;
	HWND hVTWin;
	HINSTANCE hInst;
};

static INT_PTR CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static const DlgTextInfo TextInfos[] = {
		{ IDC_GENUILANG_LABEL, "DLG_GEN_LANG_UI" },
	};

	switch (msg) {
		case WM_INITDIALOG: {
			UIPPData *data = (UIPPData *)(((PROPSHEETPAGEW_V1 *)lp)->lParam);
			TTTSet *pts = data->pts;

			SetWindowLongPtrW(hWnd, DWLP_USER, (LONG_PTR)data);

			SetDlgTextsW(hWnd, TextInfos, _countof(TextInfos), data->pts->UILanguageFileW);

			// UI Language, �ǂݍ���
			LangInfo* infos = NULL;
			size_t infos_size = 0;
			wchar_t* folder;
			aswprintf(&folder, L"%s\\%s", pts->ExeDirW, get_lang_folder());
			infos = LangAppendFileList(folder, infos, &infos_size);
			free(folder);
			if (wcscmp(pts->ExeDirW, pts->HomeDirW) != 0) {
				aswprintf(&folder, L"%s\\%s", pts->HomeDirW, get_lang_folder());
				infos = LangAppendFileList(folder, infos, &infos_size);
				free(folder);
			}
			LangRead(infos, infos_size);
			data->lng_infos = infos;
			data->lng_size = infos_size;

			// UI Language�p tipwin
			data->tipwin2 = TipWin2Create(data->hInst, hWnd);

			// UI Language, �I��
			data->selected_lang = 0;
			for (size_t i = 0; i < infos_size; i++) {
				const LangInfo* p = infos + i;
				SendDlgItemMessageW(hWnd, IDC_GENUILANG, CB_ADDSTRING, 0, (LPARAM)p->language);
				if (wcscmp(p->fullname, pts->UILanguageFileW) == 0) {
					data->selected_lang = i;
					wchar_t *info_text = LangInfoText(p);
					TipWin2SetTextW(data->tipwin2, IDC_GENUILANG, info_text);
					free(info_text);
				}
			}
			SendDlgItemMessageW(hWnd, IDC_GENUILANG, CB_SETCURSEL, data->selected_lang, 0);


			return TRUE;
		}
		case WM_DESTROY: {
			UIPPData *data = (UIPPData *)GetWindowLongPtrW(hWnd, DWLP_USER);
			LangFree(data->lng_infos, data->lng_size);
			data->lng_infos = NULL;
			data->lng_size = 0;
			break;
		}
		case WM_NOTIFY: {
			NMHDR *nmhdr = (NMHDR *)lp;
			switch (nmhdr->code) {
				case PSN_APPLY: {
					UIPPData *data = (UIPPData *)GetWindowLongPtrW(hWnd, DWLP_USER);
					TTTSet *pts = data->pts;
					LRESULT w = SendDlgItemMessageA(hWnd, IDC_GENUILANG, CB_GETCURSEL, 0, 0);
					if (w != data->selected_lang) {
						const LangInfo* p = data->lng_infos + w;
						free(pts->UILanguageFileW);
						pts->UILanguageFileW = _wcsdup(p->fullname);

						// �^�C�g���̍X�V���s���B(2014.2.23 yutaka)
						PostMessage(data->hVTWin, WM_USER_CHANGETITLE, 0, 0);
					}

					// TTXKanjiMenu �� Language �����ă��j���[��\������̂ŁA�ύX�̉\��������
					// OK �������Ƀ��j���[�ĕ`��̃��b�Z�[�W���΂��悤�ɂ����B (2007.7.14 maya)
					// ����t�@�C���̕ύX���Ƀ��j���[�̍ĕ`�悪�K�v (2012.5.5 maya)
					PostMessage(data->hVTWin, WM_USER_CHANGEMENU, 0, 0);

					break;
				}
				case PSN_HELP: {
					HWND vtwin = GetParent(hWnd);
					vtwin = GetParent(vtwin);
					PostMessageA(vtwin, WM_USER_DLGHELP2, HlpMenuSetupAdditionalUI, 0);
					break;
				}
				default:
					break;
			}
			break;
		}
		case WM_COMMAND: {
			UIPPData *data = (UIPPData *)GetWindowLongPtrW(hWnd, DWLP_USER);
			switch (wp) {
				case IDC_GENUILANG | (CBN_SELCHANGE << 16): {
					size_t ui_sel = (size_t)SendDlgItemMessageA(hWnd, IDC_GENUILANG, CB_GETCURSEL, 0, 0);
					wchar_t *info_text = LangInfoText(data->lng_infos + ui_sel);
					TipWin2SetTextW(data->tipwin2, IDC_GENUILANG, info_text);
					free(info_text);
					break;
				}
				default:
					break;
			}
			break;
		}
		default:
			return FALSE;
	}
	return FALSE;
}

static UINT CALLBACK CallBack(HWND hwnd, UINT uMsg, struct _PROPSHEETPAGEW *ppsp)
{
	(void)hwnd;
	UINT ret_val = 0;
	switch (uMsg) {
	case PSPCB_CREATE:
		ret_val = 1;
		break;
	case PSPCB_RELEASE:
		free((void *)ppsp->pResource);
		ppsp->pResource = NULL;
		free((void *)ppsp->lParam);
		ppsp->lParam = 0;
		break;
	default:
		break;
	}
	return ret_val;
}

HPROPSHEETPAGE UIPageCreate(HINSTANCE inst, TTTSet *pts)
{
	// �� common/tt_res.h �� ui_pp_res.h �Œl����v�����邱��
	int id = IDD_TABSHEET_UI;

	UIPPData *Param = (UIPPData *)calloc(1, sizeof(UIPPData));
	Param->UILanguageFileW = pts->UILanguageFileW;
	Param->pts = pts;

	PROPSHEETPAGEW_V1 psp = {};
	psp.dwSize = sizeof(psp);
	psp.dwFlags = PSP_DEFAULT | PSP_USECALLBACK | PSP_USETITLE | PSP_HASHELP;
	psp.hInstance = inst;
	psp.pfnCallback = CallBack;
	wchar_t* UIMsg;
	GetI18nStrWW("Tera Term", "DLG_TABSHEET_TITLE_UI",
		         L"UI", pts->UILanguageFileW, &UIMsg);
	psp.pszTitle = UIMsg;
	psp.pszTemplate = MAKEINTRESOURCEW(id);
	psp.dwFlags |= PSP_DLGINDIRECT;
	Param->dlg_templ = TTGetDlgTemplate(inst, MAKEINTRESOURCEW(id));
	psp.pResource = Param->dlg_templ;

	psp.pfnDlgProc = Proc;
	psp.lParam = (LPARAM)Param;

	HPROPSHEETPAGE hpsp = CreatePropertySheetPageW((LPCPROPSHEETPAGEW)&psp);
	free(UIMsg);
	return hpsp;
}
