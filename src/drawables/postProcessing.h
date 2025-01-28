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
     * @brief Bind the multi-sampled framebuffer.
     */
    void bind();

    /**
     * @brief Unbinds the framebuffer, i.e. binds the default framebuffer.
     */
    void unbind();

    /**
     * @brief Binds the multi-sampled framebuffer as read, and the post process buffer as write.
     * Then blits the contents from the multi-sampled framebuffer to the post-process buffer.

     */
    void blitBuffers();

    /**
     * @brief Delete the framebuffer.
     */
    void destroy();

    /**
     * @brief attaches the texture to the framebuffer.
     */
    void resetBufferTextures(int width, int height);

   protected:
    GLuint _textureColourBuffer;   /**< Texture handle (memory location of texture). */
    GLuint _depthStencilBuffer;    /**< Texture handle for depth and stencil (memory location of texture). */
    GLuint _postProcessingTexture; /**< Texture handle for post-processing (memory location of texture). */

    GLuint _renderBufferObject; /**< Render buffer handle (memory location of renderbuffer). */

    GLuint _frameBufferObject; /**< Frame buffer handle (memory location of framebuffer). */
    GLuint
        _postProcessFrameBufferObject; /**< Frame buffer handle for post-processing (memory location of framebuffer). */
};

#endif  // PostProcessingQuad_H
