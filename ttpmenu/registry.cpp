/*
 * Copyright (C) S.Hayakawa NTT-IT 1998-2002
 * (C) 2002- TeraTerm Project
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

//�ۑ����ini�t�@�C�����g�p�������ꍇ�́A0�o�C�g�̃t�@�C���ł悢�̂�ttpmenu.exe�Ɠ����t�H���_��ttpmenu.ini��p�ӂ���
#define		STRICT
#include	"registry.h"
#include	"stdio.h"
#include	"wchar.h"

static BOOL bUseINI = FALSE;					// �ۑ���(TRUE=INI, FALSE=���W�X�g��)
static wchar_t szSectionName[MAX_PATH];			// INI�̃Z�N�V������
static wchar_t szSectionNames[1024*10]={0};	// INI�̃Z�N�V�������ꗗ
static wchar_t szApplicationName[MAX_PATH]={0};	// INI�t�@�C���̃t���p�X

static BOOL getSection(const wchar_t *str)
{
	szSectionNames[0] = 0;
	const wchar_t *t = wcsrchr(str, L'\\');
	if(t){
		t++;
	}else{
		t = str;
	}
	wcscpy(szSectionName, t);
	return TRUE;
}

static wchar_t *getModuleName()
{
	if(*szApplicationName == 0){
		GetModuleFileNameW(NULL, szApplicationName, _countof(szApplicationName));
		wchar_t *t = szApplicationName + wcslen(szApplicationName) - 3;
		wcscpy(t, L"ini");
	}
	return szApplicationName;
}

//exe�Ɠ����t�H���_��ini�t�@�C�������݂����ini���g�p�A���̑��̏ꍇ�̓��W�X�g�����g�p
void checkIniFile()
{
	DWORD dwAttr = ::GetFileAttributesW(getModuleName());
	bUseINI = dwAttr != INVALID_FILE_ATTRIBUTES;
}

/* ==========================================================================
	Function Name	: (HKEY) RegCreate()
	Outline			: �w�肵�����W�X�g���L�[���쐬�i�܂��̓I�[�v���j����
	Arguments		: HKEY		hCurrentKey		(in)	���݂̃I�[�v���L�[
					: const wchar_t *lpszKeyName		(in)	�I�[�v������T�u�L�[��
					: 									���O
	Return Value	: ����	�I�[�v���܂��͍쐬���ꂽ�L�[�̃n���h��
					: ���s	NULL
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
HKEY RegCreate(HKEY hCurrentKey, const wchar_t *lpszKeyName)
{
	if(bUseINI){
		getSection(lpszKeyName);
		return ERROR_SUCCESS;
	}else{
		long	lError;
		HKEY	hkResult;
		DWORD	dwDisposition;

		lError = ::RegCreateKeyExW(hCurrentKey,
								lpszKeyName,
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_ALL_ACCESS,
								NULL,
								&hkResult,
								&dwDisposition);
		if (lError != ERROR_SUCCESS) {
			::SetLastError(lError);
			return (HKEY) INVALID_HANDLE_VALUE;
		}

		return hkResult;
	}
}

/* ==========================================================================
	Function Name	: (HKEY) RegOpen()
	Outline			: �w�肵�����W�X�g���L�[���I�[�v������
	Arguments		: HKEY		hCurrentKey		(in)	���݂̃I�[�v���L�[
					: const wchar_t *lpszKeyName		(in)	�I�[�v������T�u�L�[��
					: 									���O
	Return Value	: ����	�I�[�v���܂��͍쐬���ꂽ�L�[�̃n���h��
					: ���s	NULL
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
HKEY RegOpen(HKEY hCurrentKey, const wchar_t *lpszKeyName)
{
	if(bUseINI){
		getSection(lpszKeyName);
		return ERROR_SUCCESS;
	}else{
		long	lError;
		HKEY	hkResult;

		lError = ::RegOpenKeyExW(hCurrentKey,
								lpszKeyName,
								0,
								KEY_ALL_ACCESS,
								&hkResult);
		if (lError != ERROR_SUCCESS) {
			::SetLastError(lError);
			return (HKEY) INVALID_HANDLE_VALUE;
		}

		return hkResult;
	}
}

/* ==========================================================================
	Function Name	: (BOOL) RegClose()
	Outline			: �w�肵�����W�X�g���L�[���N���[�Y����
	Arguments		: HKEY		hKey			(in)	�N���[�Y����L�[�̃n���h��
	Return Value	: ����	TRUE
					: ���s	FALSE
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
BOOL RegClose(HKEY hKey)
{
	if(bUseINI){
		
	}else{
		long	lError;

		lError = ::RegCloseKey(hKey);
		if (lError != ERROR_SUCCESS) {
			::SetLastError(lError);
			return FALSE;
		}
	}

	return TRUE;
}

/* ==========================================================================
	Function Name	: (BOOL) RegSetStr()
	Outline			: ���W�X�g���L�[�̒l�ɕ��������������
	Arguments		: HKEY		hKey			(in)	�l��ݒ肷��L�[�̃n���h��
					: const wchar_t *lpszValueName	(in)	�ݒ肷��l
					: const wchar_t *buf			(in)	�l�f�[�^
	Return Value	: ����	TRUE
					: ���s	FALSE
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
BOOL RegSetStr(HKEY hKey, const wchar_t *lpszValueName, const wchar_t *buf)
{
	if(bUseINI){
		return WritePrivateProfileStringW(szSectionName, lpszValueName, buf, getModuleName());
	}else{
		long	lError;

		lError = ::RegSetValueExW(hKey,
								lpszValueName,
								0,
								REG_SZ,
								(const BYTE *) buf,
								(DWORD)((wcslen(buf) + 1) * sizeof(wchar_t)));
		if (lError != ERROR_SUCCESS) {
			::SetLastError(lError);
			return FALSE;
		}
	}

	return TRUE;
}

/* ==========================================================================
	Function Name	: (BOOL) RegGetStr()
	Outline			: ���W�X�g���L�[�̒l���當�����ǂݍ���
	Arguments		: HKEY		hKey			(in)		�l��ݒ肷��L�[��
					: 										�n���h��
					: const wchar_t *	lpszValueName	(in)		�ݒ肷��l
					: wchar_t	*buf			(out)		�l�f�[�^���i�[����
					: 										�o�b�t�@
					: DWORD		dwSize			(in/out)	������
	Return Value	: ����	TRUE
					: ���s	FALSE
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
BOOL RegGetStr(HKEY hKey, const wchar_t *lpszValueName, wchar_t *buf, DWORD dwSize)
{
	if(bUseINI){
		return GetPrivateProfileStringW(szSectionName, lpszValueName, L"", buf, dwSize, getModuleName());
	}else{
		LONG	lError;
		DWORD	dwWriteSize;
		DWORD	dwType = REG_SZ;

		dwWriteSize = dwSize * sizeof(wchar_t);

		lError = ::RegQueryValueExW(hKey, lpszValueName, 0, &dwType, (LPBYTE) buf, &dwWriteSize);
		if (lError != ERROR_SUCCESS) {
			::SetLastError(lError);
			return FALSE;
		}

		buf[dwSize - 1] = '\0';
	}

	return TRUE;
}

/* ==========================================================================
	Function Name	: (BOOL) RegSetDword()
	Outline			: ���W�X�g���L�[�̒l�� DWORD����������
	Arguments		: HKEY		hKey			(in)	�l��ݒ肷��L�[�̃n���h��
					: const wchar_t *lpszValueName	(in)	�ݒ肷��l
					: DWORD		dwValue			(in)	�l�f�[�^
	Return Value	: ����	TRUE
					: ���s	FALSE
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
BOOL RegSetDword(HKEY hKey, const wchar_t *lpszValueName, DWORD dwValue)
{
	if(bUseINI){
		wchar_t t[64];
		_swprintf(t, L"%d", dwValue);
		return WritePrivateProfileStringW(szSectionName, lpszValueName, t, getModuleName());
	}else{
		long	lError;

		lError = ::RegSetValueExW(hKey,
								lpszValueName,
								0,
								REG_DWORD,
								(CONST BYTE *) &dwValue,
								sizeof(DWORD));
		if (lError != ERROR_SUCCESS) {
			::SetLastError(lError);
			return FALSE;
		}
	}

	return TRUE;
}

/* ==========================================================================
	Function Name	: (BOOL) RegGetDword()
	Outline			: ���W�X�g���L�[�̒l���� DWORD��ǂݍ���
	Arguments		: HKEY		hKey			(in)	�l��ݒ肷��L�[�̃n���h��
					: const wchar_t *lpszValueName	(in)	�ݒ肷��l
					: DWORD		*dwValue		(out)	�l�f�[�^
	Return Value	: ����	TRUE
					: ���s	FALSE
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
BOOL RegGetDword(HKEY hKey, const wchar_t *lpszValueName, DWORD *dwValue)
{
	int defmark = 0xdeadbeef;

	if(bUseINI){
		// �ǂݍ��݂Ɏ��s�����ꍇ�� false ��Ԃ� (2007.11.14 yutaka)
		*dwValue = GetPrivateProfileIntW(szSectionName, lpszValueName, defmark, getModuleName());
		if (*dwValue == defmark) {
			*dwValue = 0;
			return FALSE;
		} else {
			return TRUE;
		}
	}else{
		long	lError;
		DWORD	dwType = REG_DWORD;
		DWORD	dwSize = sizeof(DWORD);

		lError = ::RegQueryValueExW(hKey,
									lpszValueName,
									0,
									&dwType,
									(LPBYTE) dwValue,
									&dwSize);
		if (lError != ERROR_SUCCESS) {
			::SetLastError(lError);
			return FALSE;
		}
	}

	return TRUE;
}

/* ==========================================================================
	Function Name	: (BOOL) RegSetBinary()
	Outline			: ���W�X�g���L�[�̒l���� BINARY����������
	Arguments		: HKEY		hKey			(in)	�l��ݒ肷��L�[�̃n���h��
					: const wchar_t *lpszValueName	(in)	�ݒ肷��l
					: void		*buf			(out)	�l�f�[�^
	Return Value	: ����	TRUE
					: ���s	FALSE
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
BOOL RegSetBinary(HKEY hKey, const wchar_t *lpszValueName, void *buf, DWORD dwSize)
{
	if(bUseINI){
		wchar_t t[1024] = {0};
		LPBYTE s = (LPBYTE)buf;
		for(DWORD i=0; i<dwSize; i++){
			wchar_t c[4];
			_swprintf(c, L"%02X ", s[i]);
			wcscat(t, c);
		}
		BOOL ret =  WritePrivateProfileStringW(szSectionName, lpszValueName, t, getModuleName());
		return ret;
	}else{
		long	lError;
		DWORD	dwWriteSize;

		dwWriteSize = dwSize;

		if ((lError = ::RegSetValueExW(hKey,
									lpszValueName,
									0,
									REG_BINARY,
									(CONST BYTE *) buf,
									dwWriteSize)) != ERROR_SUCCESS) {
			::SetLastError(lError);
			return FALSE;
		}
	}

	return TRUE;
}

/* ==========================================================================
	Function Name	: (BOOL) RegGetBinary()
	Outline			: ���W�X�g���L�[�̒l���� BINARY��ǂݍ���
	Arguments		: HKEY		hKey			(in)	�l��ݒ肷��L�[�̃n���h��
					: const wchar_t *lpszValueName	(in)	�ݒ肷��l
					: int		*buf			(out)	�l�f�[�^
	Return Value	: ����	TRUE
					: ���s	FALSE
	Reference		: 
	Renewal			: 
	Notes			: 
	Attention		: 
	Up Date			: 
   ======1=========2=========3=========4=========5=========6=========7======= */
