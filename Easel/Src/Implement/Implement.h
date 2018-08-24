#pragma once


//-----------------------------------------------------------
//	Includes

#include "../Utilities/Template.h"
#include <unordered_map>
#include <vector>
#include "Graphics/Shader.h"
#include "Math/Matrix/Matrix4x3.h"
//-----------------------------------------------------------

namespace easel {
	namespace Implement {
		struct RootSignature {
			RootSignature() = default;
			RootSignature(size_t id, Microsoft::WRL::ComPtr<ID3D12RootSignature> rs) : id(id), rootSignature(rs) {}
			size_t id;
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		};
		struct PipelineState {
			size_t id;
			size_t vertexInfoId;
			size_t materialInfoId;
			Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
		};
		struct DrawMaterial {
			size_t indexCount;
			size_t baseIndex;
			size_t baseVertex;
		};

		struct VertexInfo {
			size_t id;
			std::wstring vertexShader;
			Shader::InputLayout layout;
		};

		struct ShaderParameter {
			size_t blockCount;
			enum Type {
				CBV,
				SRV,
				UAV,
			};
			Type type;
		};
		//マテリアルファイルを読み込んでこれを初期化する
		class MaterialInfo {
		public:
			bool UseCustumVertexShader() const {
				return useCustumVertexInfo;
			}
			size_t GetId() const { return id; }
			size_t GetVertexInfoId() const { return vertexInfoId; }
			void SetRootSignature(size_t id) { rootSignatureId = id; }
			size_t GetRootSignatureId() const { return rootSignatureId; }
			const std::wstring& GetPixelShader() const { return pixelShader; }

		private:
			bool useCustumVertexInfo = false;
			std::wstring pixelShader;
			size_t id;
			size_t vertexInfoId;
			std::vector<ShaderParameter> parameters;

			size_t rootSignatureId;
		};

		class RootSignatureManager : public Singleton<RootSignatureManager> {
		public:
			~RootSignatureManager();
			bool CreateRootSignature(const std::vector<ShaderParameter>& parameters, size_t* id);
			RootSignature* GetRootSignature(size_t id) const { return rootSignatures[id]; }
		private:
			std::vector<RootSignature*> rootSignatures;
		};
		class PipelineStateManager : public Singleton<PipelineStateManager> {
		public:
			~PipelineStateManager();
			bool CreatePipelineState(const VertexInfo* vInfo, const MaterialInfo* mInfo);

			size_t GetPipelineStateId(size_t vertexInfoId, size_t materialInfoId);
			PipelineState* GetPipelineState(size_t id);

			//PipelineState* GetPipelineState(size_t vertexInfoId, size_t materialInfoId);
		private:
			std::vector<PipelineState*> pipelineStates;
		};

		class MaterialInfoManager : public Singleton<MaterialInfoManager> {
		public:
			bool LoadFile(const wchar_t* filename, size_t* id);

			MaterialInfo* GetMaterialInfo(size_t id) const { return infos[id]; }
		private:
			std::vector<MaterialInfo*> infos;
		};

		struct Resource {

		};
		class GraphicsContext;
		class SurfaceMaterial {
		public:
			void SetResource(GraphicsContext* context);
			size_t GetInfoId() const { return infoId; }
		private:
			void AddResource(size_t index, Resource* resource) {
				resources.emplace_back(index, resource);
			}
			size_t id;
			size_t infoId;
			std::vector<std::pair<size_t, Resource*>> resources;
		};


		class VertexInfoManager : public Singleton<VertexInfoManager> {
		public:
			VertexInfo * GetVertexInfo(size_t id) const { return vertexDescs[id]; }
		private:
			std::vector<VertexInfo*> vertexDescs;
		};

		class Mesh {
		public:
			size_t GetDrawMaterialId(size_t idx) const {
				return drawMaterialIds[idx];
			}
			size_t GetMaterialCount() const {
				return drawMaterialIds.size();
			}
			size_t GetVertexInfoId() const { return vertexInfoId; }
		private:
			std::vector<size_t> drawMaterialIds;
			size_t vertexInfoId;
		};

		class Material {
			size_t rootsignature;	//シェーダー依存	必要なのはどんなデータを何個使うかどうか
			size_t pipeline;		//シェーダー依存	シェーダーオブジェクトとレイアウト
			size_t draw;			//メッシュ依存
			size_t surface;			//一応独立、でもrootsignatureに対応していないとダメ

		};

		struct Mat {
			size_t rootsignature;
			size_t pipelinestate;
			size_t draw;
			size_t mateInfo;
			size_t surface;
		};

