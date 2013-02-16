// bcc에는 각 bcc마다 속해있는 정점의 목록이 들어감. 정점은 각 bcc 내에서 unique함.
// cut_vertex가 0이면 절점이 아니고, 0이 아니면 절점임
// 사실 bcc는 edge 목록 구현이 정석이고 속 편한데, vertex 목록인 이게 더 짧네?

//poj 3352
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

int n,m;
vector<int> gnext[1003];
int v[1003],vtime;
int up[1003];

vector<int> bcc[1003];
int bcc_cnt;
int cut_vertex[1003];

pair<int,int> mystack[1003];
int top;

void dfs(int nod, int par) {
    up[nod] = v[nod] = ++vtime;
    for(int i = 0; i < gnext[nod].size(); i++){
        int next = gnext[nod][i];
        if(v[next] == 0) {
            mystack[top++] = make_pair(nod,next);
            dfs(next, nod);
            if(up[next] >= v[nod]) {
                do {
                    bcc[bcc_cnt].push_back(mystack[--top].second);
                } while( mystack[top] != make_pair(nod,next));
                bcc[bcc_cnt ++].push_back(nod);
                cut_vertex[nod] ++;
            }
            up[nod] = min(up[nod], up[next]);
        } else if(next != par)
            up[nod] = min(up[nod], v[next]);
    }
    if(par == -1 && cut_vertex[nod] == 1)
        cut_vertex[nod] = 0;
}

int main(){
    scanf("%d%d",&n,&m);
    for(int i = 0; i< m;i ++){
        int a,b;
        scanf("%d%d",&a,&b);
        a--,b--;
        gnext[a].push_back(b);
        gnext[b].push_back(a);
    }
    dfs(0,-1);
    int ans = 0;
    for(int i = 0;i < bcc_cnt ;i++){
        int cnt = 0;
        for(int j = 0;j < bcc[i].size();j ++){
            if(cut_vertex[bcc[i][j]] > 0) {
                cnt ++;
            }
        }
        if(cnt == 1) {
            ans ++;
        }
    }
    printf("%d\n",(ans + 1) / 2);
    return 0;
}

