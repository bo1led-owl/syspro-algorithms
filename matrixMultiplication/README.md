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
python print.py
```

Results:
```
| N    | naive                      | bad (8 recursive calls)    | strassen                   |
|---------------------------------------------------------------------------------------------|
| 64   | 0.000189 0.000048 0.000184 | 0.000125 0.000011 0.000125 | 0.000125 0.000013 0.000124 |
| 128  | 0.001788 0.000314 0.001765 | 0.000938 0.000041 0.000937 | 0.000841 0.000049 0.000840 |
| 256  | 0.014632 0.000038 0.014632 | 0.009912 0.000029 0.009912 | 0.006398 0.000953 0.006343 |
| 512  | 0.140601 0.002832 0.140574 | 0.111584 0.002195 0.111562 | 0.043776 0.000162 0.043776 |
| 1024 | 4.694489 0.039741 4.694321 | 0.933256 0.000438 0.933256 | 0.307108 0.000612 0.307107 | 
```

Each cell is formatted as such:
```
<sample average> <standard deviation> <geometric mean>
```
