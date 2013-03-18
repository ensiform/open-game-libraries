open-game-libraries
===================

The Open Game Libraries are brought to you by Lusito Software.  
As of today, that includes Santo Pfingsten(TTK-Bandit) and Ensiform.

News
-------------------
Project moved to GitHub but still on ice.  
Due to all team members being busy otherwise for some time now, I'm officially putting this project on ice.

Things to consider
------------------
* There is still a lot to do before the refactoring is complete, a lot is untested and might not work.
* Linux builds have been left out completely for now, but will be re-added once we're done.
* The binaries needed will not be uploaded onto the git.  Previously they were on google project page.

The Open Game Libraries, a set of Intuitive C++ libraries for creating games.

We're focusing on performance, simplicity and being as lightweight as possible.

The Libraries _(in alphabetical order)_
------------------
* Audio - OpenAL audio management ( loading sounds from ogg/wav, positioning them, setting attributes, etc. )
* Common - Generic classes and objects for game development ( containers, text processing, etc. )
* Console - Developer console window with support for Commands and variables
* Fable - Force, Axis, Buttons Light and Easy ( simply said gamepad input management )
* FileSystem - Compressed zip file loading with a few neat features for multiplayer games.
* Font - Bitmapped Font loading and drawing ( Unicode + Kerning support )
* Gloot - (OpenGL Object Oriented Toolkit), OpenGL window creation and keyboard/mouse input.
* Image - OpenGL image loading and saving
* Math - Vector, Matrix, Quaternions, Color, etc..
* Model - Loading 3D Models in a generic way, with import functions for various formats ( ASE, MD3, MD5, SKM, SMD )
* Shared - Shared parts to lighten the dependencies

For more details and Tutorials, check out the Wiki _(not yet available)_

( The names of these libraries are not final yet, if you have better suggestions, please tell us )

Dependencies
------------------
* libJPEG ( Image and Font )
* libPNG ( Image and Font )
* zLib ( libPNG and FileSystem )
* libOgg / libVorbis ( Audio )
* OpenAL Soft ( Audio )
* OpenGL ( Gloot )
* Direct-X ( Fable )
* Visual Leak Detector ( ALL, Optional )

Icon Credits
------------
* Joystick icon made by Alexandre Moore

License
-------
zlib/PNG License  
See License.txt
