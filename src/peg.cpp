#include "peglib.h"
#include <fstream>

#include "main.h"

using namespace std;
using namespace peg;

static string readFile(string fname){
    ifstream t(fname);
    stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

void test_peg(string filename, vector<Line> &lines, vector<Arc> &arcs, vector<Box> &boxes, vector<Text> &texts, vector<Net> &nets, vector<Component> &components, vector<Poly> &polys){
    string data = readFile(filename);
    printf("Reading %s\n", filename.c_str());

    parser parser(R"(
        Prog        <- (Stmt LINE_BREAK)* Stmt?
        Stmt        <- RECORD_ID Field+
        Field       <- NUMBER / BraceList
        BraceList   <- '{' PROP_STRING '}'
        PROP_STRING <- < (!'}' .)* >
        NUMBER      <- < [-]?[0-9]+[.]?[0-9]* >
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
        return string(vs.sv())  ;   
    };

    parser["BraceList"] = [&](const SemanticValues &vs) {
        return vs[0];  
    };

    parser["NUMBER"] = [&](const SemanticValues &vs) {
        return vs.token_to_number<float>();   
    };

    parser["Field"] = [&](const SemanticValues &vs) {
        return vs[0];  
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
            else
                printf("%s ", any_cast<string>(f).c_str());
        }
        printf("\n");
        
        if(statement.id == "L "){
            lines.push_back({ (int)any_cast<float>(statement.fields[0]),
                              any_cast<float>(statement.fields[1]),
                              any_cast<float>(statement.fields[2]),
                              any_cast<float>(statement.fields[3]),
                              any_cast<float>(statement.fields[4])});
        }

        if(statement.id == "N "){
            nets.push_back({  any_cast<float>(statement.fields[0]),
                              any_cast<float>(statement.fields[1]),
                              any_cast<float>(statement.fields[2]),
                              any_cast<float>(statement.fields[3])});
        }

        if(statement.id == "B "){
            boxes.push_back({ (int)any_cast<float>(statement.fields[0]),
                              any_cast<float>(statement.fields[1]),
                              any_cast<float>(statement.fields[2]),
                              any_cast<float>(statement.fields[3]),
                              any_cast<float>(statement.fields[4])});
        }
        
        if(statement.id == "A "){
            arcs.push_back({ (int)any_cast<float>(statement.fields[0]),
                              any_cast<float>(statement.fields[1]),
                              any_cast<float>(statement.fields[2]),
                              any_cast<float>(statement.fields[3]),
                              any_cast<float>(statement.fields[4]),
                              any_cast<float>(statement.fields[5])});
        }

        if(statement.id == "T "){
            texts.push_back({ any_cast<string>(statement.fields[0]),
                              any_cast<float>(statement.fields[1]),
                              any_cast<float>(statement.fields[2]),
                              (int)any_cast<float>(statement.fields[3]),
                              (int)any_cast<float>(statement.fields[4]),
                              any_cast<float>(statement.fields[5])});
        }

        if(statement.id == "C "){
            components.push_back({ any_cast<string>(statement.fields[0]),
                              any_cast<float>(statement.fields[1]),
                              any_cast<float>(statement.fields[2]),
                              (int)any_cast<float>(statement.fields[3]),
                              (int)any_cast<float>(statement.fields[4])});
        }

        if(statement.id == "P "){
            Poly p((int)any_cast<float>(statement.fields[0]), (int)any_cast<float>(statement.fields[1]));
            for(int i=0; i < p.xs.size(); i++){
                  p.xs[i] = any_cast<float>(statement.fields[2+i*2]);
                  p.ys[i] = any_cast<float>(statement.fields[2+i*2+1]);
            }
            polys.push_back(p);
        }

    }    
}
