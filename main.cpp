#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <math.h>
#include "Eng.cpp"

using namespace std;
class Game;
class ParticleHolder;

class Kremowka: public Object {
    public:
        Kremowka(Renderer& _renderer, Depth depth, string filename, int x, int y, Game* game, ParticleHolder* ph) : Object(_renderer, depth, filename, x, y) {
            this->game = game;
            txt_p.loadFromFile("img/kremowka_p.png");
            spr_p.setTexture(txt_p);
            spr_up = *sprite;
            this->ph = ph;
        }
        void user_on_click_lmb(sf::Event ev);
        void user_on_release_lmb(sf::Event ev);
        void user_render(sf::RenderWindow* win);
        void user_step(int step_t);
    protected:
        Game* game;
        ParticleHolder* ph;
        sf::Texture txt_p;
        sf::Sprite spr_p;
        sf::Sprite spr_up;
};

class Servant: public Object {
    public:
        Servant(Renderer& _renderer, Depth depth, string filename, int x, int y, Game* game, int servant_id, sf::Font* font) : Object(_renderer, depth, filename, x, y) {
            this->game = game;
            filename.erase(filename.length()-4);
            txt_p.loadFromFile(filename+"_p.jpg");
            spr_p.setTexture(txt_p);
            spr_up = *sprite;
            spr_p.setPosition(x, y);
            txt_unaffordable.loadFromFile("img/unaffordable.jpg");
            spr_unaffordable.setTexture(txt_unaffordable);
            spr_unaffordable.setPosition(x, y);
            this->servant_id = servant_id;
            sf::Color color(33, 87, 146);
            fnt = font;
            txt.setFont(*font);
            txt.setString("INIT");
            txt.setCharacterSize(14);
            txt.setPosition(x+176, y+30);
            txt.setColor(color);
            koszt.setFont(*font);
            koszt.setString("KOSZTINIT");
            koszt.setCharacterSize(14);
            koszt.setPosition(x+176, y+48);
            koszt.setColor(color);
            kps.setFont(*font);
            kps.setString("INIT");
            kps.setCharacterSize(14);
            kps.setPosition(x+320, y+30);
            kps.setColor(color);
            clicked = false;
        }
        void user_on_click_lmb(sf::Event ev);
        void user_on_release_lmb(sf::Event ev);
        void user_render(sf::RenderWindow* win);
        void user_step(int step_t);
        int servant_id;
        sf::Text txt;
        sf::Text koszt;
        sf::Text kps;
    protected:
        Game* game;
        sf::Font* fnt;
        sf::Texture txt_p;
        sf::Texture txt_unaffordable;
        sf::Sprite spr_p;
        sf::Sprite spr_up;
        sf::Sprite spr_unaffordable;
        bool clicked;
        bool affordable;
};

class sObject: public Object {
    public:
        sObject(Renderer& _renderer, Depth depth, string filename, int x, int y) : Object(_renderer, depth, filename, x, y) {clickable = false;}
        void user_on_click_lmb(sf::Event ev);
        void user_on_release_lmb(sf::Event ev);
        void user_render(sf::RenderWindow* win);
        void user_step(int step_t);
};

class sText: public Object {
    public:
        sText(Renderer& _renderer, Depth depth, string text, sf::Font* font, int size, int x, int y, Game* game, int t_type) : Object(_renderer, depth, "", x, y) {
            clickable = false;
            fnt = font;
            sf::Color color(33, 33, 33);
            txt.setFont(*font);
            txt.setString(text);
            txt.setCharacterSize(size);
            txt.setPosition(x, y);
            txt.setColor(color);
            this->game = game;
            this->t_type = t_type;
        }
        void user_on_click_lmb(sf::Event ev);
        void user_on_release_lmb(sf::Event ev);
        void user_render(sf::RenderWindow* win);
        void user_step(int step_t);
        sf::Text txt;
    protected:
        int t_type;
        sf::Font* fnt;
        Game* game;
};

