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
    return new pschem::MainWindow(argc, argv, platformData);
}

namespace pschem {

MainWindow::MainWindow(int argc, char** argv, void* platformData)
        : fBackendType(Window::kNativeGL_BackendType),
        fRotationAngle(0), moving(false) {
  
  
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

    if(!hitSurface){
        SkImageInfo secondaryInfo = SkImageInfo::MakeN32Premul(fWindow->width(),fWindow->height());
        SkSurfaceProps props;
        hitSurface = SkSurface::MakeRenderTarget(fWindow->directContext(), skgpu::Budgeted::kNo, secondaryInfo, 0, &props);
    }
    auto hitCanvas  = hitSurface->getCanvas();

    bool viewHit=false;

    // Clear background
    hitCanvas->clear(SK_ColorBLACK);
    canvas->clear(colorMap[COLOR_BG]);
    canvas->save();
    canvas->setMatrix(ctx.view_mat);

    SkPaint paint;
    paint.setColor(colorMap[COLOR_SEL]);
    paint.setStrokeWidth(1);
    paint.setStyle(SkPaint::Style::kStroke_Style);

    canvas->drawLine(0,-5000,0,5000,paint);
    canvas->drawLine(-5000,0,5000,0,paint);

    bool invertable = ctx.view_mat.invert(&ctx.inverse_view_mat);
    ctx.canvas = canvas;
    ctx.hitCanvas = hitCanvas;
    ctx.colorMap = colorMap;
    ctx.objId = 0;
    ctx.parent = 0;
    ctx.window = this;
        
    hitCanvas->setMatrix(canvas->getTotalMatrix());
    Drawing &drawing = getDrawing("passgate.sch");//sky130_fd_pr/nfet_01v8.sym");//passgate.sch");
    anydict_t empty;
    drawDrawing(drawing,empty);    

    canvas->restore();
    if(viewHit)
       hitSurface->draw(canvas,0,0,&paint);

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
    
    if (skui::InputState::kDown == state) {
        if(moving){
            moving=false;
            return true;
        }
    
        uint32_t buf[121];
        SkImageInfo dstInfo = SkImageInfo::MakeN32Premul(11,11);
        hitSurface->readPixels(dstInfo,buf,11*sizeof(uint32_t),x-5,y-5);
        int selectedId=-1;
        for(int i=0; i <= 5; i++){
            printf("I: %d\n", i);
            for(int j=0; j < i*2+1; j++) {
                selectedId = mortonId(buf[(5-i)*11+5-i+j]);
                if(selectedId>=0)
                    break;
            }

            if(selectedId>=0)
                break;

            for(int j=0; j < i*2+1; j++) {
                selectedId = mortonId(buf[(5-i+j)*11+5+i]);
                if(selectedId>=0)
                    break;
            }

            if(selectedId>=0)
                break;
            
            for(int j=0; j < i*2+1; j++) {
                selectedId = mortonId(buf[(5+i)*11+5-i+j]);
                if(selectedId>=0)
                    break;
            }

            if(selectedId>=0)
                break;
            
            for(int j=0; j < i*2+1; j++) {
                selectedId = mortonId(buf[(5-i+j)*11+5-i]);
                if(selectedId>=0)
                    break;

            }            

            if(selectedId>=0)
                break;

        }
        ctx.selected = selectedId>=0?byId[selectedId]:0;
        printf("Selected: %d %p\n", selectedId, ctx.selected);

        fWindow->inval();    
    }
    
    if(moving) {
        SkPoint mouse_points[2] = {{begin_x,begin_y},{mouse_x,mouse_y}};
        ctx.inverse_view_mat.mapPoints(mouse_points,2);
        printf("%f %f\n", mouse_points[1].x() - mouse_points[0].x(), mouse_points[1].y() - mouse_points[0].y());
    
        ctx.selected->move(mouse_points[1].x() - mouse_points[0].x(), mouse_points[1].y() - mouse_points[0].y());
        begin_x = mouse_x;
        begin_y = mouse_y;
        fWindow->inval();     
    }
    
    return true;
}

void MainWindow::SetId(int id, Drawable* tgt){
    if(byId.size() <= id)
        byId.resize(id+1);
    byId[id] = tgt;
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
        case skui::Key::kUp: ctx.view_mat.postTranslate(0.0,t); break;
        case skui::Key::kDown: ctx.view_mat.postTranslate(0.0,-t); break;        
        default: break;
    }
    fWindow->inval();
    return true;
}

bool MainWindow::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    switch(c){
        case 'f': if(ctx.selected != 0 && (modifiers == skui::ModifierKey::kOption)) ctx.selected->flip(); break; 
        case 'r': if(ctx.selected != 0 && (modifiers == skui::ModifierKey::kOption)) ctx.selected->rotate(); break; 
        case 'm': if(ctx.selected != 0 && (modifiers == skui::ModifierKey::kNone)) beginDrag(); break; 

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

void MainWindow::onResize(int width, int height){
    hitSurface.reset();
}

void MainWindow::beginDrag(){
    moving=true;
    begin_x = mouse_x;
    begin_y = mouse_y;
}

void MainWindow::drawDrawing(Drawing &drawing, anydict_t &props){

    ctx.canvas->save();
    ctx.hitCanvas->setMatrix(ctx.canvas->getTotalMatrix());
    
    SkPaint paint;
    paint.setColor(SK_ColorGREEN);
    paint.setStrokeWidth(1);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::Join::kRound_Join);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    ctx.props = props;

    drawing.DeriveConnectivity();
    
    for(auto &l : drawing.lines){
        l.draw(paint,ctx);
    }

    for(auto &n : drawing.nets){
        n.draw(paint,ctx);
    }

	for(auto &c : drawing.arcs){
        c.draw(paint,ctx);
	}

    paint.setStyle(SkPaint::Style::kFill_Style);
    for(auto &b : drawing.boxes){
        b.draw(paint,ctx);
    }

    for(auto &p : drawing.polys){
        p.draw(paint,ctx);
    }

    paint.setStyle(SkPaint::Style::kFill_Style);
    for(auto &t : drawing.texts){
        t.draw(paint,ctx);
    }

    bool setParent = !ctx.parent;
    for(auto &c : drawing.components){
        ctx.canvas->save();
        Drawing &subDrawing = getDrawing(c.symbol);
        if(setParent)
            ctx.parent = &c;
        else
            ctx.parent = 0;
        ctx.canvas->translate(c.pt.m_x,c.pt.m_y);
        ctx.canvas->rotate(c.rot*90);
        if(c.mirror)
            ctx.canvas->scale(-1,1);

        drawDrawing(subDrawing,c.m_props);
        ctx.canvas->restore();
    }

	ctx.canvas->restore();
}

}; //Namespace pschem
