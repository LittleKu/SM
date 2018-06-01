#ifndef __SM_WEBVIEW_H__
#define __SM_WEBVIEW_H__

#pragma once

class SMWebView :
	public SWindow
{
	SOUI_CLASS_NAME_EX(SMWebView, L"sm_webview", Window)
public:
	SMWebView(void);
	~SMWebView(void);

public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, CSize size);
	void OnPaint(SOUI::IRenderTarget * pRT);
	void OnDestroy();
	void OnSetFocus(SWND wndOld);
	void OnKillFocus(SWND wndFocus);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnSetCursor(const CPoint &pt);
	LRESULT OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam);

	void LoadURL(const SStringW &strURL);

	void OnHandleDocumentReady(wkeWebView webWindow, wkeWebFrameHandle frameId);
	wkeWebView OnWebCreate(wkeWebView webWindow, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features);
	void OnPaintBitUpdate(wkeWebView webView, const void* buffer, const wkeRect* r, int width, int height);
protected:
	void CreateWebView();

	BOOL OnAttrURL(SStringW strValue, BOOL bLoading);

	SOUI_ATTRS_BEGIN()
		ATTR_CUSTOM(L"url", OnAttrURL)
	SOUI_ATTRS_END()

	SOUI_MSG_MAP_BEGIN()
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SETFOCUS_EX(OnSetFocus)
		MSG_WM_KILLFOCUS_EX(OnKillFocus)
		MSG_WM_CHAR(OnChar)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MBUTTONDBLCLK, OnMouseEvent)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KEYUP(OnKeyUp)
		MESSAGE_HANDLER_EX(WM_IME_STARTCOMPOSITION,OnImeStartComposition)
	SOUI_MSG_MAP_END()
protected:
	wkeWebView webview_;
	CAutoRefPtr<IBitmap> pixmap_;
	SStringW url_;
};

#endif