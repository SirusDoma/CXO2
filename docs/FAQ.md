# FAQ

## Where to get the client? is this going to work with 9you/1.8 client?
You have to find the client that matches with the `Client Version` stated at the top of the [README](../README.md) by yourself and I cannot help you with that.
And no, the client is not compatible to run with 1.8 client

## Where's the ring/modifier? Why I can't set my speed when joining a room? Why some equip item slots/categories missing? Where's `OJNList.dat`? Where's (insert feature here)?
One of the project goals is to remake all major O2Jam client versions that were ever published by e-Games and Nowcom.
Starting from v3.10, because it is the earliest client that is available around the internet.

To see other client version, check the other branches/tags.

### Special notes for Pre-NX client
O2Jam v3.10 does not have ring/modifier among other things. The next iteration will bring these features.

## Can I use custom music?
Absolutely, you can!

### Special notes for Pre-NX client
client and server will perform no validation to your music list.
~~This is because client v3.10 does not have OJNList.dat implemented.~~ (This is not true: the OJNList.dat exist in `Playing(1).opi` for v3.10)

It is easy to spoof, the music. If you are playing in public space, please be mindful when playing with others.

## The music title in music selection is incorrect or gibberish, it supposed to be displayed in (Chinese/Japanese/Korea/etc). Can you fix it?
Currently, the game implemented support for legacy Unicode encodings.
However, the encoding information is not included in OJN header, so the game has to guess it. Unfortunately, this mean the decoding process is not guaranteed to be reliable all the time.

You may not notice it, but this issue also appear in the original client. Each O2Jam distribution use different set of localization implementation, which mean it may use different fonts (and probably encoding handling too).
For example, using chinese title in O2Jam client v1.8 (9you) will likely work and reliable, but the game will likely fail to display the titile for any song with Korean title. The opposite also happening with O2Jam Korea client.

## How about mods? What are the file formats the client use?
The game is a drop-in replacement of original `OTwo.exe`, which mean it can read all files that original O2Jam client can read, including `.opi`/`.opa` and `.ojn`/`.ojm` (and in the future, `OJNList.dat`).

This mean modded `Interface(1).opi`, `Playing(1).opi` and/or `Avatar.opa` may work in this client, **as long as the files are compatible with the `Client Version` specified in the top of the [README](../README.md)**.

The client also extend the interface/avatar asset pipeline further by utilizing json format and extremely extendible for this front, you can use this to mod or even build the scene composition from the scratch.

Further modding (such as extending game via library (.dll)) is still under planning phase. Please note that any dll injection that you use with original `OTwo.exe` **will not** work.

Also, since the game use the same network format, you can play alongside with the original game client as long as the client and server support the specified `Client Version`.

## Can I host my own server?
Absolutely, use [Mozart.Encore](https://github.com/SirusDoma/Mozart.Encore/tree/mozart) to host your own server or for local network play.

## Is offline mode supported?
Yes, launching the game normally (via double-click) will launch the game with Offline mode. The client will generate `session.json` which store user profile information, and can be edited manually using a text editor.

## Is this Illegal?
I'm not distributing any e-Games/Nowcom/O2Jam IP. You must provide legally obtained game installer to play this game.
The game did not contain any copyrighted materials, the source code are written from scratch and none copied from the original game binaries.
