#include "pch.h"
#include "QuadShader.h"

extern float* QuadShader::c_quadVertices = QuadShader::convert_to_float_arr(QuadShader::get_vb_with_text("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC")); // max 32chars..
extern std::vector<unsigned short> QuadShader::c_quadIndices = QuadShader::get_ib_with_text("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");