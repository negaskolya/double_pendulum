#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
#define pi acos(-1)

using namespace std;

const int WIDTH = 800, HEIGHT = 600, TPS=60;
double SCALE = 120, g = -9.81;

class vec { //класс, описывающий положение системы в пространстве
public:
    double x, y, p, t;
    vec(double x, double y, double p, double t): x(x), y(y), p(p), t(t){}
    vec(): x(0), y(0), p(0), t(0){}
    vec operator + (const vec &s) {return vec(x+s.x, y+s.y, p+s.p, t+s.t);} //делаем замену операторов для класса vec
    vec operator * (double c) {return vec(c*x, c*y, c*p, c*t);}
    vec operator= (const vec &s) {
        x = s.x;
        y = s.y;
        p = s.p; //угол, между осью Y и палкой L
        t = s.t; //угол между осью Y и палкой l
        return *this;
    }
};

vec f (double a, double b, double c, double d, double e, vec v) { //уравнение Лагранжа
    return vec((e*sin(v.t)-2*b*sin(v.t-v.p)*(cos(v.t-v.p)*b/2/a*v.x*v.x+v.y*v.y)-b*d/2/a*sin(v.p)*cos(v.t-v.p))/(2*c-b*b*cos(v.t-v.p)*cos(v.t-v.p)/2/a), (d*sin(v.p)+2*b*sin(v.t-v.p)*(cos(v.t-v.p)*b/2/c*v.y*v.y+v.x*v.x)-b*e/2/c*sin(v.t)*cos(v.t-v.p))/(2*a-b*b*cos(v.t-v.p)*cos(v.t-v.p)/2/c), v.y, v.x);
}

vec RKstep (double a, double b, double c, double d, double e, vec start) { //метод Рунге-Кутты 4ого порядка
    vec k1, k2, k3, k4; 
    const double stepsize = 0.0005; //временной шаг
    k1 = f(a, b, c, d, e, start);
    k2 = f(a, b, c, d, e, start+k1*(0.5)*stepsize);
    k3 = f(a, b, c, d, e, start+k2*(0.5)*stepsize);
    k4 = f(a, b, c, d, e, start+k3*stepsize);
    return start+(k1+k2*2+k3*2+k4)*(double(1)/double(6))*stepsize;
}

void drawMaya(SDL_Renderer* renderer, vec data, int xpos, int ypos, double L, double l, double SCALE) {  //создание 2 палочек маятника
    SDL_RenderDrawLine(renderer, xpos, ypos, xpos+L*SCALE*sin(data.p), ypos+L*SCALE*cos(data.p));   // первая палочка (верхняя) - первые 2 координаты - начало, вторые две - конец
    SDL_RenderDrawLine(renderer, xpos+L*SCALE*sin(data.p), ypos+L*SCALE*cos(data.p), xpos+L*SCALE*sin(data.p)+l*SCALE*sin(data.t), ypos+L*SCALE*cos(data.p)+l*SCALE*cos(data.t)); //вторая палочка (нижняя)
}

class CApp {
private:
  bool Running;
  SDL_Surface* surface; //cтруктура содержащая набор используемых пикселей 
  SDL_Window* window; //структура которая содержит всю информацию о самом окне: размер, положение, полноэкранный режим, границы и т. д.
  SDL_Renderer* renderer; //рендеринговая структура
    vec data;
  double L, l, M, m, a, b, c, d, e;
public:
  CApp(double L, double l, double M, double m): L(L), l(l), M(M), m(m) {
    Running = true;
    window = NULL;
    renderer = NULL;
    a = M*L*L/6+m*L*L/2; //константы для уравнения Лагранжа
    b = m*l*L/4;
    c = m*l*l/6;
    d = (M/2+m)*g*L;
    e = m*g*l/2;
  }
    
    void setData(vec d) {
        data = d;
  }

    bool OnInit()
    {
        SDL_Init(SDL_INIT_EVERYTHING); //инициализирует библиотеку SDL и все её подсистемы
        window = SDL_CreateWindow("Mayatnic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0); //параметры окна
        renderer = SDL_CreateRenderer(window, -1, 0); //2d рендиринг для окна window
        // Проверяет что окно было успешно создано
        if (window == NULL) {
            // В этом случае окно не может быть создано
            std::cout << "Could not create window: " << SDL_GetError( ) << std::endl;
            return false;
        }
        return true;
    }

    void OnEvent(SDL_Event* Event) {
        if (Event->type == SDL_QUIT) Running = false; 
    }

    void OnLoop() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //устанавливает цвет используемый для операций рисования
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 148, 0, 29, SDL_ALPHA_OPAQUE);
        drawMaya(renderer, data, WIDTH/2, HEIGHT/2, L, l, SCALE);
        for (int i=1; i<10; i++) {
        data = RKstep(a,b,c,d,e,data);};
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/TPS);
    }
    

    void OnCleanup() {
        SDL_DestroyWindow(window); //закрывает окно
        SDL_Quit(); //отключает все инициализированные подсистемы
    }

    int OnExecute() {
    if(OnInit() == false) return -1;
    SDL_Event Event;
        while(Running) {
            while(SDL_PollEvent(&Event)) {
                OnEvent(&Event);
            }
            OnLoop();
        }
    OnCleanup();
    return 0;
  }
};

int main(int argv, char** args) {
    double L, l, M, m; //вводим константы маятника (массы и длины стержней), длина верхней палочки L, нижней l, аналогично массы
    L=1.5; l=1; m=1; M=1;
    CApp a(L, l, M, m); //создаём элемент класса CApp
    a.setData(vec(0, 0, -pi/2, -pi/3));
    a.OnExecute();
    return 0;
}
