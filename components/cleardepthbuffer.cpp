#include "cleardepthbuffer.h"

int ClearDepthBuffer::setup(Shader *) {
    return 0;
}

void ClearDepthBuffer::draw(Shader *) {
    glClear(GL_DEPTH_BUFFER_BIT);

}