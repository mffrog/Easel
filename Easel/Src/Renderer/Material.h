#pragma once
#include "Graphics/Shader.h"
#include <unordered_map>
#include <string>

namespace easel {
	class RenderingContext;
	class tmp {

	};
	using Resource = tmp;
	//描画に必要なリソースの管理用クラス
	class SurfaceMaterial {
	public:
		bool Draw(RenderingContext* context) const;
		void AddResource(size_t block, Resource resource) {
			resources.push_back({block, resource});
		}
		size_t GetId() const { return id; }
	private:
		size_t id;
		//sharedにするかも
		//first : block, second : resource
		std::vector<std::pair<size_t,Resource>> resources;
	};

	class SurfaceMaterialContainer {
	public:
		bool LoadMaterialFile(const wchar_t* filename);
		SurfaceMaterial* GetMaterial(const std::string& name) {
			auto itr = materialMap.find(name);
			return (itr != materialMap.end() ? itr->second.material : nullptr);
		}
		Shader::Shader* GetShader(const std::string& name) {
			auto itr = materialMap.find(name);
			return (itr != materialMap.end() ? &itr->second.pixelShader : nullptr);
		}
	private:
		struct MaterialAndShader {
			SurfaceMaterial* material;
			Shader::Shader pixelShader;
		};
		std::unordered_map<std::string, MaterialAndShader> materialMap;
	};

	class DrawMaterial {
	public:
		DrawMaterial() {}
		DrawMaterial(size_t indexCount, size_t baseIndex, size_t baseVertex)
			:indexCount(indexCount), baseIndex(baseIndex), baseVertex(baseVertex) {
		}
		bool Draw(RenderingContext* context, size_t count) const;
		size_t GetId() const { return id; }
	private:
		size_t baseVertex;
		size_t baseIndex;
		size_t indexCount;

		size_t id;
	};

	class RenderMaterial {
	public:
		
		bool Create(SurfaceMaterial* surface, DrawMaterial* draw) {
			if (!surface || !draw) {
				return false;
			}
			this->surface = surface;
			this->draw = draw;
			return true;
		}
		bool DrawInstanced(RenderingContext* context, size_t count) const {
			if (!surface->Draw(context)) {
				return false;
			}
			if (!draw->Draw(context, count)) {
				return false;
			}
		}
	private:
		SurfaceMaterial* surface;
		DrawMaterial* draw;
		Shader::GraphicsPipeline pipeline;
	};

	class RenderMaterialContainer {
	public:
		void AddRenderMaterial(SurfaceMaterial* surface, DrawMaterial* draw) {
			materials[GetHash(surface->GetId(), draw->GetId())]->Create(surface, draw);
		}
		RenderMaterial* GetRenderMaterial(SurfaceMaterial* surface, DrawMaterial* draw) {
			size_t hash = GetHash(surface->GetId(), draw->GetId());
			auto itr = materials.find(GetHash(surface->GetId(), draw->GetId()));
			if (itr == materials.end()) {
				AddRenderMaterial(surface, draw);
				return materials[hash];
			}
			else {
				return itr->second;
			}
		}

	private:
		size_t GetHash(size_t surfaceId, size_t drawId) { return 0; }

	private:
		std::unordered_map<size_t, RenderMaterial*> materials;
	};

} // namespace easel