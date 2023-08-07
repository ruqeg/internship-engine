#pragma once

#include <engine/math/MeshLocal.h>
#include <engine/math/Transform.h>

namespace math
{

class MeshGlobal : public Transform
{
public:
	explicit MeshGlobal(const std::shared_ptr<MeshLocal>& meshLocal) : meshLocal(meshLocal) {};
	inline const std::shared_ptr<MeshLocal>& getMeshLocal() const { return meshLocal; }
private:
	std::shared_ptr<MeshLocal> meshLocal;
};

}
