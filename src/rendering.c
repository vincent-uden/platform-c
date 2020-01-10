#include "../headers/rendering.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xFF000000;
    Uint32 gmask = 0x00FF0000;
    Uint32 bmask = 0x0000FF00;
    Uint32 amask = 0x000000FF;
#else
    Uint32 rmask = 0x000000FF;
    Uint32 gmask = 0x0000FF00;
    Uint32 bmask = 0x00FF0000;
    Uint32 amask = 0xFF000000;
#endif

void renderRect(SDL_Renderer* renderer, int color, Vector pos, Vector size) {
    if ( size.x < 0 ) {
        size.x *= -1;
        pos.x -= size.x;
    }
    if ( size.y < 0 ) {
        size.y *= -1;
        pos.y -= size.y;
    }
    SDL_Surface* image = SDL_CreateRGBSurface(
        0,
        size.x,
        size.y,
        32,
        rmask,
        gmask,
        bmask,
        amask
    );

    SDL_FillRect(image, NULL, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_Rect dst = { pos.x, pos.y, size.x, size.y };
    SDL_Point rotcenter = { 0, 0 };

    SDL_RenderCopyEx(renderer, texture, NULL, &dst, 0, NULL, SDL_FLIP_NONE);

    SDL_FreeSurface(image);
    SDL_DestroyTexture(texture);
}

SDL_Rect renderTextBackend(SDL_Renderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color, TTF_Font* font) {
    SDL_Surface* msgSurf = TTF_RenderText_Blended(font, text, color);
    PUSH_LT(lt, msgSurf, SDL_FreeSurface);
    SDL_Texture* msgText = SDL_CreateTextureFromSurface(renderer, msgSurf);
    PUSH_LT(lt, msgText, SDL_DestroyTexture);
    SDL_Rect txtRect;
    TTF_SizeText(font, text, &txtRect.w, &txtRect.h);
    txtRect.x = pos.x; txtRect.y = pos.y;
    if ( adj == TRIGHT ) {
        txtRect.x -= txtRect.w;
    }
    SDL_RenderCopy(renderer, msgText, NULL, &txtRect);
    POP_LT_PTR(lt, msgSurf);
    POP_LT_PTR(lt, msgText);
    return txtRect;
}

/* pos refers to top left corner if left adjusted and top right corner if right adjusted
 * Also returns the render rect for further use */
SDL_Rect renderText(SDL_Renderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color) {
    return renderTextBackend(renderer, text, adj, pos, color, sansBold);
}

SDL_Rect renderTextSmall(SDL_Renderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color) {
    return renderTextBackend(renderer, text, adj, pos, color, sansBoldSmall);
}