		/*
		 つまりシェーダとdraw,surfaceのマテリアルが必要
		 シェーダはSurfaceから取得
		 頂点シェーダーはSurfaceで設定するかMeshから取得する
		*/
		struct DrawInfoCache {
			Mesh* mesh;
			struct RenderDesc {
				RootSignature* rootSignature;
				PipelineState* pipeline;
				SurfaceMaterial* surface;
			};
			std::vector<RenderDesc> renderDescs;
			void SetMesh(Mesh* mesh) {
				if (!mesh) {
					return;
				}
				this->mesh = mesh;
				renderDescs.resize(mesh->GetMaterialCount());
			}
			void SetMaterial(SurfaceMaterial* surface, size_t index) {
				if (index < renderDescs.size()) {
					renderDescs[index].surface = surface;
					GetGraphicsPipeline(index);
				}
			}

		private:
			void GetAllGraphicsPipeline() {

			}
			void GetGraphicsPipeline(size_t index) {
				RenderDesc& desc = renderDescs[index];
				if (desc.surface) {
					MaterialInfo* mateInfo = MaterialInfoManager::GetInstance()->GetMaterialInfo(desc.surface->GetInfoId());
					size_t vertexInfoId;
					if (mateInfo->UseCustumVertexShader()) {
						vertexInfoId = mateInfo->GetVertexInfoId();
					}
					else {
						vertexInfoId = mesh->GetVertexInfoId();
					}
					desc.rootSignature = RootSignatureManager::GetInstance()->GetRootSignature(mateInfo->GetRootSignatureId());
					size_t pipeline = PipelineStateManager::GetInstance()->GetPipelineStateId(vertexInfoId, mateInfo->GetId());
					desc.pipeline = PipelineStateManager::GetInstance()->GetPipelineState(pipeline);

				}
			}
		};

		class DrawMaterialManager : public Singleton<DrawMaterialManager> {
		public:
			DrawMaterial * GetDrawMaterial(size_t id);
		};

		class SurfaceMaterialManager : public Singleton<SurfaceMaterialManager> {
		public:
			SurfaceMaterial * GetSurfaceMaterial(size_t id);
		};


		//WorldMatrixとボーン
		struct VertexResource {
			virtual void SetResource(GraphicsContext* context) {

			}
			mff::Matrix4x3<float> world;
		};

		template<int N>
		struct SkinnedVertexResource : VertexResource {
			void SetResource(GraphicsContext* context) override;
			mff::Matrix4x3<float> boneMat[N];
		};
		template<int N>
		inline void SkinnedVertexResource<N>::SetResource(GraphicsContext* context) {
			VertexResource::SetResource(context);
		}
		struct DrawInfo {
			size_t rootsignature;
			size_t pipeline;
			size_t drawMaterial;
			size_t surfaceMaterial;
		};


		template<class T>
		using Map = std::unordered_multimap<size_t, T>;

		class Renderer {
		public:
			void AddDrawTarget(const DrawInfo& info) {
				auto rsItr = map.find(info.rootsignature);
				if (rsItr != map.end()) {
					auto rs = rsItr->second;
					auto psItr = rs.find(info.pipeline);
					if (psItr != rs.end()) {
						auto ps = psItr->second;
						auto sfItr = ps.find(info.surfaceMaterial);
						if (sfItr != ps.end()) {
							auto sf = sfItr->second;
							auto drawItr = sf.find(info.drawMaterial);
							if (drawItr != sf.end()) {
								drawItr->second++;
							}
							else {
								sf.insert(std::pair<size_t, size_t>(info.drawMaterial, 1));
							}
						}
						else {
							std::unordered_map<size_t, size_t> surface;
							surface[info.drawMaterial] = 1;

							ps.insert(std::make_pair(info.surfaceMaterial, surface));
						}
					}
					else {
						std::unordered_map<size_t, size_t> surface;
						surface[info.drawMaterial] = 1;
						Map<std::unordered_map<size_t, size_t>> pso;
						pso.insert(std::make_pair(info.surfaceMaterial, surface));

						rs.insert(std::make_pair(info.pipeline, pso));
					}
				}
				else {
					std::unordered_map<size_t, size_t> surface;
					surface[info.drawMaterial] = 1;
					Map<std::unordered_map<size_t, size_t>> pso;
					pso.insert(std::make_pair(info.surfaceMaterial, surface));
					Map<Map<std::unordered_map<size_t, size_t>>> rootsignature;
					rootsignature.insert(std::make_pair(info.pipeline, pso));

					map.insert(std::make_pair(info.rootsignature, rootsignature));
				}
			}

			void Draw();

		private:
			//[rsId][psId][sfId][draw] = count
			Map<Map<Map<std::unordered_map<size_t, size_t>>>> map;
		};

	} // namespace Implement
} // namespace easel