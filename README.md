# QFileCryptor/FileCryptor

#### Instructions in English

This project refers to several similar open source projects, aiming to design a lightweight file encryption and decryption software. It does not rely on other specific third-party libraries and is built entirely using Qt components, which means that you can use Qt 5 and above versions to build your project with source code.

**Attention**: The resource file of this project is temporarily unavailable! You can use the `. rcc` binary resource file under the "resource" folder to build. The binary file is built with Qt 5.15 MinGW 64-bit. Of course, you can also find relevant pictures to form resource files and replace the relevant image paths in the source code.(**Already provided, please see Log for more details.**)

**Warning**:

1. There may be some potential bugs in this project that have not been repaired, so it is strongly recommended that the original file/folder be not deleted after encryption. Try to decrypt once and confirm that the process is correct before processing the original file/folder.
2. There are still some problems in the support of non-English characters in this project, which are under further processing.

-----

#### 中文介绍(Instructions in Chinese)

本项目参考多个类似的开源项目，旨在设计一款轻量级文件加解密软件。本项目不依赖其他特定的第三方库，完全使用Qt组件构建而成，这意味着你可以使用Qt 5及以上的版本，利用源码构建你的工程。

**注意**：本项目资源文件暂不提供！可以使用“resource”文件夹下的`.rcc`二进制资源文件进行源码构建，该二进制文件使用Qt 5.15 MinGW 64-bit构建。当然，你还可以自己寻找有关图片形成资源文件，并将源码中有关的图片路径替换即可。（**已经提供，详情请见Log**）

**警告**：

1. 本项目可能还存在一些潜在Bug未修复，因此强烈建议在加密后，先不删除原文件/文件夹。尝试解密一次并确认该过程无误后，再处理原文件/文件夹。
2. 本项目对非英文字符支持还存在一些问题，正在进一步处理中。

#### Log

> **July $8^{th}$, 2023**
>
> 1. 更新了资源文件，采用完全免费的`fontawesome 4.7.0`替换了原有的按钮图标
>
>    Update and upload resource file, using free font library named `fontawesome 4.7.0` to replace original icons of widgets.
>
> 2. 增加了`Tooltips`，方便用户查看提示信息
>
>    Add `Tooltips`  for widgets to look up for hints easily.
>
> 3. 修复了一些Bugs
>
>    Fix some bugs.
