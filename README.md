# EqEff

A small program to update `mp3index.txt` and patch `<zone>_sounds.eff` files so that mp3 files can be used in place of xmi files. To be safe you should backup your `mp3index.txt` and `_sounds.eff` files.

The mp3 files need to be named `<zone>.xmi(<subsong #).mp3` (`qeytoqrg.xmi(1).mp3` for qey2qrg.xmi subsong 1)

## Usage

To patch a single file:
`./eqeff /Applications/EverQuest.app/Contents/Resources/qeytoqrg_sounds.eff`

## Issues

Some zones like sro and permafrost have `_sounds.eff` files but don't have a corresponding `xmi` file.

## Building

`g++ -std=c++11 -Wall -Werror eqeff.cpp -o eqeff`

## License
See LICENSE file.
