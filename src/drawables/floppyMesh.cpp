#define GL_SILENCE_DEPRECATION
#define GLM_ENABLE_EXPERIMENTAL

#include "src/drawables/floppyMesh.h"

#include <QFile>
#include <QOpenGLShaderProgram>
#include <string>
#include <utility>
#include <vector>

#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "lib/tinyobj/tiny_obj_loader.h"
#include "src/config/config.h"
#include "src/utils/utils.h"

// Main constructor.
FloppyMesh::FloppyMesh(std::string meshPath, glm::vec3 initialTranslation, float initialScale, float initialRotation,
                       float subsequentRotationSpeed) {
    _meshPath = std::move(meshPath);
    _initialTranslation = initialTranslation;
    _initialScale = initialScale;
    _initialRotation = initialRotation;
    _subsequentRotationSpeed = subsequentRotationSpeed;
}
FloppyMesh::~FloppyMesh() = default;

// Constructor for next mesh parts.
FloppyMesh::FloppyMesh(std::string meshPath, uint meshIndex) {
    _meshPath = std::move(meshPath);
    _meshIndex = meshIndex;
    // Set the scale to 1.0f, so it will keep the parent's transformation.
    _initialScale = 1.0f;
}

void FloppyMesh::init() {
    // Initialize OpenGL funtions, replacing glewInit().
    initializeOpenGLFunctions();

    // Create a program for this class.
    _program = glCreateProgram();

    // Compile shader.
    GLuint vs = compileShader(GL_VERTEX_SHADER, "src/shaders/common.vs.glsl");
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, "src/shaders/cookTorrance.fs.glsl");

    // Attach shader to the program.
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);

    // Link program.
    _program = linkProgram(_program);

    // Create mesh vectors (dynamic arrays).
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<GLuint> indices;

    uint amountMeshParts = 0;

    // Load mesh.
    loadObj(_meshPath, _meshIndex, positions, normals, textureCoordinates, indices, _textureName, _shininess,
            _transparency, _emissiveColour, amountMeshParts);

    // If the current index is lower than the maximum, attach and initialize a child / subsequent mesh part.
    if (_meshIndex < amountMeshParts - 1) {
        // Initialize the next mesh.
        _nextMeshPart = std::make_shared<FloppyMesh>(_meshPath, _meshIndex + 1);
        _nextMeshPart->init();
    }

    _verticeAmount = indices.size();

    // Set up a vertex array object for the geometry.
    if (_vertexArrayObject == 0) {
        glGenVertexArrays(1, &_vertexArrayObject);
    }
    glBindVertexArray(_vertexArrayObject);

    // Fill vertex array object with data.
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    GLuint textureCoordinateBuffer;
    glGenBuffers(1, &textureCoordinateBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
    // Multiply with 2, as it is only vec2.
    glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * 2 * sizeof(float), textureCoordinates.data(),
                 GL_STATIC_DRAW);
    // Use indices of 2.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Unbind vertex array object.
    glBindVertexArray(0);

    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &textureCoordinateBuffer);
    glDeleteBuffers(1, &indexBuffer);

    // Load texture.
    _textureHandle = loadTexture("res/" + _textureName);
}

void FloppyMesh::draw(glm::mat4 projectionMatrix) {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }

    // Draw next mesh part first if it exists.
    if (_nextMeshPart != nullptr) {
        _nextMeshPart->draw(projectionMatrix);
    }

    // Load program.
    glUseProgram(_program);
    glCheckError();

    // Bind vertex array object.
    glBindVertexArray(_vertexArrayObject);
    glCheckError();

    // Set uniform variables.
    // Set matrix parameters.
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE,
                       glm::value_ptr(_modelViewMatrix));

    // Set lighting parameters.
    glUniform1f(glGetUniformLocation(_program, "eta"), Config::indexOfRefraction);
    // Map shininess [0,1000] to roughness [0,1].
    float roughness = 0.000001f * pow(_shininess - 1000, 2.0f);
    glUniform1f(glGetUniformLocation(_program, "roughness"), roughness);
    glUniform1f(glGetUniformLocation(_program, "transparency"), _transparency);
    glUniform3fv(glGetUniformLocation(_program, "emissiveColour"), 1, value_ptr(_emissiveColour));
    // TODO: Replace this light position in the future.
    glm::vec3 lightPosition = glm::vec3(-2.0f, 2.0f, 2.0f);
    glUniform3fv(glGetUniformLocation(_program, "light_position"), 1, value_ptr(lightPosition));

    // Set the background texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureHandle);
    glUniform1i(glGetUniformLocation(_program, "albedo"), 0);

    // Call draw.
    glDrawElements(GL_TRIANGLES, _verticeAmount, GL_UNSIGNED_INT, 0);
    glCheckError();

    // Unbind vertex array object.
    glBindVertexArray(0);
    glCheckError();
}

