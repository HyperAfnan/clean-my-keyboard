# Clean My Keyboard

A lightweight GTK4 utility that temporarily disables all keyboard input so you can safely wipe down your keys without triggering random shortcuts.

## Dependencies

* `gtk4`
* `libudev` (provided by `systemd-libs` on Arch)
* `cmake` (build only)
* `pkg-config` (build only)

---

## Build

To build the application locally:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

You can run the binary directly from the build directory:
```bash
./build/clean-my-keyboard
```

---

## Global Installation

### Method 1: CMake Installation (Universal Linux)

To install the application globally along with its desktop entry launcher, icons, and assets:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

This installs:
* `/usr/bin/clean-my-keyboard`
* `/usr/share/applications/clean-my-keyboard.desktop`
* `/usr/share/clean-my-keyboard/`
* `/usr/share/icons/hicolor/scalable/apps/clean-my-keyboard.svg`

### Method 2: Native Arch Linux Package (Arch/Manjaro)

A `PKGBUILD` configuration is provided in the `packaging/` directory. To install globally as a native system package:

```bash
cd packaging
makepkg -si
```

---

## Permission Configuration

Accessing `/dev/input/event*` nodes requires special permissions. Running the application under a standard user account will display a "Permission Denied" warning. You have two options to enable non-root execution:

### Option 1: Add user to the `input` group (Recommended)

```bash
sudo usermod -aG input $USER
```
*Note: You must log out and log back in (or restart your desktop session) for these changes to take effect.*

### Option 2: Add a custom udev rule

Create a new file at `/etc/udev/rules.d/99-clean-my-keyboard.rules`:

```udev
SUBSYSTEM=="input", GROUP="input", MODE="0660"
```

Reload the rules:
```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

## License

MIT — see [LICENSE](LICENSE).
