#include <stdio.h>
#include "lyric.h"

int main() {
    printf("demo");
    LYRIC *lyric = NULL;
    parseLrc("./assets/lyric/spring.lrc", lyric);
}
