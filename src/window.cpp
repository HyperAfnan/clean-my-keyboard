#include "window.hpp"
#include "logger.hpp"
#include "resources.hpp"

Window::Window(GtkApplication* app) {
    window_ = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_), "Clean My Keyboard");
    gtk_window_set_default_size(GTK_WINDOW(window_), 320, 400);
    gtk_window_set_resizable(GTK_WINDOW(window_), FALSE);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window_), box);

    std::string icon_path = Resources::path("keyboard-enabled.svg");
    icon_ = gtk_image_new_from_file(icon_path.c_str());
    gtk_image_set_pixel_size(GTK_IMAGE(icon_), 96);
    gtk_widget_add_css_class(icon_, "icon-image");
    gtk_box_append(GTK_BOX(box), icon_);

    status_label_ = gtk_label_new("Keyboard Enabled");
    gtk_widget_add_css_class(status_label_, "status-label");
    gtk_box_append(GTK_BOX(box), status_label_);

    button_ = gtk_button_new_with_label("Disable Keyboard");
    gtk_widget_add_css_class(button_, "enabled-btn");
    gtk_box_append(GTK_BOX(box), button_);
    g_signal_connect(button_, "clicked", G_CALLBACK(on_button_clicked), this);

    error_label_ = gtk_label_new("");
    gtk_widget_add_css_class(error_label_, "error-label");
    gtk_widget_set_visible(error_label_, FALSE);
    gtk_label_set_wrap(GTK_LABEL(error_label_), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(error_label_), 30);
    gtk_box_append(GTK_BOX(box), error_label_);

    Logger::info("Window created");
}

void Window::set_toggle_callback(std::function<void()> cb) {
    toggle_cb_ = std::move(cb);
}

void Window::update_state(AppState state, const std::string& detail) {
    switch (state) {
        case AppState::Active:
            gtk_image_set_from_file(GTK_IMAGE(icon_), Resources::path("keyboard-enabled.svg").c_str());
            gtk_label_set_text(GTK_LABEL(status_label_), "Keyboard Enabled");
            gtk_button_set_label(GTK_BUTTON(button_), "Disable Keyboard");
            set_button_css_class("enabled-btn");
            gtk_widget_set_sensitive(button_, TRUE);
            break;

        case AppState::Freezing:
            gtk_label_set_text(GTK_LABEL(status_label_), "Grabbing keyboards...");
            gtk_widget_set_sensitive(button_, FALSE);
            break;

        case AppState::Frozen: {
            gtk_image_set_from_file(GTK_IMAGE(icon_), Resources::path("keyboard-disabled.svg").c_str());
            std::string label = "Keyboard Disabled";
            if (!detail.empty()) {
                label = detail;
            }
            gtk_label_set_text(GTK_LABEL(status_label_), label.c_str());
            gtk_button_set_label(GTK_BUTTON(button_), "Enable Keyboard");
            set_button_css_class("disabled-btn");
            gtk_widget_set_sensitive(button_, TRUE);
            break;
        }

        case AppState::Unfreezing:
            gtk_label_set_text(GTK_LABEL(status_label_), "Releasing keyboards...");
            gtk_widget_set_sensitive(button_, FALSE);
            break;
    }
}

void Window::show_error(const std::string& message) {
    gtk_label_set_text(GTK_LABEL(error_label_), message.c_str());
    gtk_widget_set_visible(error_label_, TRUE);
}

void Window::clear_error() {
    gtk_label_set_text(GTK_LABEL(error_label_), "");
    gtk_widget_set_visible(error_label_, FALSE);
}

GtkWidget* Window::gtk_window() const {
    return window_;
}

void Window::on_button_clicked(GtkButton* , gpointer data) {
    auto* self = static_cast<Window*>(data);
    if (self->toggle_cb_) {
        self->clear_error();
        self->toggle_cb_();
    }
}

void Window::set_button_css_class(const char* css_class) {
    gtk_widget_remove_css_class(button_, "enabled-btn");
    gtk_widget_remove_css_class(button_, "disabled-btn");
    gtk_widget_add_css_class(button_, css_class);
}
