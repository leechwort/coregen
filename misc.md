Draw spectrum for wav file:
```
$ sox beep.wav -n stat -freq 2>&1 | sed -n -e :a -e '1,15!{P;N;D;};N;ba' | gnuplot -p -e 'set logscale x; plot "-" with linesp'
```
Show spectra in text form:
```
$ sox beep.wav -n stat -freq
```
