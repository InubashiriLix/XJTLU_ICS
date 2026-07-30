<!-- ──────────────── HEADER ──────────────── -->
<h1 align="center">📚 My Knowledge Vault</h1>
<p align="center">
  <em>Notes + Code for everything that keeps me curious.</em>
</p>

<p align="center">
  <a href="#-目录">🧭 Navigate</a> •
  <a href="#-快速开始">⚡ Quick Start</a> •
  <a href="#-仓库结构">🗂️ Structure</a> •
  <a href="#-许可证">📝 License (GPL-3.0)</a>
</p>

---

<!-- ──────────────── BADGES ──────────────── -->
<p align="center">
  <img alt="GitHub last commit"
       src="https://img.shields.io/github/last-commit/your-username/your-repo?style=flat-square">
  <img alt="GPL-3.0"
       src="https://img.shields.io/badge/license-GPLv3-blue?style=flat-square">
  <img alt="Made with ❤️"
       src="https://img.shields.io/badge/made%20with-%E2%9D%A4-ff69b4?style=flat-square">
</p>

---

## ✨ 简介

> 这个仓库是我的 **个人知识库**：
>
> - **Notes**：整理各科目的低质量笔记（CS, ML，DataStructure, Java Intro ... ）
> - Courses：更新了许多 XJTLU ICS 课程（CPT101/102/103/104/203/205, INT102/104/201, CAN201) (大三上摆烂了, 大三下笔记还是比较全的)
> - **Code**：示例脚本、实验 project、工具函数等
> - **Cheat-Sheets**：neorg 笔记，markdown笔记， 以及各种试卷neorg笔记与答案
> - 我是狗屎, 这句话有可能不太准确, 但是基本可信: `课程也许会非常简单或者难, 你的分数可能不取决于你做了什么和老师教的怎么样, 而是取决于老师打算把均分打到多少.`
> - 如果上一条成立, 那么基本可以说: `XJTLU 和 LTU 在学术上烂完了, 能跑路就跑路吧.`
> - XJTLU就是一个拿钱换出国的地方, 我看见过很多死妈废物躺在宿舍里啥也不干啥也不学的, 最后都能拿到出国留学的资格. 当然, 这儿极高的自由也出了很多非常厉害的大佬.
> - 哦对了, 在此祝所有`买试卷, 卖试卷的狗杂种臭傻逼`们, 还有那些因为`一己偏见而给大作业打低分的老二`们, **全家老小死光光**, 也祝你自己可以看着他们一个一个死掉, 最后享受和大地肘击输了看着自己的四肢诡异的扭曲疼痛席卷而来, 像条狗一样在地上叫了几下就死了.
> - 如果有兴趣或者有补充的话, 可以提pr, 也可以直接在仓库里提issue, 我会尽量回复的.
> - 可以的话, 能给这个仓库点个star吗? 这对我这条无家可归的野狗来说, 能多一个活下来的动力, 谢谢大家了...

