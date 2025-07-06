# jd_app
This is an in-progress application framework and custom standard library written (mostly) from scratch in C17. 

jd_app is in the very early stages, but is being designed from the ground up with cross-platform compatibility in mind. Right now, it runs on Windows using native Win32 APIs. Versions for Linux and other platforms can be expected as the project develops.

##### Features include:
- Simplified but powerful windowing and graphics capabilities
- Dynamic arrays and strings based around a custom arena allocator.
- Simple data structures utilities to create and manage linked lists, trees, and more
- Simple to use 2D renderer
- Immediate mode UI engine
- Optional dynamic code reloading for live editing of program logic
- ...and much more to come!

*Note that this is a work in progress, and many of these features are immature and liable to change.*

##### To install:
1. Clone both this repo and [jd_lib](https://github.com/jd1XV/jd_lib) to the same folder.
2. Install [freetype](https://github.com/freetype/freetype).
3. Indicate the path of freetype in build.bat and build_lib.bat.

   <sub>Note: if you put **jd_lib** in a different folder than **jd_app**, indicate its path in build.bat and build_lib.bat as well.</sub>

##### To use:
- Your static code starts in **main.c**.
- Your reloadable code goes in **reloadable.c**. Note: more dynamic code can be added by specifying more source files in **build_lib.bat**.
- Compile your program by running **build.bat**. Compile or recompile your dynamic code by running **build_lib.bat**.

  <sub>Note: You will need to run vcvarsall.bat or use the MSVC developer console to access the MSVC compiler. See [here](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170) for more info.</sub>

- To disable dynamic code reloading, create your window with flag **JD_AM_STATIC**, and specify the function_ptr member of the **jd_PlatformWindowConfig** struct instead of the **function_name** member.

##### Inspired by:
- Ryan Fleury (https://www.rfleury.com/)
- Sean Barrett (https://github.com/nothings)
- Allen Webster (https://mr4th.com/)
- Casey Muratori (https://mollyrocket.com/)
- Omar Cornut (https://github.com/ocornut)

*All rights reserved by the author. Repository is public for portfolio purposes. No warranty or license is implied by the public release of this code.*
