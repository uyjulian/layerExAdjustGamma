// Copied tvpgl functions, because we can't rely on the core to have them.

#include "tvpgl.h"
#include <math.h>

static tjs_uint32 TVPRecipTable256[256]; /* 1/x  table  ( 65536 ) multiplied */
static tjs_uint16 TVPRecipTable256_16[256]; /* 1/x  table  ( 65536 ) multiplied,
	but limitted to 32767 (signed 16bits) */

TVP_GL_FUNC_DECL(void, TVPInitDitherTable, (void))
{
	/* create TVPRecipTable256 */
	TVPRecipTable256[0] = 65536;
	TVPRecipTable256_16[0] = 0x7fff;
	for(tjs_int i = 1; i < 256; i++)
	{
		TVPRecipTable256[i] = 65536/i;
		TVPRecipTable256_16[i] = TVPRecipTable256[i] > 0x7fff ?
									0x7fff : TVPRecipTable256[i];
	}
}

TVP_GL_FUNC_DECL(void, TVPInitGammaAdjustTempData_c, (tTVPGLGammaAdjustTempData *temp, const tTVPGLGammaAdjustData *data))
{
	/* make table */

	double ramp = data->RCeil - data->RFloor;
	double gamp = data->GCeil - data->GFloor;
	double bamp = data->BCeil - data->BFloor;

	double rgamma = 1.0/data->RGamma; /* we assume data.?Gamma is a non-zero value here */
	double ggamma = 1.0/data->GGamma;
	double bgamma = 1.0/data->BGamma;

	int i;
	for(i=0;i<256;i++)
	{
#if 1
		double rate = (double)i/255.0;
		int n;
		n = (int)(pow(rate, rgamma)*ramp+0.5+(double)data->RFloor);
		if(n<0) n=0; else if(n>255) n=255;
		temp->R[i]= n;
		n = (int)(pow(rate, ggamma)*gamp+0.5+(double)data->GFloor);
		if(n<0) n=0; else if(n>255) n=255;
		temp->G[i]= n;
		n = (int)(pow(rate, bgamma)*bamp+0.5+(double)data->BFloor);
		if(n<0) n=0; else if(n>255) n=255;
		temp->B[i]= n;
#else
		// pow(x, y) == exp(y * log(x))
		double rate = log((double)i/255.0);
		int n;
		n = (int)(exp(rate * rgamma)*ramp+0.5+(double)data->RFloor);
		if(n<0) n=0; else if(n>255) n=255;
		temp->R[i]= n;
		n = (int)(exp(rate * ggamma)*gamp+0.5+(double)data->GFloor);
		if(n<0) n=0; else if(n>255) n=255;
		temp->G[i]= n;
		n = (int)(exp(rate * bgamma)*bamp+0.5+(double)data->BFloor);
		if(n<0) n=0; else if(n>255) n=255;
		temp->B[i]= n;
#endif
	}
}

TVP_GL_FUNC_DECL(void, TVPAdjustGamma_c, (tjs_uint32 *dest, tjs_int len, tTVPGLGammaAdjustTempData *temp))
{
	tjs_uint32 d1, t1;
	{
		for(int ___index = 0; ___index < len; ___index++)
		{
			d1 = dest[___index];;
			if(d1 > 0x00ffffffu)
			{
				/* process only non-fully-transparent pixel */
				t1 = temp->B[d1 & 0xffu];;
				d1 >>= 8u;;
				t1 += (temp->G[d1 & 0xffu]<<8u);;
				d1 >>= 8u;;
				t1 += (temp->R[d1 & 0xffu]<<16u);;
				t1 += ((d1 & 0xff00u) << 16u);;
				dest[___index] = t1;;
			}
		}
	}
}
TVP_GL_FUNC_DECL(void, TVPAdjustGamma_a_c, (tjs_uint32 *dest, tjs_int len, tTVPGLGammaAdjustTempData *temp))
{
	/* gamma adjustment for additive alpha */
	{
		for(int ___index = 0; ___index < len; ___index++)
		{
			{
				tjs_uint32 d;
				tjs_int alpha;
				tjs_int alpha_adj;
				tjs_int recip;
				tjs_int t, d_tmp;

				d = dest[___index];

				if(d >= 0xff000000u)
				{
					/* completely opaque */
					t = d & 0xffu;
					d_tmp =   temp->B[t];
					t = (d>>8u) & 0xffu;
					d_tmp |=  temp->G[t] << 8u;
					t = (d>>16u) & 0xffu; 
					d_tmp |=  temp->R[t] << 16u;
					d_tmp |= 0xff000000u;
					dest[___index] = d_tmp;
				}
				else if(d != 0u)
				{
					/* not completely transparent */
					alpha = d >> 24u;
					alpha_adj = alpha + (alpha >> 7u);
					recip = TVPRecipTable256_16[alpha];

					/* B */
					t = d & 0xffu;
					if(t > alpha)
						d_tmp = (temp->B[255u] * alpha_adj >> 8u) + t - alpha;
					else
						d_tmp = temp->B[recip * t >> 8u] * alpha_adj >> 8u;
					/* G */
					t = (d>>8u) & 0xffu; 
					if(t > alpha)
						d_tmp |= ((temp->G[255u] * alpha_adj >> 8u) + t - alpha) << 8u;
					else
						d_tmp |= (temp->G[recip * t >> 8u] * alpha_adj >> 8u) << 8u;
					/* R */
					t = (d>>16u) & 0xffu; 
					if(t > alpha)
						d_tmp |= ((temp->R[255u] * alpha_adj >> 8u) + t - alpha) << 16u;
					else
						d_tmp |= (temp->R[recip * t >> 8u] * alpha_adj >> 8u) << 16u;
					/* A */
					d_tmp |= d & 0xff000000u;

					dest[___index] = d_tmp;
				}
			}
		}
	}
}

TVP_GL_FUNC_DECL(void, TVPUninitGammaAdjustTempData_c, (tTVPGLGammaAdjustTempData *temp))
{
	/* nothing to do */
}
