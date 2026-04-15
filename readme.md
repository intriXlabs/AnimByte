

---

# AnimByte

**1-byte character rendering engine for terminal animation.**
Minimal API. Flat buffer. High-speed output.

---

## Overview

AnimByte is a C++ engine for terminal-based rendering using **pure ASCII characters**.

* Each cell = **1 byte**
* Flat buffer (`width × height`)
* **Delta tracking** (only changed cells tracked)
* Single `write()` syscall per frame
* No external dependencies

---

## Core Design

* **Flat Buffer**
  Continuous `char*` array → cache-friendly, predictable

* **Delta Tracking**
  `Set_Char()` pushes only changed cells into a list

* **Frame Generation**
  Buffer → newline-delimited string

* **Rendering**
  `\033[H` + single `write()` → no flicker, no clear

---

## API

### Initialise

```cpp
int Initialise(int width, int height);
```

Allocates buffer of size `width × height`

---

### Set_Char

```cpp
int Set_Char(int row, int col, char ch);
```

* 1-based indexing
* Skips if unchanged
* Tracks modified cells

Returns:

* `0` → written
* `-1` → out-of-bounds or no-op

---

### Render_Frame

```cpp
int Render_Frame();
```

* Builds full frame string
* Writes to terminal using single syscall

---

### Frame_Clean

```cpp
int Frame_Clean();
```

* Resets only modified cells to `' '`
* Uses dirty list

---

## Usage

```cpp
#include "AnimByte.cpp"

int main() {
    AnimByte ab;
    ab.Initialise(80, 24);

    while(true){
        ab.Frame_Clean();

        ab.Set_Char(12, 40, '@');

        ab.Render_Frame();
        usleep(16000); // ~60 FPS
    }
}
```

---

## Properties

* **Memory:** `width × height` bytes
* **Rendering:** full-frame write
* **Update cost:** `O(d)` (d = changed cells)
* **Dependencies:** none (STL + unistd)

---

## Notes

* Performance depends on terminal + system
* Can exceed **100+ FPS** under optimal conditions
* No color / ANSI styling (pure character rendering)

---

## Philosophy

> No abstraction. No widgets. Just characters.

---

## License

Open source. Use freely.

---

direct cpp recomended - you can use compile it for .h also: your call
