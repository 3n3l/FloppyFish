#ifndef PostProcessingQuad_H
#define PostProcessingQuad_H

#include "src/drawables/drawable.h"

class PostProcessingQuad : public Drawable {
   public:
    PostProcessingQuad();

    /**
     * @brief draw the PostProcessingQuad.
     */
    void draw();

    /**
     * @brief initialize the PostProcessingQuad.
     */
    void init() override;

    /**
     * @brief bind the framebuffer.
     */
    void bind();

    /**
     * @brief unbind the framebuffer.
     */
    void unbind();

    /**
     * @brief Delete the framebuffer.
     */
    void destroy();

    /**
     * @brief attaches the texture to the framebuffer.
     */
    void resetBufferTextures(int width, int height);

   protected:
    GLuint _textureColourBuffer; /**< Texture handle (memory location of texture). */
    GLuint _depthStencilBuffer;  /**< Texture handle for depth and stencil (memory location of texture). */
    GLuint _frameBufferObject;   /**< Frame buffer handle (memory location of framebuffer). */
};

#endif  // PostProcessingQuad_H
