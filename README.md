[![Project Status: Unsupported – The project has reached a stable, usable state but the author(s) have ceased all work on it. A new maintainer may be desired.](https://www.repostatus.org/badges/latest/unsupported.svg)](https://www.repostatus.org/#unsupported)

# Arduino Buttons Library
An interrupt-driven, fully-debounced class to manage input from physical buttons on the Arduino platform.

Although in principle this class is compatible with all members of the Arduino platform, note that it is limited to only having buttons connected to those pins that accept interrupts - on the ARM boards (Due, Zero etc) this is all pins, but on AVR boards (e.g. Uno, Leonardo, Mega 2560, etc) these pins are in limited supply.

Note that for pragmatic reasons, this is a fully static class. This means you do not need to create any instances of it and access it using static syntax, e.g.
```
byte pins[] = {1, 2};
Buttons.begin(pins, 2);
if (Buttons.clicked(1)) {
  Serial.println("Button 1 Clicked!);
}
if (Buttons.clicked(2)) {
  Serial.println("Button 2 Clicked!");
}
Buttons.clearChangeFlag();
Buttons.end();
```

The class is fully documented internally, but I may write a full usage guide here later on.

## Library Setup
Just put the buttons.hpp and buttons.cpp file into your sketch folder, then add `#include "buttons.hpp"` to your .ino source file and any other files that will reference the buttons class.

## Comments, Requests, Bugs & Contributions
All are welcome. Please file an "Issue" in the Bug Tracker.

## License
This library is licensed under the GNU LGPL v2.1, which can be found in the LICENSE file.
