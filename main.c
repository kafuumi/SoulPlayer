#include <stdio.h>
#include "lyric.h"

int main() {
    LYRIC *lyric = NULL;
    parseLrc("./assets/lyric/spring.lrc", lyric);
}
