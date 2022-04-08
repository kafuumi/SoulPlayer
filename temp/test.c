#include<stdio.h>
#include<gtk/gtk.h>
#define WIDTH 400*2
#define HIGHT 300*2
void playSong(GtkWidget* but, gpointer data) {

}
void stopSong(GtkWidget* but, gpointer data) {

}
void nextSong(GtkWidget* but, gpointer data) {


}
void previousSong(GtkWidget* but, gpointer data) {

}
void background(GtkWidget* widget)
{
	gtk_widget_set_app_paintable(widget, TRUE);
	gtk_widget_realize(widget);

	gtk_widget_queue_draw(widget);
	GdkPixbuf* src = gdk_pixbuf_new_from_file("n4.png", NULL);

	GdkPixbuf* dst = gdk_pixbuf_scale_simple(src, WIDTH, HIGHT, GDK_INTERP_BILINEAR);

	GdkPixmap* map = NULL;

	gdk_pixbuf_render_pixmap_and_mask(dst, &map, NULL, 128);

	gdk_window_set_back_pixmap(widget->window, map, FALSE);

	g_object_unref(src);
	g_object_unref(dst);
	g_object_unref(map);
}

int main(int argc, char* argv[]) {
	gtk_init(&argc, &argv); //gtk初始化

	//创建一个窗口类型
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	//设置标题
	gtk_window_set_title((GtkWindow*)window, "然然播放器");

	//设置窗口大小
	gtk_widget_set_size_request((GtkWidget*)window, 800, 600);

	//设置窗口伸缩
	gtk_window_set_resizable((GtkWindow*)window, TRUE);

	//创建多个按钮
	GtkWidget* play = gtk_button_new_with_label("播放");
	GtkWidget* theBack = gtk_button_new_with_label("上一首");
	GtkWidget* theFront = gtk_button_new_with_label("下一首");
	GtkWidget* stop = gtk_button_new_with_label("停止");

	/*设置背景图片*/
	background(window);

	/*-------------------------------按钮布局--------------------------------------------*/
	//创建一个固定容器
	GtkWidget* fixed = gtk_fixed_new();

	//按钮添加到窗口中
	gtk_fixed_put(GTK_FIXED(fixed), theBack, 300, 480);
	gtk_fixed_put(GTK_FIXED(fixed), play, 356, 480);
	gtk_fixed_put(GTK_FIXED(fixed), stop, 397, 480);
	gtk_fixed_put(GTK_FIXED(fixed), theFront, 438, 480);


	//容器添加到窗口中
	gtk_container_add(GTK_CONTAINER(window), fixed);
	/*-------------------------------------------------------------------------------*/

	//设置回调函数
	g_signal_connect(G_OBJECT(play), "clicked", G_CALLBACK(playSong), NULL);
	g_signal_connect(G_OBJECT(stop), "clicked", G_CALLBACK(stopSong), NULL);
	g_signal_connect(G_OBJECT(theFront), "clicked", G_CALLBACK(nextSong), NULL);
	g_signal_connect(G_OBJECT(theBack), "clicked", G_CALLBACK(previousSong), NULL);


	//显示窗口
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
