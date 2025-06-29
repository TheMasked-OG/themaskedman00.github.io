//===========================================================================
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//============================================================================

// CustomButton.cpp : implementation file
//

#include "stdafx.h"
#include "PvrExplorer.h"
#include "CustomButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomButton

CCustomButton::CCustomButton(UINT ResourceNum, int bx, int by, UINT ControlID, CWnd *Parent)
{
	BITMAP		bmInfo;
	RECT		rec;

	m_MouseOver = false;
	m_LastClicked = false;
	m_ButtonUp.LoadBitmap(ResourceNum);
	m_IsThereButtonDwn = m_ButtonDwn.LoadBitmap(ResourceNum+1);
	m_IsThereButtonOver = m_ButtonOver.LoadBitmap(ResourceNum+2);
	m_IsThereButtonDis = m_DisabledMap.LoadBitmap(ResourceNum+3);
	m_IsThereButtonOn = m_ButtonOn.LoadBitmap(ResourceNum+4);
	m_IsThereButtonOff = m_ButtonOff.LoadBitmap(ResourceNum+5);
	m_IsThereButtonFocus = m_ButtonFocus.LoadBitmap(ResourceNum+6);
	m_IsMapsValid = false;
	//now create the and mask and the xor mask
	if( m_ButtonUp.GetBitmap(&bmInfo)){
		m_SizeX = bmInfo.bmWidth;
		m_SizeY = bmInfo.bmHeight;

		rec.left = bx;
		rec.top = by;
		rec.right = rec.left + m_SizeX;
		rec.bottom = rec.top + m_SizeY;
		Create("",BS_OWNERDRAW | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON ,rec,Parent,ControlID);
		m_State = BST_UNCHECKED;   

		CreateMaskMaps();
	}
}

CCustomButton::~CCustomButton()
{


}


BEGIN_MESSAGE_MAP(CCustomButton, CButton)
	//{{AFX_MSG_MAP(CCustomButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomButton message handlers

void 
CCustomButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HDC	dc;
	
	if( lpDrawItemStruct->CtlType == ODT_BUTTON){ //is it a button type
		if( lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)){ //dp we need to draw it
			dc = CreateCompatibleDC(lpDrawItemStruct->hDC);
			if( lpDrawItemStruct->itemState & ODS_SELECTED ){ //is it selected then draw it in
				//draw the thing
				m_MouseOver = true; //No Mouseover while in the pushed state
				if( m_IsThereButtonDwn ){  //only draw the in state if there is one defined
					DrawButton(lpDrawItemStruct->hDC,dc,&m_ButtonDwn);
				}
			} else { //not pushed in
				if( (lpDrawItemStruct->itemState & ODS_DISABLED) && m_IsThereButtonDis ){
					//we have a disabled draw state so draw it
					m_MouseOver = true; //No Mouseover while in the disabled state
					DrawButton(lpDrawItemStruct->hDC,dc,&m_DisabledMap);
				} else {
					if( GetCheck() == BST_CHECKED) {  //if the the checked state
						if( m_IsThereButtonOn ){ //draw the on state if defined
							DrawButton(lpDrawItemStruct->hDC,dc,&m_ButtonUp);
						} else {
							DrawButton(lpDrawItemStruct->hDC,dc,&m_ButtonUp);
						}
						DrawTransparent(lpDrawItemStruct->hDC,dc,&m_ButtonOn);
						//DrawButton(lpDrawItemStruct->hDC,dc,&m_ButtonOn);
						m_MouseOver = false;
					} else { //no checked
						if( m_IsThereButtonOff ){ //draw the off state only if defined
							DrawButton(lpDrawItemStruct->hDC,dc,&m_ButtonOff);
						} else {
							DrawButton(lpDrawItemStruct->hDC,dc,&m_ButtonUp);
						}
						m_MouseOver = false;
					}
					if( (lpDrawItemStruct->itemState & ODS_FOCUS) && m_IsThereButtonFocus ){
						//we now have a focus issue , the focus bit map would mostly be trasparent
						DrawButton(lpDrawItemStruct->hDC,dc,&m_ButtonFocus);
					}
				}
			}
			DeleteDC(dc);
		}
	}
}

