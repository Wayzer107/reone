/*
 * Copyright (c) 2020-2023 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "engine.h"

#include "SDL2/SDL.h"

#include "reone/graphics/window.h"
#include "reone/resource/gameprobe.h"

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::movie;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

void Engine::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
    }
    _window = std::make_unique<Window>(_options.graphics);
    _window->init();

    _optionsView = _options.toView();
    GameProbe probe {_options.game.path};
    auto gameId = probe.probe();

    _clock = std::make_unique<Clock>();
    _systemModule = std::make_unique<SystemModule>(*_clock);
    _graphicsModule = std::make_unique<GraphicsModule>(_options.graphics);
    _audioModule = std::make_unique<AudioModule>(_options.audio);
    _movieModule = std::make_unique<MovieModule>();
    _scriptModule = std::make_unique<ScriptModule>();
    _resourceModule = std::make_unique<ResourceModule>(
        gameId,
        _options.game.path,
        _options.graphics,
        _options.audio,
        *_graphicsModule,
        *_audioModule,
        *_scriptModule);
    _sceneModule = std::make_unique<SceneModule>(
        _options.graphics,
        *_resourceModule,
        *_graphicsModule,
        *_audioModule);
    _guiModule = std::make_unique<GUIModule>(
        _options.graphics,
        *_sceneModule,
        *_graphicsModule,
        *_resourceModule);
    _gameModule = std::make_unique<GameModule>(
        gameId,
        *_optionsView,
        *_resourceModule,
        *_graphicsModule,
        *_audioModule,
        *_sceneModule,
        *_scriptModule);
    _systemModule->init();
    _graphicsModule->init();
    _audioModule->init();
    _movieModule->init();
    _scriptModule->init();
    _resourceModule->init();
    _sceneModule->init();
    _guiModule->init();
    _gameModule->init();

    _services = std::make_unique<ServicesView>(
        _gameModule->services(),
        _movieModule->services(),
        _audioModule->services(),
        _graphicsModule->services(),
        _sceneModule->services(),
        _guiModule->services(),
        _scriptModule->services(),
        _resourceModule->services(),
        _systemModule->services());
    _game = std::make_unique<Game>(gameId, _options.game.path, *_optionsView, *_services);
    _game->init();
}

void Engine::deinit() {
    _game.reset();
    _services.reset();

    _gameModule.reset();
    _guiModule.reset();
    _sceneModule.reset();
    _resourceModule.reset();
    _scriptModule.reset();
    _movieModule.reset();
    _audioModule.reset();
    _graphicsModule.reset();
    _systemModule.reset();
    _clock.reset();

    _optionsView.reset();
    _window.reset();

    SDL_Quit();
}

int Engine::run() {
    auto &clock = _services->system.clock;
    _ticks = clock.ticks();
    while (!_quit) {
        processEvents();
        if (_quit) {
            break;
        }
        auto ticks = clock.ticks();
        auto frameTime = (ticks - _ticks) / 1000.0f;
        if (_window->isInFocus()) {
            _game->update(frameTime);
            showCursor(_game->cursorType() == CursorType::None);
            setRelativeMouseMode(_game->relativeMouseMode());
            if (_options.graphics.pbr) {
                _services->graphics.pbrTextures.refresh();
            }
            _services->graphics.context.clearColorDepth();
            _game->render();
            _window->swap();
        }
        _ticks = ticks;
    }
    return 0;
}

void Engine::processEvents() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        if (sdlEvent.type == SDL_QUIT) {
            _quit = true;
            break;
        }
        if (!_window->isAssociatedWith(sdlEvent)) {
            continue;
        }
        if (_window->handle(sdlEvent)) {
            if (_window->isCloseRequested()) {
                _quit = true;
                break;
            }
            continue;
        }
        auto event = eventFromSDLEvent(sdlEvent);
        if (!event) {
            continue;
        }
        if (_game->handle(*event) && _game->isQuitRequested()) {
            _quit = true;
        }
    }
}

void Engine::showCursor(bool show) {
    if (_showCursor == show) {
        return;
    }
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
    _showCursor = show;
}

void Engine::setRelativeMouseMode(bool relative) {
    if (_relativeMouseMode == relative) {
        return;
    }
    SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
    _relativeMouseMode = relative;
}

std::optional<input::Event> Engine::eventFromSDLEvent(const SDL_Event &sdlEvent) const {
    switch (sdlEvent.type) {
    case SDL_KEYDOWN:
        return input::Event::newKeyDown(input::KeyEvent {
            sdlEvent.key.state == SDL_PRESSED,
            static_cast<input::KeyCode>(sdlEvent.key.keysym.sym),
            sdlEvent.key.keysym.mod,
            static_cast<bool>(sdlEvent.key.repeat)});
    case SDL_KEYUP:
        return input::Event::newKeyUp(input::KeyEvent {
            sdlEvent.key.state == SDL_PRESSED,
            static_cast<input::KeyCode>(sdlEvent.key.keysym.sym),
            sdlEvent.key.keysym.mod,
            static_cast<bool>(sdlEvent.key.repeat)});
    case SDL_MOUSEMOTION:
        return input::Event::newMouseMotion(input::MouseMotionEvent {
            sdlEvent.motion.x,
            sdlEvent.motion.y,
            sdlEvent.motion.xrel,
            sdlEvent.motion.yrel});
    case SDL_MOUSEBUTTONDOWN:
        return input::Event::newMouseButtonDown(input::MouseButtonEvent {
            static_cast<input::MouseButton>(sdlEvent.button.button),
            sdlEvent.button.state == SDL_PRESSED,
            sdlEvent.button.clicks,
            sdlEvent.button.x,
            sdlEvent.button.y});
    case SDL_MOUSEBUTTONUP:
        return input::Event::newMouseButtonUp(input::MouseButtonEvent {
            static_cast<input::MouseButton>(sdlEvent.button.button),
            sdlEvent.button.state == SDL_PRESSED,
            sdlEvent.button.clicks,
            sdlEvent.button.x,
            sdlEvent.button.y});
    case SDL_MOUSEWHEEL: {
        return input::Event::newMouseWheel(input::MouseWheelEvent {
            sdlEvent.wheel.x,
            sdlEvent.wheel.y,
            static_cast<input::MouseWheelDirection>(sdlEvent.wheel.direction)});
    default:
        return std::nullopt;
    }
    }
}

} // namespace reone
