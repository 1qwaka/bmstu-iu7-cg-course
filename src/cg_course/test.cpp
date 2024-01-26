#include <iostream>

struct Object;

struct Visitor
{
    virtual void accept(Object *o) {
        std::cout << "accept visitor" << std::endl;
    }
};
struct DrawVisitor : Visitor
{
    void accept(Object *o) {
        std::cout << "accept draw visitor" << std::endl;
    }
};


struct Object
{
    virtual void accept(Visitor *v) {
        std::cout << "accept object" << std::endl;
        v->accept(this);
    }
};

struct Sphere : public Object
{
    virtual void accept(Visitor *v) {
        std::cout << "accept sphere" << std::endl;
        v->accept(this);
    }
};
struct Lens : public Object
{
    virtual void accept(Visitor *v) {
        std::cout << "accept lens" << std::endl;
        v->accept(this);
    }

};

int main()
{
    Object o;
    Sphere s;
    Lens l;
    Visitor v;
    DrawVisitor dv;

    o.accept(&dv);
    s.accept(&v);
    l.accept(&dv);
    // dv.accept(&o);

}