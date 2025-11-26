#pragma once

#include <d3d12.h>         // directx 12 library
#include <dxgi1_6.h>       // more functionality
#include <d3dcompiler.h>   // compiler
#include <vector>               // vector
#pragma comment(lib, "d3d12")         // libraries
#pragma comment(lib, "dxgi")              // libraries
#pragma comment(lib, "d3dcompiler.lib")    // libraries


// pass in a resource, what state it comes in as, what state it goes to
class Barrier
{
public:
	static void add(ID3D12Resource* res, D3D12_RESOURCE_STATES first, D3D12_RESOURCE_STATES second,
		ID3D12GraphicsCommandList4* commandList) {
		D3D12_RESOURCE_BARRIER rb = {};
		rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		rb.Transition.pResource = res;
		rb.Transition.StateBefore = first;
		rb.Transition.StateAfter = second;
		rb.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &rb);
	}
};


class GPUFence 
{
public:
	ID3D12Fence* fence;
	HANDLE eventHandle;
	UINT64 value = 0;
	void create(ID3D12Device5* device)                                                 // create a fence
	{
		device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	void signal(ID3D12CommandQueue* queue)                                            
	{
		queue->Signal(fence, ++value);
	}
	void wait() 
	{
		if (fence->GetCompletedValue() < value) 
		{
			fence->SetEventOnCompletion(value, eventHandle);
			WaitForSingleObject(eventHandle, INFINITE);                   // CPU does nothing until this fence is hit
		}
	}

	~GPUFence() 
	{
		CloseHandle(eventHandle);
		fence->Release();
	}
};


class Core
{
public:
	IDXGIAdapter1* adapter;

	// Core interfaces - members
	ID3D12Device5* device;
	ID3D12CommandQueue* graphicsQueue;
	ID3D12CommandQueue* copyQueue;
	ID3D12CommandQueue* computeQueue;
	IDXGISwapChain3* swapchain;

	// Member variables - one for each frame
	ID3D12CommandAllocator* graphicsCommandAllocator[2];
	ID3D12GraphicsCommandList4* graphicsCommandList[2];

	// Backbuffer members
	ID3D12DescriptorHeap* backbufferHeap;
	ID3D12Resource** backbuffers;

	// Fences
	GPUFence graphicsQueueFence[2];

	// Descriptor Heap
	ID3D12DescriptorHeap* dsvHeap;
	ID3D12Resource* dsv;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	// Scissor
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	// Root signature
	ID3D12RootSignature* rootSignature;



	void init(HWND hwnd, int _width, int _height)     // handle to window and width and height of window
	{
		// Enumerate adapters
		IDXGIAdapter1* adapterf;
		std::vector<IDXGIAdapter1*> adapters;            // stores all adapters on the system
		IDXGIFactory6* factory = NULL;                     
		CreateDXGIFactory(__uuidof(IDXGIFactory6), (void**)&factory);
		
		int i = 0;
		while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND)    // find all adapters and push into vectors
		{
			adapters.push_back(adapterf);
			i++;
		}


		// Find the best adapter - the one with the most video memory
		long long maxVideoMemory = 0;
		int useAdapterIndex = 0;
		for (int i = 0; i < adapters.size(); i++)
		{
			DXGI_ADAPTER_DESC desc;
			adapters[i]->GetDesc(&desc);
			if (desc.DedicatedVideoMemory > maxVideoMemory)
			{
				maxVideoMemory = desc.DedicatedVideoMemory;
				useAdapterIndex = i;
			}
		}
		adapter = adapters[useAdapterIndex];
		

