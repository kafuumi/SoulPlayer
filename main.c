//程序入口
#include "lyric.h"
#include "mplayer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <gtk/gtk.h>

#define WIDTH 800
#define HEIGHT 600

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    //绘制背景图
    GdkPixbuf *back = gdk_pixbuf_new_from_file("assets/back.png", NULL);
    //缩放到和窗口一样大
    GdkPixbuf *scale = gdk_pixbuf_scale_simple(back, WIDTH, HEIGHT, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf(cr, scale, 0, 0);
    cairo_paint(cr);
    //释放资源
    g_object_unref(back);
    g_object_unref(scale);
    return TRUE;
}
//初始化窗口
static void on_activate(GtkApplication *app)
{
    // 创建一个窗口
    GtkWidget *window = gtk_application_window_new(app);
    //设置窗口图标
    GdkPixbuf *icon = gdk_pixbuf_new_from_file("assets/icon.png", NULL);
    gtk_window_set_icon(GTK_WINDOW(window), icon);
    //设置标题
    gtk_window_set_title(GTK_WINDOW(window), "然然播放器");
    //设置窗口不可伸缩
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    //设置窗口大小
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    // fixed容器
    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    //背景图绘制区域
    GtkWidget *drawArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawArea, WIDTH, HEIGHT);
    gtk_fixed_put(GTK_FIXED(fixed), drawArea, 0, 0);
    g_signal_connect(drawArea, "draw", G_CALLBACK(draw_callback), NULL);

    //按钮
    GtkWidget *play = gtk_button_new_with_label("播放");
    GtkWidget *back = gtk_button_new_with_label("后退");
    GtkWidget *prev = gtk_button_new_with_label("上一首");
    GtkWidget *next = gtk_button_new_with_label("下一首");
    GtkWidget *ahead = gtk_button_new_with_label("前进");
    GtkWidget *stop = gtk_button_new_with_label("停止");

    gtk_widget_set_size_request(GTK_WIDGET(play), )

    gtk_fixed_put(GTK_FIXED(fixed), play, 90, 450);
    gtk_fixed_put(GTK_FIXED(fixed), back, 0, 450);
    gtk_fixed_put(GTK_FIXED(fixed), prev, 0, 450);
    gtk_fixed_put(GTK_FIXED(fixed), next, 0, 450);
    gtk_fixed_put(GTK_FIXED(fixed), ahead, 0, 450);
    gtk_fixed_put(GTK_FIXED(fixed), stop, 0, 450);

    //显示窗口
    gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
    // 创建一个application
    GtkApplication *app = gtk_application_new("com.example.GtkApplication",
                                              G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}

/*int main()
{
    GtkApplication *app = gtk_application_new("com.example.player", )
    LYRIC *lyric = parseLrc("./assets/lyric/spring.lrc");
    for (LYRIC_NODE *h = lyric->head; h != NULL; h = h->next)
    {
        printf("%d = %s\n", h->time, h->lyric);
    }
    SONGLIST *songList = loadSongList("./temp/", "./assets/lrc/");
    for(SONG *song = songList->head; song!=NULL; song=song->next){
        printf("%s %s\n", song->path, song->lrc);
    }

    MPLAYER *mplayer = malloc(sizeof(MPLAYER));
    mplayer->fifoFd = 0;
    mplayer->running = 0;
    mplayer->songList = songList;

    mkfifo("./song_fifo", 0666);
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        startMplayer(mplayer);
    }else{
        //等待子进程结束
        int status;
        wait(&status);
        printf("main");
    }
    return 0;
}*/
