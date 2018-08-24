#pragma once
#include "../Utilities/Macro.h"
#include "Material.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace easel {

	class Mesh {
	public:


	protected:
		std::string name;
		size_t materialStart = 0;
		size_t materialCcount = 0;
	};

	class MeshContainer {
	public:
		bool LoadMesh(const wchar_t* filename);
		Mesh* GetMesh(const std::string& name) {
			auto itr = meshes.find(name);
			return (itr != meshes.end() ? &itr->second.mesh : nullptr);
		}
		Shader::Shader* GetShader(const std::string& name) {
			auto itr = meshes.find(name);
			return (itr != meshes.end() ? &itr->second.vertexShader : nullptr);
		}
	private:
		struct MeshAndShader {
			Shader::Shader vertexShader;
			Mesh mesh;
		};
		std::unordered_map<std::string, MeshAndShader> meshes;
		std::vector<DrawMaterial> materials;
	};

	using MeshPtr = Mesh* ;
	class Model {
	public:
		void SetMesh(const char* name) {

		}
		void SetMaterial(const char* name, size_t index) {

		}
		
	private:
		size_t meshId;
		std::vector<size_t> surfaceMaterialIds;
		std::vector<RenderMaterial*> materials;
	};

	class StaticMesh : public Mesh {
	public:
	/*	bool Init(const std::string& name, const std::vector<DrawMaterial>& draws) {
			this->name = name;
			size_t matSize = draws.size();
			materials.resize(matSize);
			for (size_t i = 0; i < matSize; ++i) {
				materials[i].draw = draws[i];
			}
			return true;
		}*/
	};

	//ƒ{[ƒ“‚ÌŠÇ—‚à‚±‚±‚Å‚·‚éH
	class SkinnedMesh : public Mesh {
	public:
		/*bool Init(const std::string& name, const std::vector<DrawMaterial>& draws) {
			this->name = name;
			size_t matSize = draws.size();
			materials.resize(matSize);
			for (size_t i = 0; i < matSize; ++i) {
				materials[i].draw = draws[i];
			}
			return true;
		}*/
	};
} // namespace easel