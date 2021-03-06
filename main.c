#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>


const int WIDTH = 1024;
const int HEIGHT = 1024;    //explodes mean grows large and is outside the Mandelbrot set
const int ITERATIONS = 50;  //Max number of iterations to check if z value "explodes"
const double LIMIT = 5;      //The number at which the z-value counts as "exploded"

int quitting; //Is the program quitting 0 = no, 1 = yes

int xDown, yDown, xDrag, yDrag; //Coords when mouse is pressed and coords when mouse is being dragged

unsigned int outColourScale = 999999990; //Colour variable
unsigned int inColourScale = -1;

double xStart, xEnd, yStart, yEnd; //Coords for viewport

SDL_Surface *image; //Contains the fractal
SDL_Surface *finalImage; //Contains the fractal and any UI elements drawn over it
SDL_Renderer *renderer;
SDL_Texture *texture;

void drawBox(int x1, int y1, int x2, int y2) {
	SDL_BlitSurface(image, '\0', finalImage, '\0'); //Get the blank fractal
	unsigned int *px = finalImage->pixels; //get pointer to pixel data

	//Always start from same corner
	if (x1 > x2) {
		int xTemp = x1;
		x1 = x2;
		x2 = xTemp;
	}
	if (y1 > y2) {
		int yTemp = y1;
		y1 = y2;
		y2 = yTemp;
	}
	int x, y;
	for (y = y1; y <= y2; y += 1) { //Cycle through relevant rows
		if (y == y1 || y-1 == y1 || y == y2 || y+1 == y2) { //If top or bottom draw a line ----- for the top of the rect
			//the y-1 y+1 business is to make the lines 2 pixels thick
			for (x = x1; x <= x2; x++) { 
				px[y*WIDTH + x] *= -1;
			}
		}
		else {					//Otherwise just draw 2 pixels for the walls of the rect .    .
			px[y*WIDTH + x1] *= -1;//                                                    .    .
			px[y*WIDTH + x2] *= -2;//                                                    .    .

			px[y*WIDTH + x1+1] *= -1; //Draw the lines 2 pixels thick
			px[y*WIDTH + x2-1] *= -2;
		}
	}
}

void update(){ //Update the fractal image
	printf("viewport: (%f,%f) , (%f, %f)\n", xStart, yStart, xEnd, yEnd);
    SDL_LockSurface(image);

    long double xScale = (xEnd - xStart)/((double) WIDTH); //The size of each pixel relative to the graph
    long double yScale = (yEnd - yStart)/((double) HEIGHT);

    unsigned int *px = image->pixels; //get a pointer to the pixel data
    unsigned int colour; //colour to set the pixels

    int i;
    int max = WIDTH*HEIGHT; //how many pixels to calculate
    long double mod;  //Modulus of the complex number
    for(i = 0; i < max; i++){

        // f(z) = z^2 + c
        // z starts as 0, c is the coords on the graph

        long double cR, cI, zR, zI; // Real and Imaginary parts of Z and C
        cR = xStart + i%WIDTH * xScale; //Coords for C
        cI = yEnd - i/WIDTH * yScale;

        zR = 0; //init Z
        zI = 0;

        int j;
        mod = 0;
        //Perform the function recursively until the modulus
        //goes over the limit or the max iterations are performed
        for(j = 0; j < ITERATIONS && mod < LIMIT; j++){
            //These 2 lines basically calculate z = z^2 + c
            long double zRTemp = zR * zR - zI * zI + cR; //Calculate new values for zR and zI
            long double zITemp = zR * zI * 2 + cI;
            zR = zRTemp; //Set them
            zI = zITemp;
            mod = sqrt(zR * zR + zI * zI); // get the modulus
        }

        if(mod < LIMIT){
            colour = inColourScale - mod*100 - mod * 100 * 255 - mod; //If inside set a random colour
        }else{                                                        //These colour "formulas" were just made by trial and error to what looked best.
            colour = outColourScale + mod*100 + mod * 100 * 255 + mod;//If outside set another random colour
        }

        px[i] = colour; //finally set the pixel to the right colour.
    }

    SDL_UnlockSurface(image);

	SDL_BlitSurface(image, '\0', finalImage, '\0'); //copy this image over onto the final image
}

void render(){ //Render the fractal image
    SDL_UpdateTexture(texture, '\0', finalImage->pixels, finalImage->pitch); //update texture
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
	finalImage = SDL_CreateRGBSurface(0, 1024, 1024, 32, 0, 0, 0, 0);

    texture = SDL_CreateTextureFromSurface(renderer, image);

    xStart = -2, xEnd = 2, yStart = -2, yEnd = 2; //initial viewport settings

    update();

    render();

    srand(SDL_GetTicks() + 100000); //Seed RNG for random colours

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

                if(state[SDL_SCANCODE_SPACE]){ //zoom out
                    xEnd += 0.5*xRange;
                    xStart -= 0.5*xRange;
                    yEnd += 0.5*yRange;
                    yStart -= 0.5*yRange;
                }
                if(state[SDL_SCANCODE_RETURN]){ //zoom in
                    xEnd -= 0.25*xRange;
                    xStart += 0.25*xRange;
                    yEnd -= 0.25*yRange;
                    yStart += 0.25*yRange;
                }
                if(state[SDL_SCANCODE_Q]){
                    outColourScale = (rand()%256) * 255 + (rand()%256) * 255 * 255 + (rand()%256) * 255 * 255 * 255;
                    inColourScale = (rand()%256) * 255 + (rand()%256) * 255 * 255 + (rand()%256) * 255 * 255 * 255;
                }
                if(state[SDL_SCANCODE_R]){ //Reset to initial viewport
                    xStart = -2, xEnd = 2, yStart = -2, yEnd = 2;
                }
                if(state[SDL_SCANCODE_E]){ //Reset the aspect ratio
                    yStart = yEnd - xRange;
                }
                if(state[SDL_SCANCODE_W]){ //WASD camera movement (up, left, down, right)
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
            if(event.type == SDL_MOUSEBUTTONDOWN){ //currently not working correctly
                SDL_GetMouseState(&xDown, &yDown);
                yDown = HEIGHT - yDown;
            }
            if(event.type == SDL_MOUSEBUTTONUP){
				yDrag = HEIGHT - yDrag;
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
				float xStartTemp = xStart;
				float yStartTemp = yStart;
                xStart = xStart + ((double)xDown)/(double)WIDTH * xRange;
                xEnd = xStartTemp + ((double)xDrag)/(double)WIDTH * xRange;
                yStart = yStart + ((double)yDown)/(double)WIDTH * yRange;
                yEnd = yStartTemp + ((double)yDrag)/(double)WIDTH * yRange;

                update();

				xDown = yDown = 0;
            }
        }

        if(xDown != 0){
            SDL_GetMouseState(&xDrag, &yDrag);
			drawBox(xDown, HEIGHT - yDown, xDrag, yDrag); //Im not sure why the yDown coord has to be inverted here, ill find and fix later
        }

        render();

    }

    SDL_FreeSurface(image); //free objects
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer;
    SDL_DestroyWindow(window);

    printf("Exited successfully!\n");
    return 0;
}
