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
#include <deque>
#include <string.h>

using namespace sk_app;

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new pschem::MainWindow(argc, argv, platformData);
}

namespace pschem {

MainWindow::MainWindow(int argc, char** argv, void* platformData)
        : fBackendType(Window::kNativeGL_BackendType), fImGuiLayer(this),
        fRotationAngle(0), moving(false), multiSelecting(false) {
  
  
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
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    mIdleRender=1;

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
        
    fImGuiLayer.render();    
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

    SkPaint paint;
    paint.setColor(colorMap[COLOR_SEL]);
    paint.setStyle(SkPaint::Style::kStroke_Style);

    canvas->setMatrix(ctx.view_mat);
    paint.setStrokeWidth(1/ctx.view_mat.getScaleX());

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

    if(multiSelecting){
        SkRect rect = SkRect::MakeLTRB(begin_x,begin_y,mouse_x,mouse_y);
        canvas->drawRect(rect,paint);
    }

    paint.setStrokeWidth(1);

    canvas->restore();
    if(viewHit)
       hitSurface->draw(canvas,0,0,&paint);
}

void MainWindow::onIdle() {
    if(mIdleRender){
        mIdleRender--;
        fImGuiLayer.render();        
    }
}

void MainWindow::idleRender() {
    mIdleRender++;
}

void MainWindow::drawImGui() {

    if(ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
         if(ImGui::MenuItem("New")) {
         }

         if(ImGui::MenuItem("Open")) {
         }

         if(ImGui::MenuItem("Save")) {
         }

         if(ImGui::MenuItem("Save As")) {
         }

         ImGui::Separator();

         if(ImGui::MenuItem("Exit")) {
         }

         ImGui::EndMenu();
       }

      if (ImGui::BeginMenu("Edit")) {
         if(ImGui::MenuItem("Undo")) {
         }

         if(ImGui::MenuItem("Redo")) {
         }

         ImGui::Separator();
         if(ImGui::MenuItem("Cut", "CTRL-X")) {
         }

         if(ImGui::MenuItem("Copy")) {
         }

         if(ImGui::MenuItem("Paste")) {
         }

         ImGui::EndMenu();
       }

       ImGui::EndMainMenuBar();
    }

    static bool open=true;

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &open, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    ImGui::End();

    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Properties", 0, 0))
    {
        ImGui::End();
        return;
    }
    
    static float x=0,y=0;
    ImGui::InputFloat("X",&x,0.01f, 1.0f, "%.3f");
    ImGui::InputFloat("Y",&y,0.01f, 1.0f, "%.3f");
    
    ImGui::End();
    
    ImGui::ShowDemoWindow(0);
}

bool MainWindow::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) {
    mouse_x = x;
    mouse_y = y;


    previousModifiers = modifiers;    
    
    if(multiSelecting){
        SkPoint mouse_points[1] = {{x,y}};
        ctx.inverse_view_mat.mapPoints(mouse_points,1);

        mouse_x = mouse_points[0].x();
        mouse_y = mouse_points[0].y();
        
        if(state == skui::InputState::kUp)
            multiSelecting=false;

        fWindow->inval();    
        return true;
    }
    
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
        
        SkPoint mouse_points[1] = {{x,y}};
        ctx.inverse_view_mat.mapPoints(mouse_points,1);

        begin_x = mouse_points[0].x();
        begin_y = mouse_points[0].y();
        mouse_x = begin_x;
        mouse_y = begin_y;
        multiSelecting = true;
           
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
        case skui::Key::kEscape: multiSelecting=false; break;        
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
    fImGuiLayer.render();        
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

    paint.setStyle(SkPaint::Style::kFill_Style);

    if(!ctx.parent)
        drawing.DeriveConnectivity(paint,ctx);

    paint.setStyle(SkPaint::Style::kStroke_Style);
    
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
