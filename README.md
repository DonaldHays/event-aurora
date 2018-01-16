# Event Aurora

An in-development game for Game Boy.

## Building

I built this project on macOS. The instructions should be pretty easy to follow from most UNIX-y operating systems. Windows users may have some troubles with the build scripts.

### Dependencies

The build pipeline has a dependency on [node.js](http://nodejs.org). You'll need to install it.

`cd` into `ihx2gb/` and run `npm install`.

`cd` into `img2gb/` and run `npm install`.

`cd` into `mapc/` and run `npm install`.

`cd` into `metac/` and run `npm install`.

`cd` into `musicc/` and run `npm install`.

`cd` into `stringc/` and run `npm install`.

Download [SDCC](http://sdcc.sourceforge.net) and place the `sdcc/` folder in the root directory alongside `ihx2gb/` and `img2gb/`, such that from root directory you could navigate to `sdcc/bin/sdcc`.

### Building the Game
`cd` into the root directory and run `make`.
