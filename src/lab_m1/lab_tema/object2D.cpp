#include "object2D.h"
#include "transform2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

#define BASE_RATIO_WIDTH 0.2f
#define BASE_RATIO_HEIGHT 0.27f

#define CIRCLE_RADIUS_RATIO 0.33f
#define SHOOTER_WIDTH 2.0f
#define SHOOTER_HEIGHT 10

Mesh* object2D::CreateTank(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    glm::vec3 up_color,
    float shooter_rotate,
    bool fill)
{
    // Base trapezoid
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(-length * BASE_RATIO_WIDTH, length * BASE_RATIO_HEIGHT, 0), color),
        VertexFormat(corner + glm::vec3(length * (BASE_RATIO_WIDTH + 1), length * BASE_RATIO_HEIGHT, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 1 };

    if (fill) {
        indices.push_back(3);
        indices.push_back(2);
    }

    // Up trapezoid
    glm::vec3 up_corner = corner + glm::vec3(-length * 2 * BASE_RATIO_WIDTH, length * BASE_RATIO_HEIGHT, 0);

    vertices.push_back(VertexFormat(up_corner, up_color));
    vertices.push_back(VertexFormat(up_corner + glm::vec3(length * (4 * BASE_RATIO_WIDTH + 1), 0, 0), up_color));
    vertices.push_back(VertexFormat(up_corner + glm::vec3(length * BASE_RATIO_WIDTH, 1.5f * length * BASE_RATIO_HEIGHT, 0), up_color));
    vertices.push_back(VertexFormat(up_corner + glm::vec3(length * (3 * BASE_RATIO_WIDTH + 1),
                                                            1.5f * length * BASE_RATIO_HEIGHT, 0), up_color));

    if (fill) {
        indices.insert(indices.end(), { 4, 5, 6, 5, 6, 7 });
    } else {
        indices.insert(indices.end(), { 4, 5, 6, 5 });
    }

    // Circle
    int circlePoints = 30;
    float stepAngle = 2.0f * M_PI / circlePoints;
    glm::vec3 center = up_corner + glm::vec3(2, 1.5f * length * BASE_RATIO_HEIGHT, 0) +
                        glm::vec3(length * (3 * BASE_RATIO_WIDTH + 1) / 2.0f, 0, 0);
    vertices.push_back(VertexFormat(center, up_color));
    int vertices_len = vertices.size() - 1;

    float r = CIRCLE_RADIUS_RATIO * length;

    for (int i = 0; i < circlePoints; i++) {
        float step = i * stepAngle;
        glm::vec3 line = center + glm::vec3(r * cos(step), r * sin(step), 0);

        vertices.push_back(VertexFormat(line, up_color));
        if (fill) {
            indices.push_back(vertices_len);
            indices.push_back(vertices_len + i + 1);
            indices.push_back(vertices_len + ((i + 1) % circlePoints) + 1);
        } else {
            indices.push_back(vertices_len + i + 1);
        }
    }

    // Rectangle with rotation
    glm::vec3 shooter_color = glm::vec3(0.059f, 0.059f, 0.051f);

    // Rotate with reference to previous circle center
    glm::mat3 apply_rotation = transform2D::Rotate(shooter_rotate);
    vertices.push_back(VertexFormat(center + apply_rotation * glm::vec3(-SHOOTER_WIDTH / 2, r, 0), shooter_color));
    vertices.push_back(VertexFormat(center + apply_rotation * glm::vec3(SHOOTER_WIDTH / 2, r, 0), shooter_color));
    vertices.push_back(VertexFormat(center + apply_rotation * glm::vec3(-SHOOTER_WIDTH / 2, r + SHOOTER_HEIGHT, 0), shooter_color));
    vertices.push_back(VertexFormat(center + apply_rotation * glm::vec3(SHOOTER_WIDTH/2, r + SHOOTER_HEIGHT, 0), shooter_color));

    unsigned int next_push = vertices.size() - 4;
    if (fill) {
        indices.insert(indices.end(), { next_push, next_push + 1, next_push + 2 });
        indices.insert(indices.end(), { next_push + 2, next_push + 1, next_push + 3 });
    } else {
        indices.insert(indices.end(), { next_push, next_push + 1, next_push + 2, next_push + 3 });
    }

    Mesh* tank = new Mesh(name);
    if (!fill) {
        tank->SetDrawMode(GL_LINE_LOOP);
    }

    tank->InitFromData(vertices, indices);
    return tank;
}


Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


Mesh* object2D::CreateBullet(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;

    // Circle
    int circlePoints = 30;
    float stepAngle = 2.0f * M_PI / circlePoints;
    vertices.push_back(VertexFormat(center, color));
    std::vector<unsigned int> indices;

    for (int i = 0; i < circlePoints; i++) {
        float step = i * stepAngle;
        glm::vec3 line = center + glm::vec3(radius * cos(step), radius * sin(step), 0);

        vertices.push_back(VertexFormat(line, color));
        if (fill) {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(((i + 1) % circlePoints) + 1);
        }
        else {
            indices.push_back(i + 1);
        }
    }

    Mesh* circle = new Mesh(name);

    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    circle->InitFromData(vertices, indices);
    return circle;
}