		// Create DX12 Device on adapter
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));

		// Create Command Queue
		D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
		graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		device->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(&graphicsQueue));
		D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
		copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&copyQueue));
		D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
		computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeQueue));

		// Structure describing SwapChain
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.Width = _width;
		scDesc.Height = _height;
		scDesc.SampleDesc.Count = 1; // MSAA here
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferCount = 2;                             // Double buffering
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		// Create a swapchain for the window
		IDXGISwapChain1* swapChain1;                              // create a Swapchain 1 and can eventually cast to Swapchain 3
		factory->CreateSwapChainForHwnd(graphicsQueue, hwnd, &scDesc, NULL, NULL, &swapChain1);
		swapChain1->QueryInterface(&swapchain);
		swapChain1->Release();

		// CReatye command allocators and command lists
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&graphicsCommandAllocator[0]));
		device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&graphicsCommandList[0]));
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&graphicsCommandAllocator[1]));
		device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&graphicsCommandList[1]));

		// Create HEAP
		D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
		renderTargetViewHeapDesc.NumDescriptors = scDesc.BufferCount;
		renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		device->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&backbufferHeap));

		// Allocate memory for Backbuffer Array
		backbuffers = new ID3D12Resource * [scDesc.BufferCount];

		// Get backbuffers and create views on heap
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = backbufferHeap->GetCPUDescriptorHandleForHeapStart();
		unsigned int renderTargetViewDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);  // how much space a view takes up
		for (unsigned int i = 0; i < 2; i++)     // create 2 render target views
		{
			swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffers[i]));
			device->CreateRenderTargetView(backbuffers[i], nullptr, renderTargetViewHandle);
			renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
		}

		// Create fences
		graphicsQueueFence[0].create(device);
		graphicsQueueFence[1].create(device);

		// Create Descriptor Heap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		memset(&dsvHeapDesc, 0, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
		dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

		// depth Buffer - fill in structs
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
		D3D12_CLEAR_VALUE depthClearValue = {};
		depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.DepthStencil.Stencil = 0;

		// depth buffer - what type of memory do we allocate to the depth buffer
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;

		// depth buffer - specify memory resource
		D3D12_RESOURCE_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.Width = _width;
		dsvDesc.Height = _height;
		dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsvDesc.DepthOrArraySize = 1;
		dsvDesc.MipLevels = 1;
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		dsvDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		// depth buffer - allocate memory resource
		device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &dsvDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClearValue, IID_PPV_ARGS(&dsv));

		// create depth stencil view - can mask different parts of the image - only specific areas of the image are drawn
		device->CreateDepthStencilView(dsv, &depthStencilDesc, dsvHandle);

		// define viewport
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = (float)_width;
		viewport.Height = (float)_height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		//define scissor rect
		scissorRect.left = 0;
		scissorRect.top = 0;
		scissorRect.right = _width;
		scissorRect.bottom = _height;

		// Create Root Signature
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ID3DBlob* serialized;
		ID3DBlob* error;
		D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
		device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized -> GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		serialized->Release();


		factory->Release();
	}

	// ensures all work is completed before mobing on
	void flushGraphicsQueue()
	{
		graphicsQueueFence[0].signal(graphicsQueue);
		graphicsQueueFence[0].wait();
	}

	// Resets the command list
	void resetCommandList()
	{
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
		graphicsCommandAllocator[frameIndex]->Reset();
		graphicsCommandList[frameIndex]->Reset(graphicsCommandAllocator[frameIndex], NULL);
	}

	// Gets current command list
	ID3D12GraphicsCommandList4* getCommandList()
	{
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
		return graphicsCommandList[frameIndex];
	}

	// Close and execute the command list
	void runCommandList()
	{
		getCommandList()->Close();
		ID3D12CommandList* lists[] = { getCommandList() };
		graphicsQueue->ExecuteCommandLists(1, lists);
	}

	void beginFrame()
	{
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();

		graphicsQueueFence[frameIndex].wait();

		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = backbufferHeap -> GetCPUDescriptorHandleForHeapStart();
		unsigned int renderTargetViewDescriptorSize = device -> GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		renderTargetViewHandle.ptr += frameIndex * renderTargetViewDescriptorSize;

		Barrier::add(backbuffers[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, getCommandList());
		getCommandList()->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &dsvHandle);
		float color[4];                    // colour of screen, currently blue
		color[0] = 0;
		color[1] = 0;
		color[2] = 1.0;
		color[3] = 1.0;
		getCommandList()->ClearRenderTargetView(renderTargetViewHandle, color, 0, NULL);
		getCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	}


	void finishFrame()
	{
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
		Barrier::add(backbuffers[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, getCommandList());
		runCommandList();
		graphicsQueueFence[frameIndex].signal(graphicsQueue);
		swapchain->Present(1, 0);
	}

	// Copies some data (pointer to memory), allocate memory in upload heap
	void uploadResource(ID3D12Resource* dstResource, const void* data, unsigned int size, D3D12_RESOURCE_STATES targetState, D3D12_PLACED_SUBRESOURCE_FOOTPRINT* texFootprint = NULL)
	{
		ID3D12Resource* uploadBuffer;
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = size;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));

		void* mappeddata = NULL;                          // Initialises data
		uploadBuffer->Map(0, NULL, &mappeddata);
		memcpy(mappeddata, data, size);
		uploadBuffer->Unmap(0, NULL);

		resetCommandList();

		if (texFootprint != NULL)
		{
			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.pResource = uploadBuffer;
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint = *texFootprint;
			D3D12_TEXTURE_COPY_LOCATION dst = {};
			dst.pResource = dstResource;
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = 0;
			getCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);
		}
		else
		{
			getCommandList()->CopyBufferRegion(dstResource, 0, uploadBuffer, 0, size);
		}

		// Transition buffer to final state after copying
		Barrier::add(dstResource, D3D12_RESOURCE_STATE_COPY_DEST, targetState, getCommandList());

		runCommandList();    // close and execute command lists

		flushGraphicsQueue();   // wait for command to finish

		uploadBuffer->Release();  //  release upload heap memory

	}

	// Functionality to set common draw functionality
	void beginRenderPass()
	{
		getCommandList()->RSSetViewports(1, &viewport);
		getCommandList()->RSSetScissorRects(1, &scissorRect);
		getCommandList()->SetGraphicsRootSignature(rootSignature);
	}


	// Frame index
	int frameIndex()
	{
		return swapchain->GetCurrentBackBufferIndex();
	}

};



