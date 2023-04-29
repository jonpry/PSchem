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

using namespace std;

class SkSurface;

class MainWindow : public sk_app::Application, sk_app::Window::Layer {
public:
    MainWindow(int argc, char** argv, void* platformData);
    ~MainWindow() override;

    void onIdle() override;

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    bool onMouseWheel(float delta, skui::ModifierKey modifiers) override;
    bool onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) override;
    bool onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) override;

private:
    void updateTitle();

    ImGuiLayer             fImGuiLayer;
    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    SkScalar fRotationAngle;
    
    void drawImGui();

    sk_sp<SkTypeface> typeface;
    SkFont font;
    SkMatrix inverse_view_mat;
    float zoom, ctr_x, ctr_y;
    int mouse_x, mouse_y;
    
    Drawing& getDrawing(string fname);
    void drawDrawing(Drawing &drawing, SkCanvas *canvas);
    map<string,Drawing> drawings;
};


