#include "StdAfx.h"
#include "SMWebView.h"

namespace {
	void OnHandleDocumentReady(wkeWebView webWindow, void *param, wkeWebFrameHandle frameId) {
		SMWebView *pThis = (SMWebView*)param;
		return pThis->OnHandleDocumentReady(webWindow, frameId);
	}

	wkeWebView OnWebCreate(wkeWebView webWindow, void *param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features) {
		SMWebView *pThis = (SMWebView*)param;
		return pThis->OnWebCreate(webWindow, navType, url, features);
	}

	void OnPaintBitUpdate(wkeWebView webView, void* param, const void* buffer, const wkeRect* r, int width, int height) {
		SMWebView *pThis = (SMWebView*)param;
		return pThis->OnPaintBitUpdate(webView, buffer, r, width, height);
	}
}

class SMiniBlinkEnv{
public:
	SMiniBlinkEnv() {
		wkeInitialize();
	}
	~SMiniBlinkEnv(){
		wkeFinalize();
	}
};

static SMiniBlinkEnv mbEnv;

SMWebView::SMWebView(void):webview_(NULL) {
	url_.Empty();
}

SMWebView::~SMWebView(void)
{
}

void SMWebView::LoadURL(const SStringW &strURL) {
	if (webview_ == NULL) {
		CreateWebView();
	}

	url_ = strURL;

	wkeLoadURL(webview_, S_CW2A(strURL, CP_UTF8));
}

int SMWebView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (webview_ == NULL) {
		CreateWebView();
	}
	return 0;
}

void SMWebView::OnSize(UINT nType, CSize size) {
	__super::OnSize(nType, size);
	if (webview_ != NULL) {
		wkeResize(webview_, size.cx, size.cy);
	}
}

void SMWebView::OnPaint(SOUI::IRenderTarget * pRT) {
	if (pixmap_) {
		CRect rcClient = GetClientRect();
		pRT->DrawBitmap(rcClient, pixmap_, 0, 0, GetStyle().m_byAlpha);
	}
}

void SMWebView::OnDestroy() {
	if (webview_) {
		wkeDestroyWebView(webview_);
		webview_ = NULL;
	}
}

void SMWebView::OnSetFocus(SWND wndOld) {
	if (webview_) {
		wkeSetFocus(webview_);
	}
}

void SMWebView::OnKillFocus(SWND wndFocus) {
	if (webview_) {
		wkeKillFocus(webview_);
	}
}

void SMWebView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	unsigned int flags = 0;
	if (nFlags & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (nFlags & KF_EXTENDED)
		flags |= WKE_EXTENDED;
	if (webview_) {
		wkeFireKeyPressEvent(webview_, nChar, flags, false);
	}
}

LRESULT SMWebView::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN)	{
		SetCapture();
		SetFocus();
	} else if (uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP || uMsg == WM_RBUTTONUP) {
		ReleaseCapture();
	}
	
	if (webview_) {
		CRect rcClient;
		GetClientRect(&rcClient);

		int x = GET_X_LPARAM(lParam)-rcClient.left;
		int y = GET_Y_LPARAM(lParam)-rcClient.top;

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		wkeFireMouseEvent(webview_, uMsg, x, y, flags);
	}
	return 0;
}

BOOL SMWebView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	
	CRect rc;
	GetWindowRect(&rc);
	pt.x -= rc.left;
	pt.y -= rc.top;

	unsigned int flags = 0;

	if (nFlags & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (nFlags & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (nFlags & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (nFlags & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (nFlags & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	if (webview_) {
		wkeFireMouseWheelEvent(webview_, pt.x, pt.y, zDelta, flags);
	}
	return 0;
}

void SMWebView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	unsigned int flags = 0;
	if (nFlags & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (nFlags & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	if (webview_) {
		wkeFireKeyDownEvent(webview_, nChar, flags, false);
	}
}
void SMWebView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	unsigned int flags = 0;
	if (nFlags & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (nFlags & KF_EXTENDED)
		flags |= WKE_EXTENDED;
	if (webview_) {
		wkeFireKeyUpEvent(webview_, nChar, flags, false);
	}
}

BOOL SMWebView::OnSetCursor(const CPoint &pt) {
	return TRUE;
}

LRESULT SMWebView::OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam) {
	if (webview_) {
		wkeRect caret = wkeGetCaretRect(webview_);

		CRect rcClient;
		GetClientRect(&rcClient);

		CANDIDATEFORM form;
		form.dwIndex = 0;
		form.dwStyle = CFS_EXCLUDE;
		form.ptCurrentPos.x = caret.x + rcClient.left;
		form.ptCurrentPos.y = caret.y + caret.h + rcClient.top;
		form.rcArea.top = caret.y + rcClient.top;
		form.rcArea.bottom = caret.y + caret.h + rcClient.top;
		form.rcArea.left = caret.x + rcClient.left;
		form.rcArea.right = caret.x + caret.w + rcClient.left;
		COMPOSITIONFORM compForm;
		compForm.ptCurrentPos=form.ptCurrentPos;
		compForm.rcArea=form.rcArea;
		compForm.dwStyle=CFS_POINT;

		HWND hWnd=GetContainer()->GetHostHwnd();
		HIMC hIMC = ImmGetContext(hWnd);
		ImmSetCandidateWindow(hIMC, &form);
		ImmSetCompositionWindow(hIMC,&compForm);
		ImmReleaseContext(hWnd, hIMC);
	}
	return 0;
}

void SMWebView::OnHandleDocumentReady(wkeWebView webWindow, wkeWebFrameHandle frameId) {
	if (wkeIsMainFrame(webWindow, frameId)) {
		///blablabla...
	}
}

wkeWebView SMWebView::OnWebCreate(wkeWebView webWindow, wkeNavigationType navType,
								  const wkeString url, const wkeWindowFeatures* features) {
	wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP,
		NULL, features->x, features->y, features->width, features->height);
	wkeShowWindow(newWindow, true);
	return newWindow;
}

void SMWebView::OnPaintBitUpdate(wkeWebView webView, const void* buffer, const wkeRect* r, int width, int height) {
	if (pixmap_ == NULL) {
		GETRENDERFACTORY->CreateBitmap(&pixmap_);
	}

	if (pixmap_->Width() != width || pixmap_->Height() != height)
		pixmap_->Init(width, height);

	LPBYTE pDst = (LPBYTE)pixmap_->LockPixelBits();
	if (pDst) {
		memcpy(pDst, buffer, width * height * 4);
		pixmap_->UnlockPixelBits(pDst);
		Invalidate();
	}
}

BOOL SMWebView::OnAttrURL(SStringW strValue, BOOL bLoading) {
	LoadURL(strValue);
	return !bLoading;
}

void SMWebView::CreateWebView() {
	if (webview_ == NULL) {
		webview_ = wkeCreateWebView();
		if (webview_ == NULL)
			throw "?????????";
		else{
			wkeOnDocumentReady2(webview_, ::OnHandleDocumentReady, this);
			wkeOnCreateView(webview_, ::OnWebCreate, this);
			//wkeOnNavigation(webview_, &OnNavigation, this);
			wkeOnPaintBitUpdated(webview_, ::OnPaintBitUpdate, this);
		}
	}
}