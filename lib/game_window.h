#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#if __has_include("SDL.h")
#	include "SDL.h"
#else
#	include "SDL2/SDL.h"
#endif

#include "stl.hpp"
#include "math/vector.hpp"

#include <string>
#include <memory>
#include <unordered_map>

namespace gt {
	enum ErrorCode {
		ErrorNone = 0,
		ErrorInvalidGameAdapter,
		ErrorInvalidObject
	};

	class GameWindow;
	class GameAdapter {
	public:
		GameAdapter() = default;
		virtual ~GameAdapter() = default;

		virtual void create(GameWindow& gameWindow) {}
		virtual void update(GameWindow& gameWindow, float dt) {}
		virtual void render(GameWindow& gameWindow) {}
		virtual void destroy(GameWindow& gameWindow) {}
	};

	class GameWindow {
	public:
		GameWindow() = default;
		~GameWindow() = default;

		GameWindow(GameAdapter* adapter, u32 width, u32 height);

		ErrorCode run();

		u32 width() const { return m_width; }
		u32 height() const { return m_height; }

		std::string title() const;
		void title(const std::string& title);

		const Vector2& mousePosition() const { return m_mousePosition; }

		double currentTime() const;

	private:
		SDL_Window* m_window;
		SDL_GLContext m_context;

		std::unique_ptr<GameAdapter> m_adapter;

		u32 m_width, m_height;

		struct State {
			bool pressed, released, held;
		};

		std::unordered_map<u32, State> m_keyboard, m_mouse;
		Vector2 m_mousePosition;

		void cleanup();
	};
}

#endif // GAME_WINDOW_H