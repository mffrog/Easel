#include "Implement.h"
namespace easel {
	namespace Implement {
		RootSignatureManager::~RootSignatureManager() {
			for (RootSignature* s : rootSignatures) {
				delete s;
			}
		}
		bool RootSignatureManager::CreateRootSignature(const std::vector<ShaderParameter>& parameters, size_t* id){
			Shader::RootSignatureFactory factory;
			size_t blockOffset = 0;
			for (size_t i = 0; i < parameters.size(); ++i) {
				switch (parameters[i].type)
				{
				case ShaderParameter::CBV:
					factory.AddParameterBlock(parameters[i].blockCount, blockOffset + i);
					break;
				case ShaderParameter::SRV:
					factory.AddResourceBlock(parameters[i].blockCount, blockOffset + i);
					break;
				default:
					break;
				}
			}
			
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
			if (!factory.Create(&Graphics::GetInstance(), rootSignature)) {
				return false;
			}
			*id = rootSignatures.size();
			rootSignatures.push_back(new RootSignature(*id, rootSignature));
			
			return true;
		}

		PipelineStateManager::~PipelineStateManager() {
			for (PipelineState* s : pipelineStates) {
				delete s;
			}
		}

		bool PipelineStateManager::CreatePipelineState(const VertexInfo* vInfo, const MaterialInfo * mInfo) {
			Shader::Shader vs;
			if (!vs.LoadShader(vInfo->vertexShader.c_str(), "vs_5_0", "main")) {
				return false;
			}
			Shader::Shader ps;
			if (!ps.LoadShader(mInfo->GetPixelShader().c_str(), "ps_5_0", "main")) {
				return false;
			}

			Shader::PipelineStateFactory factory;
			factory.SetLayout(vInfo->layout);
			factory.SetVertexShader(&vs);
			DXGI_FORMAT format[] = {
				DXGI_FORMAT_R8G8B8A8_UNORM,
			};
			factory.SetRenderTargets(_countof(format),format);
			factory.SetRootSignature(
				RootSignatureManager::GetInstance()->GetRootSignature(mInfo->GetRootSignatureId())->rootSignature.Get());
			factory.UseDepthStencil(DXGI_FORMAT_D32_FLOAT);
			Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
			if (!factory.Create(&Graphics::GetInstance(), pso)) {
				return false;
			}
			PipelineState* pipeline = new PipelineState();
			pipeline->pso = pso;
			pipeline->vertexInfoId = vInfo->id;
			pipeline->materialInfoId = mInfo->GetId();
			pipeline->id = pipelineStates.size();
			pipelineStates.push_back(pipeline);
			return true;
		}

		void Renderer::Draw() {
			GraphicsContext* context;
			ID3D12GraphicsCommandList* cmdList;
			auto rsMapItr = map.begin();
			for (auto& rs : map) {
				size_t rsId = rs.first;
				cmdList->SetGraphicsRootSignature(
					RootSignatureManager::GetInstance()->GetRootSignature(rsId)->rootSignature.Get()
				);
				for (auto& pso : rs.second) {
					size_t psoId = pso.first;
					cmdList->SetPipelineState(
						PipelineStateManager::GetInstance()->GetPipelineState(psoId)->pso.Get()
					);
					for (auto& surface : pso.second) {
						size_t surfaceId = surface.first;
						SurfaceMaterialManager::GetInstance()->GetSurfaceMaterial(surfaceId)->SetResource(context);

						for (auto& draw : surface.second) {
							size_t drawId = draw.first;
							size_t count = draw.second;
							DrawMaterial* m = DrawMaterialManager::GetInstance()->GetDrawMaterial(drawId);
							cmdList->DrawIndexedInstanced(m->indexCount,count, m->baseIndex, m->baseVertex, 0);
						}
					}
				}
			}
		}
	} // namespace Implement
} // namespace easel