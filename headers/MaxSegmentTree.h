/*

title: MaxSegmentTree.h
author: 윤상건 (Sang-geon Yun, ggj06281@kookmin.ac.kr)

*/
#include <vector>
using namespace std;

template<class T>
class MaxSegment{
private:
    vector<T> seg;
    int leftChild(int idx){
        return (idx + 1) * 2 - 1;
    }
    int rightChild(int idx){
        return (idx + 1) * 2;
    }
    bool isRange(int s, int e, int ns, int ne){
        if(ns > e || ne < s)
            return false;
        return true;
    }
    T max(T a, T b){
        if(a > b) return a;
        return b;
    }
public:
    MaxSegment(int size){
        seg.resize(size * 4);
    }
    void setValue(int idx, int s, int ns, int ne, T v){
        if(s < ns || s > ne)
            return;
        if(ns == ne){
            seg[idx] = v;
            return;
        }
        int mid = (ns + ne) >> 1;
        setValue(leftChild(idx), s, ns, mid, v);
        setValue(rightChild(idx), s, mid + 1, ne, v);
        seg[idx] = max(seg[leftChild(idx)], seg[rightChild(idx)]);
    }
    void update(int idx, int s, int ns, int ne, T v){
        if(s < ns || s > ne)
            return;
        if(ns == ne){
            seg[idx] = seg[idx] + v;
            return;
        }
        int mid = (ns + ne) >> 1;
        update(leftChild(idx), s, ns, mid, v);
        update(rightChild(idx), s, mid + 1, ne, v);
        seg[idx] = max(seg[leftChild(idx)], seg[rightChild(idx)]);
    }
    T query(int idx, int s, int e, int ns, int ne){
        if(s <= ns && ne <= e)
            return seg[idx];
        int mid = (ns + ne) >> 1;
        T ret;
        bool ch = false;
        if(isRange(s, e, ns, mid)){
            ret = query(leftChild(idx), s, e, ns, mid);
            ch = true;
        }
        if(isRange(s, e, mid + 1, ne)){
            T t = query(rightChild(idx), s, e, mid + 1, ne);
            if(ch) ret = max(ret, t);
            else ret = t;
        }
        return ret;
    }
};