#ifndef SIMPLEEXAMPLE_H
#define SIMPLEEXAMPLE_H

#define ROTATE 1
#define TRANSLATE 2

int BeginGraphics (int* argc, char** argv, const char* name);

void RegisterCallbacks();

void init();

void InitProjection();

void InitModelMatrix();

void InitLighting();

void InitState();

void InitVolumeRenderer();

void InitData();

void Display();

void Idle();

void MouseButton(int button, int mstate, int x, int y);

void MouseMove(int x, int y);

void Keyboard(unsigned char key, int x, int y);

void Rotate(int dx, int dy);

void Translate(int dx, int dy, int dz);



#endif
