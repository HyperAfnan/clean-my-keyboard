#pragma once

#include "types.hpp"

#include <gtk/gtk.h>
#include <functional>
#include <string>

class Window {
public:
    explicit Window(GtkApplication* app);

    void set_toggle_callback(std::function<void()> cb);
    void update_state(AppState state, const std::string& detail = "");
    void show_error(const std::string& message);
    void clear_error();

    GtkWidget* gtk_window() const;

private:
    GtkWidget* window_ = nullptr;
    GtkWidget* icon_ = nullptr;
    GtkWidget* status_label_ = nullptr;
    GtkWidget* button_ = nullptr;
    GtkWidget* error_label_ = nullptr;

    std::function<void()> toggle_cb_;

    static void on_button_clicked(GtkButton* btn, gpointer data);

    void set_button_css_class(const char* css_class);
};
