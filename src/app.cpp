#include "app.hpp"
#include "polkit_backend.hpp"
#include "logger.hpp"
#include "resources.hpp"
#include "signals.hpp"

#include <memory>
#include <string>

App::App(GtkApplication* gtk_app)
    : gtk_app_(gtk_app),
      service_(std::make_unique<PolkitBackend>()) {}

void App::on_activate() {
    load_css();

    GdkDisplay* display = gdk_display_get_default();
    GtkIconTheme* icon_theme = gtk_icon_theme_get_for_display(display);
    std::string icons_dir = Resources::path("icons");
    gtk_icon_theme_add_search_path(icon_theme, icons_dir.c_str());

    window_ = new Window(gtk_app_);

    window_->set_toggle_callback([this]() { on_toggle(); });

    g_signal_connect(window_->gtk_window(), "close-request",
                     G_CALLBACK(on_close_request), this);

    signals::install(gtk_app_, service_);

    window_->update_state(service_.state());
    gtk_window_present(GTK_WINDOW(window_->gtk_window()));

    Logger::info("Application activated");
}

void App::on_toggle() {
    if (service_.state() == AppState::Active) {
        FreezeResult result = service_.freeze();

        switch (result) {
            case FreezeResult::Success: {

                window_->update_state(service_.state());
                break;
            }
            case FreezeResult::PermissionDenied:
                window_->show_error(
                    "Polkit authentication failed or helper could not be started.");
                window_->update_state(service_.state());
                break;
            case FreezeResult::NoKeyboardsFound:
                window_->show_error("No keyboard devices found.");
                window_->update_state(service_.state());
                break;
            case FreezeResult::GrabFailed:
                window_->show_error("Failed to grab keyboard devices.");
                window_->update_state(service_.state());
                break;
        }
    } else if (service_.state() == AppState::Frozen) {
        service_.unfreeze();
        window_->update_state(service_.state());
    }
}

gboolean App::on_close_request(GtkWindow* , gpointer data) {
    auto* self = static_cast<App*>(data);
    Logger::info("Window close requested — unfreezing keyboards");
    self->service_.unfreeze();

    return FALSE;
}

void App::load_css() {
    GtkCssProvider* provider = gtk_css_provider_new();
    std::string css_path = Resources::path("style.css");
    gtk_css_provider_load_from_path(provider, css_path.c_str());
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    Logger::info("CSS loaded from %s", css_path.c_str());
}
