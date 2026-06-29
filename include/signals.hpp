#pragma once

#include "keyboard_service.hpp"

#include <gtk/gtk.h>

namespace signals {

void install(GtkApplication* app, KeyboardService& service);

}