所有内容在 [GNU General Public License v3.0](#-许可证) 下发布——自由、开源、传递快乐 🎉

---

## 🧭 目录

| Section                | What’s inside                |
| ---------------------- | ---------------------------- |
| [快速开始](#-快速开始) | 克隆、环境配置、笔记预览     |
| [仓库结构](#-仓库结构) | 目录一览与命名规范           |
| [贡献指南](#-贡献指南) | PR 流程、代码风格、Note 模板 |
| [许可证](#-许可证)     | 版权相关                     |

---

## 🚀 快速开始

```bash
# 1. 克隆仓库
git clone https://github.com/your-username/your-repo.git
cd your-repo

# 2. 下载neovim以及Lazyvim，并且安装neorg插件
# 我是推荐使用nvim的，但是你也可以使用别的编辑器，如果没有neorg插件或者nerog支持的编辑器，那阅读笔记可能比较痛苦

# 代码部分需要的环境：
#  python
#     sklearn, matplotlib, pandas, numpy
#  dotnet:
#     dotnet sdk 8+
#  cpp:
#     cpp 17+
# java:
#     jdk 17+
```

## 🗂️ 仓库结构

```bash
├── BASH_LN                                # bash 学习脚本与日志
│   ├── 01.sh
│   ├── 02.sh
│   ├── 03.sh
│   ├── array.sh
│   ├── cal.sh
│   ├── echo.sh
│   ├── flowControl.sh
│   ├── function.sh
│   ├── redirect.sh
│   ├── re.sh
│   ├── test.sh
│   ├── all.log
│   ├── all2.log
│   ├── failed.py
│   ├── failed2.py
│   ├── failed.log
│   └── shit.log
├── CAN201                                # 计算机网络
│   ├── 01Lects                           # 讲义
│   │   ├── ControlPlane
│   │   ├── LinkLayer
│   │   ├── NetworkLayer
│   │   ├── NetworkSecurity
│   │   └── fonts
│   ├── 02Labs                            # 实验
│   ├── 03_Papers                         # 试卷
│   │   ├── original
│   │   └── ans
│   ├── ASS1                              # 作业
│   │   ├── data
│   │   ├── file
│   │   ├── log
│   │   └── tmp
│   ├── CW1                               # Coursework 1
│   │   ├── data
│   │   ├── file
│   │   ├── log
│   │   └── tmp
│   ├── CW2                               # Coursework 2
│   └── SOCKET                            # socket 编程练习
│       ├── TcpClient
│       └── TcpServer
├── CPT101                                # Introduction to Programming
│   ├── note
│   └── papers
├── CPT102                                # Data Structures
│   ├── 1week
│   │   └── ArrayListInterface
│   ├── 2week
│   │   ├── abstract
│   │   └── MoreCollections
│   ├── 3week
│   │   ├── exerise_sum_up
│   │   ├── factorial
│   │   ├── fibonacci
│   │   └── MoreCollections
│   ├── 5week
│   │   └── MoreCollections
│   ├── 6week
│   │   ├── 8Lect
│   │   └── BinaryTree
│   ├── 8week
│   │   ├── linkedListApp
│   │   ├── linkedListCpp
│   │   └── LinkedListCS
│   ├── lect
│   ├── papers
│   ├── revisionQA
│   └── selfstudy
│       ├── AVL
│       ├── bag
│       ├── BST
│       ├── hash
│       └── MoreCollections
├── CPT103                                # Database
│   ├── notes
│   │   ├── week9
│   │   └── week10
│   ├── papers
│   └── sql_grammar
├── CPT104                                # Software Engineering
│   └── papers
├── CPT203                                # 数字逻辑 / 相关课程资料
│   ├── 01lects
│   │   └── Modling
│   └── 02_papers
│       ├── pdfs
│       └── ans
├── CPT205                                # C/C++ 数值计算与建模
│   ├── 01Lects
│   │   ├── 03LectSource
│   │   ├── graphics
│   │   └── pdf
│   ├── 02Labs
│   │   ├── 01Week
│   │   └── 05lab
│   ├── 03_papers
│   │   ├── orginal
│   │   └── ans
│   ├── 04_Math_Impl
│   │   └── Utils
│   ├── Assignment
│   │   ├── ASS1
│   │   └── ASS_COPY
│   ├── CW2
│   │   ├── br
│   │   ├── ref
│   │   └── repo
│   ├── ModelCPP
│   │   ├── include
│   │   ├── src
│   │   └── build
│   └── ModelHandBook.md
├── INT102                                # Algorithms
│   ├── 00_intro
│   ├── 1week
│   │   └── farmerCabageWolf
│   ├── 2week
│   ├── 3week
│   │   ├── BinarySearch
│   │   ├── DivideAndConquer
│   │   ├── MergeSort
│   │   └── Recursion
│   ├── 4week
│   │   └── Graph
│   ├── 5week
│   │   ├── Dijkstra
│   │   ├── Kruska
│   │   └── MST
│   ├── 6week
│   │   ├── 1Ques
│   │   ├── dynamic
│   │   ├── ques2
│   │   └── ques3
│   ├── 7week
│   │   ├── cpp
│   │   ├── cppRecursive
│   │   ├── HorspoolCPP
│   │   ├── sortingshit
│   │   └── TradeOff
│   ├── 8week
│   │   ├── BellmanFor
│   │   ├── BellmanFordCPP
│   │   ├── Floyd
│   │   ├── WarshellCPP
│   │   └── temp
│   ├── 9week
│   │   ├── LCS
│   │   ├── NeedlemanWunsch
│   │   └── PSA
│   ├── 10week
│   │   └── NPproblems
│   ├── 11week
│   │   ├── algo
│   │   └── bruteforce
│   ├── 12_week
│   │   └── BagQues
│   ├── analysis
│   ├── ass1
│   │   └── ass1
│   ├── EX
│   │   ├── CF20C
│   │   ├── ChainForwardStars
│   │   ├── DFS
│   │   ├── GFS
│   │   ├── IntervalSum
│   │   └── UnionFindSet
│   ├── papers
│   ├── prototypes
│   ├── torture2
│   │   ├── ques1
│   │   ├── Ques1
│   │   ├── ques2
│   │   ├── ques3
│   │   ├── ques4
│   │   └── ques5
│   └── tutorial
│       ├── week2
│       ├── week4
│       ├── week6
│       ├── week9
│       ├── week11
│       └── week13
├── INT104                                # Data Science / ML
│   ├── CW1
│   │   └── final1
│   ├── CW3
│   │   ├── figures
│   │   └── results
│   ├── datasets
│   ├── LectCodes
│   ├── lects
│   │   └── lectcodes
│   └── papers
│       ├── 22F
│       ├── 22R
│       ├── 23F
│       └── 23R
├── INT201                                # 信息系统 / 相关课程
│   ├── 1Lect
│   │   └── statics
│   ├── 2Tutorial
│   ├── 3PDFs
│   │   └── 1Week
│   ├── ASS1
│   ├── ASS2
│   └── papers
├── RUST_LN                               # Rust 学习记录
│   ├── index.org
│   ├── Readme.md
│   ├── GeneralTraitOptionResult
│   │   ├── src
│   │   └── target
│   ├── loopTrain
│   │   ├── src
│   │   └── target
│   ├── OwnershipAndRef
│   │   ├── src
│   │   └── target
│   ├── Result
│   │   ├── src
│   │   └── target
│   └── StructEnumMatch
│       ├── src
│       └── target
└── TS_LN                                 # TypeScript 学习记录
    ├── 01_features.ts
    └── 01_features.js
```

## 🤝 贡献指南

1. Fork + Branch：feature/my-awesome-idea
2. 写笔记 -> 提交代码 (pre-commit 自动格式化)
3. Pull Request -> 等待 review
4. 合并后即可在 main branch 看到你的贡献 ✨

## 📝 许可证

本仓库采用 GNU General Public License v3.0 发布。

- 你 可以：自由使用、修改、再发布
- 你 必须：在衍生作品中 同样使用 GPL-3.0 并标注原作者
- 你 不能：在闭源或专有软件中直接使用本仓库代码而拒绝开源衍生作品

从社区中来，回馈社区。Happy hacking! 🛠️

<p align="center"><sub>© 2025 Li Xinrong — Built with caffeine &amp; curiosity.</sub></p>