struct Particle {
    sf::Sprite* spr;
    int x;
    int y;
    float h_speed;
    float v_speed;
    int lifetime;
    int vector_id;
    bool dead;
};
class ParticleHolder {
    public:
        ParticleHolder() {
            air_friction = 0.02;
            gravity = 0.35;
            b_lifetime = 120;
            texture.loadFromFile("img/m_kremowka.jpg");
        }
        void step() {
            for (Particle* part: particles_l) {
                if (part == nullptr) {
                    continue;
                    cout << "stepping on nullptr" << endl;
                }
                if (part->dead) {
                    continue;
                    cout << "stepping on dead" << endl;
                }
                if (part->lifetime == 0) {
                    part->dead = true;
                    delete part->spr;
                    particles_l[part->vector_id] = nullptr;
                    continue;
                }
                part->x += part->h_speed;
                part->y += part->v_speed;
                bool hsp_pos = part->h_speed > 0;
                if (0 < part->h_speed) { part->h_speed -= this->air_friction; }
                else if (0 > part->h_speed) { part->h_speed += this->air_friction; }
                if ((hsp_pos && (part->h_speed < 0)) || (!hsp_pos && (part->h_speed > 0))) {
                    part->h_speed = 0;
                }
                part->v_speed += this->gravity;
                part->spr->setPosition(part->x, part->y);

                part->lifetime--;
            }
        }
        void render(sf::RenderWindow* win) {
            for (Particle* part: particles_l) {
                if (part != nullptr) {
                    if (!part->dead) {
                        win->draw(*part->spr);
                    }
                }
            }
        }
        void add(int x, int y) {
            Particle* p = new Particle;
            p->spr = new sf::Sprite();
            p->spr->setTexture(this->texture);
            p->x = x;
            p->y = y;
            p->h_speed = 3;
            p->v_speed = -4;
            p->lifetime = 120;
            p->dead = false;
            p->vector_id = particles_l.size()-1;
            particles_l.push_back(p);
        }
    private:
        float air_friction, gravity;
        int b_lifetime;
        vector<Particle*> particles_l;
        sf::Texture texture;
};

class Game {
    public:
        Game();
        float kremowki;
        float kpc;
        vector<int> servants;
        float kps;
        string k_to_string();
        string kps_to_string();
        string get_servant_count(int servant_id);
        string get_servant_kps(int servant_id);
        pair<string, string> n_jednostki(float n, bool pelne, int precision);  // prefix, n
        void k_press();
        void second_loop();
        bool buy_servant(int serv_id);
        void recalculate_kps();
        void calculate_cost();
        float get_cost(int id, int possesed);
        float s_get_cost(int id);
        int fps;
    private:
        vector<string> mianowniki;
        vector<string> mianowniki_m;
        vector<float> base_kps;
        vector<float> base_cost;
        vector<float> cost;
};

Game::Game() {
    fps = 60;
    kremowki = 0;
    kpc = 1000000;  // kremowki per click
    kps = 0;  // kremowki per second
    for (int n=0; n<7; n++) {
        servants.push_back(0);
    }
    mianowniki.push_back("Koscielny");
    mianowniki_m.push_back("Koscielni");
    mianowniki.push_back("Proboszcz");
    mianowniki_m.push_back("Proboszcze");
    mianowniki.push_back("Kardynal");
    mianowniki_m.push_back("Kardynalowie");
    mianowniki.push_back("Papiez");
    mianowniki_m.push_back("Papieze");
    mianowniki.push_back("Arcypapiez");
    mianowniki_m.push_back("Arcypapieze");
    mianowniki.push_back("Testoviron");
    mianowniki_m.push_back("Testovironowie");
    mianowniki.push_back("Bestia");
    mianowniki_m.push_back("Bestie");

    base_kps.push_back(0.1);
    base_kps.push_back(1);
    base_kps.push_back(8);
    base_kps.push_back(47);
    base_kps.push_back(260);
    base_kps.push_back(1400);
    base_kps.push_back(7800);

    base_cost.push_back(15);
    base_cost.push_back(100);
    base_cost.push_back(1100);
    base_cost.push_back(12000);
    base_cost.push_back(130000);
    base_cost.push_back(1400000);
    base_cost.push_back(20000000);

    calculate_cost();
    recalculate_kps();
}