//****************************************************************
//  DrawButton
//		This Does the mechanics of drawing a trasparent button
//		(IN) Dest - The destination DC that we are drawing to
//		(IN) Src - The Source DC to do the bitblt to
//		(IN) BitMap - What to draw
//****************************************************************
void 
CCustomButton::DrawButton(HDC Dest, HDC Src, CBitmap *BitMap)
{
	ULONG	x,y;
	HDC	dc;
	HBITMAP	temMap;

	x = m_SizeX;
	y = m_SizeY;
	dc = CreateCompatibleDC(Dest);		//create a worker DC
	
	temMap = CreateCompatibleBitmap(Dest,x,y);	//Create a bitmap the we can do the mixin
	SelectObject(dc,temMap);					//this removes the flashing artifacts
	BitBlt(dc,0,0,x,y,Dest,0,0,SRCCOPY);		// grin from the screen to the work area
	SelectObject(Src,m_ButtonAnd.m_hObject);
	BitBlt(dc,0,0,x,y,Src,0,0,SRCAND);			// and it with the AND mask
	SelectObject(Src,m_ButtonXor.m_hObject);
	BitBlt(dc,0,0,x,y,Src,0,0,SRCINVERT);		// then XORit with the XOR mask
	SelectObject(Src,BitMap->m_hObject);
	BitBlt(dc,0,0,x,y,Src,0,0,SRCINVERT);		// then XOR the bitmap in it
	BitBlt(Dest,0,0,x,y,dc,0,0,SRCCOPY);		// Copy it back
	DeleteDC(dc);
	DeleteObject(temMap);
}

//****************************************************************
//  DrawTransparent
//		This Does the mechanics of drawing a trasparent button
//		(IN) Dest - The destination DC that we are drawing to
//		(IN) Src - The Source DC to do the bitblt to
//		(IN) BitMap - What to draw
//****************************************************************
void 
CCustomButton::DrawTransparent(HDC Dest, HDC Src, CBitmap *BitMap)
{
	BITMAP		bmInfo;
	int			x,y,j,k;
	COLORREF	TransparentColor;
	CBitmap		and;
	CBitmap		xor;
	CDC			Adc,Xdc,dc,*tdc;
	HDC			adc;
	HBITMAP		temMap;

	if( BitMap->m_hObject == NULL )
		return;
	if( BitMap->GetBitmap(&bmInfo)){
		x = bmInfo.bmWidth;
		y = bmInfo.bmHeight;
		
		tdc = CDC::FromHandle(Dest);
		
		dc.CreateCompatibleDC(tdc);
		Adc.CreateCompatibleDC(tdc);
		Xdc.CreateCompatibleDC(tdc);
		
		and.CreateCompatibleBitmap(&dc,x,y);
		xor.CreateCompatibleBitmap(&dc,x,y);
		
		dc.SelectObject(BitMap);
		Adc.SelectObject(&and);
		Xdc.SelectObject(&xor);
		TransparentColor = dc.GetPixel(0,0);  //this shold be the transparent color
		for( j = 0; j < y;j++ ){
			for( k = 0; k < x; k++){
				if( dc.GetPixel(k,j) == TransparentColor ){
					Adc.SetPixel(k,j,RGB(255,255,255));
					Xdc.SetPixel(k,j,TransparentColor);
				} else {
					Adc.SetPixel(k,j,RGB(0,0,0));
					Xdc.SetPixel(k,j,RGB(0,0,0));
				}
			}
		}

		adc = CreateCompatibleDC(Dest);				//create a worker DC
		temMap = CreateCompatibleBitmap(Dest,x,y);	//Create a bitmap the we can do the mixin
		SelectObject(adc,temMap);					//this removes the flashing artifacts
		BitBlt(adc,0,0,x,y,Dest,0,0,SRCCOPY);		// grin from the screen to the work area
		//SelectObject(Src,and.m_hObject);
		BitBlt(adc,0,0,x,y,Adc.m_hDC,0,0,SRCAND);			// and it with the AND mask
		//SelectObject(Src,xor.m_hObject);
		BitBlt(adc,0,0,x,y,Xdc.m_hDC,0,0,SRCINVERT);		// then XORit with the XOR mask
		//SelectObject(Src,BitMap->m_hObject);
		BitBlt(adc,0,0,x,y,dc.m_hDC,0,0,SRCINVERT);		// then XOR the bitmap in it
		//BitBlt(adc,0,0,x,y,Xdc.m_hDC,0,0,SRCCOPY);		// then XOR the bitmap in it
		BitBlt(Dest,0,0,x,y,adc,0,0,SRCCOPY);		// Copy it back
		DeleteDC(adc);
		DeleteObject(temMap);
	}
}

BOOL CCustomButton::OnEraseBkgnd(CDC* pDC) 
{
	return(0);
	//return CButton::OnEraseBkgnd(pDC);
}

void CCustomButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDC			CompDC,*DC;

	if( !m_MouseOver ){  // do not do this if it is not a time to do the mouse over
		if( nFlags == 0 ){	// make sure no wierd mouse button actions
			if( m_IsThereButtonOver ){ // is there a mouse over map
				DC = GetDC();									//get the current DC
				CompDC.CreateCompatibleDC(DC);					//create a new one
				DrawButton(DC->m_hDC,CompDC.m_hDC,&m_ButtonOver);	//draw the bit map
				m_MouseOver = true;								//do not draw it again
				if( m_IsThereButtonOn ){  //check for check box type
					if( GetCheck() == BST_CHECKED){
						DrawTransparent(DC->m_hDC,CompDC,&m_ButtonOn);
					}
				}
				CompDC.DeleteDC();								// clean up
				ReleaseDC(DC);
				SetTimer(1,200,NULL);							//look to see if the mouse is no longer on it 200 ms later
			}
		}
	}
	CButton::OnMouseMove(nFlags, point);
}

