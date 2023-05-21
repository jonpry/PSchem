#include "main.h"
#include <set>

namespace pschem {

class SortedLine {
 public:
    SortedLine(Point p1, Point p2) {
        if(p1.m_x <= p2.m_x){
            m_p1 = p1; m_p2 = p2;
        }else{
            m_p2 = p1; m_p2 = p1;
        }
    }
    
    float distanceTo(Point pi){ 
        Eigen::Vector2f v = m_p1.Vec(), w = m_p2.Vec(), p = pi.Vec();
        float l2 = (v-w).squaredNorm();
        if (l2 == 0.0) return (v-p).norm();   // v == w case

        // Consider the line extending the segment, parameterized as v + t (w - v).
        // We find projection of point p onto the line. 
        // It falls where t = [(p-v) . (w-v)] / |w-v|^2
        // We clamp t from [0,1] to handle points outside the segment vw.
        float t = std::max(0.0f, std::min(1.0f, ((p - v).dot(w - v)) / l2));

        Eigen::Vector2f projection = v + t * (w - v);  // Projection falls on the segment
        return (p - projection).norm();        
    }
    
    Line *m_src;
    
    Point m_p1, m_p2;
};

class Sweep {
 public:
    Sweep();
   
    void SetLines(std::vector<Line> &lines);
    void Query(Point p);
    void Query(std::vector<Point> p);
   
    std::vector<SortedLine> m_lines;
    std::vector<SortedLine*> m_starts,m_ends;
};

Sweep::Sweep() {
}

void Sweep::SetLines(std::vector<Line> &lines){
    m_lines.clear();
    m_starts.clear();;
    m_ends.clear();
    
    for(Line &l : lines){
        m_lines.push_back({l.p1,l.p2});
    }
    
    std::sort(m_lines.begin(),m_lines.end(), [](const SortedLine &a, const SortedLine &b) { return a.m_p1.m_x < b.m_p1.m_x;});
    
    for(int i=0; i < m_lines.size(); i++){
        m_starts.push_back(&m_lines[i]);
        m_ends.push_back(&m_lines[i]);
    }
    
    //starts already sorted
    std::sort(m_ends.begin(),m_ends.end(), [](const SortedLine *a, const SortedLine *b) { return a->m_p2.m_x < b->m_p2.m_x;});

}

void Sweep::Query(Point p){
    std::upper_bound(m_ends.begin(), m_ends.end(), nullptr, [&](const SortedLine *a, const SortedLine *b) { return a->m_p1.m_x < p.m_x; });
}

#define TOL 0.1
void Sweep::Query(std::vector<Point> pts) {
    sort(pts.begin(),pts.end(),[](const Point &a, const Point &b) { return a.m_x < b.m_x; });

    std::set<SortedLine*> active;

    int end_ptr=0, q_ptr=0, start_ptr=0;
    for(int q_ptr=0; q_ptr < pts.size(); q_ptr++){
        float c_x  = pts[q_ptr].m_x;

        while(start_ptr < m_starts.size() && m_starts[start_ptr]->m_p1.m_x < c_x + TOL){
            active.insert(m_starts[start_ptr]);
            start_ptr++;
        }
        
        for(auto a : active){
            if(a->distanceTo(pts[q_ptr]) < TOL){
            
            }
        }
        
        while(end_ptr < m_ends.size() && m_ends[end_ptr]->m_p2.m_x < c_x - TOL){
            active.erase(m_ends[end_ptr]);
            end_ptr++;
        }

    }    
}


} //namespace pschem