// �֐��̕Ԓl�̌^��ǉ� (2006.2.18 yutaka)
int RegGetBinary(HKEY hKey, const wchar_t *lpszValueName, void *buf, LPDWORD lpdwSize)
{
	if(bUseINI){
		wchar_t t[1024] = {0};
		BOOL ret = GetPrivateProfileStringW(szSectionName, lpszValueName, L"", t, _countof(t), getModuleName());
		if(ret){
			size_t size = wcslen(t);
			while(t[size-1] == ' '){
				size--;
				t[size] = 0;
			}
			wchar_t *s = t;
			LPBYTE p = (LPBYTE)buf;
			DWORD cnt = 0;
			*p = 0;
			for(size_t i=0; i<(size+1)/3; i++){
				*p++ = (BYTE)wcstol(s, NULL, 16);
				s += 3;
				cnt ++;
			}
			*lpdwSize = cnt;
		}
		return ret;
	}else{
		long	lError;
		DWORD	dwType = REG_BINARY;
		DWORD	dwWriteSize;

		dwWriteSize = *lpdwSize * sizeof(TCHAR);

		if ((lError = ::RegQueryValueExW(hKey,
										lpszValueName,
										NULL,
										&dwType,
										(LPBYTE) buf,
										&dwWriteSize)) != ERROR_SUCCESS) {
			::SetLastError(lError);
			return FALSE;
		}
	}

	return TRUE;
}


