#include <stdio.h>
#include "lyric.h"

int main()
{
    LYRIC *lyric = parseLrc("./assets/lyric/spring.lrc");
    for (LYRIC_NODE *h = lyric->head; h != NULL; h = h->next)
    {
        printf("%d = %s\n", h->time, h->lyric);
    }
}
