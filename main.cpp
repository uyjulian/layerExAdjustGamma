/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2021-2021 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "ncbind/ncbind.hpp"
#include "tvpgl.h"
#include "LayerBitmapIntf.h"
#include "LayerBitmapUtility.h"
#include "LayerBitmapAdjustGamma.h"
#include <string.h>
#include <stdio.h>

static void PreRegistCallback()
{
	TVPInitDitherTable();
	iTJSDispatch2 *global = TVPGetScriptDispatch();
	if (global)
	{
		tTJSVariant layer_val;
		static ttstr Layer_name(TJS_W("Layer"));
		global->PropGet(0, Layer_name.c_str(), Layer_name.GetHint(), &layer_val, global);
		tTJSVariantClosure layer_valclosure = layer_val.AsObjectClosureNoAddRef();
		if (layer_valclosure.Object)
		{
			layer_valclosure.DeleteMember(TJS_IGNOREPROP, TJS_W("adjustGamma"), 0, NULL);
		}
	}
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);

class LayerLayerExAdjustGamma
{
public:
	static tjs_error TJS_INTF_METHOD adjustGamma(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{

		if(numparams == 0) return TJS_S_OK;

		tTVPGLGammaAdjustData data;
		memcpy(&data, &TVPIntactGammaAdjustData, sizeof(data));

		if(numparams >= 1 && param[0]->Type() != tvtVoid)
			data.RGamma = static_cast<float>((double)*param[0]);
		if(numparams >= 2 && param[1]->Type() != tvtVoid)
			data.RFloor = *param[1];
		if(numparams >= 3 && param[2]->Type() != tvtVoid)
			data.RCeil  = *param[2];
		if(numparams >= 4 && param[3]->Type() != tvtVoid)
			data.GGamma = static_cast<float>((double)*param[3]);
		if(numparams >= 5 && param[4]->Type() != tvtVoid)
			data.GFloor = *param[4];
		if(numparams >= 6 && param[5]->Type() != tvtVoid)
			data.GCeil  = *param[5];
		if(numparams >= 7 && param[6]->Type() != tvtVoid)
			data.BGamma = static_cast<float>((double)*param[6]);
		if(numparams >= 8 && param[7]->Type() != tvtVoid)
			data.BFloor = *param[7];
		if(numparams >= 9 && param[8]->Type() != tvtVoid)
			data.BCeil  = *param[8];

		if(objthis == NULL) TVPThrowExceptionMessage(TJS_W("Specify Layer or Bitmap class object"));
		tTJSVariant bmpobject = tTJSVariant(objthis, objthis);
		tTJSVariantClosure bmpobject_clo = bmpobject.AsObjectClosureNoAddRef();

		// Now get information (this will independ the bitmap)
		tjs_int bmpwidth = 0;
		tjs_int bmpheight = 0;
		tjs_int bmppitch = 0;
		tjs_uint8* bmpdata = NULL;
		GetBitmapInformationFromObject(bmpobject_clo, true, &bmpwidth, &bmpheight, &bmppitch, &bmpdata);
		if(!bmpdata) TVPThrowExceptionMessage(TJS_W("Specify Layer or Bitmap class object"));
		tTVPBaseBitmap bmpinfo(bmpwidth, bmpheight, bmppitch, bmpdata);

		tTVPDrawFace Face = dfAuto; // (outward) current drawing layer face
		tTVPLayerType DisplayType = ltOpaque; // actual Type
		tTVPRect ClipRect;
		tjs_int ImageLeft = 0;
		tjs_int ImageTop = 0;

		GetLayerInformationFromLayerObject(bmpobject_clo, &Face, &DisplayType, &ClipRect, NULL, &ImageLeft, &ImageTop, NULL);

		tTVPDrawFace DrawFace = GetDrawFace(Face, DisplayType); // (actual) current drawing layer face

		if(DrawFace == dfAddAlpha)
			AdjustGammaForAdditiveAlpha(&bmpinfo, ClipRect, data);
		else
			AdjustGamma(&bmpinfo, ClipRect, data);

		UpdateLayerWithLayerObject(bmpobject_clo, &ClipRect, &ImageLeft, &ImageTop);
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(LayerLayerExAdjustGamma, Layer)
{
	RawCallback("adjustGamma", &Class::adjustGamma, 0);
};
