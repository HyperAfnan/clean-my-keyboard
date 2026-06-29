#include "signals.hpp"
#include "logger.hpp"

#include <glib-unix.h>

namespace {

struct SignalContext {
    GtkApplication* app;
    KeyboardService* service;
};

static SignalContext g_signal_ctx{};

gboolean on_signal(gpointer ) {
    Logger::info("Signal received — unfreezing keyboards and quitting");
    g_signal_ctx.service->unfreeze();
    g_application_quit(G_APPLICATION(g_signal_ctx.app));
    return G_SOURCE_REMOVE;
}

}

namespace signals {

void install(GtkApplication* app, KeyboardService& service) {
    g_signal_ctx.app = app;
    g_signal_ctx.service = &service;

    g_unix_signal_add(SIGINT, on_signal, nullptr);
    g_unix_signal_add(SIGTERM, on_signal, nullptr);

    Logger::info("Signal handlers installed (SIGINT, SIGTERM)");
}

}
