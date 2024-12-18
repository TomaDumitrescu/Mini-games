#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object3D
{
    // Creates a deposit obstacle
    Mesh* CreateDepo(const std::string& name, glm::vec3 center, float l, glm::vec3 color, glm::mat4 modelMatrix);
    // Creates the drone object
    Mesh* CreateDrone(const std::string& name, glm::vec3 pos, float alpha);
    //Creates a tree
    Mesh* CreateTree(const std::string& name, glm::vec3 center, float size);
    Mesh* drawTerrain(const std::string& name, float size, glm::vec3 color);
    Mesh* CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);
}
