#include "pch.h"

namespace QuadShader {
	struct Vertex {
		XrVector3f Position;
		XrVector3f Color;
		XrVector2f TexCoords;
	};

	struct ModelConstantBuffer {
		DirectX::XMFLOAT4X4 Model;
	};

	struct ViewProjectionConstantBuffer {
		DirectX::XMFLOAT4X4 ViewProjection[2];
	};
	constexpr uint32_t MaxViewInstance = 2;

	inline std::vector<Vertex> get_vb_with_text(std::string text)
	{
		int32_t size = text.size();
		std::vector<Vertex> _vertices;

		auto reverse_lerp = [](float value, float min, float max) -> float
		{
			float val = (value - min) / (max - min);
			val = val - 0.5f;
			return val;
		};

		auto get_coords = [](char32_t c) -> XrVector2f*
		{
			XrVector2f sprite_size;
			sprite_size.x = 32.0f;
			sprite_size.y = 32.0f;

			XrVector2f texture_size;
			texture_size.x = 4096.0f;
			texture_size.y = 32.0f;

			int32_t x = c - ' ';
			float _magic = 0.999f;

			XrVector2f* coords = new XrVector2f[4];
			coords[0] = { (x * sprite_size.x) / texture_size.x, 0.0f };
			coords[1] = { ((x + 1) * sprite_size.x) / (texture_size.x) * _magic, 0.0f };
			coords[2] = { ((x + 1) * sprite_size.x) / (texture_size.x) * _magic, 1.0f };
			coords[3] = { (x * sprite_size.x) / (texture_size.x), 1.0f };

			return coords;
		};

		for (int i = 0; i < size; i++)
		{
			XrVector2f* coords = get_coords(text[i]);

			// Vertex 1
			{
				Vertex v;
				v.Position.x = reverse_lerp(i, 0, (float)size);
				v.Position.y = -0.5f * (1 / (float)size);
				v.Position.z = 0.0f;

				v.Color = { 1.0f, 1.0f, 1.0f };

				v.TexCoords = coords[0];
				_vertices.push_back(v);
			}
			// Vertex 2
			{
				Vertex v;
				v.Position.x = reverse_lerp(i + 1, 0, (float)size);
				v.Position.y = -0.5f * (1 / (float)size);
				v.Position.z = 0.0f;

				v.Color = { 1.0f, 1.0f, 1.0f };

				v.TexCoords = coords[1];
				_vertices.push_back(v);
			}
			// Vertex 3
			{
				Vertex v;
				v.Position.x = reverse_lerp(i + 1, 0, (float)size);
				v.Position.y = 0.5f * (1 / (float)size);
				v.Position.z = 0.0f;

				v.Color = { 1.0f, 1.0f, 1.0f };

				v.TexCoords = coords[2];
				_vertices.push_back(v);
			}
			// Vertex 4
			{
				Vertex v;
				v.Position.x = reverse_lerp(i, 0, (float)size);
				v.Position.y = 0.5f * (1 / (float)size);
				v.Position.z = 0.0f;

				v.Color = { 1.0f, 1.0f, 1.0f };

				v.TexCoords = coords[3];
				_vertices.push_back(v);
			}

		}

		Vertex* vertices = &_vertices[0];
		return _vertices;
	}

	inline float* convert_to_float_arr(std::vector<Vertex>& buffer)
	{
		int32_t vertex_size = (3 + 3 + 2);
		int32_t size = buffer.size() * vertex_size;
		float* final_buffer = new float[size];
		for (int i = 0; i < buffer.size(); i++)
		{
			final_buffer[(i * vertex_size) + 0] = buffer[i].Position.x;
			final_buffer[(i * vertex_size) + 1] = buffer[i].Position.y;
			final_buffer[(i * vertex_size) + 2] = buffer[i].Position.z;

			final_buffer[(i * vertex_size) + 3] = buffer[i].Color.x;
			final_buffer[(i * vertex_size) + 4] = buffer[i].Color.y;
			final_buffer[(i * vertex_size) + 5] = buffer[i].Color.z;

			final_buffer[(i * vertex_size) + 6] = buffer[i].TexCoords.x;
			final_buffer[(i * vertex_size) + 7] = buffer[i].TexCoords.y;
		}

		return final_buffer;

		// TODO: is this actually something Ive gotta do? I mean, cant I just do some &buffer[0] or smth?

	}

	inline std::vector<unsigned short> get_ib_with_text(std::string text)
	{
		int32_t size = text.size();

		std::vector<unsigned short> _indices;

		for (int i = 0; i < size; i++)
		{
			int32_t init_value = i * 4;
			_indices.push_back(init_value + 0);
			_indices.push_back(init_value + 1);
			_indices.push_back(init_value + 2);

			_indices.push_back(init_value + 2);
			_indices.push_back(init_value + 3);
			_indices.push_back(init_value + 0);

		}

		unsigned short* indices = &_indices[0];
		return _indices;
	}

	// TODO: inconsistency. Why is one a vector and the other float* (&indices[0]?...)
	extern float* c_quadVertices;
	extern std::vector<unsigned short> c_quadIndices;

	// Separate entrypoints for the vertex and pixel shader functions.
	constexpr char ShaderHlsl[] = R"_(
            struct VSOutput {
                float4 Pos : SV_POSITION;
                float3 Color : COLOR0;
                float2 texCoord : TEXCOORD;
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
            VSOutput MainVS(VSInput input) {
                VSOutput output;
                output.Pos = mul(mul(float4(input.Pos, 1), Model), ViewProjection[input.instId]);
                output.Color = input.Color;
                output.texCoord = input.texCoord;
                output.viewId = input.instId;
                return output;
            }

            Texture2D objTexture : TEXTURE : register(t0);
            SamplerState objSamplerState : SAMPLER : register(s0);

            float4 MainPS(VSOutput input) : SV_TARGET {
                float4 final_color = objTexture.Sample(objSamplerState, input.texCoord);
                //float4 color = final_color * float4(input.texCoord.x, 0.0f, input.texCoord.y, 1.0f);
                return final_color;
            }
            )_";

} // namespace QuadShader