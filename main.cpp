#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#define pi acos(-1)


class vec { //класс, описывающий положение системы в пространстве
private:
    double x, y, p, t;
public:
    vec(double x, double y, double p, double t): x(x), y(y), p(p), t(t){}
    vec(): x(0), y(0), p(0), t(0){}
    vec operator + (const vec &s) {return vec(x+s.x, y+s.y, p+s.p, t+s.t);} // делаем замену операторов для класса vec
    vec operator * (double c) {return vec(c*x, c*y, c*p, c*t);}
    friend vec operator*(double c, const vec &s);
    friend vec f(double a, double b, double c, double d, double e, vec v);
    friend vec RKstep (double a, double b, double c, double d, double e, vec start);
    friend void drawMaya(SDL_Renderer* renderer, vec data, int xpos, int ypos, double L, double l, double SCALE);
};

vec operator*(double c, const vec &s) {
    return vec(c*s.x, c*s.y, c*s.p, c*s.t);
}

vec f (double a, double b, double c, double d, double e, vec v) { // динамические уравнения Эйлера
    double A = b*b*cos(v.t-v.p)*cos(v.t-v.p)/2;
    double B = 2*b*sin(v.t-v.p)*cos(v.t-v.p)*b/2;
    return vec((e*sin(v.t) - B/a*v.x*v.x - 2*b*sin(v.t-v.p)*v.y*v.y - b*d/2/a*sin(v.p)*cos(v.t-v.p))/(2*c - A/a), 
               (d*sin(v.p) + B/c*v.y*v.y + 2*b*sin(v.t-v.p)*v.x*v.x - b*e/2/c*sin(v.t)*cos(v.t-v.p))/(2*a - A/c), 
               v.y,
               v.x);
}

vec RKstep (double a, double b, double c, double d, double e, vec start) {    // метод Рунге-Кутты 4ого порядка
    vec k1, k2, k3, k4; 
    const double stepsize = 0.0005;    // временной шаг
    k1 = f(a, b, c, d, e, start);
    k2 = f(a, b, c, d, e, start+k1*(0.5)*stepsize);
    k3 = f(a, b, c, d, e, start+k2*(0.5)*stepsize);
    k4 = f(a, b, c, d, e, start+k3*stepsize);
    return start+(k1+k2*2+k3*2+k4)*(double(1)/double(6))*stepsize; // пересчёт нового значения параметров системы ********
}

void drawMaya(SDL_Renderer* renderer, vec data, int xpos, int ypos, double L, double l, double SCALE) {    // создание 2 палочек маятника
    // первая палочка (верхняя) - первые 2 координаты - начало, вторые две - конец
    SDL_RenderDrawLine(renderer, xpos, ypos, xpos+L*SCALE*sin(data.p), ypos+L*SCALE*cos(data.p));  
    //вторая палочка (нижняя)        
    SDL_RenderDrawLine(renderer, xpos+L*SCALE*sin(data.p), ypos+L*SCALE*cos(data.p), xpos+L*SCALE*sin(data.p)+l*SCALE*sin(data.t), ypos+L*SCALE*cos(data.p)+l*SCALE*cos(data.t)); 
}

class CApp {
private:
    bool Running;
    SDL_Surface* surface;    // cтруктура содержащая набор используемых пикселей 
    SDL_Window* window;      // структура которая содержит всю информацию о самом окне: размер, положение, полноэкранный режим, границы и т. д.
    SDL_Renderer* renderer;  // рендеринговая структура
    vec data;
    const int TPS = 60;
    const int WIDTH = 800, HEIGHT = 600;
    double SCALE = 120, g = -9.81;
    double L, l, M, m, a, b, c, d, e;
public:
    CApp(double L, double l, double M, double m): L(L), l(l), M(M), m(m) {
        Running = true;
        window = NULL;
        renderer = NULL;
        a = M*L*L/6 + m*L*L/2;    // константы для уравнений Эйлера
        b = m*l*L/4;
        c = m*l*l/6;
        d = (M/2+m)*g*L;
        e = m*g*l/2;
  }
    
    void setData(vec d) {
        data = d;
  }

    bool OnInit() //инициализация всего всего и создание окна
    {
        SDL_Init(SDL_INIT_EVERYTHING);   // инициализирует библиотеку SDL и все её подсистемы (видео, аудио и т.д)
        window = SDL_CreateWindow("Mayatnic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);    // создание окна
        renderer = SDL_CreateRenderer(window, -1, 0);   // 2d рендиринг для окна window
        // Проверяет что окно было успешно создано
        if (window == NULL) {
            // В этом случае окно не может быть создано
            std::cout << "Could not create window: " << SDL_GetError( ) << std::endl;
            return false;
        }
        return true;
    }

    void OnEvent(SDL_Event* Event) {
        if (Event->type == SDL_QUIT) Running = false; //проверка на нажатие крестика в рамке окна
    }

    void OnLoop() { //меняет положения палочек на экране (графически), пересчитывает углы и угловые скорости
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);   // устанавливает цвет используемый для операций рисования квадрата
        SDL_RenderClear(renderer);   // чистим экран от нарисованных ранее текстур
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
        drawMaya(renderer, data, WIDTH/2, HEIGHT/2, L, l, SCALE);
        for (int i=1; i<10; i++) {
            data = RKstep(a,b,c,d,e,data);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/TPS);
    }
    

    void OnCleanup() { //функция для завершения выполнения
        SDL_DestroyWindow(window);  // закрывает окно
        SDL_Quit();   // отключает все инициализированные подсистемы
    }

    int OnExecute() {    // основная функция, обрабатывающая события на экране
        if(OnInit() == false) return -1;   // проверка, есть ли окно
        SDL_Event Event;     // создание объекта класса SDL_Event
        while(Running) {
            while(SDL_PollEvent(&Event)) {   // проверка, что в очереди есть события
                OnEvent(&Event);
            }
            OnLoop();
        }
        OnCleanup(); // в случае нажатия крестика всё закрывает
        return 0;
    }
};

int main() {
    double L = 1.5, l = 1, M = 1, m = 1;   // вводим константы маятника (массы и длины стержней), длина верхней палочки L, нижней l, аналогично массы
    CApp a(L, l, M, m);   // создаём элемент класса CApp
    a.setData(vec(0, 0, -pi/2, -pi/3)); // задаём начальные условия системы
    a.OnExecute();
    return 0;
}

