[![A Stunning Look at the Massive 'Blade Runner 2049' Miniatures Built by ...](https://tse3.mm.bing.net/th/id/OIP.6hMAdC2v9-Mcefh1FtV-wwHaHa?pid=Api)](https://laughingsquid.com/blade-runner-2049-miniatures-weta-workshop/?utm_source=chatgpt.com)

我看完你的作业要求了，你要的是“未来城市 + 一些科幻元素”的 3D 场景，这几个地点都非常符合要求，方便你在 OpenGL/freeglut 里做灯光、纹理、动画和移动物体。

下面我按你列的几个“东西”分别给：

* 建筑/场景的关键特征（方便你以后建模拆网格）
* 一个可以直接拿去画概念图 / AI 出参考图的英文 prompt

---

## 1. LAPD 警察楼（银翼杀手 2049 版）

**设计要点（几何+氛围）：**

* 巨大的粗野主义混凝土高塔，整体是“细长塔身 + 顶部超大外挑平台（有点像倒 T 型蘑菇头）”。([Pinterest][1])
* 顶部平台上有停机坪、闪烁的灯光和飞行警车起降区，立面上有模糊但巨大的 “LAPD” 字样。([blogs.thecdm.ca][2])
* 城市被雾霾和酸雨笼罩，远处是密集高楼和霓虹广告牌，整体冷绿色 / 蓝色调。

**Prompt 示范：**

```text
cinematic concept art of a massive brutalist concrete skyscraper, LAPD headquarters in a futuristic Los Angeles, tall narrow tower with an enormous cantilevered rooftop slab, huge faint LAPD letters on the facade, landing pads and lights on the roof, surrounded by dense cyberpunk high-rises, neon billboards, heavy fog and rain, night scene, moody volumetric lighting, wide shot
```

---

## 2. 新洛杉矶垃圾场（San Diego / Trash Mesa，有浮空垃圾车）

**设计要点：**

* 设定里是被海平面淹没后变成“洛杉矶市政垃圾处理区”的 San Diego，一望无际的垃圾高地（Trash Mesa）。([bladerunner.fandom.com][3])
* 地面是成山的废旧金属和车体残骸，遍布大型起重机、破碎机等工业机械。
* 天空阴沉，灰黄雾气中，有几艘巨大的浮空垃圾船在上方缓慢移动或倾倒垃圾。([Pinterest][4])

**Prompt：**

```text
endless futuristic junkyard wasteland outside a mega city, towering mesas of scrap metal and broken vehicles, cranes and industrial machines, grey polluted sky, huge hovering garbage ships slowly moving overhead and dumping trash, distant city skyline in smog, cinematic wide shot, gritty dystopian atmosphere
```

---

## 3. 辐射地带的旧赌场废墟（Las Vegas）

**设计要点：**

* 背景是被“脏弹”污染后的辐射城 Las Vegas，整座城笼罩在厚重的橙色沙尘和放射性雾霾中。([bladerunner.fandom.com][5])
* 赌场建筑是经典 Vegas 体量：巨大的酒店塔楼、破败的霓虹招牌、半塌的雕塑和广告牌。
* 视野里几乎没有人类，只剩飞扬的尘土、散落的废弃老虎机和桌椅。

**Prompt：**

```text
post-apocalyptic ruins of a once-glamorous Las Vegas casino in a radioactive zone, tall hotel tower skeletons, broken neon signs and giant toppled statues, everything covered in thick orange dust and haze, empty streets with scattered slot machines and tables, no people, eerie silence, cinematic wide shot, strong orange monochrome color grading
```

---

## 4. 垃圾场中的孤儿院 + 工厂 & 巨大排气扇

**设计要点：**

* Morrillcole Orphanage：位于垃圾高地中的孤儿院，主体像一个被改造成建筑的“倒扣巨型卫星天线”，底部接出一圈圈环形结构。([bladerunner.fandom.com][6])
* 附着在孤儿院周围的是简陋工厂和棚屋，内部是密密麻麻的童工流水线，堆满电子垃圾。([Reddit][7])
* 建筑外墙或内部有成排巨大的工业排气扇，布满灰尘和锈迹，昏暗灯光从扇叶缝隙中透出。

**Prompt：**

```text
dystopian orphanage and scrap-processing factory in a futuristic junkyard, main building shaped like a gigantic upside-down satellite dish converted into housing, surrounded by ramshackle industrial sheds, long rows of enormous rusty exhaust fans on the walls, dim yellow light leaking through the fan blades, piles of electronic waste and metal parts everywhere, overcast smoggy sky, cinematic concept art
```

---

## 5. 海边 / 防波堤（Sepulveda Sea Wall）

**设计要点：**

* 设定是为了抵御海平面上升而修建的 Sepulveda Sea Wall：一面高耸、近乎无限延伸的混凝土巨墙，将城市与汹涌的太平洋隔开。([bladerunner.fandom.com][8])
* 墙体有大角度坡面，外侧是怒涛巨浪，内侧是湿冷的港口区域或堆放集装箱的岸线。
* 可以在某一段墙体前设计一个斜坡平台和楼梯，方便你放打斗或车辆动画。

**Prompt：**

```text
colossal futuristic concrete sea wall protecting a mega city from a stormy ocean, endless sloping wall disappearing into fog, giant waves crashing against the base, dark cloudy sky, wet concrete surface with scattered lights and maintenance structures, narrow service road and stairways along the wall, cinematic wide shot, cold blue-grey palette
```

---

## 6. 浮空车 / Spinner（警用飞行车）

**设计要点：**

* Spinner 是可以在地面行驶又能垂直起降的飞行车，像跑车和直升机的结合体：楔形车头、上下翻开的剪刀门、两侧和尾部有喷气/反重力发动机。([维基百科][9])
* 警用版本整体比较硬朗粗犷，有明显的警灯模块、编号和粗糙装甲面。([Film and Furniture][10])

**Prompt：**

```text
futuristic police spinner flying car hovering above a rainy cyberpunk city, wedge-shaped armored body with exposed mechanical parts, glowing jet thrusters on sides and rear, scissor doors, blue and red police lights flashing, wet reflective streets and neon billboards far below, cinematic 3/4 view, detailed vehicle design render
```

---

## 7. 记忆中心大楼 & 楼前台阶（Stelline Laboratories 最后结尾那一幕）

**设计要点：**

* Stelline 记忆实验室：孤立在一片混凝土场地上的粗野主义实验楼，块状混凝土体量，四周还有其他低矮混凝土建筑。([bladerunner.fandom.com][11])
* 场景常年飘雪，天空和地面几乎连成一片灰白，建筑入口透出温暖黄光。([Taylor & Francis Online][12])
* 经典镜头：建筑前是一段宽阔的台阶（或缓坡），一个身穿长外套、受伤的男子躺在台阶上，身上积雪，远处是玻璃幕墙里温暖的实验室空间。([维基百科][13])

**Prompt：**

```text
imposing brutalist concrete research facility in a snowy futuristic landscape, Stelline-like memory laboratory, surrounded by flat concrete ground and a few low blocks, heavy snow falling from a pale grey sky, warm yellow light glowing from the main entrance, wide staircase leading up to the doors, a wounded man in a long coat lying on the steps, cinematic composition, soft snow and atmospheric lighting
```

---

如果你下一步选定其中一个建筑来做 OpenGL 场景，我可以帮你把它拆成：

* 若干个 box / cylinder / plane 的组合，
* 再一起设计灯光、纹理坐标和动画（比如：旋转排气扇、移动的垃圾飞船、在海堤上掠过的 spinner 等）。

[1]: https://www.pinterest.com/pin/404690716517511737/?utm_source=chatgpt.com "Blade Runner 2049 _ LA landscape with LAPD building ..."
[2]: https://blogs.thecdm.ca/techspeak/2022/11/14/how-the-lapd-los-angeles-police-department-was-created-in-blade-runner-2049/?utm_source=chatgpt.com "How the LAPD (Los Angeles Police Department) was ..."
[3]: https://bladerunner.fandom.com/wiki/San_Diego?utm_source=chatgpt.com "San Diego | Off-world - The Blade Runner Wiki - Fandom"
[4]: https://www.pinterest.com/ideas/blade-runner-2049-junkyard/921118838879/?utm_source=chatgpt.com "Blade Runner 2049 Junkyard"
[5]: https://bladerunner.fandom.com/wiki/Las_Vegas?utm_source=chatgpt.com "Las Vegas | Off-world: The Blade Runner Wiki | Fandom"
[6]: https://bladerunner.fandom.com/wiki/Morrillcole_Orphanage?utm_source=chatgpt.com "Morrillcole Orphanage | Off-world - The Blade Runner Wiki"
[7]: https://www.reddit.com/r/bladerunner/comments/75b1dr/blade_runner_2049_things_i_dont_get_spoilers/?utm_source=chatgpt.com "Blade Runner 2049 - Things I Don't Get (Spoilers)"
[8]: https://bladerunner.fandom.com/wiki/Sepulveda_Sea_Wall?utm_source=chatgpt.com "Sepulveda Sea Wall | Off-world - The Blade Runner Wiki"
[9]: https://en.wikipedia.org/wiki/Spinner_%28Blade_Runner%29?utm_source=chatgpt.com "Spinner (Blade Runner)"
[10]: https://filmandfurniture.com/2018/04/life-imitates-art-6-key-artefacts-in-blade-runner-2049/?utm_source=chatgpt.com "Life imitates art: 6 key artefacts in Blade Runner 2049 and ..."
[11]: https://bladerunner.fandom.com/wiki/Ana_Stelline?utm_source=chatgpt.com "Ana Stelline | Off-world - The Blade Runner Wiki - Fandom"
[12]: https://www.tandfonline.com/doi/full/10.1080/13642529.2025.2490388?utm_source=chatgpt.com "Metamodern memory: on Blade Runner, then and now"
[13]: https://en.wikipedia.org/wiki/Blade_Runner_2049?utm_source=chatgpt.com "Blade Runner 2049"

