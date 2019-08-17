#include "Shader.h"

#include <d3dcompiler.h>
#include <assert.h>

Shader::Shader(
	const std::wstring& vertexShaderFilePath, 
	const std::wstring& pixelShaderFilePath,
	const std::string& vertexShaderEntryPoint,
	const std::string& pixelShaderEntryPoint)
	: m_vertexShaderFilePath(vertexShaderFilePath),
	m_pixelShaderFilePath(pixelShaderFilePath),
	m_vertexShaderEntryPoint(vertexShaderEntryPoint),
	m_pixelShaderEntryPoint(pixelShaderEntryPoint),
	m_vs(nullptr),
	m_ps(nullptr),
	m_vsCompiled(nullptr),
	m_psCompiled(nullptr)
{
}

Shader::~Shader()
{
	m_vs->Release();
	m_ps->Release();
}

bool Shader::Compile(ID3D11Device* device)
{
	assert(device != nullptr);

	UINT flags = 0;

#if defined(DEBUG) || defined(M_DEBUG)
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* compilationMessages = nullptr;

	HRESULT hr = D3DCompileFromFile(
		m_vertexShaderFilePath.c_str(),
		nullptr,
		nullptr,
		m_vertexShaderEntryPoint.c_str(),
		"vs_5_0",
		flags,
		0,
		&m_vsCompiled,
		&compilationMessages);

	if (compilationMessages != nullptr)
	{
		m_errorQueue.push(std::wstring((LPCWSTR)compilationMessages->GetBufferPointer()));
		compilationMessages->Release();
		return false;
	}

	if (FAILED(hr))
	{
		m_errorQueue.push(std::wstring(L"Unable to compile vertex shader: ")
			+ m_vertexShaderFilePath + L".");
		return false;
	}

	hr = device->CreateVertexShader(
		m_vsCompiled->GetBufferPointer(),
		m_vsCompiled->GetBufferSize(),
		nullptr,
		&m_vs);

	if (FAILED(hr))
	{
		m_errorQueue.push(std::wstring(L"Unable to create vertex shader: ")
			+ m_vertexShaderFilePath + L".");
		return false;
	}

	hr = D3DCompileFromFile(
		m_pixelShaderFilePath.c_str(),
		nullptr,
		nullptr,
		m_pixelShaderEntryPoint.c_str(),
		"ps_5_0",
		flags,
		0,
		&m_psCompiled,
		&compilationMessages);

	if (compilationMessages != nullptr)
	{
		m_errorQueue.push(std::wstring((LPCWSTR)compilationMessages->GetBufferPointer()));
		compilationMessages->Release();
		return false;
	}

	if (FAILED(hr))
	{
		m_errorQueue.push(std::wstring(L"Unable to compile pixel shader: ")
			+ m_vertexShaderFilePath + L".");
		return false;
	}

	if (FAILED(hr))
	{
		m_errorQueue.push(std::wstring(L"Unable to create pixel shader: ")
			+ m_vertexShaderFilePath + L".");
		return false;
	}

	hr = device->CreatePixelShader(
		m_psCompiled->GetBufferPointer(),
		m_psCompiled->GetBufferSize(),
		nullptr,
		&m_ps);

	return true;
}

bool Shader::Bind(ID3D11DeviceContext* currentContext)
{
	assert(currentContext != nullptr);
	assert(m_vs != nullptr);
	assert(m_ps != nullptr);

	currentContext->VSSetShader(m_vs, nullptr, 0);
	currentContext->PSSetShader(m_ps, nullptr, 0);

	return true;
}

bool Shader::UnBind(ID3D11DeviceContext* currentContext)
{
	assert(currentContext != nullptr);
	assert(m_vs != nullptr);
	assert(m_ps != nullptr);

	currentContext->VSSetShader(nullptr, nullptr, 0);
	currentContext->PSSetShader(nullptr, nullptr, 0);

	return true;
}
const std::wstring Shader::GetError()
{
	if (m_errorQueue.size() > 0)
	{
		std::wstring error = m_errorQueue.front();
		m_errorQueue.pop();
		return error;
	}

	return L"";
}

auto Shader::GetVertexShaderCompiledData() -> ID3DBlob*
{
	assert(m_vsCompiled != nullptr);
	return m_vsCompiled;

}

auto Shader::GetPixelShaderCompiledData() -> ID3DBlob*
{
	assert(m_psCompiled != nullptr);
	return m_psCompiled;
}