### Picocom communication

```
picocom --echo --omap crlf /dev/ttyACM0
```

### Known commands
```
home, h:    Go to home position (0)

<position>: Go to <position>. Can be in the interval [0, 3300].
red, r:     Go to red (2500)
orange, o:  Go to orange (2310)
yellow, y:  Go to yellow (2250)
green, g:   Go to green (2000)
blue, b:    Go to blue (1750)
violet, v:  Go to violet (1600)

rate <r>:   Change stepper motor rate to <r>.
            Can be in the interval (0, 200], default 100.
            A rate of 100 equals 100/16 motor steps per second.

estep <s>:  Change quadrature encoder step factor to <s>.
            Can be in the interval (0, 50], default 8.
            Each quadrature encoder step translates to
            (<s> * stepper motor steps).
```
