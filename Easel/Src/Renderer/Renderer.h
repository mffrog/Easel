#pragma once
#include <Windows.h>
#include "Graphics/Graphics.h"
#include "Graphics/Shader.h"
#include "../Utilities/Template.h"
namespace easel {

	struct FrameComponent {
		struct Component {
			ID3D12Resource* renderTarget;
			CommandAllocator allocator;
		};
		Component components[2];
		Component& GetCurrentComponent() {
			return components[current];
		}
		void SetCurrent(int cur) {
			current = cur;
		}
		int GetCurrent() const {
			return current;
		}
		int current = 0;
	};

	//仮リソース
	struct Resource {
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
	};
	struct Texture : Resource {
	};

	class RenderingContext {
	public:
		void SetCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList) {
			this->commandList = commandList; 
		}
		void SetResource(size_t block, const Resource& resource) {
			commandList->SetGraphicsRootDescriptorTable(block, resource.handle);
		}
		void Draw(size_t indexCount, size_t baseIndex, size_t baseVertex, size_t instanceCount) {
			commandList->DrawIndexedInstanced(indexCount, instanceCount, baseIndex, baseVertex, 0);
		}
	
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	};

	class Renderer : public Singleton<Renderer> {
	public:
		bool Init(HWND hwnd, int width, int height);
		bool Draw();
	private:
		GraphicsCommand command;
		FrameComponent frameComponent;
		SyncObject syncObject;
		D3D12_VIEWPORT viewport;
		D3D12_RECT scissor;

		//テスト用
		Shader::GraphicsPipeline pipeline;
		struct Vertex {
			float position[3];
		};
		Shader::VertexBuffer<Vertex> vertexBuffer;
	};
} // namespace easel