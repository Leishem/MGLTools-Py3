/*
 * copyright_notice
 */

struct opengltk_export {
  PyObject* (*processerror)( GLenum errcode);
  PyThreadState* pythread;
  int threadunlocked;
  int checkerror;
};
