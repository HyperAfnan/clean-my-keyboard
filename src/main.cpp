#include "app.hpp"
#include "logger.hpp"

#include <gtk/gtk.h>

static App* g_app = nullptr;

static void on_activate(GtkApplication* gtk_app, gpointer ) {
    if (!g_app) {
        g_app = new App(gtk_app);
    }
    g_app->on_activate();
}

int main(int argc, char* argv[]) {
    Logger::info("Starting Clean My Keyboard");

    GtkApplication* gtk_app = gtk_application_new(
        "io.github.hyperafnan.CleanMyKeyboard",
        G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(gtk_app, "activate", G_CALLBACK(on_activate), nullptr);

    int status = g_application_run(G_APPLICATION(gtk_app), argc, argv);

    delete g_app;
    g_app = nullptr;
    g_object_unref(gtk_app);

    Logger::info("Clean My Keyboard exited with status %d", status);
    return status;
}
