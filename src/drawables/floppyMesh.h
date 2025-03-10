#ifndef FLOPPY_MESH_H
#define FLOPPY_MESH_H

#include <memory>
#include <vector>

#include "drawable.h"
#include "glm/ext/vector_float3.hpp"

class FloppyMesh : public Drawable {
   public:
    /// Constructor for creating a mesh from an obj.
    FloppyMesh(std::string meshPath, glm::vec3 initialTranslation = glm::vec3(0.0f), float initialScale = 1.0f,
               float initialRotation = 0.0f, float subsequentRotationSpeed = 0.0f);
    /// Simplified constructor.
    FloppyMesh(std::string meshPath, float initialScale = 1.0f, float initialRotation = 0.0f);

    /// Constructor for creating the next mesh part.
    FloppyMesh(std::string meshPath, uint meshIndex);

    ~FloppyMesh() override;

    /**
     * @brief initialize the mesh.
     */
    void init() override;

    /**
     * @brief update Updates the object's position, rotation etc.
     * @param elapsedTimeMs The elapsed time since the last update in ms
     * @param modelViewMatrix the mode view matrix of the parent object
     */
    void update(float elapsedTimeMs, glm::mat4 modelViewMatrix) override;

    /**
     * @brief draw the mesh.
     * @param projectionMatrix - transformation into NDC.
     * @param lightPositions - array holding the light positions.
     * @param moonDirection - vector holding the moon direction.
     */
    void draw(glm::mat4 projectionMatrix, GLfloat lightPositions[], glm::vec3 moonDirection) override;

    /**
     * @brief re-sets the initial rotation of the mesh.
     * @param rotation - the new initial rotation of the mesh in degrees.
     */
    void setRotation(const float rotation) { _initialRotation = rotation; }

   protected:
    GLuint _verticeAmount;                     /**< The amount of vertices used to draw bill */
    std::shared_ptr<FloppyMesh> _nextMeshPart; /**< The subsequent part of the mesh if it exists */

    // Mesh and material.
    float _shininess;          /**< The shininess of the object. */
    float _transparency;       /**< The transparency/dissolve/alpha of the object. */
    GLuint _meshIndex;         /**< The part of mesh to render, will be used to recursively render each part */
    GLuint _textureHandle;     /**< handle of the background texture */
    std::string _meshPath;     /**< The filepath of the mesh. */
    std::string _textureName;  /**< The name of the texture file. */
    glm::vec3 _emissiveColour; /**< The colour of the emission of the object. */

    // Transformations, initial and ongoing.
    glm::mat4 _modelViewMatrix;    /**< The model view matrix to get the object into model view space */
    glm::vec3 _initialTranslation; /**< The initial translation applied as a baseline to the mesh */
    float _initialScale;           /**< The initial scaling applied as a baseline to the mesh */
    float _initialRotation;        /**< The initial rotation around the Y-axis applied as a baseline to the mesh */
    /**< The subsequent rotation around the Y-axis applied to the mesh. Mostly for debugging */
    float _subsequentRotation;
    /**< The subsequent rotation speed around the Y-axis applied to the mesh. Mostly for debugging */
    float _subsequentRotationSpeed;

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
};

#endif  // FLOPPY_MESH_H
