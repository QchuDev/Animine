#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "classes/entities/curve.h"

Curve::Curve(ParamFunction formula, glm::vec3 color, Shader* s, unsigned int strokeTex, float tMin, float tMax)
    : IEntity(s), pCurve(formula), color(color), textureID(strokeTex)
{
    // Generate polyline points
    float range = tMax - tMin;
    float step = range / 200.0f; // ~200 segments regardless of range

    for (float t = tMin; t <= tMax; t += step) {
        points.push_back(formula(t));
    }

    // Triangle strip: 2 verts per point, 5 floats each (pos3 + uv2)
    m_vertexCount = (int)points.size() * 2;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertexCount * 5 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Curve::buildStrip(const glm::vec3& camPos) {
    int n = (int)points.size();
    if (n < 2) return;

    // Calculate total arc length for UV tiling
    float totalLen = 0.0f;
    std::vector<float> cumLen(n, 0.0f);
    for (int i = 1; i < n; i++) {
        totalLen += glm::length(points[i] - points[i - 1]);
        cumLen[i] = totalLen;
    }

    float capU = 0.15f; // fraction of texture for each cap
    float tileWorldSize = 1.0f; // world units per middle tile repeat

    std::vector<float> verts;
    verts.reserve(m_vertexCount * 5);

    for (int i = 0; i < n; i++) {
        // Direction at this point
        glm::vec3 dir;
        if (i == 0) dir = points[1] - points[0];
        else if (i == n - 1) dir = points[n - 1] - points[n - 2];
        else dir = points[i + 1] - points[i - 1]; // averaged

        glm::vec3 toCamera = camPos - points[i];
        glm::vec3 side = glm::cross(glm::normalize(dir), glm::normalize(toCamera));
        float sideLen = glm::length(side);
        if (sideLen < 0.0001f)
            side = glm::cross(glm::normalize(dir), glm::vec3(0, 1, 0));
        else
            side = side / sideLen; // normalize
        side *= width;

        // UV: u based on arc length
        float u;
        float ratio = (totalLen > 0.0f) ? cumLen[i] / totalLen : 0.0f;

        if (ratio <= capU) {
            // Start cap: map [0, capU] of curve → [0, capU] of texture
            u = ratio;
        } else if (ratio >= 1.0f - capU) {
            // End cap: map [1-capU, 1] of curve → [1-capU, 1] of texture
            u = ratio;
        } else {
            // Middle: tile
            float middleCurveLen = totalLen * (1.0f - 2.0f * capU);
            float middleProgress = (cumLen[i] - totalLen * capU) / tileWorldSize;
            float tiledU = middleProgress - (int)middleProgress; // fract
            u = capU + tiledU * (1.0f - 2.0f * capU);
        }

        glm::vec3 p0 = points[i] - side;
        glm::vec3 p1 = points[i] + side;

        // Bottom vertex (v=0)
        verts.push_back(p0.x); verts.push_back(p0.y); verts.push_back(p0.z);
        verts.push_back(u); verts.push_back(0.0f);
        // Top vertex (v=1)
        verts.push_back(p1.x); verts.push_back(p1.y); verts.push_back(p1.z);
        verts.push_back(u); verts.push_back(1.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(float), verts.data());
}

void Curve::draw(const glm::mat4& view, const glm::mat4& projection) {
    if (points.size() < 2) return;

    glm::mat4 invView = glm::inverse(view);
    glm::vec3 camPos = glm::vec3(invView[3]);

    buildStrip(camPos);

    shader->use();
    shader->setMat4(shader->modelLoc, transform.getModelMatrix());
    shader->setMat4(shader->viewLoc, view);
    shader->setMat4(shader->projLoc, projection);

    int tintLoc = glGetUniformLocation(shader->ID, "tintColor");
    glUniform3f(tintLoc, color.r, color.g, color.b);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setInt("strokeTexture", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
    glBindVertexArray(0);
}

Curve::~Curve() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
