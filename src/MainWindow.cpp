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
    
    zoom=0;
    ctr_x=0;
    ctr_y=0;
}

MainWindow::~MainWindow() {
    fWindow->detach();
    delete fWindow;
}

void MainWindow::updateTitle() {
    if (!fWindow) {
        return;
    }

    SkString title("Hello World ");
    if (Window::kRaster_BackendType == fBackendType) {
        title.append("Raster");
    } else {
        title.append("GL");
    }

    fWindow->setTitle(title.c_str());
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
    canvas->translate(fWindow->width()/2,fWindow->height()/2);

    SkPaint paint;
    paint.setColor(SK_ColorGRAY);
    paint.setStrokeWidth(1);
    paint.setStyle(SkPaint::Style::kStroke_Style);


    canvas->translate(ctr_x*pow(2,-zoom/10.0f),ctr_y*pow(2,-zoom/10.0f));

    canvas->drawLine(0,-5000,0,5000,paint);
    canvas->drawLine(-5000,0,5000,0,paint);

    canvas->scale(pow(2,-zoom/10.0f), pow(2,-zoom/10.0f));

    SkMatrix view_mat = canvas->getLocalToDeviceAs3x3();
    view_mat.invert(&inverse_view_mat);

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
    
    float mdx = (mouse_x - fWindow->width()/2)/pow(2,-zoom/10.0f) - ctr_x;
    printf("%f, %d %f\n", mdx, mouse_x, ctr_x);

    return true;
}

bool MainWindow::onMouseWheel(float delta, skui::ModifierKey modifiers) {
    fWindow->inval();
    if(delta<0){
        float delta_zoom = pow(2,-(zoom+delta)/10.0f)/pow(2,-zoom/10.0f) - 1.0;
        float mdx = (mouse_x - fWindow->width()/2)/pow(2,-zoom/10.0f) - ctr_x;
        float mdy = (mouse_y - fWindow->height()/2)/pow(2,-zoom/10.0f) - ctr_y;
        
        printf("%f: %f, %f, %d %f\n", delta_zoom, mdx, mdy, mouse_x, ctr_x);
        ctr_x -= mdx * delta_zoom ;
        ctr_y -= mdy * delta_zoom ;

//        ctr_x += mdx*pow(2,-(zoom)/10.0f);
//        ctr_y += mdy*pow(2,-(zoom)/10.0f);
    }
    
    zoom+=delta;
    return true;
}

bool MainWindow::onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) {
    if(state != skui::InputState::kDown)
        return false;
    switch(key){
        case skui::Key::kLeft: ctr_x -= fWindow->width() * pow(2,zoom/10.0f) / 50; break;
        case skui::Key::kRight: ctr_x += fWindow->width() * pow(2,zoom/10.0f) / 50; break;
        case skui::Key::kUp: ctr_y -= fWindow->height() * pow(2,zoom/10.0f) / 50; break;
        case skui::Key::kDown: ctr_y += fWindow->height() * pow(2,zoom/10.0f) / 50; break;
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
        
        SkMatrix cmat = canvas->getLocalToDeviceAs3x3();
        SkMatrix to_world =  inverse_view_mat * cmat;
       // to_world.dump();

    	canvas->drawLine(t.x, t.y-2, t.x, t.y+2, paint);
    	canvas->drawLine(t.x-2, t.y, t.x+2, t.y, paint);

        paint.setColor(SkColorSetARGB(40,0,255,0));

    

    
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
        
        float xpos = t.x;
        
        if(t.rot==2){
            theight *= -1;
            twidth *= -1;
        }
        if(t.mirror)
            twidth *= -1;
                    
	    SkRect rect = SkRect::MakeXYWH(xpos, t.y, twidth,theight);
        canvas->drawRect(rect,paint);

        paint.setColor(SK_ColorGREEN);
//        printf("%f %f %f %f %f %f\n", font.getSize(), font.getSpacing(), metrics.fDescent, metrics.fAscent, metrics.fLeading, (font.getSpacing() - (metrics.fDescent - metrics.fAscent)) / 2);
        for(int i=0; i < texts.size(); i++){
            float tpos = t.y+(i+1)*font.getSpacing() - metrics.fDescent;
            if(theight < 0)
                tpos += theight;
            SkTextUtils::Draw(canvas,texts[i].c_str(), texts[i].size(), SkTextEncoding::kUTF8, xpos, tpos, font, paint, twidth<0?SkTextUtils::kRight_Align:SkTextUtils::kLeft_Align);
        }
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

