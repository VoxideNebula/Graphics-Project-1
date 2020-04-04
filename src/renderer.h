#ifndef _CSCI441_RENDERER_H_
#define _CSCI441_RENDERER_H_

#include <csci441/matrix3.h>

class Renderer {
public:

    template <typename M>
    void render(const Camera& camera, M& m, const Vector4& light) {

        m.shader.use();
        Uniform::set(m.shader.id(), "model", m.model);
        Uniform::set(m.shader.id(), "projection", camera.projection);
        Uniform::set(m.shader.id(), "camera", camera.look_at());
        Uniform::set(m.shader.id(), "viewPos", camera.eye);
        Uniform::set(m.shader.id(), "light", light);

        // render the cube
        glBindVertexArray(m.vao);
        glDrawArrays(GL_TRIANGLES, 0, m.size*sizeof(float));
    }
};

#endif
