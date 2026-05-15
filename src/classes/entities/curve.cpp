#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "classes/entities/curve.h"

Curve::Curve(ParamFunction formula, glm::vec3 color, Shader* s, unsigned int strokeTex, float tMin, float tMax)
    : IEntity(s), pCurve(formula), color(color), textureID(strokeTex)
{
    // Generate polyline points
    float range = tMax - tMin;
    float step = range / 1000.0f; // ~1000 segments regardless of range

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

    float capWorldSize = 0.25f;  // fixed world units each cap occupies
    float capU = 0.25f;         // UV fraction each cap uses in texture
    float tileWorldSize = 1.0f; // world units per middle tile repeat
    float middleURange = 1.0f - 2.0f * capU;
    float endCapStart = totalLen - capWorldSize;

    // Helper: compute side vector at point i
    auto computeSide = [&](int i) -> glm::vec3 {
        glm::vec3 dir;
        if (i == 0) dir = glm::normalize(points[1] - points[0]);
        else if (i == n - 1) dir = glm::normalize(points[n - 1] - points[n - 2]);
        else dir = glm::normalize(points[i + 1] - points[i - 1]);

        glm::vec3 toCamera = camPos - points[i];
        glm::vec3 side = glm::cross(dir, glm::normalize(toCamera));
        float sideLen = glm::length(side);
        if (sideLen < 0.0001f)
            side = glm::cross(dir, glm::vec3(0, 1, 0));
        else
            side = side / sideLen;

        float miter = 1.0f;
        if (i > 0 && i < n - 1) {
            glm::vec3 d0 = glm::normalize(points[i] - points[i - 1]);
            glm::vec3 d1 = glm::normalize(points[i + 1] - points[i]);
            float cosHalf = glm::length((d0 + d1) * 0.5f);
            if (cosHalf > 0.0001f)
                miter = 1.0f / cosHalf;
            miter = glm::min(miter, 2.0f);
        }
        return side * width * miter;
    };

    // Helper: push a vertex pair at position with given u
    std::vector<float> verts;
    auto pushPair = [&](const glm::vec3& pos, const glm::vec3& side, float u) {
        glm::vec3 p0 = pos - side;
        glm::vec3 p1 = pos + side;
        verts.push_back(p0.x); verts.push_back(p0.y); verts.push_back(p0.z);
        verts.push_back(u); verts.push_back(0.0f);
        verts.push_back(p1.x); verts.push_back(p1.y); verts.push_back(p1.z);
        verts.push_back(u); verts.push_back(1.0f);
    };

    // Track which tile we're in to detect tile boundaries
    int prevTile = -1;

    for (int i = 0; i < n; i++) {
        glm::vec3 side = computeSide(i);
        float arcLen = cumLen[i];

        if (arcLen <= capWorldSize) {
            float u = (capWorldSize > 0.0f) ? (arcLen / capWorldSize) * capU : 0.0f;
            pushPair(points[i], side, u);
            prevTile = -1;
        } else if (arcLen >= endCapStart) {
            // Close last middle tile before entering end cap
            if (prevTile >= 0) {
                pushPair(points[i], side, 1.0f - capU);
                prevTile = -1;
            }
            float t = (capWorldSize > 0.0f) ? (arcLen - endCapStart) / capWorldSize : 1.0f;
            float u = (1.0f - capU) + t * capU;
            pushPair(points[i], side, u);
        } else {
            // Middle zone
            float middleArc = arcLen - capWorldSize;
            int curTile = (int)(middleArc / tileWorldSize);
            float inTile = middleArc - curTile * tileWorldSize;
            float u = capU + (inTile / tileWorldSize) * middleURange;

            // Tile boundary: close previous tile, start new one
            if (prevTile >= 0 && curTile != prevTile) {
                pushPair(points[i], side, 1.0f - capU); // close prev tile
                pushPair(points[i], side, capU);         // open new tile
            } else if (prevTile < 0) {
                // First middle point: open first tile
                pushPair(points[i], side, capU);
            }

            if (prevTile >= 0 && curTile != prevTile) {
                // Already pushed the opening pair above with capU,
                // now push the actual position within the new tile
                if (inTile > 0.0001f)
                    pushPair(points[i], side, u);
            } else {
                pushPair(points[i], side, u);
            }
            prevTile = curTile;
        }
    }

    m_vertexCount = (int)(verts.size() / 5);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
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
