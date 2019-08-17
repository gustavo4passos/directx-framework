#pragma once

#include <d3d11.h>
#include <queue>
#include <string>

class Shader
{
public:
	Shader(const std::wstring& vertexShaderFilePath,
		const std::wstring& pixelShaderFilePath,
		const std::string& vertexShaderEntryPoint = "main",
		const std::string& pixelShaderEntryPoint = "main");

	~Shader();
	
	bool Compile(ID3D11Device* device);
	bool Bind(ID3D11DeviceContext* currentContext);
	bool UnBind(ID3D11DeviceContext* currentContext);
	const std::wstring GetError();

	auto GetVertexShaderCompiledData() -> ID3DBlob*;
	auto GetPixelShaderCompiledData() -> ID3DBlob*;
private:
	std::wstring m_vertexShaderFilePath;
	std::wstring m_pixelShaderFilePath;

	std::string m_vertexShaderEntryPoint;
	std::string m_pixelShaderEntryPoint;

	ID3D11VertexShader* m_vs;
	ID3D11PixelShader* m_ps;

	ID3DBlob* m_vsCompiled;
	ID3DBlob* m_psCompiled;

	std::queue< std::wstring> m_errorQueue;
};