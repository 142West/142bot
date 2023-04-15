# 142bot

Discord bot for the residents and friends of 142 West St.

# Installation

Compile as you would any "normal" CMake project:
```
cmake -B build -DCMAKE_BUILD_TYPE='Release' -Wno-dev && cmake --build build
```

Then, do whatever you want with the `142bot` and `module_*.so` files, but they should be grouped together logically, as the `module_*.so` files contain the runtime code for dynamic modules.

Make sure to run `cmake --install build` to install the header files and necessary shared library files (like dpp, cpr, etc).

This project depends on `libpqxx`, which should be installed outside of the project. All other dependencies (dpp, etc) will be compiled and installed through CMake.

## Configuration
The main configuration can be found in the CWD's `config.json` file. There's a `config.example.json` in the repo to help you make this.

Use of the `reactions` module requires an additional file, `resources/reactions.json` that contains a mapping between keywords and what reactions to add. There's another example in the repository.

# Development

There's a separation in this project between code that responds to Discord events and code that provides functionality to those pieces of code. Any code that interacts directly with Discord should probably live in a module, whereas anything providing utility functions should live outside of modules.

Speaking of modules...

## Modules

Modules are dynamically loaded at runtime to provide extra functionality to the bot. A bot that doesn't respond to any input is pretty boring!