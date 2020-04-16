#include <vector>

class BiMatch{
public:
	int n, m;
	vector<vector<int>> adj;
	vector<int> parent;
	vector<bool> visited;

	BiMatch(int n, int m){
		this->n = n;
		this->m = m;
		adj.resize(n);
		parent.resize(m, -1);
		visited.resize(n);
	}

	void addedge(int u, int v){
		adj[u].push_back(v);
	}

	bool dfs(int now){
		if(visited[now]) return false;
		visited[now] = true;
		for(int nxt : adj[now]){
			if(parent[nxt] == -1 || dfs(parent[nxt])){
				parent[nxt] = now;
				return true;
			}
		}
		return false;
	}

	int solve(){
		int ret = 0;
		for(int i = 0; i < n; i++){
			fill(visited.begin(), visited.end(), false);
			if (dfs(i)) ret += 1;
		}
		return ret;
	}
};

class HopcroftKarp{
public:
	int n;
	vector<int> b;
	vector<int> level;
	vector<bool> trip;
	vector<vector<int>> &adj;
	HopcroftKarp(vector<vector<int>> &adj):adj(adj){
		n = adj.size();
		b = vector<int>(n);
		level = vector<int>(n);
		trip = vector<bool>(n);
	}
	void bfs(){
		fill(level.begin(), level.end(), -1);
		queue<int> q;
		for(int i = 0; i < n; i++){
			if(!trip[i]){
				q.push(i);
				level[i]=0;
			}
		}
		while(!q.empty()){
			int now = q.front();
			q.pop();
			for(int i = 0; i < adj[now].size(); i++){
				int there = adj[now][i];
				if(b[there]!=-1 && level[b[there]] == -1){
					level[b[there]] = level[now] + 1;
					q.push(b[there]);
				}
			}
		}
	}
	bool dfs(int now){
		for(int i = 0; i < adj[now].size(); i++){
			int there = adj[now][i];
			if(b[there] == -1 || (level[b[there]] == level[now] + 1 && dfs(b[there]))){
				trip[now] = true;
				b[there] = now;
				return true;
			}
		}
		return false;
	}
	int flow(){
		fill(trip.begin(), trip.end(), false);
		fill(b.begin(), b.end(), -1);
		int total_flow = 0;
		while(true){
			bfs();
			int flow = 0;
			for(int i = 0; i < n; i++){
				if(!trip[i] && dfs(i))
					flow++;
			}
			if(flow == 0)
				break;
			total_flow += flow;
		}
		return total_flow;
	}
};