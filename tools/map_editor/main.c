#include "defs.h"
#include "editor.h"
#include "input.h"

int main() {
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

    cleanup_editor(editor);
    return 0;
}

