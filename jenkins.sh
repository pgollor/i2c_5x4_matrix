# get keypad library
wget https://github.com/Chris--A/Keypad/archive/3.1.1.tar.gz
tar xf 3.1.1.tar.gz

# build with arduino builder in ./build
mkdir build
arduino-builder -build-path=$PWD/build -hardware $ARDUINO_ROOT/hardware -tools $ARDUINO_ROOT/hardware/tools/avr --tools $ARDUINO_ROOT/tools-builder -libraries $ARDUINO_ROOT/libraries -libraries ./Keypad-3.1.1/ -fqbn=arduino:avr:uno i2c_5x4_matrix.ino
