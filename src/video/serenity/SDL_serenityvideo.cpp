/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  */
extern "C" {

#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_SERENITY
#    define AK_DONT_REPLACE_STD

#    include "../../events/SDL_events_c.h"
#    include "../SDL_pixels_c.h"
#    include "../SDL_sysvideo.h"
#    include "SDL_mouse.h"
#    include "SDL_syswm.h"
#    include "SDL_video.h"
}

#    include "SDL_serenityevents_c.h"
#    include "SDL_serenitymouse.h"
#    include "SDL_serenityvideo.h"

#    include <LibCore/EventLoop.h>
#    include <LibGUI/Application.h>
#    include <LibGUI/Desktop.h>
#    include <LibGUI/Painter.h>
#    include <LibGUI/Widget.h>
#    include <LibGUI/Window.h>
#    include <LibGfx/Bitmap.h>
#    include <LibMain/Main.h>
#    include <dlfcn.h>

static SDL_Scancode scancode_map[] = {
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_TAB,
    SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_INSERT,
    SDL_SCANCODE_DELETE,
    SDL_SCANCODE_PRINTSCREEN,
    SDL_SCANCODE_SYSREQ,
    SDL_SCANCODE_HOME,
    SDL_SCANCODE_END,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_PAGEUP,
    SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_RSHIFT,
    SDL_SCANCODE_LCTRL,
    SDL_SCANCODE_LALT,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_NUMLOCKCLEAR,
    SDL_SCANCODE_SCROLLLOCK,
    SDL_SCANCODE_F1,
    SDL_SCANCODE_F2,
    SDL_SCANCODE_F3,
    SDL_SCANCODE_F4,
    SDL_SCANCODE_F5,
    SDL_SCANCODE_F6,
    SDL_SCANCODE_F7,
    SDL_SCANCODE_F8,
    SDL_SCANCODE_F9,
    SDL_SCANCODE_F10,
    SDL_SCANCODE_F11,
    SDL_SCANCODE_F12,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_1,
    SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_5,
    SDL_SCANCODE_7,
    SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_9,
    SDL_SCANCODE_0,
    SDL_SCANCODE_8,
    SDL_SCANCODE_EQUALS,
    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_MINUS,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_SLASH,
    SDL_SCANCODE_0,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_5,
    SDL_SCANCODE_6,
    SDL_SCANCODE_7,
    SDL_SCANCODE_8,
    SDL_SCANCODE_9,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_EQUALS,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_SLASH,
    SDL_SCANCODE_2,
    SDL_SCANCODE_A,
    SDL_SCANCODE_B,
    SDL_SCANCODE_C,
    SDL_SCANCODE_D,
    SDL_SCANCODE_E,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_I,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L,
    SDL_SCANCODE_M,
    SDL_SCANCODE_N,
    SDL_SCANCODE_O,
    SDL_SCANCODE_P,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_R,
    SDL_SCANCODE_S,
    SDL_SCANCODE_T,
    SDL_SCANCODE_U,
    SDL_SCANCODE_V,
    SDL_SCANCODE_W,
    SDL_SCANCODE_X,
    SDL_SCANCODE_Y,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_BACKSLASH,
    SDL_SCANCODE_6,
    SDL_SCANCODE_MINUS,
    SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_BACKSLASH,
    SDL_SCANCODE_GRAVE,
    SDL_SCANCODE_GRAVE,
    SDL_SCANCODE_UNKNOWN,
};

/* Initialization/Query functions */
static int SERENITY_VideoInit(_THIS);
static int SERENITY_SetDisplayMode(_THIS, SDL_VideoDisplay* display,
    SDL_DisplayMode* mode);
static void SERENITY_VideoQuit(_THIS);

/* SERENITY driver bootstrap functions */

static int SERENITY_Available(void) { return (1); }

static void SERENITY_DeleteDevice(SDL_VideoDevice* device)
{
    dbgln("SERENITY_DeleteDevice");
    SDL_free(device);
}

