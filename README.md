# vineyard

A work-in-progress hobby operating system for x86_64.

## Status

Currently implemented features include:
- ACPI, implemented via ACPICA: for shutdown and PCI IRQ resolving
- NVMe: read-only support
- ubsan: for catching undefined behavior

## Roadmap
Priorities in the future:
- Support for filesystems: FAT (for the EFI partition) and one for the root partition (maybe btrfs?)
- functional user-space
- SMP
- Intel HD Graphics drivers

## Prerequisites

Required:
- a Unix-ish OS with bash
- Python 3, preferably 3.7
- Python's PyYAML
- [xbstrap](https://github.com/managarm/xbstrap)
- [Kconfiglib](https://github.com/ulfalizer/Kconfiglib), a Python reimplementation of Linux' Kconfig-related utilities
- mtools
- mkfs.btrfs
- QEMU
- yasm
- clang
- lld

Optional:
- x86_64-elf-binutils
- x86_64-elf-gcc
- VirtualBox, with the Extension Pack installed (required for NVMe support!)
- VMWare

## Building
Clone the repo `vineyard-os/bootstrap` into project folder`vineyard`.

```
git clone https://github.com/vineyard-os/bootstrap vineyard
cd vineyard
```

In the root folder `vineyard`, set up xbstrap and run it:

```
xbstrap init .
xbstrap install --all
```

To test vineyard in QEMU, run this from the root project folder:

```
make -C vineyard test
```