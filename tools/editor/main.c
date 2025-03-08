#include "editor.h"
#include "input.h"

int main() {
    Editor editor;

    init_editor(&editor);

    while (!editor.quit) {
        SDL_Event ev;

        while (SDL_PollEvent(&ev) != 0) {
            handle_input(&ev, &editor);
        }

        render_editor_win(&editor);
        render_tilesheet_win(&editor);
        render_settings_win(&editor);
    }

    cleanup_editor(&editor);
    return 0;
}

