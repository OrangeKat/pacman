#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void combinePNGs(const char* outputFilename, const char** inputFilenames, int numInputFiles) {
    // Initialize SDL and create a window (not necessary for image processing)
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Image Processing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);

    // Load the input images
    SDL_Surface** inputSurfaces = malloc(numInputFiles * sizeof(SDL_Surface*));
    for (int i = 0; i < numInputFiles; i++) {
        inputSurfaces[i] = IMG_Load(inputFilenames[i]);
    }

    // Determine the dimensions of the final combined PNG file
    int combinedWidth = 0;
    int combinedHeight = 0;
    for (int i = 0; i < numInputFiles; i++) {
        combinedWidth += inputSurfaces[i]->w;
        if (inputSurfaces[i]->h > combinedHeight) {
            combinedHeight = inputSurfaces[i]->h;
        }
    }

    // Create a new surface to hold the combined PNG file
    SDL_Surface* combinedImage = SDL_CreateRGBSurface(0, combinedWidth, combinedHeight, 32, 0, 0, 0, 0);

    // Copy each input image onto the combined surface, one after the other
    SDL_Rect srcRect = {0, 0, 0, 0};
    SDL_Rect destRect = {0, 0, 0, 0};
    for (int i = 0; i < numInputFiles; i++) {
        // Set the source and destination rectangles for this input image
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = inputSurfaces[i]->w;
        srcRect.h = inputSurfaces[i]->h;

        destRect.x = i > 0 ? destRect.x + inputSurfaces[i-1]->w : 0;
        destRect.y = 0;
        destRect.w = inputSurfaces[i]->w;
        destRect.h = inputSurfaces[i]->h;

        // Copy the input image onto the combined surface
        SDL_BlitSurface(inputSurfaces[i], &srcRect, combinedImage, &destRect);
    }

    // Save the combined image as a PNG file
    IMG_SavePNG(combinedImage, outputFilename);

    // Cleanup
    for (int i = 0; i < numInputFiles; i++) {
        SDL_FreeSurface(inputSurfaces[i]);
    }
    free(inputSurfaces);
    SDL_FreeSurface(combinedImage);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    const char* inputFilenames[] = {"./tiles/HorizontalWall.png", "./tiles/CornerBottomLeft.png", "./tiles/VerticalWall.png"};
    int numInputFiles = sizeof(inputFilenames) / sizeof(char*);
    combinePNGs("combined.png", inputFilenames, numInputFiles);
    return 0;
}
