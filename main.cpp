#include <iostream>
#include "SDL2/SDL.h"

using namespace std;

const int WIDTH = 800, HEIGHT = 600, TPS=60;

class CApp {
private:
  bool Running;
  SDL_Surface* surface; //cтруктура содержащая набор используемых пикселей 
  SDL_Window* window; //структура которая содержит всю информацию о самом окне: размер, положение, полноэкранный режим, границы и т. д.
  SDL_Renderer* renderer; //рендеринговая структура
  double L, l, M, m;
public:
  CApp(double L, double l, double M, double m): L(L), l(l), M(M), m(m) {
    Running = true;
    window = NULL;
    renderer = NULL;
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
    a.OnExecute();
    return 0;
}