extern int Serenity_CreateWindow(_THIS, SDL_Window* window);
extern void Serenity_ShowWindow(_THIS, SDL_Window* window);
extern void Serenity_HideWindow(_THIS, SDL_Window* window);
extern void Serenity_SetWindowTitle(_THIS, SDL_Window* window);
extern void Serenity_SetWindowSize(_THIS, SDL_Window* window);
extern void Serenity_SetWindowFullscreen(_THIS, SDL_Window* window,
    SDL_VideoDisplay* display,
    SDL_bool fullscreen);
extern void Serenity_SetWindowIcon(_THIS, SDL_Window* window, SDL_Surface* icon);
extern void Serenity_DestroyWindow(_THIS, SDL_Window* window);
extern SDL_bool Serenity_GetWindowWMInfo(_THIS, SDL_Window *window, struct SDL_SysWMinfo *info);
extern int Serenity_CreateWindowFramebuffer(_THIS, SDL_Window* window,
    Uint32* format, void** pixels,
    int* pitch);
extern int Serenity_UpdateWindowFramebuffer(_THIS, SDL_Window* window,
    const SDL_Rect* rects,
    int numrects);
extern void Serenity_DestroyWindowFramebuffer(_THIS, SDL_Window* window);
extern SDL_GLContext Serenity_GL_CreateContext(_THIS, SDL_Window* window);
extern void Serenity_GL_DeleteContext(_THIS, SDL_GLContext context);
extern void* Serenity_GL_GetProcAddress(_THIS, const char* proc);
extern int Serenity_GL_LoadLibrary(_THIS, const char* path);
extern int Serenity_GL_MakeCurrent(_THIS, SDL_Window* window, SDL_GLContext context);
extern int Serenity_GL_SwapWindow(_THIS, SDL_Window* window);

static SDL_VideoDevice* SERENITY_CreateDevice(void)
{
    SDL_VideoDevice* device;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice*)SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        return (0);
    }
    device->is_dummy = SDL_FALSE;

    /* Set the function pointers */
    device->VideoInit = SERENITY_VideoInit;
    device->VideoQuit = SERENITY_VideoQuit;
    device->SetDisplayMode = SERENITY_SetDisplayMode;
    device->PumpEvents = SERENITY_PumpEvents;

    device->CreateWindowFramebuffer = Serenity_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = Serenity_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = Serenity_DestroyWindowFramebuffer;
    device->CreateSDLWindow = Serenity_CreateWindow;
    device->ShowWindow = Serenity_ShowWindow;
    device->HideWindow = Serenity_HideWindow;
    device->SetWindowTitle = Serenity_SetWindowTitle;
    device->SetWindowSize = Serenity_SetWindowSize;
    device->SetWindowFullscreen = Serenity_SetWindowFullscreen;
    device->SetWindowIcon = Serenity_SetWindowIcon;
    device->DestroyWindow = Serenity_DestroyWindow;
    device->GetWindowWMInfo = Serenity_GetWindowWMInfo;

    device->GL_CreateContext = Serenity_GL_CreateContext;
    device->GL_DeleteContext = Serenity_GL_DeleteContext;
    device->GL_GetProcAddress = Serenity_GL_GetProcAddress;
    device->GL_LoadLibrary = Serenity_GL_LoadLibrary;
    device->GL_MakeCurrent = Serenity_GL_MakeCurrent;
    device->GL_SwapWindow = Serenity_GL_SwapWindow;

    device->free = SERENITY_DeleteDevice;

    Serenity_GL_LoadLibrary(device, nullptr);

    return device;
}

VideoBootStrap SERENITYVIDEO_bootstrap = { "serenity", "SDL serenity video driver", SERENITY_CreateDevice };

static RefPtr<GUI::Application> g_app;

int SERENITY_VideoInit(_THIS)
{
    dbgln("{}: Initialising SDL application", __FUNCTION__);

    if (!g_app) {
        g_app = MUST(GUI::Application::create(Main::Arguments {}));
        g_app->set_quit_when_last_window_deleted(false);
    }

    SERENITY_InitMouse(_this);

    // We only add the active desktop resolution until we properly support multiple
    // fullscreen resolutions
    auto desktop_rect = GUI::Desktop::the().rect();

    SDL_DisplayMode mode;
    mode.format = SDL_PIXELFORMAT_RGB888;
    mode.w = desktop_rect.width();
    mode.h = desktop_rect.height();
    mode.refresh_rate = 60;
    mode.driverdata = nullptr;

    if (SDL_AddBasicVideoDisplay(&mode) < 0)
        return -1;

    SDL_AddDisplayMode(&_this->displays[0], &mode);

    /* We're done! */
    return 0;
}

