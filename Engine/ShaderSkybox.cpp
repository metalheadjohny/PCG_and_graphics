#include "ShaderSkybox.h"
#include "Model.h"

ShaderSkybox::ShaderSkybox() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
}


ShaderSkybox::~ShaderSkybox() {
}


bool ShaderSkybox::Initialize(ID3D11Device* device, HWND hwnd, const std::vector<std::wstring> filePaths) {

	this->filePaths = filePaths;
	return InitializeShader(device, hwnd);
}


bool ShaderSkybox::InitializeShader(ID3D11Device* device, HWND hwnd) {

	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;

	//these store the shaders while they are being processed and are released after compilation... I think...
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];	//description of buffer data
	unsigned int numElements;					//number of elements in the poligon layout... this is dumb...

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC variableBufferDesc;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(filePaths.at(0).c_str(), NULL, NULL, "CMVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, *(filePaths.at(0).c_str()));
		else
			MessageBox(hwnd, filePaths.at(0).c_str(), L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(filePaths.at(1).c_str(), NULL, NULL, "CMFS", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, *(filePaths.at(1).c_str()));
		else
			MessageBox(hwnd, filePaths.at(1).c_str(), L"Missing Shader File", MB_OK);

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result)) {
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// Setup the description of the variable dynamic constant buffer that is in the vertex shader.
	variableBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	variableBufferDesc.ByteWidth = sizeof(VariableBuffer);
	variableBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	variableBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	variableBufferDesc.MiscFlags = 0;
	variableBufferDesc.StructureByteStride = 0;

	// Create the variable constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&variableBufferDesc, NULL, &m_variableBuffer);
	if (FAILED(result))
		return false;

	return true;
}


void ShaderSkybox::ShutdownShader()
{

	// Release the variable constant buffer.
	if (m_variableBuffer) {
		m_variableBuffer->Release();
		m_variableBuffer = 0;
	}
	// Release the matrix constant buffer.
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the sampler state.
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the layout.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

}


void ShaderSkybox::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR shaderFilename) {

	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", &shaderFilename, MB_OK);
}


bool ShaderSkybox::SetShaderParameters(ID3D11DeviceContext* deviceContext, Model& model, const SMatrix& v, const SMatrix& p,
	const SVec3& eyePos, float deltaTime, ID3D11ShaderResourceView* tex) {


	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber = 0;
	MatrixBuffer* dataPtr;
	VariableBuffer* dataPtr3;

	SMatrix mT = model.transform.Transpose();
	SMatrix vT = v.Transpose();
	SMatrix pT = p.Transpose();

	// Lock the constant matrix buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	dataPtr = (MatrixBuffer*)mappedResource.pData;

	dataPtr->world = mT;
	dataPtr->view = vT;
	dataPtr->projection = pT;

	deviceContext->Unmap(m_matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	//END MATRIX BUFFER



	//VARIABLE BUFFER
	result = deviceContext->Map(m_variableBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	dataPtr3 = (VariableBuffer*)mappedResource.pData;
	dataPtr3->deltaTime = deltaTime;
	dataPtr3->padding = SVec3();

	deviceContext->Unmap(m_variableBuffer, 0);

	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_variableBuffer);
	//END VARIABLE BUFFER


	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->PSSetShaderResources(0, 1, &(tex));

	return true;
}



bool ShaderSkybox::ReleaseShaderParameters(ID3D11DeviceContext* deviceContext) {
	deviceContext->PSSetShaderResources(0, 1, &(unbinder[0]));
	return true;
}