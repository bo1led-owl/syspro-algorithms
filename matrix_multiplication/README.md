Setup the build system:
```
meson setup <BUILDDIR> --buildtype=release
```

Run tests:
```
meson test -C <BUILDDIR>
```

Run measurements:
```
meson compile -C <BUILDDIR> && ./<BUILDDIR>/main
```
