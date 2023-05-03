/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "MainWindow.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "include/core/SkShader.h"
#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTileMode.h"
#include "include/core/SkFontMetrics.h"
#include "include/utils/SkTextUtils.h"
#include "include/effects/SkGradientShader.h"
#include "include/gpu/GrDirectContext.h"
#include "tools/sk_app/DisplayParams.h"
#include "tools/flags/CommonFlags.h"

#include <sstream>
#include <string.h>

using namespace sk_app;

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new MainWindow(argc, argv, platformData);
}

MainWindow::MainWindow(int argc, char** argv, void* platformData)
        : fBackendType(Window::kNativeGL_BackendType),
        fRotationAngle(0) {
    SkGraphics::Init();

    DisplayParams displayParams;
 //   displayParams.fMSAASampleCount = FLAGS_msaa;
 //   displayParams.fEnableBinaryArchive = FLAGS_binaryarchive;
//    CommonFlags::SetCtxOptions(&displayParams.fGrContextOptions);
 //   displayParams.fGrContextOptions.fPersistentCache = &fPersistentCache;
 //   displayParams.fGrContextOptions.fShaderCacheStrategy =
 //           GrContextOptions::ShaderCacheStrategy::kSkSL;
 //   displayParams.fGrContextOptions.fShaderErrorHandler = &gShaderErrorHandler;
//    displayParams.fGrContextOptions.fSuppressPrints = true;
//    displayParams.fGrContextOptions.fSupportBilerpFromGlyphAtlas = true;
#if 0
    if (FLAGS_dmsaa) {
        displayParams.fSurfaceProps = SkSurfaceProps(
                displayParams.fSurfaceProps.flags() | SkSurfaceProps::kDynamicMSAA_Flag,
                displayParams.fSurfaceProps.pixelGeometry());
    }
#endif   
    fWindow = Window::CreateNativeWindow(platformData);
    fWindow->setRequestedDisplayParams(displayParams);

    fImGuiLayer.setScaleFactor(2.0); //fWindow->scaleFactor());
    // register callbacks
    fWindow->pushLayer(this);
    fWindow->pushLayer(&fImGuiLayer);

    fWindow->attach(fBackendType);
    
    typeface = SkTypeface::MakeFromName("Arial",SkFontStyle());
    ctx.font = SkFont(typeface);
    ctx.font.setSubpixel(true);
    ctx.font.setSize(80);    
    
    uint32_t colors[] = {0x000000, 0x00ccee, 0x3f3f3f, 0xcccccc, 0x88dd00,
                        0xbb2200, 0x00ccee, 0xff0000, 0xffff00, 0xffffff,
                        0xff00ff, 0x00ff00, 0x0000cc, 0xaaaa00, 0xaaccaa,
                        0xff7777, 0xbfff81, 0x00ffcc, 0xce0097, 0xd2d46b,
                        0xef6158, 0xfdb200};

    for(int i=0; i < 22; i++)
        colorMap[i] = SkColorSetARGB(255,colors[i] >> 16, (colors[i] >> 8) & 0xff, colors[i] & 0xff);
}

MainWindow::~MainWindow() {
    fWindow->detach();
    delete fWindow;
}

void MainWindow::updateTitle() {
    if (!fWindow) {
        return;
    }

    fWindow->setTitle("PSchem");
    
    ctx.view_mat = ctx.view_mat.Translate(fWindow->width()/2,fWindow->height()/2);

}

void MainWindow::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}


void MainWindow::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(colorMap[COLOR_BG]);
    canvas->save();
    canvas->setMatrix(ctx.view_mat);

    SkPaint paint;
    paint.setColor(colorMap[COLOR_SEL]);
    paint.setStrokeWidth(1);
    paint.setStyle(SkPaint::Style::kStroke_Style);

    canvas->drawLine(0,-5000,0,5000,paint);
    canvas->drawLine(-5000,0,5000,0,paint);

    ctx.view_mat.invert(&ctx.inverse_view_mat);

    Drawing &drawing = getDrawing("passgate.sch");//sky130_fd_pr/nfet_01v8.sym");//passgate.sch");
    drawDrawing(drawing,canvas);    
    canvas->restore();
    this->drawImGui();
}

void MainWindow::onIdle() {
}

void MainWindow::drawImGui() {
    bool t = true;
    ImGui::ShowDemoWindow(&t);
}

bool MainWindow::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) {
    mouse_x = x;
    mouse_y = y;
    return true;
}

bool MainWindow::onMouseWheel(float delta, skui::ModifierKey modifiers) {
    fWindow->inval();
    
    float scale = delta>0?1.1:0.9;
    ctx.view_mat.postTranslate(-mouse_x,-mouse_y);    
    ctx.view_mat.postScale(scale,scale);
    ctx.view_mat.postTranslate(mouse_x,mouse_y);    
    return true;
}

bool MainWindow::onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) {
    if(state != skui::InputState::kDown)
        return false;
    float t=20;
    switch(key){
        case skui::Key::kLeft: ctx.view_mat.postTranslate(-t,0.0); break;
        case skui::Key::kRight: ctx.view_mat.postTranslate(t,0.0); break;
        case skui::Key::kUp: ctx.view_mat.postTranslate(0.0,-t); break;
        case skui::Key::kDown: ctx.view_mat.postTranslate(0.0,t); break;
        default: break;
    }
    fWindow->inval();
    return true;
}


Drawing &MainWindow::getDrawing(string fname){
    if(drawings.find(fname) != drawings.end())
        return drawings.at(fname);
        
    Drawing drawing(fname);
    drawings.emplace(fname,drawing);
    return drawings.at(fname);
}

void MainWindow::drawDrawing(Drawing &drawing, SkCanvas *canvas){

    canvas->save();
    
    SkPaint paint;
    paint.setColor(SK_ColorGREEN);
    paint.setStrokeWidth(1);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::Join::kRound_Join);
    paint.setStyle(SkPaint::Style::kStroke_Style);


    for(auto l : drawing.lines){
        paint.setColor(colorMap[l.layer]);
        l.draw(canvas,paint,ctx);
    }

    for(auto n : drawing.nets){
        paint.setColor(colorMap[COLOR_NET]);
        n.draw(canvas,paint,ctx);
    }

	for(auto c : drawing.arcs){
        paint.setColor(colorMap[c.layer]);
        c.draw(canvas,paint,ctx);
	}

    paint.setStyle(SkPaint::Style::kFill_Style);
    for(auto b : drawing.boxes){
        paint.setColor(colorMap[b.layer]);
        b.draw(canvas,paint,ctx);
    }

    paint.setStyle(SkPaint::Style::kStroke_Style);
    for(auto p : drawing.polys){
        paint.setColor(colorMap[p.layer]);
        p.draw(canvas,paint,ctx);
    }


    paint.setStyle(SkPaint::Style::kFill_Style);
    for(auto t : drawing.texts){
        paint.setColor(colorMap[t.layer]);
        t.draw(canvas,paint,ctx);
    }

    for(auto c : drawing.components){
        canvas->save();
        Drawing &subDrawing = getDrawing(c.symbol);
        canvas->translate(c.x,c.y);
        canvas->rotate(c.rot*90);
        if(c.mirror)
            canvas->scale(-1,1);

        drawDrawing(subDrawing,canvas);
        canvas->restore();
    }

	canvas->restore();
}

