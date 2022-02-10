
#pragma once

#include "tjsCommHead.h"

#define TVPInitGammaAdjustTempData TVPInitGammaAdjustTempData_c
#define TVPAdjustGamma TVPAdjustGamma_c
#define TVPAdjustGamma_a TVPAdjustGamma_a_c
#define TVPUninitGammaAdjustTempData TVPUninitGammaAdjustTempData_c

#define TVP_GL_FUNCNAME(funcname) funcname
#define TVP_GL_FUNC_DECL(rettype, funcname, arg)  rettype TVP_GL_FUNCNAME(funcname) arg
#define TVP_GL_FUNC_STATIC_DECL(rettype, funcname, arg)  static rettype TVP_GL_FUNCNAME(funcname) arg
#define TVP_GL_FUNC_EXTERN_DECL(rettype, funcname, arg)  extern rettype TVP_GL_FUNCNAME(funcname) arg
#define TVP_GL_FUNC_PTR_EXTERN_DECL TVP_GL_FUNC_EXTERN_DECL

TVP_GL_FUNC_PTR_EXTERN_DECL(void, TVPInitGammaAdjustTempData,  (tTVPGLGammaAdjustTempData *temp, const tTVPGLGammaAdjustData *data));
TVP_GL_FUNC_PTR_EXTERN_DECL(void, TVPAdjustGamma,  (tjs_uint32 *dest, tjs_int len, tTVPGLGammaAdjustTempData *temp));
TVP_GL_FUNC_PTR_EXTERN_DECL(void, TVPAdjustGamma_a,  (tjs_uint32 *dest, tjs_int len, tTVPGLGammaAdjustTempData *temp));
TVP_GL_FUNC_PTR_EXTERN_DECL(void, TVPUninitGammaAdjustTempData,  (tTVPGLGammaAdjustTempData *temp));
TVP_GL_FUNC_PTR_EXTERN_DECL(void, TVPInitDitherTable, (void));
