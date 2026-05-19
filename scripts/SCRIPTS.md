<h1 style="display: flex; align-items: center; margin: 20px 0;">
  <img src="pic/bash_icon.png" width="140" style="margin-right: 20px; flex-shrink: 0;">
  
  <span style="font-size: 42px; line-height: 1.2; font-weight: bold; color: inherit;">
    SCRIPTS UTILIZATION<br>IN lib-dnml
  </span>
</h1>

### Scripts are extremely useful components of lib-dnml, enhancing the development workflow with both speed, elegance, and certainty. However, there a quite a few ground rules when it comes to the leverage of scripts in [/scripts](/scripts/)

- For the bets development experience, please run the Bash script [dev_setup.sh](/scripts/dev_setup.sh) to download both necessary dependencies (CMake, Ninja, Rust + Cargo) and also Quality of Life ones (RISC-V64 Toolchains, Python, Lua, etc) for the best and msot integrated development experience if you're on a typical, modern desktop environment

- However, for people who are on a more restricted development environment, or just wants a minimalsitic development stack, please execute the Bash script [need_dependency.sh](/scripts/need_dependecy.sh) to only install CMake, Ninja, and Rust's Cargo.

<img src="pic/setup_scripts.png" alt="Picture of Developer setup scripts">

- Furthermore, it is actually fully possible to develop and maintain lib-dnml with only a C compiler and STD Library. Despite certain tests and compilation components utilize CMake and Rust + Cargo for a heightened development experience, it is entirely possible to compile using just compiler commands or platform-native Build files (eg: Makefiles on Linux) and run C-native alternatives. Despite such barebone experience, there are, of course, limitations regarding cross-platform compability, maintenance and debugging issues, and execution speed.

- Quality of Life scripts must be seperated into different directories with different suffixes representing the sole language that the scripts inside will be written in (eg: bigInt.py ---> written in Python, side_script.lua ----> written in Lua) for compatibility with the root directory program [```script_launcher.lua```](/scripts/script_launcher.lua). It is also highly advised that scripts are organized based on functionality and operation type

<img src="pic/dir_suffix.png" alt="Example of directory suffix here">

- The main scripting language for lib-dnml is primarily Python (for its robust integrated library, high scripting velocity, and integrated bigInt and bigFloat arithmetic) and Lua (light, robust, highly compatible with C). However, other scripting languages might be permitted as well depending on the user's second-hand expertise (Ruby, JavaScript with Node.JS, Perl, etc).

- Furthermore, the scripting language MUST be interpreted. This is to prevent the pollution of multiple [```script_launcher.lua```](/scripts/script_launcher.lua) simple and fast without handling compilation --> execution like in a traditional compiled-langauge

- With the introduction of any new scripting language into the stack, it is recommended for addition of the language's source file suffix and the engine in which it would be intrepreted/JIT-compiled in inside the file ```.languages.txt``` in the root of [```/script```](/scripts/) for the TUI execution of scripts via the central executor [```script_launcher.lua```](/scripts/script_launcher.lua). However, it is not fully mandatory for developers to adhere to the usage of [```script_launcher.lua```](/scripts/script_launcher.lua) for executing scripts, as straight execution of such scripts in their respective directory is completely acceptable.

<img src="pic/lang_txt.png" style="width: 50%">