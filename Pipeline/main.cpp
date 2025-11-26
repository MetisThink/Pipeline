#include "window.h"
#include "core.h"
#include "maths.h"
#include "PipeLineState.h"
#include <fstream>
#include <sstream>
using namespace std;

struct PRIM_VERTEX
{
	Vec3 position;
	Colour colour;
};


class Mesh
{
public:
	ID3D12Resource* vertexBuffer;      // vertex buffer member variable
	D3D12_VERTEX_BUFFER_VIEW vbView;   // view member variable
	D3D12_INPUT_ELEMENT_DESC inputLayout[2];     // Vec3 and colour
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;     // overall description of layout, array of invididual elements


	void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices)   // number of bytes per vertex and number of vertices
	{
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;                  // only one GPU
		heapprops.VisibleNodeMask = 1;

		// Create vertex buffer on the heap
		D3D12_RESOURCE_DESC vbDesc = {};
		vbDesc.Width = numVertices * vertexSizeInBytes;
		vbDesc.Height = 1;
		vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vbDesc.DepthOrArraySize = 1;
		vbDesc.MipLevels = 1;
		vbDesc.SampleDesc.Count = 1;
		vbDesc.SampleDesc.Quality = 0;
		vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// Allocatre memory
		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));

		// Copy vertices using our helper function
		core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// Fill in view in helper function
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.StrideInBytes = vertexSizeInBytes;                         // how big each vertex is
		vbView.SizeInBytes = numVertices * vertexSizeInBytes;

		// Fill in layout
		inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		inputLayout[1] = { "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		inputLayoutDesc.NumElements = 2;
		inputLayoutDesc.pInputElementDescs = inputLayout;
	}

	// WHAT TO DRAW
	// Add commands for drawing
	// Specify type of geometry, where the geometry is (the view), issue command to draw
	void draw(Core* core)
	{
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->getCommandList()->IASetVertexBuffers(0, 1, &vbView);
		core->getCommandList()->DrawInstanced(3, 1, 0, 0);    // one instance of three vertices
	}
};


class ScreenSpaceTriangle     // only comtains one triangle with three vertices
{
public:
	PRIM_VERTEX vertices[3];
	Mesh mesh; // stores vertex buffer

	ScreenSpaceTriangle() {}
	void init(Core* core)
	{
		vertices[0].position = Vec3(0, 1.0f, 0);
		vertices[0].colour = Colour(0, 1.0f, 0);
		vertices[1].position = Vec3(-1.0f, -1.0f, 0);
		vertices[1].colour = Colour(1.0f, 0, 0);
		vertices[2].position = Vec3(1.0f, -1.0f, 0);
		vertices[2].colour = Colour(0, 0, 1.0f);

		mesh.init(core, &vertices[0], sizeof(PRIM_VERTEX), 3);
	}
	void draw(Core* core)
	{
		mesh.draw(core);
	}
};


class Triangle1
{
public:
	// Vertex and pixel shader - member variables
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	// Instance of triangle
	ScreenSpaceTriangle triangle;
	// create instance of Pipeline State Manager
	PSOManager psos;

	//ConstantBuffer constantBuffer;

	// Function to read in a file
	string ReadFile(string filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void LoadShaders(Core *core)
	{
		// Compile Vertex shader
		std::string vsSource = ReadFile("VertexShader.hlsl");

		ID3DBlob* status;
		HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
			NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);

		// CHeck if vertex shader compiles
		if (FAILED(hr))
		{
			if (status)
				OutputDebugStringA((char*)status->GetBufferPointer());
		}

		// Compile pixel shader
		std::string psSource = ReadFile("PixelShader.hlsl");

		hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
			NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

		// CHeck if pixel shader compiles
		if (FAILED(hr))
		{
			if (status)
				OutputDebugStringA((char*)status->GetBufferPointer());
		}

		// Create pipeline state
		psos.createPSO(core, "Triangle", vertexShader, pixelShader, triangle.mesh.inputLayoutDesc);
	}

	void init(Core* core)
	{
		triangle.init(core);
		LoadShaders(core);
	}

	void draw(Core* core) 
	{
		core->beginRenderPass();
		//constantBuffer.update(cb, sizeof(ConstantBuffer1), core->frameIndex());
		//core->getCommandList()->SetGraphicsRootConstantBufferView(1, constantBuffer.getGPUAddress(core->frameIndex());
		psos.bind(core, "Triangle");
		triangle.draw(core);
	}
};


class ConstantBufferClass
{
	ID3D12Resource* constantBuffer;
	unsigned char* buffer;
	unsigned int cbSizeInBytes;


	void init(Core* core, unsigned int sizeInBytes, unsigned int _maxDrawCalls = 1024)
	{
		cbSizeInBytes = (sizeInBytes + 255) & ~255;
		/*unsigned int cbSizeInBytesAligned = cbSizeInBytes * maxDrawCalls;
		numInstances = _maxDrawCalls;
		offsetIndex = 0;*/
		HRESULT hr;
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC cbDesc = {};
		//cbDesc.Width = cbSizeInBytesAligned;
		cbDesc.Height = 1;
		cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbDesc.DepthOrArraySize = 1;
		cbDesc.MipLevels = 1;
		cbDesc.SampleDesc.Count = 1;
		cbDesc.SampleDesc.Quality = 0;
		cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &cbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
			IID_PPV_ARGS(&constantBuffer));
		constantBuffer->Map(0, NULL, (void**)&buffer);
	}

	/*void update(std::string name, void* data)
	{
		ConstantBufferVariable cbVariable = constantBufferData[name];
		unsigned int offset = offsetIndex * cbSizeInBytes;
		memcpy(&buffer[offset + cbVariable.offset], data, cbVariable.size);
	}*/


	/*D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const
	{
		return (constantBuffer->GetGPUVirtualAddress() + (offsetIndex * cbSizeInBytes));
	}*/

	/*void next()
	{
		offsetIndex++;
		if (offsetIndex >= maxDrawCalls)
		{
			offsetIndex = 0;
		}
	}*/

};

//struct alignas(16) ConstantBuffer2
//{
//	float time;
//	float padding[3];
//	Vec4 lights[4];
//	dt = timer.dt();
//};




int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)   // similar to main which was done previously
{
	Window win;    // creates an instance object of win

	Core core;

	// Create an instance of Triangle1
	Triangle1 triangle1Draw;

	// Load the shaders


	win.initialize("My Window", 1024, 1024);    // creates a window
	core.init(win.hwnd, 1024, 1024);            // intialises graphics engine with a window handle

	triangle1Draw.init(&core);
	while (true)
	{
		core.resetCommandList();        // clears command list
		core.beginFrame();              // begins a new rendering frame

		win.processMessages();           // processes pending windows message

		if (win.keys[VK_ESCAPE] == 1)   // escape key closes the window
		{
			break;
		}

		// Draw the triangle
		triangle1Draw.draw(&core);

		/*dt = timer.dt();
		constBufferCPU2.time += dt;
		for (int i = 0; i < 4; i++)
		{
			float angle = constBufferCPU2.time + (i * M_PI / 2.0f);
			constBufferCPU2.lights[i] = Vec4(WIDTH / 2.0f + (cosf(angle) * (WIDTH * 0.3f)), HEIGHT / 2.0f + (sinf(angle) * (HEIGHT * 0.3f)), 0, 0);
		}*/
		 
		core.finishFrame();             // finished rendering

	}
	core.flushGraphicsQueue();         // ensures GPU commands finish
	
	return 0;
}

