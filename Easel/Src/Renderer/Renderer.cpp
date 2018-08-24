#include "Renderer.h"
namespace easel {
	bool Renderer::Init(HWND hwnd, int width, int height) {
		Graphics& graphics = Graphics::GetInstance();
		if (!graphics.Initialize(hwnd, width, height)) {
			return false;
		}
		frameComponent.SetCurrent(graphics.GetCurrentBackBufferIndex());
		for (int i = 0; i < 2; ++i) {
			if (!frameComponent.components[i].allocator.Init(&graphics, CommandType_Direct)) {
				return false;
			}
			frameComponent.components[i].renderTarget = graphics.GetRenderTarget(i).Get();
		}

		if (!command.Init(&frameComponent.GetCurrentComponent().allocator)) {
			return false;
		}

		if (!syncObject.Init(&graphics)) {
			return false;
		}

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		scissor.top = 0;
		scissor.left = 0;
		scissor.right = width;
		scissor.bottom = height;

		{
			Shader::RootSignatureFactory factory;
			if (!factory.Create(&graphics, pipeline.rootSignature)) {
				return false;
			}
		}
		{
			Shader::InputLayout layout;
			layout.AddElement("POSITION", Shader::InputLayout::Float, 3);
			
			Shader::Shader vs;
			if (!vs.LoadShader(L"Res/VertexShader.hlsl", "vs_5_0", "main")) {
				return false;
			}
			Shader::Shader ps;
			if (!ps.LoadShader(L"Res/PixelShader.hlsl", "ps_5_0", "main")) {
				return false;
			}
			Shader::PipelineStateFactory factory;
			factory.SetRootSignature(pipeline.rootSignature.Get());
			factory.SetLayout(layout);
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
			factory.SetRenderTargets(1, &format);
			factory.SetVertexShader(&vs);
			factory.SetPixelShader(&ps);
			if (!factory.Create(&graphics, pipeline.pso)) {
				return false;
			}
		}

		Vertex vertices[3] = {
			{ 0.0f, 0.5f,0.0f },
			{ 0.5f,-0.5f,0.0f },
			{-0.5f,-0.5f,0.0f },
		};
		if (!vertexBuffer.Init(&graphics, 3)) {
			return false;
		}
		vertexBuffer.Update(vertices, sizeof(vertices));
		return true;
	}

	bool Renderer::Draw() {
		Graphics& graphics = Graphics::GetInstance();
		frameComponent.SetCurrent(graphics.GetCurrentBackBufferIndex());
		if (!command.Reset(&frameComponent.GetCurrentComponent().allocator)) {
			return false;
		}
		int current = frameComponent.GetCurrent();
		command.GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				frameComponent.GetCurrentComponent().renderTarget,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			));

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = command.GetCommandList();
		commandList->OMSetRenderTargets(1, &graphics.GetRTVHandle(current), FALSE, nullptr);
		const float clearColor[4] = { 0.1f,0.1f,0.1f,1.0f };
		commandList->ClearRenderTargetView(graphics.GetRTVHandle(current), clearColor, 0, nullptr);
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissor);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		pipeline.Use(commandList);
		commandList->IASetVertexBuffers(0, 1, &vertexBuffer.GetView());
		commandList->DrawInstanced(3, 1, 0, 0);
		command.GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				frameComponent.GetCurrentComponent().renderTarget,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			));

		if (FAILED(command.GetCommandList()->Close())) {
			return false;
		}
		ID3D12CommandList* commands[] = {
			command.GetCommandList().Get(),
		};
		graphics.GetCommandQueue()->ExecuteCommandLists(_countof(commands), commands);
		if (!graphics.Swap()) {
			return false;
		}
		if (!syncObject.WaitGpuProcess(graphics.GetCommandQueue())) {
			return false;
		}
		return true;
	}
} // namespace easel