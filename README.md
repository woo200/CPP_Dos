# DOSTool

## Description
This is a very simple tool to flood UDP packets to a target. It is effective enough to completely wipe out my friends gigabit internet connection. (With his consent, of course.)

## Disclaimer
This software, or any derivative works thereof, shall not be used to target any individual or entity without their express consent.
See the [License File](LICENSE.md) for more information.

## Compiling

### Linux and macOS:
```bash
git clone https://github.com/woo200/CPP_Dos.git
cd CPP_Dos
g++ -o dostool dostool.cpp
```

### Windows:
Requires MinGW, which can be installed from [here](https://github.com/skeeto/w64devkit/releases). You must add the MinGW bin folder to your PATH environment variable, or you can rawdog it and just run the g++ executable with the full path.

```bash
git clone https://github.com/woo200/CPP_Dos.git
cd CPP_Dos
g++ -o dostool dostool.cpp -lws2_32 # Requires MinGW
```

## Usage
```bash
./dostool <target ip>
```
Pretty simple, eh?