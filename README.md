open-game-libraries
===================

The Open Game Libraries are brought to you by Lusito Software.
As of today, that includes Santo Pfingsten(TTK-Bandit) and Ensiform.

So first of all:
--------------------------------------
 There is still a lot to do before the refactoring is complete, a lot is untested and might not work.
 Linux builds have been left out completely for now, but will be re-added once we're done.
 The binaries needed will not be uploaded onto the svn, get them on the google project page.
--------------------------------------

Please visit http://code.google.com/p/open-game-libraries/ for a detailed Description of the Open Game Libraries.
All of our code is released on the z-lib license. See License.txt for details.

List of directories and what they contain:
- Binaries
  Destination folder for executables and resources.
  
- Build:
  Workspaces, makefiles, etc..

- Examples:
  This directory contains small and big Showcase demos on how to use the libraries we wrote.
  These demos keep the dependencies to a minimum to ensure easy unterstanding.
  Except for the Asteroids Clone, which is a little bigger to have a working game example.

- Libraries
  The code to the Open Game Libraries.
  The Source folder is only needed to compile the libraries.
  The Include folder contains all header and inline files needed.

- Thirdparty:
  This one contains libraries, which some of the Open Game Libraries use.
  Which of our libraries use what Thirdparty library you can see on the homepage.
  Remember, all the code in that directory is not written by us and might be under a different license.
  We simply have them here so there won't be any incompatiblity issues
  when trying to compile our code with newer or older versions.
  Of course, you are free to try newer versions and will likely succeed.

- Tools
  This contains helper tool sources, like a secure hash generator.
  Plans are to add a few more tools like a model converter, font utilities, etc.
