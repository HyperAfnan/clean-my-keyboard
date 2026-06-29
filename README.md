# Clean My Keyboard

A lightweight GTK4 utility that temporarily disables all keyboard input so you can safely wipe down your keys without triggering random shortcuts.

## Preview

https://github.com/user-attachments/assets/160c4041-40ea-4a49-abcb-46ba63693102

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

Accessing Linux input device files (`/dev/input/event*`) requires elevated privileges. Running the application under a standard user account will show a "Permission Denied" warning. 

You have three options to run the application:

### Option 1: Run with `sudo` (Quickest)
You can launch the application directly as root to bypass permission checks:
```bash
sudo ./build/clean-my-keyboard
```
*(If globally installed)*:
```bash
sudo clean-my-keyboard
```

### Option 2: Add your user to the `input` group (Recommended)
This grants your user account permanent read/write access to input devices:
```bash
sudo usermod -aG input $USER
```
*Important: You must log out of your desktop session and log back in (or reboot) for this group change to take effect.*

### Option 3: Add a custom udev rule
You can define a custom udev rule to set appropriate group permissions on startup.

Create the file `/etc/udev/rules.d/99-clean-my-keyboard.rules`:
```udev
SUBSYSTEM=="input", GROUP="input", MODE="0660"
```

Reload and trigger the rules:
```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```


## License

MIT — see [LICENSE](LICENSE).
