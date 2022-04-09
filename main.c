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
#include <glib.h>
#include <ctype.h>

#define WIDTH 800
#define HEIGHT 600

static MPLAYER *mplayer = NULL;
static GtkWidget *infoBar;
static GtkWidget *infoBarLabel;
static GtkWidget *play;
//切歌互斥锁
static pthread_mutex_t musicChangeMutex;
//歌词切换互斥锁
static pthread_mutex_t lyricChangeMutex;
//当前显示的歌词
static LYRIC_NODE *currentLyric;

//歌词框
static GtkWidget *lyricLabel;
//歌曲信息框
static GtkWidget *songLabel;

static void alertInfo(char *msg, GtkMessageType type)
{

    gtk_label_set_text(GTK_LABEL(infoBarLabel), msg);
    gtk_info_bar_set_message_type(GTK_INFO_BAR(infoBar), GTK_MESSAGE_ERROR);
    if (!gtk_widget_get_visible(infoBar))
    {
        gtk_widget_show(infoBar);
        gtk_widget_show(infoBarLabel);
    }
}

//绘制背景回调
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
//回退五秒
static void back_callback(GtkWidget *but, gpointer data)
{
    if (mplayer->playing)
    {
        backMusic(mplayer);
    }
}
static void playButtonChange()
{
    if (mplayer->playing)
    {
        gtk_button_set_image(GTK_BUTTON(play), gtk_image_new_from_file("assets/pause.png"));
    }
    else
    {
        gtk_button_set_image(GTK_BUTTON(play), gtk_image_new_from_file("assets/play.png"));
    }
}
//上一首歌曲
static void prev_callback(GtkWidget *but, gpointer data)
{
    pthread_mutex_lock(&musicChangeMutex);
    SONGLIST *list = mplayer->songList;
    SONG *now = list->now;
    if (now == NULL)
    {
        pthread_mutex_unlock(&musicChangeMutex);
        return;
    }
    SONG *prev = now->prev;
    if (prev == NULL)
    {
        alertInfo("没有上一首！", GTK_MESSAGE_INFO);
    }
    else
    {
        mplayer->playing = 1;
        playButtonChange();
        playMusic(prev->path, mplayer);
        list->now = prev;
        gtk_label_set_text(GTK_LABEL(songLabel), list->now->title);
        pthread_mutex_lock(&lyricChangeMutex);
        currentLyric = list->now->lrc;
        pthread_mutex_unlock(&lyricChangeMutex);
    }
    pthread_mutex_unlock(&musicChangeMutex);
}
//播放/暂停
static void play_callback(GtkWidget *but, gpointer data)
{
    if (mplayer->playing)
    {
        //暂停
        mplayer->playing = 0;
        pausePlayer(mplayer);
    }
    else
    {
        SONGLIST *songList = mplayer->songList;
        SONG *now = songList->now;
        if (now == NULL)
        {
            return;
        }
        else
        { //继续播放
            unpausePlayer(mplayer);
        }
        mplayer->playing = 1;
    }
    playButtonChange();
}
//下一首歌曲
static void next_callback(GtkWidget *but, gpointer data)
{
    pthread_mutex_lock(&musicChangeMutex);
    SONGLIST *list = mplayer->songList;
    SONG *now = list->now;
    if (now == NULL)
    {
        pthread_mutex_unlock(&musicChangeMutex);
        return;
    }
    SONG *next = now->next;
    if (next == NULL)
    {
        alertInfo("没有下一首!", GTK_MESSAGE_INFO);
    }
    else
    {
        mplayer->playing = 1;
        playButtonChange();
        playMusic(next->path, mplayer);
        list->now = next;
        gtk_label_set_text(GTK_LABEL(songLabel), list->now->title);
        pthread_mutex_lock(&lyricChangeMutex);
        currentLyric = list->now->lrc;
        pthread_mutex_unlock(&lyricChangeMutex);
    }
    pthread_mutex_unlock(&musicChangeMutex);
}
//前进五秒
static void ahead_callback(GtkWidget *but, gpointer data)
{
    if (mplayer->playing)
    {
        aheadMusic(mplayer);
    }
}

