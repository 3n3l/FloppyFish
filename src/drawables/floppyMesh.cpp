#include "glm/gtc/type_ptr.hpp"
#define GL_SILENCE_DEPRECATION
#define GLM_ENABLE_EXPERIMENTAL

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
#include "src/drawables/floppyMesh.h"
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
    loadObj(positions, normals, textureCoordinates, indices, amountMeshParts);

    // If the current index is lower than the maximum, attach and initialize a child / subsequent mesh part.
    if (_meshIndex < amountMeshParts - 1) {
        // Initialize the next mesh.
        _nextMeshPart = std::make_shared<FloppyMesh>(_meshPath, _meshIndex + 1);
        _nextMeshPart->init();
    }

    // Set up a vertex array object for the geometry.
    // NOTE: This needs to be called after loadObj().
    bindVertexArrayObject();

    _verticeAmount = indices.size();

    // Fill vertex array object with data.
    GLuint textureCoordinateBuffer = loadTextureCoordinates(textureCoordinates);
    GLuint positionBuffer = loadPositions(positions);
    GLuint normalBuffer = loadNormals(normals);
    GLuint indexBuffer = loadIndices(indices);

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

bool FloppyMesh::loadObj(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals,
                         std::vector<glm::vec2>& textureCoordinates, std::vector<unsigned int>& indices,
                         uint& amountMeshParts) {
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
    if (!objReader.ParseFromFile(_meshPath, objReaderConfig)) {
        fprintf(stderr, "%s\n", objReader.Error().c_str());
        return false;
    }

    // Retrieve vectors from mesh.
    std::vector<tinyobj::shape_t> shapes = objReader.GetShapes();
    if (objReader.GetShapes().empty()) {
        fprintf(stderr, "%s: cannot understand data\n", _meshPath.c_str());
        return false;
    }
    amountMeshParts = shapes.size();
    tinyobj::attrib_t attributes = objReader.GetAttrib();
    std::vector<tinyobj::material_t> materials = objReader.GetMaterials();

    // Validate.
    bool hasNormals = (attributes.normals.size() / 3 == attributes.vertices.size() / 3);
    bool hasTexture = (attributes.texcoords.size() / 2 == attributes.vertices.size());
    bool dataIsNotUnderstood = (hasTexture && attributes.texcoords.size() / 2 != attributes.vertices.size());
    dataIsNotUnderstood |= (hasNormals && attributes.normals.size() / 3 != attributes.vertices.size() / 3);
    dataIsNotUnderstood |= shapes[_meshIndex].mesh.indices.size() % 3 != 0;
    dataIsNotUnderstood |= shapes[_meshIndex].mesh.indices.empty();
    dataIsNotUnderstood |= attributes.texcoords.size() % 2 != 0;
    dataIsNotUnderstood |= attributes.vertices.size() % 3 != 0;
    dataIsNotUnderstood |= attributes.normals.size() % 3 != 0;
    dataIsNotUnderstood = attributes.vertices.empty();
    if (dataIsNotUnderstood) {
        positions.clear();
        normals.clear();
        textureCoordinates.clear();
        indices.clear();
        fprintf(stderr, "%s: cannot understand data\n", _meshPath.c_str());
        return false;
    }

    // Indices.
    uint vertPosition = 0;
    uint vertNormal = 0;
    uint vertTexCoord = 0;

    for (size_t i = 0; i < shapes[_meshIndex].mesh.indices.size(); i++) {
        tinyobj::shape_t shape = shapes[_meshIndex];
        vertPosition = shape.mesh.indices[i].vertex_index;
        vertNormal = shape.mesh.indices[i].normal_index;
        vertTexCoord = shape.mesh.indices[i].texcoord_index;
        float px = attributes.vertices[3 * vertPosition + 0];
        float py = attributes.vertices[3 * vertPosition + 1];
        float pz = attributes.vertices[3 * vertPosition + 2];
        positions.emplace_back(px, py, pz);
        float nx = attributes.normals[3 * vertNormal + 0];
        float ny = attributes.normals[3 * vertNormal + 1];
        float nz = attributes.normals[3 * vertNormal + 2];
        normals.emplace_back(nx, ny, nz);
        float tx = attributes.texcoords[2 * vertTexCoord + 0];
        float tz = attributes.texcoords[2 * vertTexCoord + 1];
        textureCoordinates.emplace_back(tx, tz);
        indices.push_back(i);
    }

    // Set material properties.
    // Assume each mesh part has a distinct material, i.e. no other face has a different material.
    int materialIndex = shapes[_meshIndex].mesh.material_ids[0];
    tinyobj::material_t material = materials[materialIndex];
    if (materialIndex >= 0) {
        _emissiveColour = glm::vec3(material.emission[0], material.emission[1], material.emission[2]);
        _textureName = material.diffuse_texname;
        _transparency = material.dissolve;
        _shininess = material.shininess;
    } else {
        _emissiveColour = glm::vec3(0.0f);
        _textureName = "";
        _transparency = 1.0f;
        _shininess = 0.5f;
    }

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
