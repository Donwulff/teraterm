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

/* teraterm/ ���ŋ��ʂɎg���֐��Ȃ� */
/* ts�\���̂Ɋ֘A����֐��Ȃ� */

#include <assert.h>

#include "tslib.h"

#include "dlglib.h"		// for GetFontPixelFromPoint()
#include "codeconv.h"

/**
 *	�ݒ�(TTTSet)���� logfont �̎擾
 *
 *	@param[in]		TTTSet *pts		�ݒ�
 *	@param[in]		type			0	VT�E�B���h�E
 *									1	TEK�E�B���h�E
 *									2	�_�C�A���O�̃t�H���g
 *	@param[in]		dpi				DPI�ɍ��킹���T�C�Y�̃t�H���g���擾
 *									0 �̂Ƃ� hWnd ��DPI���g�p����
 *	@param[out]		logfont			�擾�����t�H���g���
 */
void TSGetLogFont(HWND hWnd, const TTTSet *pts, int type, unsigned int dpi, LOGFONTW *logfont)
{
	*logfont = {};

	if (dpi == 0) {
		HDC DC = GetDC(hWnd);
		dpi = GetDeviceCaps(DC, LOGPIXELSY);
		ReleaseDC(hWnd, DC);
	}
	assert(dpi != 0);

	logfont->lfWeight = FW_NORMAL;
	logfont->lfItalic = 0;
	logfont->lfUnderline = 0;
	logfont->lfStrikeOut = 0;
	logfont->lfOutPrecision  = OUT_CHARACTER_PRECIS;
	logfont->lfClipPrecision = CLIP_CHARACTER_PRECIS;
	logfont->lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
	switch (type) {
	default:
		assert(0);
	case 0:
		// VT�E�B���h�E
		ACPToWideChar_t(pts->VTFont, logfont->lfFaceName, _countof(logfont->lfFaceName));
		logfont->lfWidth = pts->VTFontSize.x;
		logfont->lfHeight = pts->VTFontSize.y;
		logfont->lfCharSet = pts->VTFontCharSet;
		logfont->lfQuality = (BYTE)pts->FontQuality;
		logfont->lfWidth = MulDiv(logfont->lfWidth, dpi, 96);
		logfont->lfHeight = MulDiv(logfont->lfHeight, dpi, 96);
		break;
	case 1:
		// TEK�E�B���h�E
		ACPToWideChar_t(pts->TEKFont, logfont->lfFaceName, _countof(logfont->lfFaceName));
		logfont->lfWidth = pts->TEKFontSize.x;
		logfont->lfHeight = pts->TEKFontSize.y;
		logfont->lfCharSet = pts->TEKFontCharSet;
		logfont->lfQuality = (BYTE)pts->FontQuality;
		logfont->lfWidth = MulDiv(logfont->lfWidth, dpi, 96);
		logfont->lfHeight = MulDiv(logfont->lfHeight, dpi, 96);
		break;
	case 2:
		// �_�C�A���O�t�H���g
		// DPI�̏�����OS���s���̂ŁA�T�C�Y�̒������Ȃ�
		if (pts->DialogFontNameW[0] == 0) {
			// �t�H���g���ݒ肳��Ă��Ȃ�������OS�̃t�H���g���g�p����
			GetMessageboxFontW(logfont);
		} else {
			wcsncpy_s(logfont->lfFaceName, _countof(logfont->lfFaceName), pts->DialogFontNameW, _TRUNCATE);
			logfont->lfHeight = -GetFontPixelFromPoint(hWnd, pts->DialogFontPoint);
			logfont->lfCharSet = pts->DialogFontCharSet;
			logfont->lfQuality = DEFAULT_QUALITY;
		}
		break;
	}
	GetFontPitchAndFamily(hWnd, logfont);
}

/**
 *	logfont ����ݒ�(TTTSet) �փZ�b�g
 *
 *	@param[in]		logfont			�擾�����t�H���g���
 *	@param[in]		type			0	VT�E�B���h�E
 *									1	TEK�E�B���h�E
 *									2	�_�C�A���O�̃t�H���g
 *	@param[in]		dpi				DPI�ɍ��킹���T�C�Y�̃t�H���g���擾
 *									0 �̂Ƃ� hWnd ��DPI���g�p����
 *	@param[out]		TTTSet *pts		�ݒ�
 */
void TSSetLogFont(HWND hWnd, const LOGFONTW *logfont, int type, unsigned int dpi, TTTSet *pts)
{
	if (dpi == 0) {
		HDC DC = GetDC(hWnd);
		dpi = GetDeviceCaps(DC, LOGPIXELSY);
		ReleaseDC(hWnd, DC);
	}

	switch (type) {
	default:
		assert(0);
	case 0:
		// VT�E�B���h�E
		WideCharToACP_t(logfont->lfFaceName, pts->VTFont, sizeof(pts->VTFont));
		pts->VTFontSize.x = logfont->lfWidth  * 96 / (int)dpi;
		pts->VTFontSize.y = logfont->lfHeight * 96 / (int)dpi;
		pts->VTFontCharSet = logfont->lfCharSet;
		break;
	case 1:
		// TEK�E�B���h�E
		WideCharToACP_t(logfont->lfFaceName, pts->TEKFont, sizeof(pts->TEKFont));
		pts->TEKFontSize.x = logfont->lfWidth * 96 / (int)dpi;
		pts->TEKFontSize.y = logfont->lfHeight * 96 / (int)dpi;
		pts->TEKFontCharSet = logfont->lfCharSet;
		break;
	case 2:
		// �_�C�A���O�t�H���g
		wcsncpy_s(pts->DialogFontNameW, _countof(pts->DialogFontNameW), logfont->lfFaceName, _TRUNCATE);
		pts->DialogFontPoint = GetFontPointFromPixel(hWnd, -logfont->lfHeight);
		pts->DialogFontCharSet = logfont->lfCharSet;
		break;
	}
}
