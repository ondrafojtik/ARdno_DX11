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


namespace {
    namespace CubeShader {
        struct Vertex {
            XrVector3f Position;
            XrVector3f Color;
            XrVector2f TexCoords;
        };

        constexpr XrVector3f Red{1, 0, 0};
        constexpr XrVector3f DarkRed{0.25f, 0, 0};
        constexpr XrVector3f Green{0, 1, 0};
        constexpr XrVector3f DarkGreen{0, 0.25f, 0};
        constexpr XrVector3f Blue{0, 0, 1};
        constexpr XrVector3f DarkBlue{0, 0, 0.25f};

        // Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
        constexpr XrVector3f LBB{-0.5f, -0.5f, -0.5f};
        constexpr XrVector3f LBF{-0.5f, -0.5f, 0.5f};
        constexpr XrVector3f LTB{-0.5f, 0.5f, -0.5f};
        constexpr XrVector3f LTF{-0.5f, 0.5f, 0.5f};
        constexpr XrVector3f RBB{0.5f, -0.5f, -0.5f};
        constexpr XrVector3f RBF{0.5f, -0.5f, 0.5f};
        constexpr XrVector3f RTB{0.5f, 0.5f, -0.5f};
        constexpr XrVector3f RTF{0.5f, 0.5f, 0.5f};



#define CUBE_SIDE(V1, V2, V3, V4, V5, V6, COLOR) {V1, COLOR}, {V2, COLOR}, {V3, COLOR}, {V4, COLOR}, {V5, COLOR}, {V6, COLOR},


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

