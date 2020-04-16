#include <vector>
using namespace std;
const int INF = 0x3c3c3c3c;
const long long INFL = 0x3c3c3c3c3c3c3c3c;

struct Dinic{
    struct edge{
        int to, cap, rev;
    };
    int size, src, sink;
    vector<vector<edge> > G;
    vector<int> level, iter;
    Dinic(){}
    Dinic(int size, int src, int sink) {
        this->size = size;
        this->src = src;
        this->sink = sink;
        G = vector<vector<edge> >(size);
        level = vector<int>(size, -1);
        iter = vector<int>(size, 0);
    }
    void addEdge(int from, int to, int cap){
        G[from].push_back({to, cap, (int)G[to].size()});
        G[to].push_back({from, 0, (int)G[from].size()-1});
    }
    bool bfs(int src, int sink){
        queue<int> q;
        q.push(src);
        level[src] = 0;
        while(!q.empty() && level[sink] == -1){
            int here = q.front();
            q.pop();
            for(auto e : G[here]){
                if(e.cap <= 0 || level[e.to] != -1) continue;
                level[e.to] = level[here] + 1;
                q.push(e.to);
            }
        }
        return level[sink] != -1;
    }
 
    int dfs(int here, int minFlow, int sink){
        if(here == sink) return minFlow;
        for(int& i = iter[here]; i < (int)G[here].size(); i++){
            auto& e = G[here][i];
            if(e.cap == 0 || level[e.to] != level[here] + 1) continue;
            int f = dfs(e.to, min(minFlow, e.cap), sink);
            if(f > 0){
                e.cap -= f;
                G[e.to][e.rev].cap += f;
                return f;
            }
        }
        return 0;
    }
    
    int getMaxflow(){
        int ret = 0;
        while(1){
            level = vector<int>(size, -1);
            iter = vector<int>(size, 0);
            if(!bfs(src, sink)) break;
            while(int f = dfs(src, INF, sink)) ret += f;
        }
        return ret;
    }
};
 
struct LRMaxFlow{
    Dinic dinic;
    int size, src, sink, fsrc, fsink;
    vector<int> inSum, outSum;
    LRMaxFlow(int size, int src, int sink){
        this->size = size;
        this->src = src;
        this->sink = sink;
        fsrc = size;
        fsink = size + 1;
        dinic = Dinic(size + 2, fsrc, fsink);
        inSum = vector<int>(size, 0);
        outSum = vector<int>(size, 0);
    }
    void addEdge(int from, int to, int upper, int lower){
        dinic.addEdge(from, to, upper - lower);
        inSum[to] += lower;
        outSum[from] += lower;
    }
    int getMaxflow(){
        for(int i = 0 ; i < size; i++) if(inSum[i]) dinic.addEdge(fsrc, i, inSum[i]);
        for(int i = 0 ; i < size; i++) if(outSum[i]) dinic.addEdge(i, fsink, outSum[i]);
        dinic.addEdge(sink, src, INF);
        return dinic.getMaxflow();
    }
};