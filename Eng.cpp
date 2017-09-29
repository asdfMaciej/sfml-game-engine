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
class Object;
class Stepper;
class Renderer;
class EventHandler;

class Object {
    public:
        Object(Renderer& _renderer, Depth depth, string filename, int x, int y);
        Object();
        void render(sf::RenderWindow* win);
        void change_depth(Depth depth);
        void begin_step(Stepper& stepper);
        void step(Stepper& stepper);
        void end_step(Stepper& stepper);
        void on_click_lmb(sf::Event ev);
        void on_release_lmb(sf::Event ev);
        bool contains_xy(int x, int y);
        int id;
        int stepper_id;
        Depth depth;
        bool clickable;
    protected:
        Renderer *renderer;
        sf::Sprite *sprite;
        sf::Texture *texture;
        int rid;
    private:
        virtual void user_on_click_lmb(sf::Event ev) = 0;
        virtual void user_on_release_lmb(sf::Event ev) = 0;
        virtual void user_render(sf::RenderWindow* win) = 0;
        virtual void user_step(int step_t) = 0;
};

class Renderer {
    public:
        void init(sf::RenderWindow* win);
        void render();
        pair<int, Object*> add_item(Object* obj, Depth depth);
        void delete_item(int id);
        Object* get_item(int id);
        Object* find_item(int x, int y);
        sf::RenderWindow* get_window();
    protected:
        vector<vector<Object*>> q;
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

class EventHandler {
    public:
        EventHandler(Stepper& step, Renderer& render);
        void handle_events(sf::Event event);
    protected:
        Stepper* stepper;
        Renderer* renderer;
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
    cout << "usun";
    q[d].erase(q[d].begin() + n);
}
int Stepper::add_object(Object* obj, Step step) {
    int int_s = iStep[step];
    int id = int_s*10000+q[int_s].size();
    q[int_s].push_back(obj);
    obj->stepper_id = id;
    return id;
}
void Stepper::begin_step() { _step(Step::BEGIN); }
void Stepper::step() { _step(Step::STEP); }
void Stepper::end_step() { _step(Step::END); }

void Renderer::init(sf::RenderWindow* win) {
    window = win;
    for (int i=0; i<5; i++) {
        vector<Object*> t;
        q.push_back(t);
    }
    iDepth[Depth::D1] = 0;
    iDepth[Depth::D2] = 1;
    iDepth[Depth::D3] = 2;
    iDepth[Depth::D4] = 3;
    iDepth[Depth::D5] = 4;
}
pair<int, Object*> Renderer::add_item(Object* obj, Depth depth) {
    int int_d = iDepth[depth];
    obj->depth = depth;
    obj->id = int_d*10000+q[int_d].size();
    q[int_d].push_back(obj);
    pair<int, Object*> ret;
    ret = make_pair(obj->id, get_item(obj->id));
    return ret;
}

Object* Renderer::get_item(int id) {
    int d = id / 10000;
    int n = id - (d * 10000);
    return q[d][n];
}

void Renderer::delete_item(int id) {
    int d = id / 10000;
    int n = id - (d * 10000);
    q[d].erase(q[d].begin() + n);
}

Object* Renderer::find_item(int x, int y) {
    bool found = false;
    Object* ret;
    for (int i=0; i<5; i++) {
        for (Object* obj: q[i]) {
            if (obj->contains_xy(x, y)) {
                found = true;
                ret = obj;
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
            obj->render(window);
        }
    }
}

sf::RenderWindow* Renderer::get_window() {
    return window;
}

bool Object::contains_xy(int x, int y) {
    if (!clickable) {
        return false;
    }
    bool does = this->sprite->getGlobalBounds().contains(x, y);
    if (does) {
        int newx = x - sprite->getPosition().x;
        int newy = y - sprite->getPosition().y;
        sf::Color col = sprite->getTexture()->copyToImage().getPixel(newx, newy);  // SEGFAULT
        if (((int) col.a) == 0) {
            does = false;
        }
    }
    return does;
}

Object::Object() {}
Object::Object(Renderer& _renderer, Depth depth, string filename, int x, int y) {
    renderer = &_renderer;
    rid = renderer->add_item(this, depth).first;
    texture = new sf::Texture();
    texture->loadFromFile(filename);
    sprite = new sf::Sprite();
    sprite->setTexture(*texture);
    sprite->setPosition(x, y);
    clickable = true;
}
void Object::render(sf::RenderWindow* win) {
    user_render(win);
}
void Object::change_depth(Depth depth) {
    renderer->delete_item(rid);
    pair<int, Object*> ret = renderer->add_item(this, depth);
    delete &rid;
    rid = ret.first;
}

void Object::begin_step(Stepper& stepper) {
    user_step(0);
}
void Object::step(Stepper& stepper) {
    this->user_step(1);
}
void Object::end_step(Stepper& stepper) {
    user_step(2);
}
void Object::on_click_lmb(sf::Event ev) {
    user_on_click_lmb(ev);
}

void Object::on_release_lmb(sf::Event ev) {
    user_on_release_lmb(ev);
}

EventHandler::EventHandler(Stepper& step, Renderer& render) {
    stepper = &step;
    renderer = &render;
}
void EventHandler::handle_events(sf::Event event) {
    if (event.type == sf::Event::Closed)
        renderer->get_window()->close();

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            Object* ob = renderer->find_item(event.mouseButton.x, event.mouseButton.y);
            if (ob != nullptr) {
                ob->on_click_lmb(event);
            }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            Object* ob = renderer->find_item(event.mouseButton.x, event.mouseButton.y);
            if (ob != nullptr) {
                ob->on_release_lmb(event);
            }
        }
    }
}
