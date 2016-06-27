#libvot - C++11多线程图像检索库

[![Join the chat at https://gitter.im/hlzz/libvot](https://badges.gitter.im/hlzz/libvot.svg)](https://gitter.im/hlzz/libvot?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/hlzz/libvot.svg?branch=master)](https://travis-ci.org/hlzz/libvot) 
[![Build Status](https://travis-ci.org/hlzz/libvot.svg?branch=feature)](https://travis-ci.org/hlzz/libvot) 
[![License](https://img.shields.io/badge/license-BSD-blue.svg)](LICENSE)
<a href="https://scan.coverity.com/projects/hlzz-libvot">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/8983/badge.svg"/>
</a>
[![todofy badge](https://todofy.org/b/hlzz/libvot/master)](https://todofy.org/r/hlzz/libvot/master)

####[English Version](../README.md)

##简介
*libvot*是一套完整的图像检索工具箱，其中包含特征提取工具和一个高效的vocabulary tree实现。在这个工具箱中，我们用到C++11的标准多线程库来加速算法，
取得了快速准确的检索结果。目前，这个库主要用于我们的科研和生产，并且在积极维护中。如果觉得有用，欢迎star或者fork这个项目！

##安装
编译安装libvot非常简单，整个编译安装基于[CMake](http://cmake.org)。为了保证能够使用全部的C++11特性，这个项目要求CMake版本高于2.8.12。
我们主要在Unix-like的系统(Ubuntu 14.04, CentOS 7, MacOS 10.10)中用gcc进行测试。简单的编译安装包括以下几个步骤:

1. 下载本项目或者clone
2. 建立一个build文件夹
3. 运行CMake来设置编译选项
4. 用make和gcc进行编译
5. 运行make test
6. 在src/example中查看示例程序

在类Unix系统中，以上步骤可以用如下命令执行：

    $ cd libvot
    $ git submodule init & git submodule update  
    $ mkdir build && cd build
    $ cmake ..
    $ make && make test

##初次尝试
libvot包含一个特征提取工具。你可以用`./libvot_feature <image_list>`来生成一些特征点文件，然后用特征点文件来进行图片索引。
注意，这里`libvot_feature`的参数是一个文件路径列表，而不是一个图片文件的路径。你可以像下面这样先产生`image_list`，然后运行`libvot_feature`:

    $ ls -d $PWD/*.jpg > image_list
    $ $LIBVOT_ROOT/build/bin/libvot_feature <image_list>

编译成功之后，你可以找到一个叫`image_search`的程序，它的用法很简单：`./image_search <sift_list> <output_dir> [depth] [branch_num] [sift_type] [num_matches] [thread_num]`。我们在data文件夹中加入了一个示例数据[fountain-P11](http://cvlabwww.epfl.ch/data/multiview/denseMVS.html)来进行演示。如果你执行了上述的安装过程并且成功在build文件夹中编译了libvot，以下命令可以将结果生成在`build/bin/vocab_out`文件夹中。注意，你可能需要修改一下sift_list中的文件前缀。

    $ cd bin
    $ ./image_search <sift_list> <output_folder> [depth] [branch_num] [sift_type] [num_matches] [thread_num]  
    $ (e.g.) ./image_search ../../test_data/fountain_dense/sift_list ./vocab_out

`vocab_out`文件夹中将会出现一个名为`match.out`的结果，每一行结果包含三个数字:   
“first_index second_index similarity_score”。

##文档
[主页](http://hlzz.github.io/libvot/)和[文档](http://hlzz.github.io/libvot/doc/html/index.html).

##贡献
我们正在向下一个版本(0.2)迈进，如果你对这个项目的开发感兴趣，可以看一下当前的[开发计划](Roadmap.md)和libvot的[代码风格](coding_style.md)。我们欢迎并感谢任何形式的贡献，包括文档编辑，测试，新功能开发，意见及建议等等[。](http://tianweishen.com)

##协议
The BSD 3-Clause License

##联系及捐赠
如果有问题或者建议，请发信给<tshenaa@ust.hk>.
如果你想帮助这个项目，可以考虑为我们贡献代码，也给点击以下[链接](https://pledgie.com/campaigns/30901)给开发者捐(投)赠(食)，感谢！

<a href='https://pledgie.com/campaigns/30901'><img alt='Click here to lend your support to: Open-Source Image Retrieval Project and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/30901.png?skin_name=chrome' border='0' ></a>
