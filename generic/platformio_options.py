# Split C/C++ build flags
# https://docs.platformio.org/en/latest/scripting/examples/split_build_flags.html

Import("env")

# General options that are passed to the C++ compiler
env.Append(CXXFLAGS=["-fno-use-cxa-atexit"])
