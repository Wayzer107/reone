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

#include <boost/test/unit_test.hpp>

#include "reone/game/game.h"
#include "reone/system/stream/fileoutput.h"

#include "../fixtures/audio.h"
#include "../fixtures/engine.h"
#include "../fixtures/game.h"
#include "../fixtures/graphics.h"
#include "../fixtures/movie.h"
#include "../fixtures/resource.h"
#include "../fixtures/scene.h"
#include "../fixtures/script.h"
#include "../fixtures/system.h"

using namespace std;

using namespace reone;
using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::movie;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::ReturnRef;

BOOST_AUTO_TEST_SUITE(game)

BOOST_AUTO_TEST_CASE(should_play_legal_movie_on_launch) {
    // given
    auto gamePath = boost::filesystem::path();

    auto engine = make_unique<TestEngine>();
    engine->init();

    auto sceneNames = set<string>();
    EXPECT_CALL(engine->sceneModule().graphs(), sceneNames())
        .WillOnce(Return(sceneNames));

    auto moduleNames = set<string>();
    EXPECT_CALL(engine->gameModule().resourceLayout(), moduleNames())
        .WillOnce(Return(moduleNames));

    auto game = Game(GameID::KotOR, gamePath, engine->options(), engine->services());
    game.init();

    auto legalMovie = make_shared<MockMovie>();
    EXPECT_CALL(*legalMovie, update(_)).Times(1);
    EXPECT_CALL(*legalMovie, isFinished()).WillOnce(Return(false));
    EXPECT_CALL(*legalMovie, render()).Times(1);

    EXPECT_CALL(engine->movieModule().movies(), get(_))
        .WillOnce(Return(legalMovie));

    EXPECT_CALL(engine->graphicsModule().window(), processEvents(_))
        .WillOnce(Invoke([](bool &quit) { quit = false; }))
        .WillRepeatedly(Invoke([](bool &quit) { quit = true; }));

    EXPECT_CALL(engine->graphicsModule().window(), isInFocus())
        .WillOnce(Return(true));

    // when
    game.run();

    // then
    auto movie = game.movie();
    BOOST_TEST(static_cast<bool>(movie));
    BOOST_TEST((movie == legalMovie));
}

BOOST_AUTO_TEST_CASE(should_present_main_menu_on_launch_when_movie_is_finished) {
    // given
    auto gamePath = boost::filesystem::path();

    auto engine = make_unique<TestEngine>();
    engine->init();

    auto sceneNames = set<string>();
    EXPECT_CALL(engine->sceneModule().graphs(), sceneNames())
        .WillOnce(Return(sceneNames));

    auto moduleNames = set<string>();
    EXPECT_CALL(engine->gameModule().resourceLayout(), moduleNames())
        .WillOnce(Return(moduleNames));

    auto game = Game(GameID::KotOR, gamePath, engine->options(), engine->services());
    game.init();

    auto legalMovie = make_shared<MockMovie>();
    EXPECT_CALL(*legalMovie, update(_)).Times(1);
    EXPECT_CALL(*legalMovie, isFinished()).WillOnce(Return(true));

    EXPECT_CALL(engine->movieModule().movies(), get(_))
        .WillOnce(Return(legalMovie));

    auto gui = make_shared<MockGUI>();
    auto label = make_shared<Label>(
        *gui,
        engine->sceneModule().graphs(),
        engine->graphicsModule().services(),
        engine->resourceModule().strings());
    auto button = make_shared<Button>(
        *gui,
        engine->sceneModule().graphs(),
        engine->graphicsModule().services(),
        engine->resourceModule().strings());
    auto listBox = make_shared<ListBox>(
        *gui,
        engine->sceneModule().graphs(),
        engine->graphicsModule().services(),
        engine->resourceModule().strings());
    EXPECT_CALL(*gui, getControl("LB_MODULES")).WillOnce(Return(listBox));
    EXPECT_CALL(*gui, getControl("LBL_3DVIEW")).WillOnce(Return(label));
    EXPECT_CALL(*gui, getControl("LBL_GAMELOGO")).WillOnce(Return(label));
    EXPECT_CALL(*gui, getControl("LBL_BW")).WillOnce(Return(label));
    EXPECT_CALL(*gui, getControl("LBL_LUCAS")).WillOnce(Return(label));
    EXPECT_CALL(*gui, getControl("BTN_LOADGAME")).WillOnce(Return(button));
    EXPECT_CALL(*gui, getControl("BTN_NEWGAME")).WillOnce(Return(button));
    EXPECT_CALL(*gui, getControl("BTN_MOVIES")).WillOnce(Return(button));
    EXPECT_CALL(*gui, getControl("BTN_OPTIONS")).WillOnce(Return(button));
    EXPECT_CALL(*gui, getControl("LBL_NEWCONTENT")).WillOnce(Return(label));
    EXPECT_CALL(*gui, getControl("BTN_EXIT")).WillOnce(Return(button));
    EXPECT_CALL(*gui, getControl("BTN_WARP")).WillOnce(Return(button));
    EXPECT_CALL(*gui, getControl("LBL_MENUBG")).WillOnce(Return(label));
    EXPECT_CALL(*gui, update(_)).Times(1);
    EXPECT_CALL(*gui, draw()).Times(1);

    EXPECT_CALL(engine->guiModule().guis(), get("mainmenu16x12", _))
        .WillOnce(Return(gui));

    auto mainMenuScene = make_shared<MockSceneGraph>();
    EXPECT_CALL(engine->sceneModule().graphs(), get(kSceneMainMenu))
        .WillOnce(ReturnRef(*mainMenuScene));

    EXPECT_CALL(engine->graphicsModule().window(), processEvents(_))
        .WillOnce(Invoke([](bool &quit) { quit = false; }))
        .WillRepeatedly(Invoke([](bool &quit) { quit = true; }));

    EXPECT_CALL(engine->graphicsModule().window(), isInFocus())
        .WillOnce(Return(true));

    auto mainScene = make_shared<MockSceneGraph>();
    EXPECT_CALL(engine->sceneModule().graphs(), get(kSceneMain))
        .WillOnce(ReturnRef(*mainScene));

    // when
    game.run();

    // then
    BOOST_TEST((game.currentScreen() == Game::Screen::MainMenu));
}

BOOST_AUTO_TEST_SUITE_END()
