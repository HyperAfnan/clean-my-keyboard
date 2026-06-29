# Clean My Keyboard

A lightweight GTK4 utility that temporarily disables all keyboard input so you can safely wipe down your keys without triggering random shortcuts.

## Preview

https://github.com/user-attachments/assets/160c4041-40ea-4a49-abcb-46ba63693102

## Dependencies

* `cmake`
* `gtk4`
* `libadwaita`
* `polkit`

---

## Installation

### Method 1: CMake Installation 

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

### Method 2: Native Arch Linux Package (Arch/Manjaro)

```bash
cd packaging
makepkg -si
```


## License

MIT — see [LICENSE](LICENSE).
