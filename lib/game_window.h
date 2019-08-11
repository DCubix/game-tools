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

		ErrorCode run(double frameRate = 60.0);

		u32 width() const { return m_width; }
		u32 height() const { return m_height; }

		std::string title() const;
		void title(const std::string& title);

		const Vector2& mousePosition() const { return m_mousePosition; }

		double currentTime() const;
		float msPerFrame() const { return m_frameRate; }

		bool keyPressed(u32 key) const;
		bool keyReleased(u32 key) const;
		bool keyHeld(u32 key) const;

		bool mousePressed(u32 key) const;
		bool mouseReleased(u32 key) const;
		bool mouseHeld(u32 key) const;

	private:
		SDL_Window* m_window;
		SDL_GLContext m_context;

		std::unique_ptr<GameAdapter> m_adapter;

		u32 m_width, m_height;

		float m_frameRate;
		u32 m_frame;

		struct State {
			bool pressed, released, held;
		};

		std::unordered_map<u32, State> m_keyboard, m_mouse;
		Vector2 m_mousePosition;

		void cleanup();
	};
}

#endif // GAME_WINDOW_H