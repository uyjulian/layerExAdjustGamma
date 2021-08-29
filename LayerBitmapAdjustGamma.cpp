
#include "tjsCommHead.h"
#include "LayerBitmapIntf.h"
#include "LayerBitmapAdjustGamma.h"

#define RET_VOID
#define BOUND_CHECK(x) \
{ \
	tjs_int i; \
	if(rect.left < 0) rect.left = 0; \
	if(rect.top < 0) rect.top = 0; \
	if(rect.right > (i=destbmp->GetWidth())) rect.right = i; \
	if(rect.bottom > (i=destbmp->GetHeight())) rect.bottom = i; \
	if(rect.right - rect.left <= 0 || rect.bottom - rect.top <= 0) \
		return x; \
}

const tTVPGLGammaAdjustData TVPIntactGammaAdjustData =
{ 1.0, 0, 255, 1.0, 0, 255, 1.0, 0, 255 };

void AdjustGamma(tTVPBaseBitmap *destbmp, tTVPRect rect, const tTVPGLGammaAdjustData & data)
{
#if 0
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);
#endif

	BOUND_CHECK(RET_VOID);

	if(!memcmp(&data, &TVPIntactGammaAdjustData, sizeof(tTVPGLGammaAdjustData)))
		return;

	tTVPGLGammaAdjustTempData temp;
	TVPInitGammaAdjustTempData(&temp, &data);

	try
	{
		tjs_int h = rect.bottom - rect.top;
		tjs_int w = rect.right - rect.left;

		tjs_int pitch = destbmp->GetPitchBytes();
		tjs_uint8 * line = (tjs_uint8*)destbmp->GetScanLineForWrite(rect.top);


		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPAdjustGamma((tjs_uint32*)line, w, &temp);
			line += pitch;
		}

	}
	catch(...)
	{
		TVPUninitGammaAdjustTempData(&temp);
		throw;
	}

	TVPUninitGammaAdjustTempData(&temp);
}
//---------------------------------------------------------------------------
void AdjustGammaForAdditiveAlpha(tTVPBaseBitmap *destbmp, tTVPRect rect, const tTVPGLGammaAdjustData & data)
{
#if 0
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);
#endif

	BOUND_CHECK(RET_VOID);

	if(!memcmp(&data, &TVPIntactGammaAdjustData, sizeof(tTVPGLGammaAdjustData)))
		return;

	tTVPGLGammaAdjustTempData temp;
	TVPInitGammaAdjustTempData(&temp, &data);

	try
	{
		tjs_int h = rect.bottom - rect.top;
		tjs_int w = rect.right - rect.left;

		tjs_int pitch = destbmp->GetPitchBytes();
		tjs_uint8 * line = (tjs_uint8*)destbmp->GetScanLineForWrite(rect.top);


		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPAdjustGamma_a((tjs_uint32*)line, w, &temp);
			line += pitch;
		}

	}
	catch(...)
	{
		TVPUninitGammaAdjustTempData(&temp);
		throw;
	}

	TVPUninitGammaAdjustTempData(&temp);
}
