#include <stdio.h>
#include "lyric.h"

int main() {
    LYRIC *lyric = parseLrc("./assets/lyric/spring.lrc");
    for (LIST_NODE *h = lyric->head; h != NULL; h = h->next)
    {
        printf("%d = %s\n", h->data->time, h->data->lyric);
    }
}
