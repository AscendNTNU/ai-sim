Building with the simulator library
-----------------------------------
Include the following lines in *one* source file

    #define SIM_IMPLEMENTATION
    #include "sim.h"

You may #include "sim.h" anywhere else if you want the declared symbols, as long as SIM_IMPLEMENTATION is only defined in one of your source files.

See example1.cpp for an example of a full application that communicates with the real-time simulation, and also uses the library internally.

Running the real-time simulator
-------------------------------
Linux

    $ cd build
    $ g++ ../gui.cpp -o sim -lGL `sdl2-config --cflags --libs`

OSX

    $ cd build
    $ g++ ../gui.cpp -o sim -framework OpenGL `sdl2-config --cflags --libs`

Windows

    > cd build
    > run_gui.bat
    Or compile manually:
    > cl -I../lib/sdl ../gui.cpp -MT /link -subsystem:console SDL2.lib SDL2main.lib opengl32.lib -out:gui.exe

---------------------------------------------------------------------------------------------

Example on how to send text to gui.cpp for debug purposes:

        //Code for how to use custom debug text. Remember to include sim.h to use this.
        //All targets must have a text, and if you have less than 10 targets,
        //you have to delete some of the targets from the list below.
        //If you only want to debug a single groundrobot, all other texts may be left blank(as in "");
        sim_Command cmd;
        const char *target_text[Num_Targets];
        target_text[0] = "Text for zeroth ground robot";
        target_text[1] = "Text for first ground robot";
        target_text[2] = "Text for second ground robot";
        target_text[3] = "Text for third ground robot";
        target_text[4] = "Text for fourth ground robot";
        target_text[5] = "Text for fifth ground robot";
        target_text[6] = "Text for sixt ground robot";
        target_text[7] = "Text for seventh ground robot";
        target_text[8] = "Text for eight ground robot";
        target_text[9] = "Text for ninth ground robot";

        //Set the command text
        get_char_text(cmd.text,"text for drone", target_text);

        //Send the text
        sim_send_cmd(&cmd);
