#ifndef ksp_volume_controller_h
#define ksp_volume_controller_h

#include <FL/Fl_Valuator.H>

/**
 * Custom volume controller
 */
class KSP_Volume_Controller : public Fl_Valuator
{
public:
    KSP_Volume_Controller(int xx, int yy, int ww, int hh);
    float value2();
    void increase(int v = 1);
    void decrease(int v = 1);

private:
    void draw();
    int handle(int event);
};

#endif
