// kingimageView.cpp : implementation of the CKingimageView class
//

#include "stdafx.h"
#include "kingimage.h"

#include "kingimageDoc.h"
#include "kingimageView.h"
#include <ctgmath>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKingimageView

IMPLEMENT_DYNCREATE(CKingimageView, CScrollView)

BEGIN_MESSAGE_MAP(CKingimageView, CScrollView)
	//{{AFX_MSG_MAP(CKingimageView)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_PROCESS, OnProcess)
	ON_COMMAND(ID_any, Onany)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_SOBEL, &CKingimageView::OnSobel)
	ON_COMMAND(ID_UNSHARPMASKING, &CKingimageView::OnUnsharpmasking)
	ON_COMMAND(ID_LOGMASK4, &CKingimageView::OnLogmask4)
	ON_COMMAND(ID_LOGMASK3, &CKingimageView::OnLogmask3)
	ON_COMMAND(ID_LOGMASK, &CKingimageView::OnLogmask)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKingimageView construction/destruction

CKingimageView::CKingimageView()
{
	// TODO: add construction code here

}

CKingimageView::~CKingimageView()
{
}

BOOL CKingimageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView drawing

void CKingimageView::OnDraw(CDC* pDC)
{
	CKingimageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
//	pDC->SetStretchBltMode(COLORONCOLOR);
//	int x,y,bytes;
	if (pDoc->imagetype==PCX)
		(pDoc->_pcx)->draw(pDC);
	if (pDoc->imagetype==BMP)
		(pDoc->_bmp)->draw(pDC);
	if (pDoc->imagetype==GIF)
		(pDoc->_gif)->draw(pDC);
	if (pDoc->imagetype==JPG)
		(pDoc->_jpg)->draw(pDC);

}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView printing

BOOL CKingimageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKingimageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKingimageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CKingimageView diagnostics

#ifdef _DEBUG
void CKingimageView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CKingimageView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CKingimageDoc* CKingimageView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKingimageDoc)));
	return (CKingimageDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKingimageView message handlers



void CKingimageView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CSize totalSize=CSize(::GetSystemMetrics(SM_CXSCREEN),
		                  ::GetSystemMetrics(SM_CYSCREEN));
	CSize pageSize=CSize(totalSize.cx/2,totalSize.cy/2);
	CSize lineSize=CSize(totalSize.cx/10,totalSize.cy/10);

	SetScrollSizes(MM_TEXT,totalSize,pageSize,lineSize);
}

void CKingimageView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CScrollView::OnMouseMove(nFlags, point);
}



void CKingimageView::OnUnsharpmasking()
{
	CKingimageDoc* pDoc = GetDocument();

	int iBitPerPixel = pDoc->_bmp->bitsperpixel;
	int iWidth = pDoc->_bmp->width;
	int iHeight = pDoc->_bmp->height;
	BYTE *pImg = pDoc->_bmp->point;

	int Wp = iWidth;
	if (iBitPerPixel == 8)  ////Grey scale 8 bits image
	{
		int r = iWidth % 4;
		int p = (4 - r) % 4;
		Wp = iWidth + p;
	}
	else if (iBitPerPixel == 24)	// RGB image
	{
		int r = (3 * iWidth) % 4;
		int p = (4 - r) % 4;
		Wp = 3 * iWidth + p;
	}


	double *blur = new double[Wp*iHeight];
	double *invblur = new double[Wp*iHeight];
	double *unmask = new double[Wp*iHeight];
	double *newlayer = new double[Wp*iHeight];
	double *final = new double[Wp*iHeight];
	int mask[] = {1,2,1,2,4,2,1,2,1 };


	for (int i = 1; i<iHeight - 1; i++)
		for (int j = 1; j<iWidth - 1; j++)
		{
			blur[i*Wp + j] = (pImg[(i - 1)*Wp + (j - 1)] * mask[0] + pImg[(i - 1)*Wp + j] * mask[1] + pImg[(i - 1)*Wp + (j + 1)] * mask[2] + pImg[i*Wp + (j - 1)] * mask[3] + pImg[i*Wp + j] * mask[4] + pImg[i*Wp + (j + 1)] * mask[5] + pImg[(i + 1)*Wp + (j - 1)] * mask[6] + pImg[(i + 1)*Wp + j] * mask[7] + pImg[(i + 1)*Wp + (j + 1)] * mask[8]) /16; //blurring
			unmask[i*Wp + j] = pImg[i*Wp + j] - blur[i*Wp + j];
			invblur[i*Wp + j] = 255 - blur[i*Wp + j];
			newlayer[i*Wp + j] = 255 - (pImg[i*Wp + j] + invblur[i*Wp + j]);
			final[i*Wp + j] = pImg[i*Wp + j] + unmask[i*Wp + j] - newlayer[i*Wp + j];

			pImg[i*Wp + j] = final[i*Wp + j];
			
		//	pImg[i*Wp + j] = pImg[i*Wp + j] + unmask[i*Wp + j];
			if (pImg[i*Wp + j] > 255)
				pImg[i*Wp + j] = 255;
			else if (pImg[i*Wp + j] < 0)
				pImg[i*Wp + j] = 0;
			
		}

	OnDraw(GetDC());
}