static int SERENITY_SetDisplayMode(_THIS, SDL_VideoDisplay* display,
    SDL_DisplayMode* mode)
{
    dbgln("SERENITY_SetDisplayMode");
    return 0;
}

void SERENITY_VideoQuit(_THIS)
{
    dbgln("SERENITY_VideoQuit");
    SERENITY_QuitMouse(_this);
}

SerenitySDLWidget::SerenitySDLWidget(SDL_Window* sdl_window)
    : m_sdl_window(sdl_window)
{
}

void SerenitySDLWidget::paint_event(GUI::PaintEvent& event)
{
    if (!m_buffer)
        return;

    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());
    painter.blit(event.rect().location(), *m_buffer, event.rect());
}

void SerenitySDLWidget::resize_event(GUI::ResizeEvent&)
{
    SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_RESIZED, width(), height());
}
void SerenitySDLWidget::show_event(GUI::ShowEvent&)
{
    SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_SHOWN, 0, 0);
}
void SerenitySDLWidget::hide_event(GUI::HideEvent&)
{
    SDL_SendWindowEvent(m_sdl_window, SDL_WINDOWEVENT_HIDDEN, 0, 0);
}

static int map_button(GUI::MouseButton button)
{
    switch (button) {
    case GUI::MouseButton::Primary:
        return SDL_BUTTON_LEFT;
    case GUI::MouseButton::Middle:
        return SDL_BUTTON_MIDDLE;
    case GUI::MouseButton::Secondary:
        return SDL_BUTTON_RIGHT;
    case GUI::MouseButton::Forward:
        return SDL_BUTTON_X1;
    case GUI::MouseButton::Backward:
        return SDL_BUTTON_X2;
    }

    VERIFY_NOT_REACHED();
    return 0;
}

void SerenitySDLWidget::mousedown_event(GUI::MouseEvent& event)
{
    SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
    SDL_SendMouseButton(m_sdl_window, 0, SDL_PRESSED, map_button(event.button()));
}

void SerenitySDLWidget::mousemove_event(GUI::MouseEvent& event)
{
    SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
}

void SerenitySDLWidget::mouseup_event(GUI::MouseEvent& event)
{
    SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
    SDL_SendMouseButton(m_sdl_window, 0, SDL_RELEASED, map_button(event.button()));
}

void SerenitySDLWidget::mousewheel_event(GUI::MouseEvent& event)
{
    SDL_SendMouseMotion(m_sdl_window, 0, 0, event.x(), event.y());
    SDL_SendMouseWheel(m_sdl_window, 0, (float)event.wheel_raw_delta_x(), -(float)event.wheel_raw_delta_y(), SDL_MOUSEWHEEL_NORMAL);
}

void SerenitySDLWidget::keydown_event(GUI::KeyEvent& event)
{
    SDL_SendKeyboardKey(SDL_PRESSED, scancode_map[event.key()]);
    auto text = event.text();
    if (!text.is_empty())
        SDL_SendKeyboardText(text.characters());
}

void SerenitySDLWidget::keyup_event(GUI::KeyEvent& event)
{
    SDL_SendKeyboardKey(SDL_RELEASED, scancode_map[event.key()]);
}

void SerenitySDLWidget::enter_event(Core::Event&)
{
    SDL_SetMouseFocus(m_sdl_window);
}

void SerenitySDLWidget::leave_event(Core::Event&)
{
    SDL_SetMouseFocus(nullptr);
}

int Serenity_CreateWindow(_THIS, SDL_Window* window)
{
    dbgln("{}: Creating new window of size {}x{}", __FUNCTION__, window->w, window->h);

    auto w = new SerenityPlatformWindow(window);
    window->driverdata = w;
    w->window()->set_double_buffering_enabled(false);
    w->widget()->set_fill_with_background_color(false);
    w->window()->set_main_widget(w->widget());
    w->window()->on_close_request = [window] {
        SDL_SendWindowEvent(window, SDL_WINDOWEVENT_CLOSE, 0, 0);
        return GUI::Window::CloseRequestDecision::StayOpen;
    };
    w->window()->on_active_window_change = [window](bool is_active_window) {
        SDL_SetKeyboardFocus(is_active_window ? window : nullptr);
    };
    w->window()->on_input_preemption_change = [window](bool is_preempted) {
        SDL_SetKeyboardFocus(is_preempted ? nullptr : window);
    };
    SERENITY_PumpEvents(_this);

    return 0;
}

