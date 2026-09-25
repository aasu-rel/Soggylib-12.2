# Soggylib (12.2 edition!!!!)

## NOTE: I won't updating this repository because I don't lib hypermodern, go do smth with yourself

A shared library for **Plants vs. Zombies 2** modding. (64-bit ONLY!!!!!)
Yes, another one. No, this one's actually good.

This library is used in **Plants vs Zombies 2 Tiempos Dificiles**, A mod that I totally didn't get gunned to my head. (joke, just helping levet)

---

# Features

- **Full lawn (wide view)** toggle in Settings - see more lawn. (add widescreen fix yourself tho)
- Custom tab in settings (Go find uses yourself) and it's customizable!
- Hide worldmap path will not be included
```

---

# Build

Uses **CMake**. (Surprise hah.)

Just run `build.bat`. If it explodes, you're probably missing:

- Android NDK **r25+**
- CMake **3.18+**
- Ninja
- Patience

Output: `libSoggy.so`

---

# Usage

1. Yeet `libSoggy.so` into your APK's `lib/arm64-v8a/` folder (make sure to delete armeabi-v7a)
2. Paste this code in `smali_classes5\com\popcap\PvZ2\PvZ2GameActivity`
   ```
   .line 57
    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

    const-string v0, "Soggy"
   ```
4. Launch the game
5. Figure out the feature

---

# Target

PvZ2 **12.2** (ARM64).

Got a different version? Cool. Update `offsets.h` yourself. good luck

---

# To-do list

- None, check out [9.6.1 Soggylib repository](https://github.com/aasu-rel/Soggylib)/ instead

---

# Credits

- **[Soggylib](https://github.com/aasu-rel/Soggylib)** - Me obviously
- **And64InlineHook** - Rprop
- **[Blazey's Example Mod](https://github.com/BlazeyLol/PVZ2ExpansionMod)** - Original repo for 9.6.1 libbing
- **[Plants vs Zombie discord (not offical one)](https://discord.gg/pvz)** - Awesome community
- **[Original Full lawn tab](https://github.com/CongJian833/PvZ2-LawnZoomTab)** - Original repo for full lawn from 9.8.1 [video](https://www.bilibili.com/video/BV1iNbX6DEy7/)
- **[Renojackson's Awesome lib](https://github.com/RenoJson/ARM64_example_injection_for_PvZ2/)** - Bunches of cool stuff for ARM64 lib (9.6.1)

---

# License

MIT. Do whatever, just don't blame me.

PvZ2 belongs to PopCap / EA. Not affiliated, not endorsed, not sued (yet). No game assets included — go play the game.
