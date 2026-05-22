#include "classes/entities/mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>

Mesh::Mesh(const std::string& objPath, Shader* s, unsigned int texID, float scale)
    : IEntity(s), textureID(texID), meshScale(scale)
{
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig cfg;
    cfg.triangulate = true;

    if (!reader.ParseFromFile(objPath, cfg)) {
        std::cerr << "Mesh: failed to load " << objPath << "\n";
        if (!reader.Error().empty()) std::cerr << "  " << reader.Error() << "\n";
        return;
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    // Build interleaved buffer: pos(3) + normal(3) + uv(2)
    std::vector<float> verts;
    for (auto& shape : shapes) {
        for (auto& idx : shape.mesh.indices) {
            float vx = attrib.vertices[3 * idx.vertex_index + 0] * scale;
            float vy = attrib.vertices[3 * idx.vertex_index + 1] * scale;
            float vz = attrib.vertices[3 * idx.vertex_index + 2] * scale;
            verts.push_back(vx); verts.push_back(vy); verts.push_back(vz);

            if (idx.normal_index >= 0) {
                verts.push_back(attrib.normals[3 * idx.normal_index + 0]);
                verts.push_back(attrib.normals[3 * idx.normal_index + 1]);
                verts.push_back(attrib.normals[3 * idx.normal_index + 2]);
            } else {
                verts.push_back(0); verts.push_back(1); verts.push_back(0);
            }

            if (idx.texcoord_index >= 0) {
                verts.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                verts.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
            } else {
                verts.push_back(0); verts.push_back(0);
            }
        }
    }

    vertexCount = (int)(verts.size() / 8);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::draw(const glm::mat4& view, const glm::mat4& projection) {
    if (vertexCount == 0) return;

    shader->use();
    shader->setMat4(shader->modelLoc, transform.getModelMatrix());
    shader->setMat4(shader->viewLoc, view);
    shader->setMat4(shader->projLoc, projection);

    int tintLoc = glGetUniformLocation(shader->ID, "tintColor");
    glUniform3f(tintLoc, color.r, color.g, color.b);

    int hasTexLoc = glGetUniformLocation(shader->ID, "hasTexture");
    int lightDirLoc = glGetUniformLocation(shader->ID, "lightDir");
    glUniform3f(lightDirLoc, 0.5f, 1.0f, 0.3f); // directional light from upper-right

    if (textureID != 0) {
        glUniform1i(hasTexLoc, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader->setInt("meshTexture", 0);
    } else {
        glUniform1i(hasTexLoc, 0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
