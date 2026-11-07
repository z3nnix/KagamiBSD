# NetBSD/amd64 toolchain
Prebuilt cross toolchain for the KagamiBSD kernel build.

- Archive: netbsd-amd64-tools-14.4.0-36eddeb.tar.zst
- NetBSD source base: 36eddebcc36cb0775aa2962005b9c05419a6ff01
- GCC: 14.4.0
- Binutils: 2.46.0.20260210
- Host: Linux x86-64, glibc

The archive contains the tools/ directory. make bootstrap verifies SHA256SUMS, extracts it to the repository root, and runs scripts/relocate-toolchain.sh so absolute build paths are rewritten to the current checkout location.

The toolchain is a build-time dependency. It is not linked into the kernel.
