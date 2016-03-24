#ifndef ksp_menu_item_h
#define ksp_menu_item_h

#include <FL/Fl_Menu_.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Menu_Window.H>

/**
 * This custom menu draws a check box with a white selector.
 */
class KSP_Menu_Item : public Fl_Menu_Item
{
public:
    KSP_Menu_Item(const char* label = 0, int shortcut1 = 0, Fl_Callback* callback = 0, void* data = 0, int flags1 = 0);

    void draw(int x, int y, int w, int h, const Fl_Menu_*, int t=0) const;

    const KSP_Menu_Item* popup(int X, int Y) const;
    const KSP_Menu_Item* pulldown(int X, int Y) const;

    void set_label_icon(const char* text, Fl_Image* icon, bool prepend_space = true);
    void set_toggled(bool toggled);
};

#endif