void Serenity_ShowWindow(_THIS, SDL_Window* window)
{
    SerenityPlatformWindow::from_sdl_window(window)->window()->show();
}

void Serenity_HideWindow(_THIS, SDL_Window* window)
{
    SerenityPlatformWindow::from_sdl_window(window)->window()->hide();
}

void Serenity_SetWindowTitle(_THIS, SDL_Window* window)
{
    SerenityPlatformWindow::from_sdl_window(window)->window()->set_title(window->title);
}

void Serenity_SetWindowSize(_THIS, SDL_Window* window)
{
    SerenityPlatformWindow::from_sdl_window(window)->window()->resize(window->w, window->h);
}

void Serenity_SetWindowFullscreen(_THIS, SDL_Window* window,
    SDL_VideoDisplay* display,
    SDL_bool fullscreen)
{
    dbgln("Attempting to set SDL Window fullscreen to {}", (bool)fullscreen);
    SerenityPlatformWindow::from_sdl_window(window)->window()->set_fullscreen(fullscreen);
}

static Gfx::Color get_color_from_sdl_pixel(SDL_PixelFormat const& format, u32 pixel)
{
    u8 r = ((pixel & format.Rmask) >> format.Rshift) << format.Rloss;
    u8 g = ((pixel & format.Gmask) >> format.Gshift) << format.Gloss;
    u8 b = ((pixel & format.Bmask) >> format.Bshift) << format.Bloss;
    u8 a = ((pixel & format.Amask) >> format.Ashift) << format.Aloss;
    return {r, g, b, a};
}

static RefPtr<Gfx::Bitmap> create_bitmap_from_surface(SDL_Surface& icon)
{
    auto bitmap_or_error = Gfx::Bitmap::create(Gfx::BitmapFormat::BGRA8888, {icon.w, icon.h});
    if (bitmap_or_error.is_error())
        return {};
    auto bitmap = bitmap_or_error.release_value();

    SDL_LockSurface(&icon);

    auto const& pixel_format = *icon.format;
    auto pixels = static_cast<u32*>(icon.pixels);
    size_t offset = 0;
    switch (pixel_format.format) {
    case SDL_PIXELFORMAT_ARGB8888:
        for (size_t y = 0; y < icon.h; ++y) {
            for (size_t x = 0; x < icon.w; ++x) {
                bitmap->set_pixel({x, y}, get_color_from_sdl_pixel(pixel_format, pixels[offset]));
                ++offset;
            }
        }
        break;
    default:
        warnln("Unable to convert SDL_Surface with format {} to bitmap", SDL_GetPixelFormatName(pixel_format.format));
    }

    SDL_UnlockSurface(&icon);

    return bitmap;
}

void Serenity_SetWindowIcon(_THIS, SDL_Window* window, SDL_Surface* icon)
{
    auto serenity_window = SerenityPlatformWindow::from_sdl_window(window)->window();
    if (!icon) {
        serenity_window->set_icon(nullptr);
        return;
    }

    auto icon_bitmap = create_bitmap_from_surface(*icon);
    if (icon_bitmap)
        serenity_window->set_icon(icon_bitmap);
}

void Serenity_DestroyWindow(_THIS, SDL_Window* window)
{
    auto platform_window = SerenityPlatformWindow::from_sdl_window(window);
    delete platform_window;
    window->driverdata = nullptr;
}

SDL_bool Serenity_GetWindowWMInfo(_THIS, SDL_Window *window, SDL_SysWMinfo *info)
{
    info->subsystem = SDL_SYSWM_SERENITY;
    return SDL_TRUE;
}

