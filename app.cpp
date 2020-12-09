#include <iostream>
#include <string>

#include "SDL.h"

#include "app.h"

// 函數 (方法) 的實作 (implementations)

/**
 *  App constructor.  Initialize the first SDL window and its
 *  renderer.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
App::App(const std::string& name):
    renderer_{ nullptr }, surface_{ nullptr}, window_{ nullptr },
    end_{ false }, invoked_{ name } {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { // initialize SDL
        SDL_LogCritical(                // Video subsystem.
            SDL_LOG_CATEGORY_VIDEO, "Error: %s\n", SDL_GetError()
        );
    }

    if (TTF_Init() == -1) { // initialize SDL_ttf extension.
        SDL_LogCritical(
            SDL_LOG_CATEGORY_VIDEO, "Error: %s\n", SDL_GetError()
        );
    }

    window_ = SDL_CreateWindow( // create the main SDL window.
        name.c_str(), 100, 100, 800, 600, SDL_WINDOW_SHOWN
    );

    if (!window_) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION, "Error: %s\n", SDL_GetError()
        );
    }

    renderer_ = SDL_CreateRenderer(window_, -1, 0); // create the renderer.

    if (!renderer_) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION, "Error: %s\n", SDL_GetError()
        );
    }
} // App::App()

/**
 *  App destructor.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
App::~App() {
    SDL_DestroyWindow(window_);

    TTF_Quit();

    SDL_Quit();
} // App::~App()

/**
 *  Register a Event Listener。
 *
 *  @param const std::string&: The reference of Event name.
 *  @param const Listener *: Pointer to the Event Listner.
 *  @return none.
 *  @since  0.1.0
 **/
void App::add_listener(const std::string& event, Listener *listener) {
    listener_[event] = listener;
} // App::add_listener

/**
 *  Render the 'Hello'... messages to the screen.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void App::hello() {
    Text text_ = Text("../resources/font/jason_writing_1.ttf");
    SDL_Texture *hello = text_.get_text(renderer_, "SEVENTEEN");

    SDL_Rect rect;
    rect.x = (800 - 400) / 2;
    rect.y = (600 - 200) / 2;
    rect.w = 400;
    rect.h = 200;

    SDL_Rect rect2;
    rect2.x = 0;
    rect2.y = 0;
    rect2.w = 200;
    rect2.h = 100;

    SDL_RenderCopy(renderer_, hello, &rect2, &rect);
    SDL_RenderPresent(renderer_);
} // App::hello()

/**
 *  Starts the game.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void App::start() {
    loop_();
} // App::start()

/**
 *  Print the App's version info.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void App::version() {
    std::cout
        << ENV_SYMBOL(PROJECT_NAME) << " "
        << ENV_SYMBOL(VERSION_MAJOR) << "."
        << ENV_SYMBOL(VERSION_MINOR) << "."
        << ENV_SYMBOL(VERSION_PATCH) << "b"
        << ENV_SYMBOL(BUILD_NUMBER) << ". "
        << "bulit at "
        << ENV_SYMBOL(BUILD_TIME) << ", "
        << ENV_SYMBOL(BUILD_DATE)
        << std::endl;
} // App::version()

/**
 *  The main-loop of the game.  Game over when the loop ends.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void App::loop_() {
    while (!end_) {  // 程式主迴圈 (main-loop)
        hello();        // 印出訊息

        get_input_();   // 取得使用者輸入；轉換成 Event 物件
                        // 放進 Event queue (佇列)

        dispatch_();    // 處理 Event queue 裡的事件
    } // od
} // App::loop_()

/**
 *  Event processing loop.  Pop out Event from event queue,
 *  passing it to corresponding listeners, loop.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void App::dispatch_() {
    while (!queue_.empty()) {
        std::shared_ptr<Event> e = queue_.front();
        queue_.pop();

        listener_[e->event]->on(e);
    }
} // App::dispatch_()

/**
 *  Get user input from keyboard/mouse and put into event queue for
 *  later processing.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void App::get_input_() {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {   // 取得使用者輸入
        switch (e.type) {
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                get_mouse_(e);

                break;

            case SDL_WINDOWEVENT:
                if (e.window.event != SDL_WINDOWEVENT_CLOSE) {
                    break;
                }

            case SDL_QUIT:
                end_ = true;

                break;

            default:
                break;
        }
    }
} // App::get_input_()

/**
 *  Get mouse event, traslate into MouseEvent object, and put into
 *  event queue for later processing.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void App::get_mouse_(SDL_Event& e) {
    if (
        e.type == SDL_MOUSEBUTTONUP &&
        e.button.button == SDL_BUTTON_LEFT
    ) {
        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION, "Mouse Left Button clicked!\n"
        );

        queue_.push(
            std::make_shared<Event>(
                "click", e.motion.x, e.motion.y, 0
            )
        );
    }
} // App::get_mouse_()

// app.cpp