LONG RegEnumEx(HKEY hKey, DWORD dwIndex, wchar_t *lpName, LPDWORD lpcName, LPDWORD lpReserved, wchar_t *lpClass, LPDWORD lpcClass, PFILETIME lpftLastWriteTime)
{
	static wchar_t *ptr = szSectionNames;
	if(bUseINI){
		if(*szSectionNames == 0){
			GetPrivateProfileSectionNamesW(szSectionNames, _countof(szSectionNames), getModuleName());
			ptr = szSectionNames;
		}
		if(wcscmp(ptr, L"TTermMenu") == 0){
			//skip
			while(*ptr++);
//			ptr++;
		}
		if(*ptr == 0){
			return ERROR_NO_MORE_ITEMS;
		}
		wcscpy(lpName, ptr);
		while(*ptr++);
//		ptr++;
		return ERROR_SUCCESS;
	}else{
		return ::RegEnumKeyExW(hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime);
	}
}

LONG RegDelete(HKEY hKey, const wchar_t *lpSubKey)
{
	if(bUseINI){
		return WritePrivateProfileStringW(szSectionName, NULL, NULL, getModuleName()) ? ERROR_SUCCESS : ERROR_ACCESS_DENIED;
	}else{
		return ::RegDeleteKeyW(hKey, lpSubKey);
	}
}
