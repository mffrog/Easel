#include "Material.h"
#include "Renderer.h"
namespace easel {
	bool SurfaceMaterial::Draw(RenderingContext* context) const {
		size_t count = resources.size();
		for (size_t i = 0; i < count; ++i) {
			context->SetResource(resources[i].first, resources[i].second);
		}
		return true;
	}
	bool DrawMaterial::Draw(RenderingContext * context, size_t count) const {
		context->Draw(indexCount, baseIndex, baseVertex, count);
		return true;
	}
} // namespace easel