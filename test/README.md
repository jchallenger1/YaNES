# Running The Tests
Create and use the testing envrionment of YaNES
## Installing
Run qmake and make to create the testing program
```
qmake
make
./testing
```
## Usage
```
USAGE: testing -- [OPTION]...
Allowed options:
  --help                Produce help message
  --env                 Use the predefault programming environment for YaNES's classes
  --opcode              Performs opcode tests
  --nestest             Peform nesTest cpu tests
  --ppureg              Performs register tests for the ppu
  -a [ --all ]          Performs all tests
```
## Example
```
$ ./testing -- --opcode
Running 9 test cases...
 --- Running Opcode Test Cases ---
*** No errors detected
```
