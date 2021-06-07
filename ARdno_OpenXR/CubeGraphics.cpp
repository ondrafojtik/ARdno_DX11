//*********************************************************
//    Copyright (c) Microsoft. All rights reserved.
//
//    Apache 2.0 License
//
//    You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
//*********************************************************

#include "pch.h"
#include "OpenXrProgram.h"
#include "DxUtility.h"
#include <DirectXTex.h>
#include <wrl\client.h>
#include <wincodec.h>
#include <WICTextureLoader.h>


#include <iostream>
#include <fstream>

#include "liver.h"
#include "arrow.h"
#include "coord_space_model.h"

#include "Bly7/OBJ_Loader.h"

#include "winrt/windows.Storage.h"
#include "winrt/windows.Foundation.h"
#include "winrt/windows.Foundation.Collections.h"

#include <experimental/resumable>
#include <experimental/coroutine>
#include <ppltasks.h>
#include <pplawait.h>

#include <filesystem>

#include "CubeShader.h"

// new ones
//#include "new_liver.h"
//#include "cancer_mesh.h"

namespace {
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

        std::vector<Vertex> get_vb_with_text(std::string text)
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

        float* convert_to_float_arr(std::vector<Vertex>& buffer)
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

        std::vector<unsigned short> get_ib_with_text(std::string text)
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
        float* c_quadVertices = convert_to_float_arr(get_vb_with_text("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC")); // max 32chars..
        std::vector<unsigned short> c_quadIndices = get_ib_with_text("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");

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

	struct Mesh
	{
		winrt::com_ptr<ID3D11Device> m_device;
		winrt::com_ptr<ID3D11DeviceContext> m_deviceContext;

		winrt::com_ptr<ID3D11Buffer> m_vertexBuffer;
		winrt::com_ptr<ID3D11Buffer> m_indexBuffer;

		std::vector<CubeShader::Vertex> vb;
		std::vector<unsigned short> ib;

		Mesh(std::vector<CubeShader::Vertex> vb, std::vector<unsigned short> ib)
		{
			this->vb = vb;
			this->ib = ib;
		}

		void bind()
		{
			const UINT strides_cube[] = { sizeof(CubeShader::Vertex) };
			ID3D11Buffer* vb_cube[] = { m_vertexBuffer.get() };
			const UINT offsets[] = { 0 };
			m_deviceContext->IASetVertexBuffers(0, (UINT)std::size({ m_vertexBuffer.get() }), vb_cube, strides_cube, offsets);
			m_deviceContext->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);

		}

