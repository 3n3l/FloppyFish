#ifndef FLOPPY_MESH_H
#define FLOPPY_MESH_H

#include <QOpenGLFunctions_4_1_Core>

#include "glm/detail/type_mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

class FloppyMesh : protected QOpenGLFunctions_4_1_Core {
   public:
    /// Constructor for creating a mesh from an obj.
    FloppyMesh(std::string meshPath, glm::vec3 initialTranslation = glm::vec3(0.0f), float initialScale = 1.0f,
               float initialRotation = 0.0f, float subsequentRotationSpeed = 0.0f);

    /// Constructor for creating the next mesh part.
    FloppyMesh(std::string meshPath, uint meshIndex);

    ~FloppyMesh();

    // Enumerator to pass with textures.
    enum TextureType { RGB, SRGB, NormalMap, Monochrome };

    /**
     * @brief draw the mesh.
     */
    virtual void draw(glm::mat4 projectionMatrix);

    /**
     * @brief initialize the mesh.
     */
    virtual void init();

    /**
     * @brief update Updates the object's position, rotation etc.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     * @param modelViewMatrix the mode view matrix of the parent object
     */
    virtual void update(float elapsedTimeMs, glm::mat4 modelViewMatrix);

   protected:
    GLuint _program{};                         /**< The opengl program handling the shaders */
    GLuint _vertexArrayObject{};               /**< The vertex array object containing the vertices */
    GLuint _verticeAmount{};                   /**< The amount of vertices used to draw bill */
    std::shared_ptr<FloppyMesh> _nextMeshPart; /**< The subsequent part of the mesh if it exists */

    // Mesh and material.
    std::string _meshPath;     /**< The filepath of the mesh. */
    uint _meshIndex;           /**< The part of mesh to render, will be used to recursively render each part */
    std::string _textureName;  /**< The name of the texture file. */
    GLuint _textureHandle{};   /**< handle of the background texture */
    float _shininess;          /**< The shininess of the object. */
    float _transparency;       /**< The transparency/dissolve/alpha of the object. */
    glm::vec3 _emissiveColour; /**< The colour of the emission of the object. */

    // Transformations, initial and ongoing.
    glm::mat4 _modelViewMatrix{};       /**< The model view matrix to get the object into model view space */
    glm::vec3 _initialTranslation{};    /**< The initial translation applied as a baseline to the mesh */
    glm::vec3 _subsequentTranslation{}; /**< The subsequent translation applied to the mesh */
    float _initialScale;                /**< The initial scaling applied as a baseline to the mesh */
    float _initialRotation;             /**< The initial rotation around the Y-axis applied as a baseline to the mesh */
    float
        _subsequentRotation; /**< The subsequent rotation around the Y-axis applied to the mesh. Mostly for debugging */
    float _subsequentRotationSpeed; /**< The subsequent rotation speed around the Y-axis applied to the mesh. Mostly for
                                       debugging */

    /**
     * @brief loadObj loads a mesh from a given path.
     * @param filename the path to the obj.
     * @param positions of the vertices.
     * @param normals of said vertices.
     * @param textureCoordinates of said vertices.
     * @param indices of the vertices.
     * @param textureName the name of the texture.
     * @param shininess of the object.
     * @param transparency of the object applied globally.
     * @param emissiveColour of the object.
     * @param amountMeshParts the amount of parts the mesh has.
     * @return true if it was successful.
     */
    bool loadObj(const std::string& filename, uint partIndex, std::vector<glm::vec3>& positions,
                 std::vector<glm::vec3>& normals, std::vector<glm::vec2>& textureCoordinates,
                 std::vector<unsigned int>& indices, std::string& textureName, float& shininess, float& transparency,
                 glm::vec3& emissiveColour, uint& amountMeshParts);

    /**
     * @brief loadTexture loads a texture from a given path.
     * @param path the path to the texture.
     * @param type the way the texture shall be parsed
     * @return the GLuint pointing to the texture.
     */
    GLuint loadTexture(std::string path, TextureType type = SRGB);
};

#endif  // FLOPPY_MESH_H