void CCustomButton::OnTimer(UINT nIDEvent) 
{
	POINT	p;
	RECT	rec;

	if( m_MouseOver ){		// are we in a no mouse blocked mouse over condition
		GetCursorPos(&p);		//get the mouse screen position
		GetWindowRect(&rec);	//get the button position
		if( p.x < rec.left || p.x > rec.right || p.y < rec.top || p.y > rec.bottom ){
			m_MouseOver = true;	//ok we are out of the region
			KillTimer(nIDEvent);	//kill the timer and look no more
			InvalidateRect(NULL,FALSE);	//force a button redraw
			UpdateWindow();
		}
	}
	CButton::OnTimer(nIDEvent);
}

 void 
CCustomButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	RECT	rec;
 
 	GetClientRect(&rec);
 	if( point.x >= rec.left && point.x <= rec.right && point.y >= rec.top && point.y <= rec.bottom){
 		if( GetCheck() == BST_UNCHECKED ){
 			SetCheck(BST_CHECKED);
 		} else {
 			SetCheck(BST_UNCHECKED);
 		}
 		m_MouseOver = false;
		m_LastClicked = true;
 	}
 	CButton::OnLButtonUp(nFlags, point);
 }

void 
CCustomButton::SetCheck(int state)
{
	m_State = state;
	InvalidateRect(NULL,FALSE);
	UpdateWindow();
}

int 
CCustomButton::GetCheck()
{
	return(m_State);
}



bool CCustomButton::IsLastClicked()
{
	bool	rc;

	rc = m_LastClicked;
	m_LastClicked = false;
	return(rc);
}


void CCustomButton::CreateMaskMaps()
{

	COLORREF	TransparentColor;
	CDC			*dc,Adc,Xdc,bitDC;
	int			j,k;
	RECT		rec;
	CRgn		*NewRgn;
	CBitmap		*rc;

	
	dc = GetDC();
	bitDC.CreateCompatibleDC(dc);
	Adc.CreateCompatibleDC(dc);
	Xdc.CreateCompatibleDC(dc);
	m_ButtonAnd.CreateCompatibleBitmap(dc,m_SizeX,m_SizeY);
	m_ButtonXor.CreateCompatibleBitmap(dc,m_SizeX,m_SizeY);
	rc = bitDC.SelectObject(&m_ButtonUp);
	rc = Adc.SelectObject(&m_ButtonAnd);
	rc = Xdc.SelectObject(&m_ButtonXor);
		
	TransparentColor = bitDC.GetPixel(0,0);  //this shold be the transparent color
	//TransparentColor = Adc.GetPixel(0,0);  //this shold be the transparent color
	if( TransparentColor == 0xFFFFFFFF )
		return;
	m_rgn.CreateRectRgn(0,0,0,0);
	for( j = 0; j < m_SizeY;j++ ){
		NewRgn = NULL;
		for( k = 0; k < m_SizeX; k++){
			if( bitDC.GetPixel(k,j) == TransparentColor ){
				Adc.SetPixel(k,j,RGB(255,255,255));
				Xdc.SetPixel(k,j,bitDC.GetPixel(k,j));
				// now do the region stuff
				if( NewRgn != NULL ){
					rec.right = k;
					rec.bottom = j + 1;
					NewRgn->CreateRectRgn(rec.left,rec.top,rec.right,rec.bottom);
					m_rgn.CombineRgn(&m_rgn,NewRgn,RGN_OR);
					delete NewRgn;
					NewRgn = NULL;
				} 
			} else {
				Adc.SetPixel(k,j,RGB(0,0,0));
				Xdc.SetPixel(k,j,RGB(0,0,0));
				if( NewRgn == NULL ){ // start looking for the new region
					rec.left = k;
					rec.top = j;
					NewRgn = (CRgn *)new CRgn;
				}
			}
		}
	}
	bitDC.DeleteDC();
	Adc.DeleteDC();
	Xdc.DeleteDC();
	// now figure out the region
	if( NewRgn != NULL ){ 
		rec.right = k;
		rec.bottom = j + 1;
		NewRgn->CreateRectRgn(rec.left,rec.top,rec.right,rec.bottom);
		m_rgn.CombineRgn(&m_rgn,NewRgn,RGN_OR);
		delete NewRgn;
		NewRgn = NULL;
	}
	SetWindowRgn((HRGN)m_rgn,TRUE);
	m_IsMapsValid = true;
}
