#include <stdio.h>
#include "complex.h"
#include <SDL.h>


const int WIDTH = 1024;
const int HEIGHT = 1024;    //explodes mean grows large and is outside the Mandelbrot set
const int ITERATIONS = 50;  //Max number of iterations to check if z value "explodes"
const double LIMIT = 5;      //The number at which the z-value counts as "exploded"

int quitting;

int xDown, yDown, xDrag, yDrag;

double colourScale = 222;

double xStart, xEnd, yStart, yEnd; //Coords for viewport

SDL_Surface *image;
SDL_Renderer *renderer;
SDL_Texture *texture;

void update(){

    SDL_LockSurface(image);

    double xScale = (xEnd - xStart)/WIDTH; //The size of each pixel relative to the graph
    double yScale = (yEnd - yStart)/HEIGHT;

    unsigned int *px = image->pixels; //get a pointer to the pixel data
    unsigned int colour; //colour to set the pixels

    int i;
    int max = WIDTH*HEIGHT; //how many pixels to calculate
    double mod;  //Modulus of the complex number
    for(i = 0; i < max; i++){

        // f(z) = z^2 + c
        // z starts as 0, c is the coords on the graph

        struct cNum c; //complex number struct, defined in complex.h
        c.real = xStart + i%WIDTH * xScale;
        c.imag = yEnd - i/WIDTH * yScale;

        struct cNum z;
        z.real = 0;
        z.imag = 0;

        int j;
        mod = 0;
        //Perform the function recursively until the modulus
        //goes over the limit or the max iterations are performed
        for(j = 0; j < ITERATIONS && mod < LIMIT; j++){
            z = cAdd(cSqr(z), c);
            mod = cMod(z);
        }

        if(mod < LIMIT){//if inside the set, colour white
            colour = -1;
        }else{
            colour = colourScale*mod; //otherwise colour some random colour
        }

        px[i] = colour; //finally set the pixel to the right colour.
    }

    SDL_UnlockSurface(image);
}

void render(){
    SDL_UpdateTexture(texture, '\0', image->pixels, image->pitch); //update texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, '\0', '\0');//render it
    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]){
    //init
    quitting = 0;

    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Error!\n");
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Fracc", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    image = SDL_CreateRGBSurface(0, 1024, 1024, 32, 0, 0, 0, 0);

    texture = SDL_CreateTextureFromSurface(renderer, image);

    xStart = -2, xEnd = 2, yStart = -2, yEnd = 2; //initial viewport settings

    update();

    render();

    SDL_Event event;
    while(!quitting){

        while(SDL_PollEvent(&event)){

            if(event.type == SDL_QUIT){
                quitting = 1;
            }

            double xRange = xEnd - xStart;
            double yRange = yEnd - yStart;

            if(event.type == SDL_KEYDOWN){

                Uint8 *state = SDL_GetKeyboardState('\0');

                double xRange = xEnd - xStart;
                double yRange = yEnd - yStart;

                if(state[SDL_SCANCODE_SPACE]){
                    xEnd += 0.5*xRange;
                    xStart -= 0.5*xRange;
                    yEnd += 0.5*yRange;
                    yStart -= 0.5*yRange;
                }
                if(state[SDL_SCANCODE_RETURN]){
                    xEnd -= 0.25*xRange;
                    xStart += 0.25*xRange;
                    yEnd -= 0.25*yRange;
                    yStart += 0.25*yRange;
                }
                if(state[SDL_SCANCODE_Q]){

                }
                if(state[SDL_SCANCODE_R]){
                    xStart = -2, xEnd = 2, yStart = -2, yEnd = 2;
                }
                if(state[SDL_SCANCODE_E]){
                    yStart = yEnd - xRange;
                }
                if(state[SDL_SCANCODE_W]){
                    yStart += 0.2*xRange;
                    yEnd += 0.2*xRange;
                }
                if(state[SDL_SCANCODE_A]){
                    xStart -= 0.2*yRange;
                    xEnd -= 0.2*yRange;
                }
                if(state[SDL_SCANCODE_S]){
                    yStart -= 0.2*xRange;
                    yEnd -= 0.2*xRange;
                }
                if(state[SDL_SCANCODE_D]){
                    xStart += 0.2*yRange;
                    xEnd += 0.2*yRange;
                }

                update();
            }
            if(event.type == SDL_MOUSEBUTTONDOWN){
                SDL_GetMouseState(&xDown, &yDown);
                yDown = HEIGHT - yDown;
            }
            if(event.type == SDL_MOUSEBUTTONUP){
                if(xDown > xDrag){
                    int temp = xDown;
                    xDown = xDrag;
                    xDrag = temp;
                }
                if(yDown > yDrag){
                    int temp = yDown;
                    yDown = yDrag;
                    yDrag = temp;
                }

                xStart = xStart + (double)xDown/WIDTH * xRange;
                xEnd = xStart + (double)xDrag/WIDTH * xRange;
                yStart = yStart + (double)yDown/WIDTH * yRange;
                yEnd = yStart + (double)yDrag/WIDTH * yRange;

                update();
            }
        }

        if(xDown != 0){
            SDL_GetMouseState(&xDrag, &yDrag);
            yDrag = HEIGHT - yDrag;
            SDL_SetRenderDrawColor(renderer, 0xAA, 0xFF, 0xAA, 0xFF);
            SDL_RenderDrawLine(renderer, xStart, yStart, xStart, yEnd);
            SDL_RenderDrawLine(renderer, xStart, yStart, xEnd, yStart);
            SDL_RenderDrawLine(renderer, xStart, yEnd, xEnd, yEnd);
            SDL_RenderDrawLine(renderer, xEnd, yStart, xEnd, yEnd);
        }

        render();

    }

    SDL_FreeSurface(image);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer;
    SDL_DestroyWindow(window);

    printf("Exited successfully!\n");
    return 0;
}