//程序退出时的回调
static void exit_callback()
{
    //结束mplayer
    quitMplayer(mplayer);
    freeMplayer(mplayer);
    pthread_mutex_destroy(&musicChangeMutex);
    pthread_mutex_destroy(&lyricChangeMutex);
    //删除有名管道
    unlink("./song_fifo");
    int status;
    //等待子进程结束
    wait(&status);
}

static void updateLyricText()
{
    char *lyric = currentLyric->lyric;
    // printf("%s\n", lyric);
    gtk_label_set_text(GTK_LABEL(lyricLabel), lyric);
}

//读取进度信息
void *readMplayer(void *args)
{
    GtkWidget *bar = (GtkWidget *)args;
    char *buffer = malloc(sizeof(char) * 1024);
    int len = 0;
    int value = 0;
    float timePoint = 0.0;

    char *ansStr[2];
    //进度信息字符串
    ansStr[0] = malloc(sizeof(char) * 32);
    //时间点信息
    ansStr[1] = malloc(sizeof(char) * 32);
    while (1)
    {
        len = read(mplayer->pipeFd[0], buffer, 1023);
        // EOF，退出
        if (len == EOF)
        {
            break;
        }
        else if (len == 0)
        {
            continue;
        }
        buffer[len] = '\0';
        //分割获取的结果
        if (strncmp(buffer, "ANS_", 4) == 0)
        {
            int i = 0;
            for (; i < len && buffer[i] != '\n'; i++)
            {
                ansStr[0][i] = buffer[i];
            }
            ansStr[0][i] = '\0';
            i++;
            int j = 0;
            for (; i < len && buffer[i] != '\n'; i++, j++)
            {
                ansStr[1][j] = buffer[i];
            }
            ansStr[1][j] = '\0';
        }
        else
        {
            continue;
        }
        if (strncmp(ansStr[0], "ANS_PERCENT_POSITION=", 21) == 0) //进度
        {
            sscanf(ansStr[0], "%*21s%d\n", &value); //裁剪
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bar), value * 0.01);
            //播放下一首
            if (value >= 99)
            {
                next_callback(NULL, NULL);
            }
        }
        if (strncmp(ansStr[1], "ANS_TIME_POSITION=", 18) == 0)
        { //当前播放的时间点
            sscanf(ansStr[1], "%*18s%f\n", &timePoint);
            if (currentLyric != NULL)
            {
                pthread_mutex_lock(&lyricChangeMutex);
                LYRIC_NODE *prev_lyric = currentLyric->prev;
                LYRIC_NODE *next_lyric = currentLyric->next;
                if (prev_lyric != NULL && timePoint <= prev_lyric->time)
                {
                    currentLyric = prev_lyric;
                    updateLyricText();
                }
                else if (next_lyric != NULL && timePoint >= next_lyric->time)
                {
                    currentLyric = next_lyric;
                    updateLyricText();
                }
                pthread_mutex_unlock(&lyricChangeMutex);
            }
        }
    }
    free(buffer);
    free(ansStr[1]);
    free(ansStr[0]);
    return NULL;
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
    //居中
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    // fixed容器
    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    //背景图绘制区域
    GtkWidget *drawArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawArea, WIDTH, HEIGHT);
    gtk_fixed_put(GTK_FIXED(fixed), drawArea, 0, 0);
    g_signal_connect(drawArea, "draw", G_CALLBACK(draw_callback), NULL);

    //按钮
    play = gtk_button_new_with_label("");
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
    gtk_button_set_image(GTK_BUTTON(play), gtk_image_new_from_file("assets/pause.png"));
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

    //进度条
    GtkWidget *progressBar = gtk_progress_bar_new();
    gtk_widget_set_size_request(progressBar, 600, 1);
    gtk_fixed_put(GTK_FIXED(fixed), progressBar, 100, 480);
    //歌曲信息框
    songLabel = gtk_label_new(mplayer->songList->now->title);
    gtk_widget_set_size_request(songLabel, 800, 50);
    gtk_label_set_justify(GTK_LABEL(songLabel), GTK_JUSTIFY_CENTER);
    GtkCssProvider *songCss = gtk_css_provider_new();
    gtk_css_provider_load_from_data(songCss,
                                    ".lyric{color: #FF50aa; font-size: 26px; font-weight: bold;}",
                                    59, NULL);
    GtkStyleContext *songLabelStyle = gtk_widget_get_style_context(songLabel);
    gtk_style_context_add_class(songLabelStyle, "lyric");
    gtk_style_context_add_provider(songLabelStyle, GTK_STYLE_PROVIDER(songCss), GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
    g_object_unref(songCss);
    gtk_label_set_single_line_mode(GTK_LABEL(songLabel), TRUE);
    gtk_label_set_selectable(GTK_LABEL(songLabel), FALSE);
    gtk_fixed_put(GTK_FIXED(fixed), songLabel, 0, 380);

    //歌词框
    lyricLabel = gtk_label_new("");
    gtk_widget_set_size_request(lyricLabel, 800, 50);
    gtk_label_set_justify(GTK_LABEL(lyricLabel), GTK_JUSTIFY_CENTER);
    GtkCssProvider *labelCss = gtk_css_provider_new();
    gtk_css_provider_load_from_data(labelCss,
                                    ".lyric{color: #FF50a0; font-size: 32px; font-weight: bold;}",
                                    59, NULL);
    GtkStyleContext *lyricLabelStyle = gtk_widget_get_style_context(lyricLabel);
    gtk_style_context_add_class(lyricLabelStyle, "lyric");
    gtk_style_context_add_provider(lyricLabelStyle, GTK_STYLE_PROVIDER(labelCss), GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
    g_object_unref(labelCss);
    gtk_label_set_single_line_mode(GTK_LABEL(lyricLabel), TRUE);
    gtk_label_set_selectable(GTK_LABEL(lyricLabel), FALSE);
    gtk_fixed_put(GTK_FIXED(fixed), lyricLabel, 0, 420);

    //消息框
    infoBar = gtk_info_bar_new();
    gtk_widget_set_size_request(infoBar, 300, 50);
    gtk_info_bar_add_button(GTK_INFO_BAR(infoBar), "OK", GTK_RESPONSE_OK);
    GtkWidget *area = gtk_info_bar_get_content_area(GTK_INFO_BAR(infoBar));

    infoBarLabel = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(area), infoBarLabel);
    g_signal_connect(G_OBJECT(infoBar), "response", G_CALLBACK(gtk_widget_hide), NULL);
    gtk_widget_set_no_show_all(infoBar, TRUE);
    gtk_fixed_put(GTK_FIXED(fixed), infoBar, 500, 10);
    g_thread_new("read thread", readMplayer, progressBar);
    //向mplayer发命令
    g_thread_new("send thread", sendPlayer, mplayer);
    //结束的回调
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit_callback), NULL);
    //显示窗口
    gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
    //播放列表
    mplayer = initMplayer();
    //判断是否存在有名管道
    if (access("./song_fifo", F_OK) == -1)
    {
        int ret = mkfifo("./song_fifo", 0666);
        if (ret != 0)
        {
            fatal("mkfifo, error", "");
        }
    }
    //创建无名管道
    if (pipe(mplayer->pipeFd) == -1)
    {
        fatal("pipe error", "");
    }

    //创建子进程
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        fatal("fork error", "");
    }
    else if (pid == 0)
    {
        //子进程

        //关闭读
        close(mplayer->pipeFd[0]);
        //重定向输出流
        dup2(mplayer->pipeFd[1], 1);
        //启动mplayer
        startMplayer(mplayer);
        //启动mplayer失败
        fatal("mplayer error", "");
        return -1;
    }
    else
    {
        //关闭写
        close(mplayer->pipeFd[1]);
    }
    mplayer->fifoFd = open("./song_fifo", O_WRONLY);
    //标记正在运行mplayer
    mplayer->running = 1;
    mplayer->playing = 1;
    currentLyric = mplayer->songList->head->lrc;
    pthread_mutex_init(&musicChangeMutex, NULL);
    pthread_mutex_init(&lyricChangeMutex, NULL);
    // 创建一个application
    GtkApplication *app = gtk_application_new("com.example.GtkApplication",
                                              G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
