#ifndef ksp_check_button_h
#define ksp_check_button_h

#include <FL/Fl_Light_Button.H>

/**
 * Custom check button that draws a white selector.
 */
class KSP_Check_Button : public Fl_Light_Button
{
public:
    KSP_Check_Button (int X, int Y, int W, int H, const char* L = 0);

private:
    void draw();
};

#endif
