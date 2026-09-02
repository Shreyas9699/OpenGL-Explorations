#include "Application.h"

// Force the discrete NVIDIA GPU on hybrid-graphics laptops
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main(void)
{
    Application app("OpenGL Project", false);
    app.Run();
    return 0;
}