int Serenity_CreateWindowFramebuffer(_THIS, SDL_Window* window, Uint32* format,
    void** pixels, int* pitch)
{
    dbgln("{}: Creating a new framebuffer of size {}x{}", __FUNCTION__, window->w, window->h);
    auto win = SerenityPlatformWindow::from_sdl_window(window);
    *format = SDL_PIXELFORMAT_RGB888;
    auto bitmap_or_error = Gfx::Bitmap::create(Gfx::BitmapFormat::BGRx8888, {window->w, window->h});
    if (bitmap_or_error.is_error())
        return SDL_OutOfMemory();
    win->widget()->m_buffer = bitmap_or_error.release_value();
    *pitch = win->widget()->m_buffer->pitch();
    *pixels = win->widget()->m_buffer->scanline(0);
    return 0;
}

int Serenity_UpdateWindowFramebuffer(_THIS, SDL_Window* window,
    const SDL_Rect* rects, int numrects)
{
    auto win = SerenityPlatformWindow::from_sdl_window(window);
    for (int i = 0; i < numrects; i++) {
        win->widget()->update(Gfx::IntRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h));
    }
    SERENITY_PumpEvents(_this);

    return 0;
}

void Serenity_DestroyWindowFramebuffer(_THIS, SDL_Window* window)
{
    auto widget = SerenityPlatformWindow::from_sdl_window(window)->widget();
    dbgln("{}: Destroy framebuffer {}", __FUNCTION__, widget->m_buffer->size());
    widget->m_buffer = nullptr;
}

SDL_GLContext Serenity_GL_CreateContext(_THIS, SDL_Window* window)
{
    auto platform_window = SerenityPlatformWindow::from_sdl_window(window);

    Uint32 format;
    void* pixels;
    int pitch;
    Serenity_CreateWindowFramebuffer(_this, window, &format, &pixels, &pitch);

    auto gl_context_or_error = GL::create_context(*platform_window->widget()->m_buffer);
    if (gl_context_or_error.is_error()) {
        SDL_SetError("Failed to initialize the GL context");
        return nullptr;
    }
    auto gl_context = gl_context_or_error.release_value();
    auto serenity_gl_context = new SerenityGLContext(move(gl_context));
    platform_window->set_serenity_gl_context(serenity_gl_context);
    GL::make_context_current(&serenity_gl_context->gl_context());
    return serenity_gl_context;
}

void Serenity_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    auto platform_context = static_cast<SerenityGLContext*>(context);
    delete platform_context;
}

int Serenity_GL_LoadLibrary(_THIS, const char* path)
{
    if (_this->gl_config.driver_loaded) {
        SDL_SetError("OpenGL library is already loaded");
        return -1;
    }

    _this->gl_config.dll_handle = dlopen("libgl.so.serenity", RTLD_LAZY | RTLD_LOCAL);
    if (!_this->gl_config.dll_handle) {
        dbgln("Could not load OpenGL library: {}", dlerror());
        _this->gl_config.driver_loaded = SDL_FALSE;
        return -1;
    }

    _this->gl_config.driver_loaded = SDL_TRUE;
    return 0;
}

void* Serenity_GL_GetProcAddress(_THIS, const char* proc)
{
    if (!_this->gl_config.dll_handle) {
        SDL_SetError("OpenGL library not available");
        return nullptr;
    }
    auto* res = dlsym(_this->gl_config.dll_handle, proc);
    dbgln("GetProcAddress: {} -> {}", proc, res);
    return res;
}

int Serenity_GL_MakeCurrent(_THIS, SDL_Window* window, SDL_GLContext context)
{
    if (!window || !context)
        return 0;

    auto platform_window = SerenityPlatformWindow::from_sdl_window(window);
    auto serenity_gl_context = static_cast<SerenityGLContext*>(context);
    platform_window->set_serenity_gl_context(serenity_gl_context);
    GL::make_context_current(&serenity_gl_context->gl_context());
    return 0;
}

int Serenity_GL_SwapWindow(_THIS, SDL_Window* window)
{
    auto platform_window = SerenityPlatformWindow::from_sdl_window(window);
    if (platform_window->serenity_gl_context())
        platform_window->serenity_gl_context()->gl_context().present();

    platform_window->widget()->repaint();
    return 0;
}

#endif /* SDL_VIDEO_DRIVER_SERENITY */

/* vi: set ts=4 sw=4 expandtab: */
