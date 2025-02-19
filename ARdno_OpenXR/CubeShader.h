#include "pch.h"

namespace CubeShader {
	struct Vertex {
		XrVector3f Position;
		XrVector3f Color; // color == normal
		XrVector2f TexCoords;
	};

	//       POSITION       |      COLOR      | TEXCOORD
	constexpr Vertex c_cubeVertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // side 1 (front)
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // side 2 (right)
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // side 3 (back)
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // side 4 (left)
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // side 5 (bottom)
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // side 6 (top)
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	};

	constexpr unsigned short c_cubeIndices[] = {
		0, 1, 2, //front
		2, 3, 0,
		4, 5, 6, //right
		6, 7, 4,
		8, 9, 10, //back
		10, 11, 8,
		12, 13, 14, //left
		14, 15, 12,
		16, 17, 18, //bottom
		18, 19, 16,
		20, 21, 22, //top
		22, 23, 20,
	};

	struct ModelConstantBuffer {
		DirectX::XMFLOAT4X4 Model;
	};

	struct ViewProjectionConstantBuffer {
		DirectX::XMFLOAT4X4 ViewProjection[2];
	};

	struct ColorBuffer
	{
		DirectX::XMFLOAT4 Color;
	};

	struct LightPosBuffer
	{
		DirectX::XMFLOAT4 lightPos;
	};

	constexpr uint32_t MaxViewInstance = 2;

	// Separate entrypoints for the vertex and pixel shader functions.
	constexpr char ShaderHlsl[] = R"_(
            struct VSOutput {
                float4 Pos : SV_POSITION;
                float3 Color : COLOR0;
                float2 texCoord : TEXCOORD;
                float3 normal : NORMAL;
                float3 lightPos : LIGHTPOS;
                uint viewId : SV_RenderTargetArrayIndex;
            };
            struct VSInput {
                float3 Pos : POSITION;
                float3 Color : COLOR0;
                float2 texCoord : TEXCOORD;
                uint instId : SV_InstanceID;
            };
            cbuffer ModelConstantBuffer : register(b0) {
                float4x4 Model;
            };
            cbuffer ViewProjectionConstantBuffer : register(b1) {
                float4x4 ViewProjection[2];
            };
            cbuffer ColorBuffer : register(b2) {
                float4 Color;
            };
            cbuffer LightPosBuffer : register(b3) {
                float4 lightPos;
            };

            float4x4 inverse(float4x4 m) 
            {
                float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
                float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
                float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
                float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

                float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
                float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
                float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
                float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

                float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
                float idet = 1.0f / det;

                float4x4 ret;

                ret[0][0] = t11 * idet;
                ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
                ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
                ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

                ret[1][0] = t12 * idet;
                ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
                ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
                ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

                ret[2][0] = t13 * idet;
                ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
                ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
                ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

                ret[3][0] = t14 * idet;
                ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
                ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
                ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

                return ret;
            }

            VSOutput MainVS(VSInput input) {
                VSOutput output;
                output.Pos = mul(mul(float4(input.Pos, 1), Model), ViewProjection[input.instId]);
                output.Color = float3(Color.x, Color.y, Color.z);//input.Color;
                output.texCoord = input.texCoord;

                output.normal = mul((float3x3)inverse(transpose(Model)), input.Color);
                output.normal = input.Color;

                output.lightPos = lightPos.xyz;
                output.viewId = input.instId;
                return output;
            }

            Texture2D objTexture : TEXTURE : register(t0);
            SamplerState objSamplerState : SAMPLER : register(s0);

            float4 MainPS(VSOutput input) : SV_TARGET {
                float4 albedo = objTexture.Sample(objSamplerState, input.texCoord) * float4(input.Color.x, input.Color.y, input.Color.z, 1.0f);
                float ambientStrength = 0.3f;
                float3 norm = normalize(input.normal);
                float3 lightDir = normalize(input.lightPos - input.Pos.xyz);
                float diffuseStrength = max(dot(norm, lightDir), 0.0f);
                float3 diffuse = mul(diffuseStrength, float3(albedo.x, albedo.y, albedo.z));
                float3 ambient = albedo.xyz * ambientStrength;
                float3 result = mul((diffuseStrength + ambientStrength), albedo.xyz);
                float4 _result = float4(result, 1.0f);
                float4 _color = float4(ambient, 1.0f) + float4(diffuse, 1.0f);
                //norm.x = (norm.x + 1.0f) / 3;
                //norm.y = (norm.y + 1.0f) / 3;
                //norm.z = (norm.z + 1.0f) / 3;
                norm.x = abs(norm.x);
                norm.y = abs(norm.y);
                norm.z = abs(norm.z);

                _result = float4(norm.x, norm.y, norm.z, 1.0f);
                return _result;
            }
            )_";

} // namespace CubeShader