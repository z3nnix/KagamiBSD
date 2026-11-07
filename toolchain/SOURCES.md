# Toolchain sources
The prebuilt toolchain was produced from NetBSD/src at commit:

```
36eddebcc36cb0775aa2962005b9c05419a6ff01
```

- Source: https://github.com/NetBSD/src/tree/36eddebcc36cb0775aa2962005b9c05419a6ff01
- Build entry point: kernel/netbsd/build.sh
- Build command: ./build.sh -U -u -c gcc -O <obj> -T <tools> -m amd64 -j<JOBS> tools

GCC and Binutils are GPLv3 components. If this binary artifact is redistributed, provide the corresponding source or a written offer as required by the GPL. The NetBSD kernel source remains under its original BSD licenses.
