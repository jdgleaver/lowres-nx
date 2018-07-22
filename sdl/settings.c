//
// Copyright 2017 Timo Kloss
//
// This file is part of LowRes NX.
//
// LowRes NX is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LowRes NX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with LowRes NX.  If not, see <http://www.gnu.org/licenses/>.
//

#include "settings.h"
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

const char *defaultProgramsPath = "programs/";

void settings_init(struct Settings *settings, int argc, const char * argv[])
{
    // default values
    
    char *basePath = SDL_GetBasePath();
    if (basePath) {
        size_t len = SDL_strlen(basePath) + SDL_strlen(defaultProgramsPath) + 1;
        char *programsPath = SDL_malloc(len);
        SDL_strlcpy(programsPath, basePath, len);
        SDL_strlcat(programsPath, defaultProgramsPath, len);
        settings->programsPath = programsPath;
        SDL_free(basePath);
    } else {
        settings->programsPath = SDL_strdup(defaultProgramsPath);
    }
    
    // parse arguments
    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];
        if (*arg == '-') {
            i++;
            if (i < argc)
            {
                const char *value = argv[i];
                if (strcmp(arg, "-fullscreen") == 0) {
                    if (strcmp(value, "yes") == 0)
                    {
                        settings->fullscreen = true;
                    }
                }
                else if (strcmp(arg, "-programs") == 0) {
                    SDL_free((void *)settings->programsPath);
                    settings->programsPath = SDL_strdup(value);
                }
            }
            else
            {
                SDL_Log("missing value for parameter %s", arg);
            }
        } else {
            settings->program = arg;
        }
    }
}

void settings_deinit(struct Settings *settings)
{
    SDL_free((void *)settings->programsPath);
}
