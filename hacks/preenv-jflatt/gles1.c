#include <SDL.h>
#include <SDL_gles.h>
#include <glib.h>

#define GL_GLEXT_PROTOTYPES 1

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include "debug.h"

#define DEBUG_DOMAIN "GLES1"

/* This file contains some GL_GLEXT_PROTOTYPES that the Pre libGLES exports,
 * but our local libGLES might not. */

#define PRE_GL_PROTO_VOID(funcname, typename, args, args_names) \
	void funcname args { \
		static typename func = NULL; \
		if (!func)	{ \
			func = (typename) eglGetProcAddress( G_STRINGIFY (funcname) ); \
			if (!func) { \
				WARN("You do not have GLES1 function '%s'", \
					G_STRINGIFY (funcname) ); \
				return; \
			} \
		} \
		func args_names; \
	}

#define PRE_GL_PROTO(funcname, typename, args, args_names, return_type) \
	return_type funcname args { \
		static typename func = NULL; \
		if (!func)	{ \
			func = (typename) eglGetProcAddress( G_STRINGIFY (funcname) ); \
			if (!func) { \
				WARN("You do not have GLES1 function '%s'", \
					G_STRINGIFY (funcname) ); \
				return 0; \
			} \
		} \
		return func args_names; \
	}

#ifdef GL_OES_framebuffer_object
PRE_GL_PROTO(glIsRenderbufferOES, PFNGLISRENDERBUFFEROESPROC, (GLuint renderbuffer),(renderbuffer), GLboolean);
PRE_GL_PROTO_VOID(glBindRenderbufferOES, PFNGLBINDRENDERBUFFEROESPROC, (GLenum target, GLuint renderbuffer), (target, renderbuffer));
PRE_GL_PROTO_VOID(glDeleteRenderbuffersOES, PFNGLDELETERENDERBUFFERSOESPROC, (GLsizei n, const GLuint* renderbuffers), (n, renderbuffers));
PRE_GL_PROTO_VOID(glGenRenderbuffersOES, PFNGLGENRENDERBUFFERSOESPROC, (GLsizei n, GLuint* renderbuffers), (n, renderbuffers));
PRE_GL_PROTO_VOID(glRenderbufferStorageOES, PFNGLRENDERBUFFERSTORAGEOESPROC, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height), (target, internalformat, width, height));
PRE_GL_PROTO_VOID(glGetRenderbufferParameterivOES, PFNGLGETRENDERBUFFERPARAMETERIVOESPROC, (GLenum target, GLenum pname, GLint* params), (target, pname, params));
PRE_GL_PROTO(glIsFramebufferOES, PFNGLISFRAMEBUFFEROESPROC, (GLuint framebuffer), (framebuffer), GLboolean);
PRE_GL_PROTO_VOID(glBindFramebufferOES, PFNGLBINDFRAMEBUFFEROESPROC, (GLenum target, GLuint framebuffer), (target, framebuffer));
PRE_GL_PROTO_VOID(glDeleteFramebuffersOES, PFNGLDELETEFRAMEBUFFERSOESPROC, (GLsizei n, const GLuint* framebuffers), (n, framebuffers));
PRE_GL_PROTO_VOID(glGenFramebuffersOES, PFNGLGENFRAMEBUFFERSOESPROC, (GLsizei n, GLuint* framebuffers), (n, framebuffers));
PRE_GL_PROTO(glCheckFramebufferStatusOES, PFNGLCHECKFRAMEBUFFERSTATUSOESPROC, (GLenum target), (target), GLenum);
PRE_GL_PROTO_VOID(glFramebufferRenderbufferOES, PFNGLFRAMEBUFFERRENDERBUFFEROESPROC, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer), (target, attachment, renderbuffertarget, renderbuffer));
PRE_GL_PROTO_VOID(glFramebufferTexture2DOES, PFNGLFRAMEBUFFERTEXTURE2DOESPROC, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level), (target, attachment, textarget, texture, level));
PRE_GL_PROTO_VOID(glGetFramebufferAttachmentParameterivOES, PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC, (GLenum target, GLenum attachment, GLenum pname, GLint* params), (target, attachment, pname, params));
PRE_GL_PROTO_VOID(glGenerateMipmapOES, PFNGLGENERATEMIPMAPOESPROC, (GLenum target), (target));
#endif /* GL_OES_framebuffer_object */

#ifdef GL_OES_matrix_palette
PRE_GL_PROTO_VOID(glCurrentPaletteMatrixOES, PFNGLCURRENTPALETTEMATRIXOESPROC, (GLuint matrixpaletteindex), (matrixpaletteindex));
PRE_GL_PROTO_VOID(glLoadPaletteFromModelViewMatrixOES, PFNGLLOADPALETTEFROMMODELVIEWMATRIXOESPROC, (void), ());
PRE_GL_PROTO_VOID(glMatrixIndexPointerOES, PFNGLMATRIXINDEXPOINTEROESPROC, (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer), (size, type, stride, pointer));
PRE_GL_PROTO_VOID(glWeightPointerOES, PFNGLWEIGHTPOINTEROESPROC, (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer), (size, type, stride, pointer));
#endif /* GL_OES_matrix_palette */

