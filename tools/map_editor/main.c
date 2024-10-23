#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_image.h>
#include "defs.h"
#include "editor.h"
#include "input.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != (IMG_INIT_PNG)) {
        SDL_Quit();
        return 1;
    }

    Editor* editor = malloc(sizeof(Editor));
    init_editor(editor);

    while (!editor->quit) {
        // === Handle Input ===
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0) {
            handle_input(&e, editor);
        }

        // === Handle Rendering ===
        render_editor_win(editor);
        render_tilesheet_win(editor);
        render_settings_win(editor);
    }

    destroy_texture_cache(editor->e_cache);
    destroy_texture_cache(editor->t_cache);
    cleanup_editor(editor);
    return 0;
}

