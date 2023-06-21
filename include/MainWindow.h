#pragma once

#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"
#include "tools/sk_app/Application.h"
#include "tools/sk_app/Window.h"
#include "tools/skui/ModifierKey.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontTypes.h"

#include "ImGuiLayer.h"
#include "main.h"

#include <string>
#include <map>

#include "misc/cpp/imgui_stdlib.h"

using namespace std;

class SkSurface;

namespace pschem {

class MainWindow : public sk_app::Application, sk_app::Window::Layer, IIdReceiver, ImGuiRenderer {
public:
    MainWindow(int argc, char** argv, void* platformData);
    ~MainWindow() override;

    void onIdle() override;

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    void onResize(int width, int height) override;
    bool onMouseWheel(float delta, skui::ModifierKey modifiers) override; 
    bool onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) override;
    bool onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

    void SetId(int id, Drawable* tgt) override;
    void drawImGui() override;
    void idleRender() override;

    void beginDrag();
private:
    int mIdleRender;
    void updateTitle();

    ImGuiLayer             fImGuiLayer;
    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    SkScalar fRotationAngle;
    std::array<SkColor,22> colorMap;
    

    sk_sp<SkTypeface> typeface;
    DrawContext ctx;
    int mouse_x, mouse_y, begin_x, begin_y;
    int mouse_drawing_x, mouse_drawing_y, begin_drawing_x, begin_drawing_y;
    bool moving;
    bool multiSelecting;
    
    Drawing& getDrawing(string fname);
    void drawDrawing(Drawing &drawing, anydict_t &props);
    map<string,Drawing> drawings;
    vector<Drawable*> byId;
    
    Drawing* m_drawing;
    
    sk_sp<SkSurface>           hitSurface;
    skui::ModifierKey          previousModifiers;
};

}; //Namespace pschem
