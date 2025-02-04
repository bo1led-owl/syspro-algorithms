Setup the build system:
```
meson setup <BUILDDIR> --buildmode=release
```

Build:
```
meson compile -C <BUILDDIR>
```

This will produce two executables: `<BUILDDIR>/multiplication` and `<BUILDDIR>/division`