		void create_buffers()
		{

			// creating vb
			D3D11_SUBRESOURCE_DATA vertexBufferData{ 0 };
			vertexBufferData.pSysMem = &vb[0];
			CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(CubeShader::Vertex) * vb.size(), D3D11_BIND_VERTEX_BUFFER);
			CHECK_HRCMD(m_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_vertexBuffer.put()));

			// creating ib
			D3D11_SUBRESOURCE_DATA indexBufferData{ 0 };
			indexBufferData.pSysMem = &ib[0];
			CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * ib.size(), D3D11_BIND_INDEX_BUFFER);
			CHECK_HRCMD(m_device->CreateBuffer(&indexBufferDesc, &indexBufferData, m_indexBuffer.put()));
		}
	};

	struct Model
	{
		std::vector<Mesh*> meshes;

		Model() { ; }
	};

	struct objLoader
	{
		std::vector<std::string> paths;
		std::vector<Model*> models;

		void init()
		{
			using namespace winrt;
			using namespace winrt::Windows::Foundation;
			using namespace winrt::Windows::Storage;

			models.push_back(new Model());

			StorageFolder picturesFolder = KnownFolders::GetFolderForUserAsync(nullptr, KnownFolderId::PicturesLibrary).get();
			Collections::IVectorView<StorageFolder> folderList = picturesFolder.GetFoldersAsync().get();

			// saved
			std::vector<std::string> obj_files_;
			std::string data_file;

			for (StorageFolder& folder : folderList)
			{
				std::string name = winrt::to_string(folder.DisplayName());


				// get the world data (meaning the objects from the file..)
				if (name == "ARdno_data")
				{
					StorageFolder dataFolder = folder;
					Collections::IVectorView<StorageFile> data_files = dataFolder.GetFilesAsync().get();
					for (StorageFile const& file : data_files)
					{
						std::string name = winrt::to_string(file.Name());
						data_file = name;
					}
				}
				// loading actuall models..
				if (name == "ARdno_obj")
				{
					StorageFolder objFolder = folder;
					Collections::IVectorView<StorageFile> obj_files = objFolder.GetFilesAsync().get();
					for (StorageFile const& file : obj_files)
					{
						std::string name = winrt::to_string(file.Name());
						obj_files_.push_back(name);

						// read text of file
						hstring text = FileIO::ReadTextAsync(file, Streams::UnicodeEncoding::Utf8).get();
						std::string _text = winrt::to_string(text);

						Model* model = new Model();

						// parser
						{
							std::vector<std::string> _vertices;
							std::vector<std::string> _indices;

							std::string word = "";

							bool after_ast = false;
							for (char c : _text)
							{
								// new mesh
								if (c == '~')
								{
									std::vector<CubeShader::Vertex> vertices;
									std::vector<unsigned short> indices;

									for (int i = 0; i < _vertices.size() / 8; i++)
									{
										CubeShader::Vertex v{};
										v.Position.x = std::stof(_vertices[(i * 8) + 0]);
										v.Position.y = std::stof(_vertices[(i * 8) + 1]);
										v.Position.z = std::stof(_vertices[(i * 8) + 2]);

										v.Color.x = std::stof(_vertices[(i * 8) + 3]);
										v.Color.y = std::stof(_vertices[(i * 8) + 4]);
										v.Color.z = std::stof(_vertices[(i * 8) + 5]);

										v.TexCoords.x = std::stof(_vertices[(i * 8) + 6]);
										v.TexCoords.y = std::stof(_vertices[(i * 8) + 7]);

										vertices.push_back(v);
									}

									for (std::string ind : _indices)
									{
										unsigned int number = (unsigned short)std::strtoul(ind.c_str(), NULL, 0);
										indices.push_back(number);
									}

									Mesh* m = new Mesh(vertices, indices);
									model->meshes.push_back(m);

									vertices.clear();
									indices.clear();
									_vertices.clear();
									_indices.clear();
									after_ast = 0;
								}



								if (c == '*')
									after_ast = 1;

								if (c == ',')
								{
									if (after_ast)
										_indices.push_back(word);
									else
										_vertices.push_back(word);

									word = "";
								}
								else if (c != '~')
									word += c;
							}
						}

						if (name == "space_origin2.txt")
							models[0] = model;
						else
							models.push_back(model);

					}
				}


			}
		}
	};

	struct CubeGraphics : sample::IGraphicsPluginD3D11 {
		ID3D11Device* InitializeDevice(LUID adapterLuid, const std::vector<D3D_FEATURE_LEVEL>& featureLevels) override {
			const winrt::com_ptr<IDXGIAdapter1> adapter = sample::dx::GetAdapter(adapterLuid);

			sample::dx::CreateD3D11DeviceAndContext(adapter.get(), featureLevels, m_device.put(), m_deviceContext.put());

			InitializeD3DResources();

			return m_device.get();
		}

		void InitializeD3DResources() {


			const winrt::com_ptr<ID3DBlob> vertexShaderBytes = sample::dx::CompileShader(CubeShader::ShaderHlsl, "MainVS", "vs_5_0");
			CHECK_HRCMD(m_device->CreateVertexShader(
				vertexShaderBytes->GetBufferPointer(), vertexShaderBytes->GetBufferSize(), nullptr, m_CubeVertexShader.put()));

			const winrt::com_ptr<ID3DBlob> pixelShaderBytes = sample::dx::CompileShader(CubeShader::ShaderHlsl, "MainPS", "ps_5_0");
			CHECK_HRCMD(m_device->CreatePixelShader(
				pixelShaderBytes->GetBufferPointer(), pixelShaderBytes->GetBufferSize(), nullptr, m_CubePixelShader.put()));


			const winrt::com_ptr<ID3DBlob> _vertexShaderBytes = sample::dx::CompileShader(QuadShader::ShaderHlsl, "MainVS", "vs_5_0");
			CHECK_HRCMD(m_device->CreateVertexShader(
				_vertexShaderBytes->GetBufferPointer(), _vertexShaderBytes->GetBufferSize(), nullptr, m_QuadVertexShader.put()));

			const winrt::com_ptr<ID3DBlob> _pixelShaderBytes = sample::dx::CompileShader(QuadShader::ShaderHlsl, "MainPS", "ps_5_0");
			CHECK_HRCMD(m_device->CreatePixelShader(
				_pixelShaderBytes->GetBufferPointer(), _pixelShaderBytes->GetBufferSize(), nullptr, m_QuadPixelShader.put()));


			const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			CHECK_HRCMD(m_device->CreateInputLayout(vertexDesc,
				(UINT)std::size(vertexDesc),
				vertexShaderBytes->GetBufferPointer(),
				vertexShaderBytes->GetBufferSize(),
				m_CubeInputLayout.put()));

			CHECK_HRCMD(m_device->CreateInputLayout(vertexDesc,
				(UINT)std::size(vertexDesc),
				_vertexShaderBytes->GetBufferPointer(),
				_vertexShaderBytes->GetBufferSize(),
				m_QuadInputLayout.put()));



			// init of CubeShader
			const CD3D11_BUFFER_DESC modelConstantBufferDesc(sizeof(CubeShader::ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			CHECK_HRCMD(m_device->CreateBuffer(&modelConstantBufferDesc, nullptr, m_modelCBuffer.put()));

			const CD3D11_BUFFER_DESC viewProjectionConstantBufferDesc(sizeof(CubeShader::ViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			CHECK_HRCMD(m_device->CreateBuffer(&viewProjectionConstantBufferDesc, nullptr, m_viewProjectionCBuffer.put()));

			const CD3D11_BUFFER_DESC colorConstantBufferDesc(sizeof(CubeShader::ColorBuffer), D3D11_BIND_CONSTANT_BUFFER);
			CHECK_HRCMD(m_device->CreateBuffer(&colorConstantBufferDesc, nullptr, m_colorCBuffer.put()));

			const CD3D11_BUFFER_DESC lightPosConstantBufferDesc(sizeof(CubeShader::LightPosBuffer), D3D11_BIND_CONSTANT_BUFFER);
			CHECK_HRCMD(m_device->CreateBuffer(&lightPosConstantBufferDesc, nullptr, m_lightPosCBuffer.put()));



			// initing the VB's and IB's
			loader.init();
			for (Model* model_ : loader.models)
			{
				for (Mesh* m : model_->meshes)
				{
					m->m_device = m_device;
					m->m_deviceContext = m_deviceContext;
					m->create_buffers();
				}
			}

			{
				// Dynamic VB
				D3D11_BUFFER_DESC vertexBufferDesc;
				ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
				vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				vertexBufferDesc.ByteWidth = sizeof(QuadShader::Vertex) * QuadShader::get_vb_with_text("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC").size(); // 8 * 32
				vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				D3D11_SUBRESOURCE_DATA initial_data{ QuadShader::c_quadVertices };
				HRESULT hr = m_device->CreateBuffer(&vertexBufferDesc, &initial_data, m_quadVertexBuffer.put());

				// Dynamic IB
				D3D11_BUFFER_DESC indexBufferDesc;
				ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
				indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				indexBufferDesc.ByteWidth = sizeof(unsigned short) * QuadShader::get_ib_with_text("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC").size();;
				indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				D3D11_SUBRESOURCE_DATA data{ QuadShader::c_quadIndices.data() };
				hr = m_device->CreateBuffer(&indexBufferDesc, &data, m_quadIndexBuffer.put());
			}

			// other renderer options and shit
			D3D11_FEATURE_DATA_D3D11_OPTIONS3 options;
			m_device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &options, sizeof(options));
			CHECK_MSG(options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer,
				"This sample requires VPRT support. Adjust sample shaders on GPU without VRPT.");

			CD3D11_DEPTH_STENCIL_DESC  depthStencilDesc(CD3D11_DEFAULT{});
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			CHECK_HRCMD(m_device->CreateDepthStencilState(&depthStencilDesc, m_reversedZDepthNoStencilTest.put()));

			// alpha blending
			ID3D11BlendState* blend_state;
			D3D11_BLEND_DESC blend_desc;
			float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			ZeroMemory(&blend_desc, sizeof(blend_desc));
			blend_desc.RenderTarget[0].BlendEnable = true;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			m_device->CreateBlendState(&blend_desc, &blend_state);
			m_deviceContext->OMSetBlendState(blend_state, blend_factor, 0xFFFFFFFF);

			// culling
			ID3D11RasterizerState* rasterizer_state;
			D3D11_RASTERIZER_DESC rasterizer_desc;
			rasterizer_desc.FillMode = D3D11_FILL_SOLID;
			rasterizer_desc.CullMode = D3D11_CULL_NONE;
			rasterizer_desc.FrontCounterClockwise = FALSE;
			rasterizer_desc.DepthBias = 0;
			rasterizer_desc.SlopeScaledDepthBias = 0.0f;
			rasterizer_desc.DepthBias = 0.0f;
			rasterizer_desc.DepthClipEnable = TRUE;
			rasterizer_desc.ScissorEnable = FALSE;
			rasterizer_desc.MultisampleEnable = FALSE;
			rasterizer_desc.AntialiasedLineEnable = FALSE;
			HRESULT h = m_device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
			m_deviceContext->RSSetState(rasterizer_state);

			// creating sampler
			D3D11_SAMPLER_DESC sampler;
			ZeroMemory(&sampler, sizeof(sampler));
			sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampler.MinLOD = 0;
			sampler.MaxLOD = D3D11_FLOAT32_MAX;
			m_device->CreateSamplerState(&sampler, sampler_state.GetAddressOf());

			// creating textures
			HRESULT hr = Windows::Foundation::Initialize(RO_INIT_TYPE::RO_INIT_MULTITHREADED);
			hr = DirectX::CreateWICTextureFromFile(m_device.get(), L"Assets\\font_sheet_5.png", nullptr, font_texture.GetAddressOf());
			hr = DirectX::CreateWICTextureFromFile(m_device.get(), L"Assets\\StoreLogo.png", nullptr, cube_texture.GetAddressOf());
			hr = DirectX::CreateWICTextureFromFile(m_device.get(), L"Assets\\Blank.png", nullptr, blank_texture.GetAddressOf());
			hr = DirectX::CreateWICTextureFromFile(m_device.get(), L"Assets\\space_origin2_final.png", nullptr, space_origin_texture.GetAddressOf());


		}

		const std::vector<DXGI_FORMAT>& SupportedColorFormats() const override {
			const static std::vector<DXGI_FORMAT> SupportedColorFormats = {
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
			};
			return SupportedColorFormats;
		}

		const std::vector<DXGI_FORMAT>& SupportedDepthFormats() const override {
			const static std::vector<DXGI_FORMAT> SupportedDepthFormats = {
				DXGI_FORMAT_D32_FLOAT,
				DXGI_FORMAT_D16_UNORM,
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
			};
			return SupportedDepthFormats;
		}

		void RenderView(const XrRect2Di& imageRect,
			const float renderTargetClearColor[4],
			const std::vector<xr::math::ViewProjection>& viewProjections,
			DXGI_FORMAT colorSwapchainFormat,
			ID3D11Texture2D* colorTexture,
			DXGI_FORMAT depthSwapchainFormat,
			ID3D11Texture2D* depthTexture,
			const std::vector<const sample::Cube*>& cubes,
			const std::vector<const sample::Cube*>& quads,
			const sample::Cube& light,
			const std::vector<const sample::Cube*>& space_origins,
			bool should_render_next_hologram) override {
			const uint32_t viewInstanceCount = (uint32_t)viewProjections.size();
			CHECK_MSG(viewInstanceCount <= CubeShader::MaxViewInstance,
				"Sample shader supports 2 or fewer view instances. Adjust shader to accommodate more.")

				CD3D11_VIEWPORT viewport(
					(float)imageRect.offset.x, (float)imageRect.offset.y, (float)imageRect.extent.width, (float)imageRect.extent.height);
			m_deviceContext->RSSetViewports(1, &viewport);

			// Create RenderTargetView with the original swapchain format (swapchain image is typeless).
			winrt::com_ptr<ID3D11RenderTargetView> renderTargetView;
			const CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DARRAY, colorSwapchainFormat);
			CHECK_HRCMD(m_device->CreateRenderTargetView(colorTexture, &renderTargetViewDesc, renderTargetView.put()));

			// Create a DepthStencilView with the original swapchain format (swapchain image is typeless)
			winrt::com_ptr<ID3D11DepthStencilView> depthStencilView;
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DARRAY, depthSwapchainFormat);
			CHECK_HRCMD(m_device->CreateDepthStencilView(depthTexture, &depthStencilViewDesc, depthStencilView.put()));

			const bool reversedZ = viewProjections[0].NearFar.Near > viewProjections[0].NearFar.Far;
			const float depthClearValue = reversedZ ? 0.f : 1.f;

			// Clear swapchain and depth buffer. NOTE: This will clear the entire render target view, not just the specified view.
			m_deviceContext->ClearRenderTargetView(renderTargetView.get(), renderTargetClearColor);
			m_deviceContext->ClearDepthStencilView(depthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthClearValue, 0);
			m_deviceContext->OMSetDepthStencilState(reversedZ ? m_reversedZDepthNoStencilTest.get() : nullptr, 0);

			ID3D11RenderTargetView* renderTargets[] = { renderTargetView.get() };
			m_deviceContext->OMSetRenderTargets((UINT)std::size(renderTargets), renderTargets, depthStencilView.get());

			CubeShader::ColorBuffer colorCBuffer;
			//colorCBuffer.Color = DirectX::XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
			colorCBuffer.Color = DirectX::XMFLOAT4(0.85f, 0.2f, 0.1f, 1.0f);

			CubeShader::LightPosBuffer lightPosCBuffer;
			lightPosCBuffer.lightPos = DirectX::XMFLOAT4(light.PoseInAppSpace.position.x, light.PoseInAppSpace.position.y, light.PoseInAppSpace.position.z, 1.0f);


			m_deviceContext->UpdateSubresource(m_colorCBuffer.get(), 0, nullptr, &colorCBuffer, 0, 0);
			m_deviceContext->UpdateSubresource(m_lightPosCBuffer.get(), 0, nullptr, &lightPosCBuffer, 0, 0);

			ID3D11Buffer* const constantBuffers[] = { m_modelCBuffer.get(), m_viewProjectionCBuffer.get(), m_colorCBuffer.get(), m_lightPosCBuffer.get() };
			m_deviceContext->VSSetConstantBuffers(0, (UINT)std::size(constantBuffers), constantBuffers);
			m_deviceContext->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

			CubeShader::ViewProjectionConstantBuffer viewProjectionCBufferData{};

			for (uint32_t k = 0; k < viewInstanceCount; k++) {
				const DirectX::XMMATRIX spaceToView = xr::math::LoadInvertedXrPose(viewProjections[k].Pose);
				const DirectX::XMMATRIX projectionMatrix = ComposeProjectionMatrix(viewProjections[k].Fov, viewProjections[k].NearFar);

				// Set view projection matrix for each view, transpose for shader usage.
				DirectX::XMStoreFloat4x4(&viewProjectionCBufferData.ViewProjection[k],
					DirectX::XMMatrixTranspose(spaceToView * projectionMatrix));
			}
			m_deviceContext->UpdateSubresource(m_viewProjectionCBuffer.get(), 0, nullptr, &viewProjectionCBufferData, 0, 0);

			m_deviceContext->VSSetShader(m_CubeVertexShader.get(), nullptr, 0);
			m_deviceContext->PSSetShader(m_CubePixelShader.get(), nullptr, 0);


			// stupid switch for the holo switch action..
			if (should_render_next_hologram == true)
			{
				if (id_hologram_to_render == loader.models.size() - 1)
					id_hologram_to_render = 0;
				else
					id_hologram_to_render += 1;
			}

			Model* currModel = loader.models[0];

			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_deviceContext->IASetInputLayout(m_CubeInputLayout.get());
			m_deviceContext->PSSetShaderResources(0, 1, space_origin_texture.GetAddressOf());

			// Render each space origin (meaninig right hand and current origin)
			for (const sample::Cube* cube : space_origins)
			{
				for (Mesh* m : currModel->meshes)
				{
					m->bind();
					CubeShader::ModelConstantBuffer model;
					const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(0.4f, 0.4f, 0.4f);
					DirectX::XMStoreFloat4x4(&model.Model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, 0) * scaleMatrix * xr::math::LoadXrPose(cube->PoseInAppSpace)));
					m_deviceContext->UpdateSubresource(m_modelCBuffer.get(), 0, nullptr, &model, 0, 0);

					m_deviceContext->DrawIndexedInstanced((UINT)m->ib.size(), viewInstanceCount, 0, 0, 0);
				}
			}

			currModel = loader.models[id_hologram_to_render];
			m_deviceContext->PSSetShaderResources(0, 1, blank_texture.GetAddressOf());

			// Render each cube
			// TODO: rewrite this thing..

			for (const sample::Cube* cube : cubes)
			{
				for (Mesh* m : currModel->meshes)
				{
					m->bind();
					CubeShader::ModelConstantBuffer model;
					const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(0.2f, 0.2f, 0.2f);
					DirectX::XMStoreFloat4x4(&model.Model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, 0) * scaleMatrix * xr::math::LoadXrPose(cube->PoseInAppSpace)));
					m_deviceContext->UpdateSubresource(m_modelCBuffer.get(), 0, nullptr, &model, 0, 0);
					m_deviceContext->DrawIndexedInstanced((UINT)m->ib.size(), viewInstanceCount, 0, 0, 0);
				}
			}

			m_deviceContext->VSSetShader(m_QuadVertexShader.get(), nullptr, 0);
			m_deviceContext->PSSetShader(m_QuadPixelShader.get(), nullptr, 0);
			const UINT strides_quad[] = { sizeof(CubeShader::Vertex) };
			ID3D11Buffer* vb_quad[] = { m_quadVertexBuffer.get() };
			const UINT offsets[] = { 0 };
			m_deviceContext->IASetVertexBuffers(0, (UINT)std::size({ m_quadVertexBuffer.get() }), vb_quad, strides_quad, offsets);
			m_deviceContext->IASetIndexBuffer(m_quadIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_deviceContext->IASetInputLayout(m_QuadInputLayout.get());

			m_deviceContext->PSSetShaderResources(0, 1, font_texture.GetAddressOf());

			for (const sample::Cube* quad : quads)
			{
				QuadShader::ModelConstantBuffer model;
				const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(quad->Scale.x * 2, quad->Scale.y * 2, quad->Scale.z * 2);
				DirectX::XMStoreFloat4x4(&model.Model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, 0)
					* scaleMatrix * xr::math::LoadXrPose(quad->PoseInAppSpace)));
				m_deviceContext->UpdateSubresource(m_modelCBuffer.get(), 0, nullptr, &model, 0, 0);


				std::vector<QuadShader::Vertex> cc = QuadShader::get_vb_with_text("OLA");
				std::string word_to_render = quad->text;
				{
					// VB
					D3D11_MAPPED_SUBRESOURCE resource;
					HRESULT hr = m_deviceContext->Map(m_quadVertexBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
					memcpy(resource.pData, static_cast<void*>(QuadShader::get_vb_with_text(word_to_render).data()), QuadShader::get_vb_with_text(word_to_render).size() * sizeof(float) * 8);
					m_deviceContext->Unmap(m_quadVertexBuffer.get(), 0);
				}

				{
					// IB
					D3D11_MAPPED_SUBRESOURCE resource;
					HRESULT hr = m_deviceContext->Map(m_quadIndexBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
					memcpy(resource.pData, static_cast<void*>(QuadShader::get_ib_with_text(word_to_render).data()), QuadShader::get_ib_with_text(word_to_render).size() * sizeof(unsigned short));
					m_deviceContext->Unmap(m_quadIndexBuffer.get(), 0);
				}

				const UINT strides_quad[] = { sizeof(float) * 8 };
				ID3D11Buffer* vb_quad[] = { m_quadVertexBuffer.get() };
				m_deviceContext->IASetVertexBuffers(0, (UINT)std::size({ m_quadVertexBuffer.get() }), vb_quad, strides_quad, offsets);
				m_deviceContext->IASetIndexBuffer(m_quadIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);

				m_deviceContext->DrawIndexedInstanced((UINT)std::size(QuadShader::c_quadIndices), viewInstanceCount, 0, 0, 0);

			}

		}

	private:
		winrt::com_ptr<ID3D11Device> m_device;
		winrt::com_ptr<ID3D11DeviceContext> m_deviceContext;
		winrt::com_ptr<ID3D11VertexShader> m_CubeVertexShader;
		winrt::com_ptr<ID3D11PixelShader> m_CubePixelShader;
		winrt::com_ptr<ID3D11VertexShader> m_QuadVertexShader;
		winrt::com_ptr<ID3D11PixelShader> m_QuadPixelShader;
		winrt::com_ptr<ID3D11InputLayout> m_CubeInputLayout;
		winrt::com_ptr<ID3D11InputLayout> m_QuadInputLayout;
		winrt::com_ptr<ID3D11Buffer> m_modelCBuffer;
		winrt::com_ptr<ID3D11Buffer> m_viewProjectionCBuffer;
		winrt::com_ptr<ID3D11Buffer> m_colorCBuffer;
		winrt::com_ptr<ID3D11Buffer> m_lightPosCBuffer;
		winrt::com_ptr<ID3D11Buffer> m_quadVertexBuffer;
		winrt::com_ptr<ID3D11Buffer> m_quadIndexBuffer;
		winrt::com_ptr<ID3D11DepthStencilState> m_reversedZDepthNoStencilTest;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> font_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cube_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> blank_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> space_origin_texture;

		objLoader loader{};
		int32_t id_hologram_to_render = 0;

	};
} // namespace

namespace sample {
	std::unique_ptr<sample::IGraphicsPluginD3D11> CreateCubeGraphics() {
		return std::make_unique<CubeGraphics>();
	}
} // namespace sample