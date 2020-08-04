#ifndef __UIBASE_H__
#define __UIBASE_H__
#define    DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);                \
    TypeName& operator=(const TypeName&)
namespace nglui{

typedef struct Point{
    int x;
    int y;
    void set(int x,int y);
}POINT;

typedef struct Rectangle{
    int x;
    int y;
    int width;
    int height;
    Rectangle();
    Rectangle(int,int,int,int);
    int bottom()const{return y+height;}
    int right()const{return x+width;}
    void set(int x_,int y_,int w,int h);
    void inflate(int dx,int dy);
    bool empty()const;
    void offset(int dx,int dy);
    bool intersect(const Rectangle&a,const Rectangle&b);
    bool intersect(const Rectangle&b);
    bool intersect(int x_,int y_);
}RECT;

typedef struct Size{
    int x;
    int y;
    void set(int x_,int y_){x=x_;y=y_;}
    int width(){return x;}
    int height(){return y;}
}SIZE;
}
#endif

