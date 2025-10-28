# HypeOS

research kernel and operating system

## Build Instructions

only supports x86-64 and unix-like systems (yes WSL is included in that)

i'm on fedora `sudo dnf install -y gcc binutils make xorriso qemu-system-x86 grub2-tools`

if you're on ubuntu/mint/pop: something like this `sudo apt install build-essential xorriso qemu-system-x86 grub-pc-bin`

not sure about mac

but just run `make` and then `make run` to launch qemu

## License

GNU GPLv2

See `LICENSE` for the complete license agreement
