#pragma once

#include "Application.h"
#include "GameTimer.h"
#include "Shader.h"

class TriangleApp : public Application
{
public:
    TriangleApp() noexcept;
    virtual ~TriangleApp();
    virtual bool Init() override;
    virtual void Draw();

private:
    auto InitializeBuffers() -> bool;
    auto InitializeLayout() -> bool;
    auto InitializeTextures() -> bool;

    Shader m_shader;
    ID3D11Buffer* m_vb;
    ID3D11InputLayout* m_inputLayout;

    ID3D11Buffer* m_transforms;

    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;
    ID3D11SamplerState* m_samplerState;

    GameTimer m_timer;
};