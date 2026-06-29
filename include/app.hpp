#pragma once

#include "keyboard_service.hpp"
#include "window.hpp"

#include <gtk/gtk.h>

class App {
public:
    explicit App(GtkApplication* gtk_app);

    void on_activate();

private:
    GtkApplication* gtk_app_;
    KeyboardService service_;
    Window* window_ = nullptr;

    void on_toggle();
    static gboolean on_close_request(GtkWindow* window, gpointer data);
    void load_css();
};