float Game::s_get_cost(int id) {
    return cost[id];
}
void Game::k_press() {
    kremowki += kpc;
}
void Game::second_loop() {
    kremowki += kps;
}
bool Game::buy_servant(int serv_id) {
    bool bought = false;
    float c = get_cost(serv_id, servants[serv_id]);
    if (kremowki >= c) {
        kremowki -= c;
        servants[serv_id] += 1;
        bought = true;
    }
    calculate_cost();
    recalculate_kps();
    return bought;
}
void Game::recalculate_kps() {
    kps = 0;
    for (int n=0; n<7; n++) {
        kps += servants[n] * base_kps[n];
    }
}
void Game::calculate_cost() {
    cost.clear();
    for (int n=0; n<7; n++) {
        cost.push_back(get_cost(n, servants[n]));
    }
}
float Game::get_cost(int id, int possesed) {
    float c = base_cost[id] * pow(1.15, possesed);
    return c;
}
string Game::k_to_string() {
    string odmiana = "";
    string ret = "";
    if (kremowki == 0) {
        odmiana = "Kremowek";
    } else if (kremowki == 1) {
        odmiana = "Kremowka";
    } else if (kremowki > 1 && kremowki < 5) {
        odmiana = "Kremowki";
    } else {
        odmiana = "Kremowek";
    }
    pair<string, string> pr = n_jednostki(kremowki, true, 6);
    ret = pr.second + " " + pr.first + odmiana;
    return ret;
}
pair<string, string> Game::n_jednostki(float m, bool pelne, int precision) {
    pelne = !pelne;
    float _n = m;
    string prefix = "";
    if (_n >= pow(10, 24)) {
        prefix = pelne ? "Y" : "Jotta"; _n=_n/pow(10, 24);
    } else if (_n >= pow(10, 21)) {prefix = pelne ? "Z": "Zetta"; _n=_n/pow(10, 21);}
      else if (_n >= pow(10, 18)) {prefix = pelne ? "E": "Eksa"; _n=_n/pow(10, 18);}
      else if (_n >= pow(10, 15)) {prefix = pelne ? "P": "Peta"; _n=_n/pow(10, 15);}
      else if (_n >= pow(10, 12)) {prefix = pelne ? "T": "Tera"; _n=_n/pow(10, 12);}
      else if (_n >= pow(10, 9)) {prefix = pelne ? "G": "Giga"; _n=_n/pow(10, 9);}
      else if (_n >= pow(10, 6)) {prefix = pelne ? "M": "Mega"; _n=_n/pow(10, 6);}
      else if (_n >= pow(10, 3)) {prefix = pelne ? "k": "Kilo"; _n=_n/pow(10, 3);}
    stringstream ss;
    ss << std::setprecision(6) << _n;
    pair<string, string> pairss;
    pairss = make_pair(prefix, ss.str());
    return pairss;

}
string Game::kps_to_string() {
    pair<string, string> tkst = n_jednostki(kps, true, 4);
    string ret = tkst.second + " " + tkst.first + "Kremowki/s";
    return ret;
}
string Game::get_servant_count(int servant_id) {
    stringstream ss;
    ss << mianowniki_m[servant_id] << ": " << servants[servant_id];
    return ss.str();
}
string Game::get_servant_kps(int servant_id) {
    float _kps;
    _kps = servants[servant_id] * base_kps[servant_id];
    pair<string, string> tkst = n_jednostki(_kps, false, 5);
    return tkst.second + " " + tkst.first + "Kr/s";
}

void sText::user_render(sf::RenderWindow* win) {win->draw(txt);}
void sText::user_on_click_lmb(sf::Event ev) {}
void sText::user_step(int step_t) {
    if (step_t == 2) {  // end step
        if (t_type == 0) { // kremowki
            this->txt.setString(game->k_to_string());
        } else if (t_type == 1) {  // kremowki per second
            this->txt.setString(game->kps_to_string());
        }
    }
}
void sText::user_on_release_lmb(sf::Event ev) {}
void sObject::user_on_click_lmb(sf::Event ev) {}
void sObject::user_render(sf::RenderWindow* win) {win->draw(*sprite);}
void sObject::user_step(int step_t) {}
void sObject::user_on_release_lmb(sf::Event ev) {}
void Kremowka::user_render(sf::RenderWindow* win) {win->draw(*sprite);}
void Kremowka::user_on_click_lmb(sf::Event ev) {
    cout << "KREMOWECZKA!" << endl;
    this->sprite = &this->spr_p;
    game->k_press();

    //sParticle mini();
    ph->add(ev.mouseButton.x, ev.mouseButton.y);
}
void Kremowka::user_on_release_lmb(sf::Event ev) {
    this->sprite = &this->spr_up;
}
void Kremowka::user_step(int step_t) {
}

