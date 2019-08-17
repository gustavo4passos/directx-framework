#include "TriangleApp.h"
#include "WaveFrontReader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <DirectXMath.h>
#include <assert.h>


struct VERTEXA 
{
    union
    {
        struct { float x, y, z; };
        struct { float r, g, b; }; 
    };
};

struct VERTEX
{
    DirectX::XMFLOAT3 v;
};


TriangleApp::TriangleApp() noexcept
    : Application(),
    m_shader(L"resources/shaders/vs.hlsl", L"resources/shaders/ps.hlsl"),
    m_transforms(nullptr)
{ 
}

TriangleApp::~TriangleApp() 
{ 
    m_shader.UnBind(m_d3de->GetContext());
}

bool TriangleApp::Init()
{
    if(!Application::Init()) return false;
    if(!m_shader.Compile(m_d3de->GetDevice())) return false;
    if(!InitializeBuffers()) return false;
    if(!InitializeTextures()) return false;
    m_timer.Start();

    return true;
}

auto TriangleApp::Draw() -> void
{
    using namespace DirectX;
    m_timer.Tick();

    m_shader.Bind(m_d3de->GetContext());
    XMMATRIX rotation = XMMatrixTranspose(XMMatrixRotationZ(m_timer.TotalTime()));
    m_d3de->GetContext()->UpdateSubresource(m_transforms, 0, nullptr, &rotation, 0, 0);
    m_d3de->GetContext()->VSSetConstantBuffers(0, 1, &m_transforms);

    m_d3de->GetContext()->PSSetSamplers(0, 1, &m_samplerState);
    m_d3de->GetContext()->PSSetShaderResources(0, 1, &m_textureView);

    m_d3de->GetContext()->IASetInputLayout(m_inputLayout);
    UINT stride = sizeof(VERTEX) * 3;
    UINT offset = 0;
    
    m_d3de->GetContext()->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
    m_d3de->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_d3de->GetContext()->Draw(6, 0);
}

auto TriangleApp::InitializeBuffers() -> bool
{
    VERTEX OurVertices[] =
    {
        // Vertex                               Color                                   TexCoord
        DirectX::XMFLOAT3(  0.5f,  0.5f, 0.5f ), DirectX::XMFLOAT3( 1.f, 0.f, 0.f ), DirectX::XMFLOAT3( 0.f, 0.f, 0.f ),
        DirectX::XMFLOAT3(  0.5f, -0.5f, 0.5f ), DirectX::XMFLOAT3( 0.f, 1.f, 0.f ), DirectX::XMFLOAT3( 1.f, 0.f, 0.f ),
        DirectX::XMFLOAT3( -0.5f, -0.5f, 0.5f ), DirectX::XMFLOAT3( 0.f, 0.f, 1.f ), DirectX::XMFLOAT3( 1.f, 1.f, 0.f ),
        DirectX::XMFLOAT3( -0.5f, -0.5f, 0.5f ), DirectX::XMFLOAT3( 0.f, 0.f, 1.f ), DirectX::XMFLOAT3( 1.f, 1.f, 0.f ),
        DirectX::XMFLOAT3( -0.5f,  0.5f, 0.5f ), DirectX::XMFLOAT3( 0.f, 0.f, 1.f ), DirectX::XMFLOAT3( 0.f, 1.f, 0.f ),
        DirectX::XMFLOAT3(  0.5f,  0.5f, 0.5f ), DirectX::XMFLOAT3( 1.f, 0.f, 0.f ), DirectX::XMFLOAT3( 0.f, 0.f, 0.f )
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(OurVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = (const void*)OurVertices;

    HRESULT hr = m_d3de->GetDevice()->CreateBuffer(&bd, &initData, &m_vb);
    assert(!FAILED(hr));

    D3D11_INPUT_ELEMENT_DESC ied[] = 
    {
        {
            "POSITION", 0,
            DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
            D3D11_INPUT_PER_VERTEX_DATA, 0 
        },
        {
            "COLOR", 0,
            DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 
            D3D11_INPUT_PER_VERTEX_DATA, 0 
        },
        {
            "TEXCOORD", 0,
            DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 
            D3D11_INPUT_PER_VERTEX_DATA, 0 
        }
    };

    hr = m_d3de->GetDevice()->CreateInputLayout(
        ied, 3, m_shader.GetVertexShaderCompiledData()->GetBufferPointer(),
            m_shader.GetVertexShaderCompiledData()->GetBufferSize(),
            &m_inputLayout);
    
    if(FAILED(hr)) return false;
    assert(!FAILED(hr));

    DirectX::XMMATRIX identity = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(2.f, 2.f, 1.f));

    D3D11_BUFFER_DESC tdesc;
    ZeroMemory(&tdesc, sizeof(tdesc));
    tdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    tdesc.ByteWidth = sizeof(identity);

    hr = m_d3de->GetDevice()->CreateBuffer(&tdesc, nullptr, &m_transforms);
    if(FAILED(hr)) assert(false);

    m_d3de->GetContext()->UpdateSubresource(m_transforms, 0, nullptr, &identity, 0, 0);

    return true;
}

auto TriangleApp::InitializeTextures() -> bool
{
    // ID3D11Resource* m_texture = nullptr;;
    // ID3D11ShaderResourceView* m_textureView = nullptr;

    // HRESULT hr = DirectX::CreateWICTextureFromFile(m_d3de->GetDevice(), 
    //     L"resources/images/test.png", &m_texture, &m_textureView);
    
    // if(FAILED(hr)) OutputDebugStringW(L"Load texture failed.");

    // return true;
    int width, height;
    int nChannels;
    unsigned char* data = nullptr;
    data = stbi_load("resources/sprites/test.png", &width, &height, &nChannels, 0);

    if(data == nullptr)
    {
        OutputDebugStringW(L"Unable to load image.");
        return false;
    }

    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = data;
    initData.SysMemPitch = width * nChannels;

    HRESULT hr = m_d3de->GetDevice()->CreateTexture2D(&td, &initData, &m_texture);
    if(FAILED(hr))
    {
        OutputDebugStringW(L"Failed to load texture");
        return false;
    }

    m_textureView = nullptr;
    hr = m_d3de->GetDevice()->CreateShaderResourceView(m_texture, nullptr, &m_textureView);
    
    if(FAILED(hr))
    {
        OutputDebugStringW(L"Failed to create shader resource view");
        return false;
    }

    D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;

    hr = m_d3de->GetDevice()->CreateSamplerState(&sd, &m_samplerState);

    if(FAILED(hr))
    {
        OutputDebugStringW(L"Unable to create sampler state.");
        return false;
    }
    return true;
}