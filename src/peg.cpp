#include "peglib.h"
#include <fstream>

#include "main.h"

using namespace std;
using namespace peg;

namespace pschem {

static string readFile(string fname){
    ifstream t(fname);
    stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

static float getFloat(any field){
    if(field.type() == typeid(float))
        return any_cast<float>(field);
    return any_cast<int>(field);
}

void test_peg(string filename, vector<Line> &lines, vector<Arc> &arcs, vector<Box> &boxes, vector<Text> &texts, vector<Net> &nets, vector<Component> &components, vector<Poly> &polys){
    string data = readFile(filename);
    printf("Reading %s\n", filename.c_str());

    parser parser(R"(
        Prog        <- (Stmt LINE_BREAK)* Stmt?
        Stmt        <- RECORD_ID BraceList? NUMBER* Dict?
        BraceList   <- '{' PROP_STRING '}'
        Dict        <- '{' Item* '}'
        Item        <- KEY '=' (Value/QUOTED_STRING) LINE_BREAK?
        KEY         <- < [a-zA-Z_]+ >
        Value       <- NUMBER/BOOL/STRING
        NUMBER      <- FLOAT/INT
        STRING      <- < [a-zA-Z0-9_]+ >
        BOOL        <- 'true' / 'false'
        QUOTED_STRING <-   < ["] (!'"' .)* ["] >
        
        PROP_STRING <- < (!'}' .)* >
        INT         <- < [-]?[0-9]+ >
        FLOAT       <- < [-]?[0-9]+[.][0-9]* >
        RECORD_ID   <- < [a-zA-Z] >
        LINE_BREAK  <- < [\r]?[\n] >
        %whitespace <- [ \t]*
    )");
    
    
    parser.set_logger([](size_t line, size_t col, const string& msg, const string &rule) {
        cerr << line << ":" << col << ": " << msg << "\n";
    });
    assert(static_cast<bool>(parser) == true);
    
    parser.enable_packrat_parsing(); // Enable packrat parsing.    

    struct Statement {
        string id;
        vector<any> fields;
    };

    vector<Statement> statements;

    parser["PROP_STRING"] = [&](const SemanticValues &vs) {
        return string(vs.token());   
    };

    parser["BraceList"] = [&](const SemanticValues &vs) {
        return vs[0];  
    };

    parser["Dict"] = [&](const SemanticValues &vs) {
        map<string,any> ret;
        for(int i=0; i < vs.size(); i++){
            auto p = any_cast<pair<string,any>>(vs[i]);
            ret[p.first] = p.second;
        }
        return ret;
    };
    
    parser["Item"] = [&](const SemanticValues &vs) {
        return make_pair(any_cast<string>(vs[0]),vs[1]);
    };
    
    parser["KEY"] = [&](const SemanticValues &vs) { 
        return string(vs.token());       
    };

    parser["QUOTED_STRING"] = [&](const SemanticValues &vs) { 
        string ret(vs.token());   
        ret = ret.substr(1,ret.size()-2);
        return ret;    
    };

    parser["STRING"] = [&](const SemanticValues &vs) { 
        string ret(vs.token());
        assert(ret.find(" ") == -1);
        return ret;       
    };
    
    parser["BOOL"] = [&](const SemanticValues &vs) { 
        return (bool)(string(vs.token()) == "true"?1:0);       
    };

    parser["NUMBER"] = [&](const SemanticValues &vs) {
        return vs[0];   
    };

    
    parser["FLOAT"] = [&](const SemanticValues &vs) {
        return vs.token_to_number<float>();   
    };

    parser["INT"] = [&](const SemanticValues &vs) {
        return vs.token_to_number<int>();   
    };

    parser["RECORD_ID"] = [&](const SemanticValues &vs) {
        return string(vs.sv());   
    };

    parser["Stmt"] = [&](const SemanticValues &vs) {
        Statement stmt = {any_cast<string>(vs[0])};
        for(int i=1; i < vs.size(); i++)
            stmt.fields.push_back(vs[i]);
        statements.push_back(stmt);
    };

    int val;    
    parser.parse(data, val);
        
    for(auto statement : statements){
        printf("%s ", statement.id.c_str());
        for(auto f : statement.fields){
            if(f.type() == typeid(float))
                printf("%f ", any_cast<float>(f));
            else if(f.type() == typeid(string))
                printf("%s ", any_cast<string>(f).c_str());
            else if(f.type() == typeid(int))
                printf("%d ", any_cast<int>(f));
            else{ //map
                auto d = any_cast<map<string,any>>(f);
                for(auto p : d){
                    if(p.second.type() == typeid(string))
                        printf("%s=\"%s\" ", p.first.c_str(), any_cast<string>(p.second).c_str());
                    else if(p.second.type() == typeid(float))
                        printf("%s=%f ", p.first.c_str(), any_cast<float>(p.second));
                    else if(p.second.type() == typeid(int))
                        printf("%s=%d ", p.first.c_str(), any_cast<int>(p.second));
                    else if(p.second.type() == typeid(bool))
                        printf("%s=%s ", p.first.c_str(), any_cast<bool>(p.second)?"BOOL_true":"BOOL_false");
                }
            }
        }
        printf("\n");
        
        if(statement.id == "L "){
            lines.push_back({ any_cast<int>(statement.fields[0]),
                              getFloat(statement.fields[1]),
                              getFloat(statement.fields[2]),
                              getFloat(statement.fields[3]),
                              getFloat(statement.fields[4]),
                              any_cast<map<string,any>>(statement.fields[5])});
        }

        if(statement.id == "N "){
            nets.push_back({  getFloat(statement.fields[0]),
                              getFloat(statement.fields[1]),
                              getFloat(statement.fields[2]),
                              getFloat(statement.fields[3]),
                              any_cast<map<string,any>>(statement.fields[4])});
        }

        if(statement.id == "B "){
            boxes.push_back({ any_cast<int>(statement.fields[0]),
                              getFloat(statement.fields[1]),
                              getFloat(statement.fields[2]),
                              getFloat(statement.fields[3]),
                              getFloat(statement.fields[4]),
                              any_cast<map<string,any>>(statement.fields[5])});
        }
        
        if(statement.id == "A "){
            arcs.push_back({ any_cast<int>(statement.fields[0]),
                              getFloat(statement.fields[1]),
                              getFloat(statement.fields[2]),
                              getFloat(statement.fields[3]),
                              getFloat(statement.fields[4]),
                              getFloat(statement.fields[5]),
                              any_cast<map<string,any>>(statement.fields[6])});
        }

        if(statement.id == "T "){
            texts.push_back({ any_cast<string>(statement.fields[0]),
                              getFloat(statement.fields[1]),
                              getFloat(statement.fields[2]),
                              any_cast<int>(statement.fields[3]),
                              any_cast<int>(statement.fields[4]),
                              getFloat(statement.fields[5]),
                              any_cast<map<string,any>>(statement.fields[7])});                              
        }

        if(statement.id == "C "){
            components.push_back({ any_cast<string>(statement.fields[0]),
                              getFloat(statement.fields[1]),
                              getFloat(statement.fields[2]),
                              any_cast<int>(statement.fields[3]),
                              any_cast<int>(statement.fields[4]),
                              any_cast<map<string,any>>(statement.fields[5])});
        }

        if(statement.id == "P "){
            Poly p(any_cast<int>(statement.fields[0]), any_cast<int>(statement.fields[1]), any_cast<map<string,any>>(statement.fields[statement.fields.size()-1]));
            for(int i=0; i < p.pts.size(); i++){
                  p.pts[i].m_x = getFloat(statement.fields[2+i*2]);
                  p.pts[i].m_y = getFloat(statement.fields[2+i*2+1]);
            }
            polys.push_back(p);
        }

    }    
}

}; //Namespace pschem