void Servant::user_render(sf::RenderWindow* win) {
    win->draw(*sprite);
    if (!clicked) {
        win->draw(txt);
        win->draw(koszt);
        win->draw(kps);
    }
}
void Servant::user_step(int step_t) {
    if (step_t == 2) {  // end step
        this->txt.setString(game->get_servant_count(this->servant_id));
        float kst =game->s_get_cost(this->servant_id);
        pair<string, string> kk = game->n_jednostki(kst, false, 5);
        this->koszt.setString("Koszt: "+kk.second+" "+kk.first+"Kr");
        this->kps.setString(game->get_servant_kps(this->servant_id));
    }
}
void Servant::user_on_click_lmb(sf::Event ev) {
    bool bought = game->buy_servant(this->servant_id);
    this->sprite = bought ? &this->spr_p : &this->spr_unaffordable;
    clicked = true;
}
void Servant::user_on_release_lmb(sf::Event ev) {
    this->sprite = &this->spr_up;
    clicked = false;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!");
    window.setFramerateLimit(60);
    Renderer r;
    Stepper s;
    EventHandler ev(s, r);
    r.init(&window);
    sf::Font font;
    Game game;
    ParticleHolder ph;
    font.loadFromFile("impact.ttf");
    sObject obj1(r, Depth::D5, "img/background.jpg", 0, 0);
    sObject obj3(r, Depth::D4, "img/shadow.png", 0, 0);
    Kremowka obj2(r, Depth::D3, "img/kremowka.png", 0, 0, &game, &ph);
    sText txtKremowki(r, Depth::D3, "", &font, 48, 480, 8, &game, 0);
    sText txtKps(r, Depth::D3, "", &font, 12, 580, 72, &game, 1);
    sObject objSklep(r, Depth::D3, "img/sklep.png", 0, 0);
    Servant objKoscielny(r, Depth::D3, "img/koscielny.jpg", 853, 94, &game, 0, &font);
    Servant objProboszcz(r, Depth::D3, "img/proboszcz.jpg", 853, 170, &game, 1, &font);
    Servant objKardynal(r, Depth::D3, "img/kardynal.jpg", 853, 246, &game, 2, &font);
    Servant objPapiez(r, Depth::D3, "img/papiez.jpg", 853, 322, &game, 3, &font);
    Servant objArcypapiez(r, Depth::D3, "img/arcypapiez.jpg", 853, 398, &game, 4, &font);
    Servant objTestoviron(r, Depth::D3, "img/testoviron.jpg", 853, 474, &game, 5, &font);
    Servant objBestia(r, Depth::D3, "img/bestia.jpg", 853, 550, &game, 6, &font);
    s.add_object(&txtKremowki, Step::END);
    s.add_object(&txtKps, Step::END);
    s.add_object(&objKoscielny, Step::END);
    s.add_object(&objProboszcz, Step::END);
    s.add_object(&objKardynal, Step::END);
    s.add_object(&objPapiez, Step::END);
    s.add_object(&objArcypapiez, Step::END);
    s.add_object(&objTestoviron, Step::END);
    s.add_object(&objBestia, Step::END);
    ph.add(300, 300);
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ev.handle_events(event);
        }
        window.clear();
        int s_elapsed = clock.getElapsedTime().asMilliseconds();
        if (s_elapsed >= 1000) {
            clock.restart();
            game.second_loop();
        }
        s.begin_step();
        s.step();
        s.end_step();
        ph.step();

        r.render();
        ph.render(&window);
        window.display();
    }

    return 0;
}
