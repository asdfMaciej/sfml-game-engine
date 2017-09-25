#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <map>
using namespace std;

enum class Depth {
    D1,
    D2,
    D3,
    D4,
    D5
};
enum class Step {
    BEGIN,
    STEP,
    END
};
struct RenderObject;
class Object;
class Stepper;
class Renderer;

struct RenderObject {
    int id = -1;
    Object *object;
    Depth depth;
};

class Object {
    public:
        Object(sf::Sprite& spr, Renderer& _renderer, Depth depth);
        void render(sf::RenderWindow* win);
        void change_depth(Depth depth);
        void begin_step(Stepper& stepper);
        void step(Stepper& stepper);
        void end_step(Stepper& stepper);
        void on_click_lmb(sf::Event ev);
        bool contains_xy(int x, int y);
    protected:
        Renderer *renderer;
        RenderObject *robj;
        sf::Sprite *sprite;
        int rid;
};

class Renderer {
    public:
        void init(sf::RenderWindow* win);
        void render();
        pair<int, RenderObject*> add_item(Object* obj, Depth depth);
        void delete_item(int id);
        RenderObject* get_item(int id);
        RenderObject* find_item(int x, int y);
    protected:
        vector<vector<RenderObject>> q;
        map<Depth, int> iDepth;
        sf::RenderWindow* window;
};

class Stepper {
    public:
        Stepper();
        void begin_step();
        void step();
        void end_step();
        int add_object(Object* obj, Step step);
        void remove_object(int id);
    protected:
        void _step(Step s);
        map<Step, int> iStep;
        vector<vector<Object*>> q;
};

Stepper::Stepper() {
    for (int i=0; i<3; i++) {
        vector<Object*> v;
        q.push_back(v);
    }
    iStep[Step::BEGIN] = 0;
    iStep[Step::STEP] = 1;
    iStep[Step::END] = 2;
}

void Stepper::_step(Step s) {
    int i = iStep[s];
    for (Object* obj: q[i]) {
        if (i == 0) {
            obj->begin_step(*this);
        } else if (i == 1) {
            obj->step(*this);
        } else if (i == 2) {
            obj->end_step(*this);
        }
    }
}

void Stepper::remove_object(int id) {
    int d = id / 10000;
    int n = id - (d * 10000);
    q[d].erase(q[d].begin() + n);
}
int Stepper::add_object(Object* obj, Step step) {
    int int_s = iStep[step];
    int id = int_s*10000+q[int_s].size();
    q[int_s].push_back(obj);
    return id;
}
void Stepper::begin_step() { _step(Step::BEGIN); }
void Stepper::step() { _step(Step::STEP); }
void Stepper::end_step() { _step(Step::END); }

void Renderer::init(sf::RenderWindow* win) {
    window = win;
    for (int i=0; i<5; i++) {
        vector<RenderObject> t;
        q.push_back(t);
    }
    iDepth[Depth::D1] = 0;
    iDepth[Depth::D2] = 1;
    iDepth[Depth::D3] = 2;
    iDepth[Depth::D4] = 3;
    iDepth[Depth::D5] = 4;
}
pair<int, RenderObject*> Renderer::add_item(Object* obj, Depth depth) {
    int int_d = iDepth[depth];
    RenderObject new_r;
    new_r.depth = depth;
    new_r.object = obj;
    new_r.id = int_d*10000+q[int_d].size();
    q[int_d].push_back(new_r);
    pair<int, RenderObject*> ret;
    ret = make_pair(new_r.id, get_item(new_r.id));
    return ret;
}

RenderObject* Renderer::get_item(int id) {
    int d = id / 10000;
    int n = id - (d * 10000);
    return &q[d][n];
}

void Renderer::delete_item(int id) {
    int d = id / 10000;
    int n = id - (d * 10000);
    q[d].erase(q[d].begin() + n);
}

RenderObject* Renderer::find_item(int x, int y) {
    bool found = false;
    RenderObject* ret;
    for (int i=0; i<5; i++) {
        for (RenderObject obj: q[i]) {
            if (obj.object->contains_xy(x, y)) {
                found = true;
                ret = &obj;
                break;
            }
        }
        if (found) {break;}
    }
    if (found) {
        return ret;
    } else {
        return nullptr;
    }
}
void Renderer::render() {
    for (int i=4; i>=0; i--) {
        for(auto const& obj: q[i]) {
            obj.object->render(window);
        }
    }
}

bool Object::contains_xy(int x, int y) {
    return sprite->getGlobalBounds().contains(x, y);
}

Object::Object(sf::Sprite& spr, Renderer& _renderer, Depth depth) {
    renderer = &_renderer;
    pair<int, RenderObject*> ret = renderer->add_item(this, depth);
    rid = ret.first;
    robj = ret.second;
    sprite = &spr;
}
void Object::render(sf::RenderWindow* win) {
    win->draw(*sprite);
}
void Object::change_depth(Depth depth) {
    renderer->delete_item(rid);
    pair<int, RenderObject*> ret = renderer->add_item(this, depth);
    delete &rid;
    delete &robj;
    rid = ret.first;
    robj = ret.second;
}

void Object::begin_step(Stepper& stepper) {}
void Object::step(Stepper& stepper) {
    sprite->rotate(2);

}
void Object::end_step(Stepper& stepper) {}
void Object::on_click_lmb(sf::Event ev) {
    cout << rid << endl;
}
int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!");
    sf::Texture kolko;
    sf::Texture krzyzyk;
    kolko.loadFromFile("kot1.jpg");
    krzyzyk.loadFromFile("kot4.jpg");
    sf::Sprite sKolko, sKrzyzyk;
    sKolko.setTexture(kolko);
    sKrzyzyk.setTexture(krzyzyk);
    sKolko.setPosition(200, 200);
    sKrzyzyk.setPosition(350, 350);
    window.setFramerateLimit(60);
    Renderer r;
    Stepper s;
    r.init(&window);
    Object obj1(sKolko, r, Depth::D1);
    Object obj2(sKrzyzyk, r, Depth::D2);
    s.add_object(&obj1, Step::STEP);
    s.add_object(&obj2, Step::STEP);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    RenderObject* ob = r.find_item(event.mouseButton.x, event.mouseButton.y);
                    if (ob != nullptr) {
                        ob->object->on_click_lmb(event);
                    }
                }
            }
        }

        window.clear();
        //window.draw(shape);
        s.step();
        r.render();
        window.display();
    }

    return 0;
}
