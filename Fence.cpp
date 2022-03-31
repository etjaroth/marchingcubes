#include "Fence.h"

Fence::Fence(bool active) {
	if (active) {
		set();
	}
}

Fence::~Fence() {
	release();
}


bool Fence::isDone() {
	if (!active) {
		return true;
	}

	GLint syncStatus[1] = { GL_UNSIGNALED };
	glGetSynciv(fence, GL_SYNC_STATUS, sizeof(GLint), NULL, syncStatus);
	return (syncStatus[0] == GL_SIGNALED);
}

void Fence::waitUntilDone() {
	while (true) {
		GLenum e = glClientWaitSync(fence, 0, 100);
		if (e == GL_CONDITION_SATISFIED || e == GL_ALREADY_SIGNALED) {
			break;
		}
	}
}

void Fence::set() {
	if (!active) {
		fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		active = true;
	}
}

void Fence::release() {
	if (active) {
		glDeleteSync(fence);
		active = false;
	}
}
