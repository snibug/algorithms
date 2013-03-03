typedef int ValueType;

struct lcnode{
        struct lcnode *linkparent; // not preferred path
        ValueType value;
        struct lcnode *parent; // preferred path
        struct lcnode *child[2]; // preferred path
        int splaymarker; // upper node is 0, lower node is 1
        lcnode(const ValueType &_value) : linkparent(nullptr), value(_value), parent(nullptr) { child[0] = child[1] = nullptr; splaymarker = -1;}
};

namespace lctree {
        inline lcnode *cutpath(int idx, lcnode *par) {lcnode *child = par->child[idx]; if(child) child->parent = nullptr;par->child[idx] = nullptr; return child;}
        inline lcnode *connectpath(int idx, lcnode *par,lcnode *child) {par->child[idx]=child; if(child) child->parent=par; return child;}

        //약간은 내부구현 느낌
        lcnode *getsplayroot(lcnode *cur) {
                cur->splaymarker = -1;
                while(cur->parent != nullptr) {
                        lcnode *tmp = cur->parent;
                        tmp->splaymarker = ( (cur == tmp->child[0]) ? 0 : 1 );
                        cur = tmp;
                }
                return cur;
        }
        void splay(lcnode *cur) {
                lcnode *root = getsplayroot(cur);
                lcnode *linkparent = root->linkparent;
                lcnode *LR[2] = {nullptr,nullptr}, *bLR[2] = {nullptr,nullptr};
                while(true) {
                        if(root->splaymarker == -1) break;
                        int dir;
                        bool zigzig = 0;
                        lcnode *next;
                        dir = root->splaymarker;
                        next = root->child[dir];

                        if(next->splaymarker == root->splaymarker){
                                //zig-zig prepare
                                next = next->child[dir];
                                connectpath(dir,root,next->parent->child[dir^1]);
                                connectpath(dir^1,next->parent,root);
                        }
                        //zig
                        if(LR[dir^1] == nullptr) LR[dir^1] = bLR[dir^1] = next->parent;
                        else bLR[dir^1] = connectpath(dir,bLR[dir^1],next->parent);
                        cutpath(dir,next->parent);
                        root = next;
                }
                //reassemble tree
                for(int i = 0;i < 2;i ++){
                        if(LR[i] && root->child[i]) connectpath(i^1,bLR[i],root->child[i]);
                        if(LR[i]) connectpath(i,root,LR[i]);
                }
                root->linkparent = linkparent;
        }
        lcnode *getsplaymin(lcnode *cur) {
                splay(cur);
                while(cur->child[0] != nullptr)
                        cur = cur->child[0];
                splay(cur);
                return cur;
        }

        void access(lcnode *cur){
                lcnode *org = cur;
                splay(cur);
                while(true){
                        lcnode *next = cur->linkparent;
                        if(next == nullptr) break;
                        splay(next);
                        // 기존prefer정리
                        if(next->child[1] != nullptr) next->child[1]->linkparent = next;
                        cutpath(1,next);
                        // new prefer edge
                        cur->linkparent = nullptr;
                        connectpath(1,next,cur);
                        cur = next;
                }
                splay(org);
        }

        //반드시 노출할 것들.
        lcnode *findroot(lcnode *cur) {
                access(cur);
                return getsplaymin(cur);
        }

        void cut(lcnode *child) {
                access(child);
                cutpath(0,child);
        }

        void connect(lcnode *par, lcnode *cur) {
                access(par);
                access(cur);
                cutpath(0,cur);
                cur->child[0] = par;
        }
};

