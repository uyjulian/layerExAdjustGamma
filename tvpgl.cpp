// Copied tvpgl functions, because we can't rely on the core to have them.

#include "tvpgl.h"
#include <math.h>
#include <simde/x86/mmx.h>

#define BYTEn(x, n)  (*((tjs_uint8 *)&(x) + n))
#define BYTE1(x)  BYTEn(x, 1)

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
	tjs_uint32 * v3;  // edi
	tjs_uint32 * v4;  // esi
	tjs_uint32   v5;  // eax
	simde__m64    v6;  // mm2
	simde__m64    v9;  // mm1
	simde__m64    v10; // mm3
	simde__m64    v11; // mm3
	simde__m64    v12; // mm6
	tjs_uint32   v13; // eax

	v3 = dest;
	v4 = &dest[len];
	if (dest < v4)
	{
		do
		{
			while (1)
			{
				v5 = *v3;
				if (*v3 < 0xFF000000)
					break;
				*v3 = (temp->R[(v5 >> 16) & 0xFF] << 16) | (temp->G[BYTE1(v5)] << 8) | temp->B[(tjs_uint8)v5] | 0xFF000000;
			_TVPAdjustGamma_a_mmx_pfraction_a_ptransp:
				++v3;
				if (v3 >= v4)
					goto _TVPAdjustGamma_a_mmx_pfraction_a_pexit;
			}
			if (!v5)
				goto _TVPAdjustGamma_a_mmx_pfraction_a_ptransp;
			v6  = simde_m_punpcklbw(simde_mm_cvtsi32_si64(v5), simde_mm_setzero_si64());
			v9  = simde_mm_set1_pi16(TVPRecipTable256_16[v5 >> 24]);
			v10 = simde_mm_set1_pi16(v5 >> 24);
			v11 = v10;
			v12 = simde_mm_cvtsi32_si64(v5 & 0xFF000000);
			v13 = simde_mm_cvtsi64_si32(simde_m_packuswb(simde_m_por(simde_m_psrlwi(simde_m_pmullw(v9, v6), 8u), simde_m_psrlwi(simde_m_pcmpgtw(v6, v11), 8u)), simde_mm_setzero_si64()));
			++v3;
			*(v3 - 1) = simde_mm_cvtsi64_si32(
				simde_m_por(
					simde_m_packuswb(
						simde_m_paddw(
							simde_m_psrlwi(
								simde_m_pmullw(
									simde_m_punpcklbw(
										simde_mm_cvtsi32_si64((temp->R[(v13 >> 16) & 0xFF] << 16) | (temp->G[BYTE1(v13)] << 8) | (tjs_uint32)temp->B[(tjs_uint8)v13]),
										simde_mm_setzero_si64()),
									simde_m_paddw(v11, simde_m_psrlwi(v11, 7u))),
								8u),
							simde_m_psubusb(v6, v11)),
						simde_mm_setzero_si64()),
					v12));
		} while (v3 < v4);
	}
_TVPAdjustGamma_a_mmx_pfraction_a_pexit:
	simde_m_empty();
}

TVP_GL_FUNC_DECL(void, TVPUninitGammaAdjustTempData_c, (tTVPGLGammaAdjustTempData *temp))
{
	/* nothing to do */
}
