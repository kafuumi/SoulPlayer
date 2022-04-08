//程序入口
#include "lyric.h"
#include "mplayer.h"
#include "common.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include <fcntl.h>

#define WIDTH 800
#define HEIGHT 600

static int playing = 0;
static MPLAYER *mplayer = NULL;

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    //绘制背景图
    GdkPixbuf *back = gdk_pixbuf_new_from_file("assets/background.png", NULL);
    //缩放到和窗口一样大
    GdkPixbuf *scale = gdk_pixbuf_scale_simple(back, WIDTH, HEIGHT, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf(cr, scale, 0, 0);
    cairo_paint(cr);
    //释放资源
    g_object_unref(back);
    g_object_unref(scale);
    return TRUE;
}

static void back_callback(GtkWidget *but, gpointer data)
{
    if (playing)
    {
        backMusic(mplayer);
    }
}
static void prev_callback(GtkWidget *but, gpointer data)
{
    if (!playing)
    {
        return;
    }
    SONGLIST *list = mplayer->songList;
    SONG *now = list->now;
    if(now == NULL){
        return;
    }
    SONG *prev = now->prev;
    if (prev != NULL)
    {
        playMusic(prev->path, mplayer);
        list->now = prev;
    }
}
static void play_callback(GtkWidget *but, gpointer data)
{
    if (playing)
    {
        gtk_button_set_image(GTK_BUTTON(but), gtk_image_new_from_file("assets/play.png"));
        //暂停
        playing = 0;
        pausePlayer(mplayer);
    }
    else
    {
        gtk_button_set_image(GTK_BUTTON(but), gtk_image_new_from_file("assets/pause.png"));
        SONGLIST *songList = mplayer->songList;
        SONG *now = songList->now;
        if (now == NULL)
        { //播放第一首
            now = songList->head;
            songList->now = now;
            playMusic(now->path, mplayer);
        }
        else
        { //继续播放
            unpausePlayer(mplayer);
        }
        playing = 1;
    }
}
static void next_callback(GtkWidget *but, gpointer data)
{
    if (!playing)
    {
        return;
    }
    SONGLIST *list = mplayer->songList;
    SONG *now = list->now;
    if(now == NULL){
        return;
    }
    SONG *next = now->next;
    if (next != NULL)
    {
        playMusic(next->path, mplayer);
        list->now = next;
    }
}
static void ahead_callback(GtkWidget *but, gpointer data)
{
    if (playing)
    {
        aheadMusic(mplayer);
    }
}

static void exit_callback()
{
    //结束mplayer
    quitMplayer(mplayer);
    int status;
    //等待子进程结束
    wait(&status);
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
    GtkWidget *play = gtk_button_new_with_label("");
    GtkWidget *back = gtk_button_new_with_label("");
    GtkWidget *prev = gtk_button_new_with_label("");
    GtkWidget *next = gtk_button_new_with_label("");
    GtkWidget *ahead = gtk_button_new_with_label("");

    //按钮显示图标
    gtk_button_set_always_show_image(GTK_BUTTON(play), TRUE);
    gtk_button_set_always_show_image(GTK_BUTTON(back), TRUE);
    gtk_button_set_always_show_image(GTK_BUTTON(prev), TRUE);
    gtk_button_set_always_show_image(GTK_BUTTON(next), TRUE);
    gtk_button_set_always_show_image(GTK_BUTTON(ahead), TRUE);
    //设置按钮图标
    gtk_button_set_image(GTK_BUTTON(play), gtk_image_new_from_file("assets/play.png"));
    gtk_button_set_image(GTK_BUTTON(back), gtk_image_new_from_file("assets/back.png"));
    gtk_button_set_image(GTK_BUTTON(prev), gtk_image_new_from_file("assets/prev.png"));
    gtk_button_set_image(GTK_BUTTON(next), gtk_image_new_from_file("assets/next.png"));
    gtk_button_set_image(GTK_BUTTON(ahead), gtk_image_new_from_file("assets/ahead.png"));

    //点击事件
    g_signal_connect(G_OBJECT(back), "clicked", G_CALLBACK(back_callback), NULL);
    g_signal_connect(G_OBJECT(prev), "clicked", G_CALLBACK(prev_callback), NULL);
    g_signal_connect(G_OBJECT(play), "clicked", G_CALLBACK(play_callback), NULL);
    g_signal_connect(G_OBJECT(next), "clicked", G_CALLBACK(next_callback), NULL);
    g_signal_connect(G_OBJECT(ahead), "clicked", G_CALLBACK(ahead_callback), NULL);

    //添加按钮到fixed中
    gtk_fixed_put(GTK_FIXED(fixed), back, 180, 500);
    gtk_fixed_put(GTK_FIXED(fixed), prev, 270, 500);
    gtk_fixed_put(GTK_FIXED(fixed), play, 360, 500);
    gtk_fixed_put(GTK_FIXED(fixed), next, 450, 500);
    gtk_fixed_put(GTK_FIXED(fixed), ahead, 540, 500);
    //结束的回调
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit_callback), NULL);
    //显示窗口
    gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
    //播放列表
    SONGLIST *songList = loadSongList("./temp/", "./assets/lrc/");

    mplayer = malloc(sizeof(MPLAYER));
    mplayer->fifoFd = 0;
    mplayer->running = 0;
    mplayer->songList = songList;

    if (access("./song_fifo", F_OK) == -1)
    {
        int ret = mkfifo("./song_fifo", 0666);
        if (ret != 0)
        {
            fatal("mkfifo, error", "");
        }
    }
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        fatal("fork error", "");
    }
    //子进程
    if (pid == 0)
    {
        //启动mplayer
        startMplayer(mplayer);
        //启动mplayer失败
        fatal("mplayer error", "");
        return -1;
    }
    mplayer->fifoFd = open("./song_fifo", O_WRONLY);
    //标记正在运行mplayer
    mplayer->running = 1;
    // 创建一个application
    GtkApplication *app = gtk_application_new("com.example.GtkApplication",
                                              G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
