# SoulPlayer

一个实训课程的项目，基于mplayer+gtk3实现的音乐播放器。

## 工作流程

1. 建立一个有名管道。
2. fork出一个子进程，启动mplayer，并指定从有名管道中读取命令。
3. 父进程向有名管道中发送命令，实现控制mplayer的行为。
4. 父进程和mplayr之间建立无名管道，用于读取mplayer的输出信息。

## 运行环境

- linux
- gtk3.0
- mpalyer

## 构建项目

### 安装依赖库

```bash
sudo apt install gcc g++ make
sudo apt install pkg-config
sudo apt install libgtk-3-dev
sudo apt install mplayer
```

### 编译源码

```bash
git clone https://github.com/Hami-Lemon/SoulPlayer.git
cd ./SoulPlayer
make
```

## 额外说明

mp3文件需要放置在`assets/music/`中，对应的歌词文件需要放置在`assets/lyric/`中。

## 运行结果

![run](http://i0.hdslb.com/bfs/album/004e92c07cccd7f76269001273fcd1cdfe6fdfa6.png)

## 图像来源

所有图像资源来源于网络和[IconFont](https://www.iconfont.cn/)