        constexpr uint32_t MaxViewInstance = 2;

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
                return final_color;
            }
            )_";

    } // namespace CubeShader

    namespace QuadShader {
        struct Vertex {
            XrVector3f Position;
            XrVector3f Color;
            XrVector2f TexCoords;
        };

        constexpr XrVector3f Red{ 1, 0, 0 };
        constexpr XrVector3f DarkRed{ 0.25f, 0, 0 };
        constexpr XrVector3f Green{ 0, 1, 0 };
        constexpr XrVector3f DarkGreen{ 0, 0.25f, 0 };
        constexpr XrVector3f Blue{ 0, 0, 1 };
        constexpr XrVector3f DarkBlue{ 0, 0, 0.25f };

        // Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
        constexpr XrVector3f LBB{ -0.5f, -0.5f, -0.5f };
        constexpr XrVector3f LBF{ -0.5f, -0.5f, 0.5f };
        constexpr XrVector3f LTB{ -0.5f, 0.5f, -0.5f };
        constexpr XrVector3f LTF{ -0.5f, 0.5f, 0.5f };
        constexpr XrVector3f RBB{ 0.5f, -0.5f, -0.5f };
        constexpr XrVector3f RBF{ 0.5f, -0.5f, 0.5f };
        constexpr XrVector3f RTB{ 0.5f, 0.5f, -0.5f };
        constexpr XrVector3f RTF{ 0.5f, 0.5f, 0.5f };

        //       POSITION       |      COLOR      | TEXCOORD
        constexpr Vertex c_quadVertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // side 1 (front)
             0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        };

        constexpr unsigned short c_quadIndices[] = {
            0, 1, 2, //front
            2, 3, 0,
        };

        struct ModelConstantBuffer {
            DirectX::XMFLOAT4X4 Model;
        };

        struct ViewProjectionConstantBuffer {
            DirectX::XMFLOAT4X4 ViewProjection[2];
        };

        constexpr uint32_t MaxViewInstance = 2;

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
                return final_color;
            }
            )_";

    } // namespace QuadShader


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
                vertexShaderBytes->GetBufferPointer(), vertexShaderBytes->GetBufferSize(), nullptr, m_vertexShader.put()));

            const winrt::com_ptr<ID3DBlob> pixelShaderBytes = sample::dx::CompileShader(CubeShader::ShaderHlsl, "MainPS", "ps_5_0");
            CHECK_HRCMD(m_device->CreatePixelShader(
                pixelShaderBytes->GetBufferPointer(), pixelShaderBytes->GetBufferSize(), nullptr, m_pixelShader.put()));

            const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };

            CHECK_HRCMD(m_device->CreateInputLayout(vertexDesc,
                                                    (UINT)std::size(vertexDesc),
                                                    vertexShaderBytes->GetBufferPointer(),
                                                    vertexShaderBytes->GetBufferSize(),
                                                    m_inputLayout.put()));
            // init of CubeShader
            const CD3D11_BUFFER_DESC modelConstantBufferDesc(sizeof(CubeShader::ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
            CHECK_HRCMD(m_device->CreateBuffer(&modelConstantBufferDesc, nullptr, m_modelCBuffer.put()));

            const CD3D11_BUFFER_DESC viewProjectionConstantBufferDesc(sizeof(CubeShader::ViewProjectionConstantBuffer),
                                                                      D3D11_BIND_CONSTANT_BUFFER);
            CHECK_HRCMD(m_device->CreateBuffer(&viewProjectionConstantBufferDesc, nullptr, m_viewProjectionCBuffer.put()));

            // cube
            {
                const D3D11_SUBRESOURCE_DATA vertexBufferData{CubeShader::c_cubeVertices};
                const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(CubeShader::c_cubeVertices), D3D11_BIND_VERTEX_BUFFER);
                CHECK_HRCMD(m_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_cubeVertexBuffer.put()));

                const D3D11_SUBRESOURCE_DATA indexBufferData{CubeShader::c_cubeIndices};
                const CD3D11_BUFFER_DESC indexBufferDesc(sizeof(CubeShader::c_cubeIndices), D3D11_BIND_INDEX_BUFFER);
                CHECK_HRCMD(m_device->CreateBuffer(&indexBufferDesc, &indexBufferData, m_cubeIndexBuffer.put()));
            }
            // quad
            {
                const D3D11_SUBRESOURCE_DATA vertexBufferData{ QuadShader::c_quadVertices };
                const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(QuadShader::c_quadVertices), D3D11_BIND_VERTEX_BUFFER);
                CHECK_HRCMD(m_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_quadVertexBuffer.put()));

                const D3D11_SUBRESOURCE_DATA indexBufferData{ QuadShader::c_quadIndices };
                const CD3D11_BUFFER_DESC indexBufferDesc(sizeof(QuadShader::c_quadIndices), D3D11_BIND_INDEX_BUFFER);
                CHECK_HRCMD(m_device->CreateBuffer(&indexBufferDesc, &indexBufferData, m_quadIndexBuffer.put()));
            }
            D3D11_FEATURE_DATA_D3D11_OPTIONS3 options;
            m_device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &options, sizeof(options));
            CHECK_MSG(options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer,
                      "This sample requires VPRT support. Adjust sample shaders on GPU without VRPT.");

            CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(CD3D11_DEFAULT{});
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
            CHECK_HRCMD(m_device->CreateDepthStencilState(&depthStencilDesc, m_reversedZDepthNoStencilTest.put()));

            // test

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

            // creating texture
            HRESULT hr = Windows::Foundation::Initialize(RO_INIT_TYPE::RO_INIT_MULTITHREADED);
            hr = DirectX::CreateWICTextureFromFile(m_device.get(), L"Assets\\font_sheet_5.png", nullptr, myTexture.GetAddressOf());
            //hr = DirectX::CreateWICTextureFromFile(m_device.get(), L"Assets\\StoreLogo.png", nullptr, myTexture.GetAddressOf());
            if (FAILED(hr))
            {
                m_deviceContext->PSSetShaderResources(0, 1, myTexture.GetAddressOf());

            }
            m_deviceContext->PSSetShaderResources(0, 1, myTexture.GetAddressOf());

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
                        const std::vector<const sample::Cube*>& quads) override {
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

            ID3D11RenderTargetView* renderTargets[] = {renderTargetView.get()};
            m_deviceContext->OMSetRenderTargets((UINT)std::size(renderTargets), renderTargets, depthStencilView.get());

            ID3D11Buffer* const constantBuffers[] = {m_modelCBuffer.get(), m_viewProjectionCBuffer.get()};
            m_deviceContext->VSSetConstantBuffers(0, (UINT)std::size(constantBuffers), constantBuffers);
            m_deviceContext->VSSetShader(m_vertexShader.get(), nullptr, 0);
            m_deviceContext->PSSetShader(m_pixelShader.get(), nullptr, 0);
            m_deviceContext->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
            m_deviceContext->PSSetShaderResources(0, 1, myTexture.GetAddressOf());

            CubeShader::ViewProjectionConstantBuffer viewProjectionCBufferData{};

            for (uint32_t k = 0; k < viewInstanceCount; k++) {
                const DirectX::XMMATRIX spaceToView = xr::math::LoadInvertedXrPose(viewProjections[k].Pose);
                const DirectX::XMMATRIX projectionMatrix = ComposeProjectionMatrix(viewProjections[k].Fov, viewProjections[k].NearFar);

                // Set view projection matrix for each view, transpose for shader usage.
                DirectX::XMStoreFloat4x4(&viewProjectionCBufferData.ViewProjection[k],
                                         DirectX::XMMatrixTranspose(spaceToView * projectionMatrix));
            }
            m_deviceContext->UpdateSubresource(m_viewProjectionCBuffer.get(), 0, nullptr, &viewProjectionCBufferData, 0, 0);

            // Set cube primitive data.
            //const UINT strides[] = {sizeof(CubeShader::Vertex)};
            //const UINT offsets[] = {0};
            //ID3D11Buffer* vertexBuffers[] = {m_cubeVertexBuffer.get()};

            /*const UINT strides[] = { sizeof(QuadShader::Vertex) };
            const UINT offsets[] = { 0 };
            ID3D11Buffer* vertexBuffers[] = { m_quadVertexBuffer.get() };
            m_deviceContext->IASetVertexBuffers(0, (UINT)std::size(vertexBuffers), vertexBuffers, strides, offsets);
            m_deviceContext->IASetIndexBuffer(m_quadIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);*/
            
            const UINT strides_cube[] = { sizeof(CubeShader::Vertex) };
            ID3D11Buffer* vb_cube[] = { m_cubeVertexBuffer.get() };
            const UINT offsets[] = { 0 };
            m_deviceContext->IASetVertexBuffers(0, (UINT)std::size({ m_cubeVertexBuffer.get() }), vb_cube, strides_cube, offsets);
            m_deviceContext->IASetIndexBuffer(m_cubeIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetInputLayout(m_inputLayout.get());
            
            // Render each cube
            for (const sample::Cube* cube : cubes) {
                // Compute and update the model transform for each cube, transpose for shader usage.
                CubeShader::ModelConstantBuffer model;
                const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(cube->Scale.x, cube->Scale.y, cube->Scale.z);
                DirectX::XMStoreFloat4x4(&model.Model, DirectX::XMMatrixTranspose(scaleMatrix * xr::math::LoadXrPose(cube->PoseInAppSpace)));
                m_deviceContext->UpdateSubresource(m_modelCBuffer.get(), 0, nullptr, &model, 0, 0);

                // Draw the cube.
                m_deviceContext->DrawIndexedInstanced((UINT)std::size(CubeShader::c_cubeIndices), viewInstanceCount, 0, 0, 0);
            }

            
            const UINT strides_quad[] = { sizeof(CubeShader::Vertex) };
            ID3D11Buffer* vb_quad[] = { m_cubeVertexBuffer.get() };
            m_deviceContext->IASetVertexBuffers(0, (UINT)std::size({ m_quadVertexBuffer.get() }), vb_quad, strides_quad, offsets);
            m_deviceContext->IASetIndexBuffer(m_quadIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetInputLayout(m_inputLayout.get());
            

            for (const sample::Cube* quad : quads) {
                // Compute and update the model transform for each cube, transpose for shader usage.
                QuadShader::ModelConstantBuffer model;
                const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(quad->Scale.x, quad->Scale.y, quad->Scale.z);
                DirectX::XMStoreFloat4x4(&model.Model, DirectX::XMMatrixTranspose(scaleMatrix * xr::math::LoadXrPose(quad->PoseInAppSpace)));
                m_deviceContext->UpdateSubresource(m_modelCBuffer.get(), 0, nullptr, &model, 0, 0);

                // Draw the cube.
                m_deviceContext->DrawIndexedInstanced((UINT)std::size(QuadShader::c_quadIndices), viewInstanceCount, 0, 0, 0);
            }

        }

        void RenderView(const XrRect2Di& imageRect,
            const float renderTargetClearColor[4],
            const std::vector<xr::math::ViewProjection>& viewProjections,
            DXGI_FORMAT colorSwapchainFormat,
            ID3D11Texture2D* colorTexture,
            DXGI_FORMAT depthSwapchainFormat,
            ID3D11Texture2D* depthTexture,
            const std::vector<const sample::Quad*>& quads) override {
            const uint32_t viewInstanceCount = (uint32_t)viewProjections.size();
            CHECK_MSG(viewInstanceCount <= QuadShader::MaxViewInstance,
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

            // binding
            ID3D11Buffer* const constantBuffers[] = { m_modelCBuffer.get(), m_viewProjectionCBuffer.get() };
            m_deviceContext->VSSetConstantBuffers(0, (UINT)std::size(constantBuffers), constantBuffers);
            m_deviceContext->VSSetShader(m_vertexShader.get(), nullptr, 0);
            m_deviceContext->PSSetShader(m_pixelShader.get(), nullptr, 0);
            m_deviceContext->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
            m_deviceContext->PSSetShaderResources(0, 1, myTexture.GetAddressOf());

            QuadShader::ViewProjectionConstantBuffer viewProjectionCBufferData{};

            for (uint32_t k = 0; k < viewInstanceCount; k++) {
                const DirectX::XMMATRIX spaceToView = xr::math::LoadInvertedXrPose(viewProjections[k].Pose);
                const DirectX::XMMATRIX projectionMatrix = ComposeProjectionMatrix(viewProjections[k].Fov, viewProjections[k].NearFar);

                // Set view projection matrix for each view, transpose for shader usage.
                DirectX::XMStoreFloat4x4(&viewProjectionCBufferData.ViewProjection[k],
                    DirectX::XMMatrixTranspose(spaceToView * projectionMatrix));
            }
            m_deviceContext->UpdateSubresource(m_viewProjectionCBuffer.get(), 0, nullptr, &viewProjectionCBufferData, 0, 0);

            // Set quad primitive data.
            const UINT strides[] = { sizeof(QuadShader::Vertex) };
            const UINT offsets[] = { 0 };
            ID3D11Buffer* vertexBuffers[] = { m_quadVertexBuffer.get() };
            m_deviceContext->IASetVertexBuffers(0, (UINT)std::size(vertexBuffers), vertexBuffers, strides, offsets);
            m_deviceContext->IASetIndexBuffer(m_quadIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetInputLayout(m_inputLayout.get());

            // Render each quad
            //for (const sample::Cube* quad : quads) {
            //    // Compute and update the model transform for each quad, transpose for shader usage.
            //    QuadShader::ModelConstantBuffer model;
            //    const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(quad->Scale.x, quad->Scale.y, quad->Scale.z);
            //    DirectX::XMStoreFloat4x4(&model.Model, DirectX::XMMatrixTranspose(scaleMatrix * xr::math::LoadXrPose(quad->PoseInAppSpace)));
            //    m_deviceContext->UpdateSubresource(m_modelCBuffer.get(), 0, nullptr, &model, 0, 0);
            //
            //    // Draw the quad.
            //    m_deviceContext->DrawIndexedInstanced((UINT)std::size(QuadShader::c_quadIndices), viewInstanceCount, 0, 0, 0);
            //}
        }


    private:
        winrt::com_ptr<ID3D11Device> m_device;
        winrt::com_ptr<ID3D11DeviceContext> m_deviceContext;
        winrt::com_ptr<ID3D11VertexShader> m_vertexShader;
        winrt::com_ptr<ID3D11PixelShader> m_pixelShader;
        winrt::com_ptr<ID3D11InputLayout> m_inputLayout;
        winrt::com_ptr<ID3D11Buffer> m_modelCBuffer;
        winrt::com_ptr<ID3D11Buffer> m_viewProjectionCBuffer;
        winrt::com_ptr<ID3D11Buffer> m_cubeVertexBuffer;
        winrt::com_ptr<ID3D11Buffer> m_cubeIndexBuffer;
        winrt::com_ptr<ID3D11Buffer> m_quadVertexBuffer;
        winrt::com_ptr<ID3D11Buffer> m_quadIndexBuffer;
        winrt::com_ptr<ID3D11DepthStencilState> m_reversedZDepthNoStencilTest;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;

    };
} // namespace

namespace sample {
    std::unique_ptr<sample::IGraphicsPluginD3D11> CreateCubeGraphics() {
        return std::make_unique<CubeGraphics>();
    }
} // namespace sample
