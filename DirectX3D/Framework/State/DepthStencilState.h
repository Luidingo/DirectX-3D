#pragma once

class DepthStencilState
{
public:
    DepthStencilState();
    ~DepthStencilState();

    void SetState();

    void DepthEnable(bool value);

    void Changed();

private:
    D3D11_DEPTH_STENCIL_DESC desc = {};
    ID3D11DepthStencilState* state = nullptr;
};