bool FloppyMesh::loadObj(const std::string& filename, uint partIndex, std::vector<glm::vec3>& positions,
                         std::vector<glm::vec3>& normals, std::vector<glm::vec2>& textureCoordinates,
                         std::vector<unsigned int>& indices, std::string& textureName, float& shininess,
                         float& transparency, glm::vec3& emissiveColour, uint& amountMeshParts) {
    positions.clear();
    normals.clear();
    textureCoordinates.clear();
    indices.clear();

    // Use V2 API of tiny obj loader.
    // Create config for loading.
    tinyobj::ObjReaderConfig objReaderConfig = tinyobj::ObjReaderConfig();
    // Disable vertex colour.
    objReaderConfig.vertex_color = false;

    // Parse the file.
    auto objReader = tinyobj::ObjReader();
    if (!objReader.ParseFromFile(filename, objReaderConfig)) {
        fprintf(stderr, "%s\n", objReader.Error().c_str());
        return false;
    }

    // Retrieve vectors from mesh.
    std::vector<tinyobj::shape_t> shapes = objReader.GetShapes();
    if (objReader.GetShapes().empty()) {
        fprintf(stderr, "%s: cannot understand data\n", filename.c_str());
        return false;
    }
    amountMeshParts = shapes.size();
    tinyobj::attrib_t attributes = objReader.GetAttrib();
    std::vector<tinyobj::material_t> materials = objReader.GetMaterials();

    bool have_normals = (attributes.normals.size() / 3 == attributes.vertices.size() / 3);
    bool have_texcoords = (attributes.texcoords.size() / 2 == attributes.vertices.size());

    // Validate.
    if (attributes.vertices.empty() || attributes.vertices.size() % 3 != 0 || attributes.normals.size() % 3 != 0 ||
        (have_normals && attributes.normals.size() / 3 != attributes.vertices.size() / 3) ||
        attributes.texcoords.size() % 2 != 0 ||
        (have_texcoords && attributes.texcoords.size() / 2 != attributes.vertices.size()) ||
        shapes[partIndex].mesh.indices.empty() || shapes[partIndex].mesh.indices.size() % 3 != 0) {
        positions.clear();
        normals.clear();
        textureCoordinates.clear();
        indices.clear();
        fprintf(stderr, "%s: cannot understand data\n", filename.c_str());
        return false;
    }

    // Indices.
    uint vertPosition = 0;
    uint vertNormal = 0;
    uint vertTexCoord = 0;

    for (size_t i = 0; i < shapes[partIndex].mesh.indices.size(); i++) {
        vertPosition = shapes[partIndex].mesh.indices[i].vertex_index;
        vertNormal = shapes[partIndex].mesh.indices[i].normal_index;
        vertTexCoord = shapes[partIndex].mesh.indices[i].texcoord_index;
        positions.emplace_back(attributes.vertices[3 * vertPosition + 0], attributes.vertices[3 * vertPosition + 1],
                               attributes.vertices[3 * vertPosition + 2]);
        normals.emplace_back(attributes.normals[3 * vertNormal + 0], attributes.normals[3 * vertNormal + 1],
                             attributes.normals[3 * vertNormal + 2]);
        textureCoordinates.emplace_back(attributes.texcoords[2 * vertTexCoord + 0],
                                        attributes.texcoords[2 * vertTexCoord + 1]);
        indices.push_back(i);
    }

    // Set material properties.
    // Assume each mesh part has a distinct material, i.e. no other face has a different material.
    int materialIndex = shapes[partIndex].mesh.material_ids[0];
    textureName = materialIndex >= 0 ? materials[materialIndex].diffuse_texname : "";
    shininess = materialIndex >= 0 ? materials[materialIndex].shininess : 0.5f;
    transparency = materialIndex >= 0 ? materials[materialIndex].dissolve : 1.0f;
    emissiveColour = materialIndex >= 0
                         ? glm::vec3(materials[materialIndex].emission[0], materials[materialIndex].emission[1],
                                     materials[materialIndex].emission[2])
                         : glm::vec3(0.0f);

    return true;
}

void FloppyMesh::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    // Translations.
    modelViewMatrix = translate(modelViewMatrix, _initialTranslation);
    modelViewMatrix = translate(modelViewMatrix, _subsequentTranslation);

    // Scale.
    modelViewMatrix = scale(modelViewMatrix, glm::vec3(_initialScale));

    // Rotations.
    modelViewMatrix = rotate(modelViewMatrix, glm::radians(_initialRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    if (_subsequentRotationSpeed > 0.0f) {
        // Rotate around Y for debug reasons.
        _subsequentRotation += _subsequentRotationSpeed * elapsedTimeMs;
        _subsequentRotation = _subsequentRotation >= 360.0f ? 0.0f : _subsequentRotation;
        modelViewMatrix = rotate(modelViewMatrix, glm::radians(_subsequentRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Update the next mesh part if applicable.
    if (_nextMeshPart != nullptr) {
        _nextMeshPart->update(elapsedTimeMs, modelViewMatrix);
    }

    _modelViewMatrix = modelViewMatrix;
}
