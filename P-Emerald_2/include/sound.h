#pragma once

namespace sound{
    void playBGM(const char* name, const char* path, bool loop = true);
    void playSound(const char* name, const char* path);
}