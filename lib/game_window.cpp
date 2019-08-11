#include "game_window.h"

#include <iostream>
#include "glad/glad.h"

#include "log.h"

static void APIENTRY DebugCallback(
		GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* msg, const void* ud
) {
	std::string src = "";
	switch (source) {
		case GL_DEBUG_SOURCE_API: src = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: src = "WINDOW SYSTEM"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: src = "SHADER COMPILER"; break;
		case GL_DEBUG_SOURCE_APPLICATION: src = "APPLICATION"; break;
		default: src = "OTHER"; break;
	}

	std::string typ = "";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: typ = "ERROR"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typ = "DEPRECATED"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typ = "U.B."; break;
		case GL_DEBUG_TYPE_PERFORMANCE: typ = "PERFORMANCE"; break;
		default: src = "OTHER"; break;
	}

	Log::LogLevel lvl = Log::Info;
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW: lvl = Log::Info; break;
		case GL_DEBUG_SEVERITY_MEDIUM: lvl = Log::Warning; break;
		case GL_DEBUG_SEVERITY_HIGH: lvl = Log::Error; break;
		default: break;
	}

	LogPrint(lvl, "OpenGL [", typ, "][", src, "] ", msg);
}

namespace gt {
	GameWindow::GameWindow(GameAdapter* adapter, u32 width, u32 height) {
		if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
			LogE(SDL_GetError());
			return;
		}

		m_width = width;
		m_height = height;
		m_adapter = std::unique_ptr<GameAdapter>(adapter);

		m_window = SDL_CreateWindow(
			"Game Tools",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);

		if (!m_window) {
			LogE(SDL_GetError());
			cleanup();
			return;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#ifndef NDEBUG
		int contextFlags = 0;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
		contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
#endif

		m_context = SDL_GL_CreateContext(m_window);
		if (!m_context) {
			LogE(SDL_GetError());
			cleanup();
			return;
		}

		if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
			LogE(SDL_GetError());
			cleanup();
			return;
		}

		LogI("Created a ", width, "x", height, " window.");

#ifndef NDEBUG
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugCallback, 0);
#endif
	}

	void GameWindow::cleanup() {
		if (m_context) SDL_GL_DeleteContext(m_context);
		if (m_window) SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	double GameWindow::currentTime() const {
		return double(SDL_GetTicks()) / 1000.0;
	}

	std::string GameWindow::title() const {
		return std::string(SDL_GetWindowTitle(m_window));
	}

	void GameWindow::title(const std::string& title) {
		SDL_SetWindowTitle(m_window, title.c_str());
	}

	ErrorCode GameWindow::run() {
		if (!m_window) {
			cleanup();
			return ErrorInvalidObject;
		}

		if (!m_adapter) {
			cleanup();
			return ErrorInvalidGameAdapter;
		}

		m_adapter->create(*this);

		SDL_Event evt;
		bool running = true;

		const double timeStep = 1.0 / 60.0;
		double lastTime = currentTime(), accum = 0.0;

		while (running) {
			bool canRender = false;
			double currTime = currentTime();
			double delta = currTime - lastTime;
			lastTime = currTime;
			accum += delta;

			// Input Handling
			while (SDL_PollEvent(&evt)) {
				switch (evt.type) {
					case SDL_QUIT: running = false; break;
					case SDL_KEYDOWN: {
						u32 k = evt.key.keysym.sym;
						m_keyboard[k].pressed = true;
						m_keyboard[k].held = true;
					} break;
					case SDL_KEYUP: {
						u32 k = evt.key.keysym.sym;
						m_keyboard[k].released = true;
						m_keyboard[k].held = false;
					} break;
					case SDL_MOUSEBUTTONDOWN: {
						u32 b = evt.button.button;
						m_mouse[b].pressed = true;
						m_mouse[b].held = true;
						m_mousePosition.x = evt.button.x;
						m_mousePosition.y = evt.button.y;
					} break;
					case SDL_MOUSEBUTTONUP: {
						u32 b = evt.button.button;
						m_mouse[b].released = true;
						m_mouse[b].held = false;
						m_mousePosition.x = evt.button.x;
						m_mousePosition.y = evt.button.y;
					} break;
					case SDL_MOUSEMOTION: {
						m_mousePosition.x = evt.motion.x;
						m_mousePosition.y = evt.motion.y;
					} break;
				}
			}

			while (accum >= timeStep) {
				accum -= timeStep;
				m_adapter->update(*this, float(timeStep));
				canRender = true;

				for (auto& [k, v] : m_keyboard) {
					v.pressed = false;
					v.released = false;
				}

				for (auto& [k, v] : m_mouse) {
					v.pressed = false;
					v.released = false;
				}
			}

			if (canRender) {
				m_adapter->render(*this);
				SDL_GL_SwapWindow(m_window);
			}
		}

		m_adapter->destroy(*this);
		cleanup();

		return ErrorNone;
	}

	bool GameWindow::keyPressed(u32 key) const {
		auto pos = m_keyboard.find(key);
		if (pos == m_keyboard.end()) return false;
		return pos->second.pressed;
	}

	bool GameWindow::keyReleased(u32 key) const {
		auto pos = m_keyboard.find(key);
		if (pos == m_keyboard.end()) return false;
		return pos->second.released;
	}

	bool GameWindow::keyHeld(u32 key) const {
		auto pos = m_keyboard.find(key);
		if (pos == m_keyboard.end()) return false;
		return pos->second.held;
	}

	bool GameWindow::mousePressed(u32 key) const {
		auto pos = m_mouse.find(key);
		if (pos == m_mouse.end()) return false;
		return pos->second.pressed;
	}

	bool GameWindow::mouseReleased(u32 key) const {
		auto pos = m_mouse.find(key);
		if (pos == m_mouse.end()) return false;
		return pos->second.released;
	}

	bool GameWindow::mouseHeld(u32 key) const {
		auto pos = m_mouse.find(key);
		if (pos == m_mouse.end()) return false;
		return pos->second.held;
	}

}