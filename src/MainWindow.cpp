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
    font = SkFont(typeface);
    font.setSubpixel(true);
    font.setSize(80);    
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
    
    view_mat = view_mat.Translate(fWindow->width()/2,fWindow->height()/2);

}

void MainWindow::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}


void MainWindow::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();

    // Clear background
    canvas->clear(SK_ColorBLACK);
    canvas->save();
    canvas->setMatrix(view_mat);

    SkPaint paint;
    paint.setColor(SK_ColorGRAY);
    paint.setStrokeWidth(1);
    paint.setStyle(SkPaint::Style::kStroke_Style);

    canvas->drawLine(0,-5000,0,5000,paint);
    canvas->drawLine(-5000,0,5000,0,paint);

    view_mat.invert(&inverse_view_mat);

    Drawing &drawing = getDrawing("letters.sch");//sky130_fd_pr/nfet_01v8.sym");//passgate.sch");
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
    view_mat = view_mat.postTranslate(-mouse_x,-mouse_y);    
    view_mat = view_mat.postScale(scale,scale);
    view_mat = view_mat.postTranslate(mouse_x,mouse_y);    
    return true;
}

bool MainWindow::onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) {
    if(state != skui::InputState::kDown)
        return false;
    float t=20;
    switch(key){
        case skui::Key::kLeft: view_mat = view_mat.postTranslate(-t,0.0); break;
        case skui::Key::kRight: view_mat = view_mat.postTranslate(t,0.0); break;
        case skui::Key::kUp: view_mat = view_mat.postTranslate(0.0,-t); break;
        case skui::Key::kDown: view_mat = view_mat.postTranslate(0.0,t); break;
        default: break;
    }
    fWindow->inval();
    return true;
}


Drawing &MainWindow::getDrawing(string fname){
    if(drawings.find(fname) != drawings.end())
        return drawings.at(fname);
        
    drawings.emplace(fname,fname);
    return drawings.at(fname);
}

std::vector<std::string> split(const std::string& target, char c)
{
	std::string temp;
	std::stringstream stringstream { target };
	std::vector<std::string> result;

	while (std::getline(stringstream, temp, c)) {
		result.push_back(temp);
	}

	return result;
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
    	canvas->drawLine(l.x1, l.y1, l.x2, l.y2, paint);
    }

    for(auto n : drawing.nets){
    	canvas->drawLine(n.x1, n.y1, n.x2, n.y2, paint);
    }

	for(auto c : drawing.arcs){
	    SkRect rect = SkRect::MakeXYWH(c.cx - c.rad, c.cy - c.rad, c.rad*2, c.rad*2);
		canvas->drawArc(rect, c.sa, c.ea, false, paint);
	}

    for(auto b : drawing.boxes){
	    SkRect rect = SkRect::MakeLTRB(b.x1,b.y1,b.x2,b.y2);
        canvas->drawRect(rect,paint);
    }

    for(auto p : drawing.polys){
        SkPath path;
        path.moveTo(p.xs[0],p.ys[0]);
        for(int i=1; i < p.xs.size(); i++){
            path.lineTo(p.xs[i],p.ys[i]);
        }
        canvas->drawPath(path,paint);
    }


    paint.setStyle(SkPaint::Style::kFill_Style);
    for(auto t : drawing.texts){
        canvas->save();
        paint.setStrokeWidth(0.25);
        
        canvas->translate(t.x,t.y);
        canvas->rotate(t.rot*90);

        SkMatrix cmat = canvas->getLocalToDeviceAs3x3();
        SkMatrix to_world =  inverse_view_mat * cmat;
       // to_world.dump();

    
        font.setSize(t.size*50);
        SkFontMetrics metrics;
        font.getMetrics(&metrics);


        auto texts = split(t.text,'\n');

        float max_width=0;
        for(int i=0; i < texts.size(); i++){
            max_width = std::max(max_width,font.measureText(texts[i].c_str(), texts[i].size(), SkTextEncoding::kUTF8));
        }
        float twidth = max_width;
        float theight = font.getSpacing() * texts.size();

        if(t.mirror)
            twidth *= -1;
       
        SkPoint text_points[4] = {{0,0},{twidth,0},{0,theight},{twidth,theight}};
        to_world.mapPoints(text_points,text_points,4);
        printf("%f, %f\n", text_points[0].x(), text_points[0].y());

        canvas->save();
        canvas->setMatrix(view_mat);
    	canvas->drawLine(text_points[0].x()-2, text_points[0].y(), text_points[0].x()+2, text_points[0].y(), paint);
    	canvas->drawLine(text_points[0].x(), text_points[0].y()-2, text_points[0].x(), text_points[0].y()+2, paint);

	    SkRect rect = SkRect::MakeLTRB(text_points[0].x(), text_points[0].y(), text_points[3].x(),text_points[3].y());
        paint.setColor(SkColorSetARGB(40,0,255,0));
        canvas->drawRect(rect,paint);

        paint.setColor(SK_ColorGREEN);
        bool isVert = abs(text_points[1].y() - text_points[0].y())>EPS;

        if(isVert){
            for(int i=0; i < texts.size(); i++){
                canvas->save();
                float tpos = (i+1)*font.getSpacing() - metrics.fDescent + text_points[0].x();
                float theight = text_points[2].x() - text_points[0].x();
                if(theight < 0)
                    tpos += theight;
                canvas->translate(tpos, text_points[0].y());
                canvas->rotate(-90);
                SkTextUtils::Draw(canvas,texts[i].c_str(), texts[i].size(), SkTextEncoding::kUTF8, 0, 0, font, paint, text_points[1].y()>text_points[0].y()?SkTextUtils::kRight_Align:SkTextUtils::kLeft_Align);
                canvas->restore();

            }                
        }else{
            for(int i=0; i < texts.size(); i++){
                canvas->save();
                float tpos = (i+1)*font.getSpacing() - metrics.fDescent + text_points[0].y();
                float theight = text_points[2].y() - text_points[0].y();
                if(theight < 0)
                    tpos += theight;
                canvas->translate(text_points[0].x(), tpos);
                SkTextUtils::Draw(canvas,texts[i].c_str(), texts[i].size(), SkTextEncoding::kUTF8, 0, 0, font, paint, text_points[1].x()<text_points[0].x()?SkTextUtils::kRight_Align:SkTextUtils::kLeft_Align);
                canvas->restore();

            }        
        }
        canvas->restore();

//        printf("%f %f %f %f %f %f\n", font.getSize(), font.getSpacing(), metrics.fDescent, metrics.fAscent, metrics.fLeading, (font.getSpacing() - (metrics.fDescent - metrics.fAscent)) / 2);
        canvas->restore();
    }

    for(auto c : drawing.components){
        canvas->save();
        Drawing &subDrawing = getDrawing(c.symbol);
        canvas->translate(c.x,c.y);
        if(c.mirror)
            canvas->scale(-1,1);
        canvas->rotate(-c.rot*90);
        
        drawDrawing(subDrawing,canvas);
        canvas->restore();
    }

	canvas->restore();
}

