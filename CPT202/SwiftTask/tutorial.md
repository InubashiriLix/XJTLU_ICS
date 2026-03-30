可以，我直接给你一套**适合新手、能跑、还能顺便理解 MVC** 的版本。

我这里不走 “Spring Boot + JPA + SQLite” 这条最常见但对新手不太友好的路，而是先带你走：

**Spring Boot + Spring Web + Spring JDBC + SQLite**

原因很简单：
Spring Boot 官方推荐用 Spring Initializr 来快速起项目，Spring Boot 现在要求 Java 17+；而 SQLite 虽然能和 Hibernate/JPA 一起用，但 Hibernate 官方把 SQLite 放在 community/incubating 那一类，不算最稳最省心的主流组合。入门教学里先用 JDBC，概念更直，报错更少。([Home][1])

---

# 1. 你先把这玩意理解成啥

我们要做的是一个最简单的 TodoList：

- 新增待办
- 查看全部待办
- 按 id 查看
- 修改完成状态
- 删除

然后用 MVC 去拆：

- **Model**：数据长什么样子，比如一个 Todo 有 id、title、done
- **View**：展示层。因为我们先做后端教学，所以这里先不搞 Thymeleaf 页面，直接返回 JSON
- **Controller**：接收前端请求
- **Service**：写业务逻辑
- **Repository**：和数据库直接打交道

你可以先把它理解成：

**浏览器/前端 → Controller → Service → Repository → SQLite 数据库**

这和 Spring Boot 官方教程里常见的“Controller 处理请求、数据访问层负责存取”的思路是一致的。Spring Data 的官方文档也明确强调 repository abstraction 的目标就是减少数据访问层样板代码。([Home][2])

---

# 2. 项目结构应该长这样

```text
todolist-demo/
├─ src/
│  ├─ main/
│  │  ├─ java/
│  │  │  └─ com/example/todolist/
│  │  │     ├─ TodolistApplication.java
│  │  │     ├─ controller/
│  │  │     │  └─ TodoController.java
│  │  │     ├─ service/
│  │  │     │  └─ TodoService.java
│  │  │     ├─ repository/
│  │  │     │  └─ TodoRepository.java
│  │  │     ├─ model/
│  │  │     │  └─ Todo.java
│  │  │     └─ dto/
│  │  │        └─ CreateTodoRequest.java
│  │  └─ resources/
│  │     ├─ application.properties
│  │     └─ schema.sql
└─ pom.xml
```

你可以把它强行记成：

- `controller`：管 URL
- `service`：管逻辑
- `repository`：管 SQL
- `model`：管数据对象
- `resources`：放配置和建表 SQL

---

# 3. 第一步：创建项目

去 **Spring Initializr** 建项目最省事，这是官方提供的项目生成方式。依赖这里建议你选：

- Spring Web
- JDBC API
- SQLite Driver 不在 Initializr 默认依赖里，后面手动加 Maven 依赖

Spring Initializr 本身就是官方的 quickstart 入口。([Home][3])

---

# 4. `pom.xml` 怎么写

下面给你一个**最小可跑版** Maven 配置。

```xml
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0
         https://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>3.5.6</version>
        <relativePath/>
    </parent>

    <groupId>com.example</groupId>
    <artifactId>todolist</artifactId>
    <version>0.0.1-SNAPSHOT</version>
    <name>todolist</name>
    <description>Simple TodoList with Spring Boot + SQLite</description>

    <properties>
        <java.version>17</java.version>
    </properties>

    <dependencies>
        <!-- Web -->
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-web</artifactId>
        </dependency>

        <!-- JDBC -->
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-jdbc</artifactId>
        </dependency>

        <!-- SQLite JDBC Driver -->
        <dependency>
            <groupId>org.xerial</groupId>
            <artifactId>sqlite-jdbc</artifactId>
            <version>3.51.3.0</version>
        </dependency>

        <!-- 可选：开发时自动重启 -->
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-devtools</artifactId>
            <scope>runtime</scope>
            <optional>true</optional>
        </dependency>

        <!-- 测试 -->
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-test</artifactId>
            <scope>test</scope>
        </dependency>
    </dependencies>

    <build>
        <plugins>
            <plugin>
                <groupId>org.springframework.boot</groupId>
                <artifactId>spring-boot-maven-plugin</artifactId>
            </plugin>
        </plugins>
    </build>
</project>
```

这里有两个你要记住的点：

1. Spring Boot 3.x 需要 Java 17+。([Home][1])
2. SQLite JDBC 常用驱动就是 xerial 这个库。([GitHub][4])

---

# 5. 主启动类

`src/main/java/com/example/todolist/TodolistApplication.java`

```java
package com.example.todolist;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class TodolistApplication {
    public static void main(String[] args) {
        SpringApplication.run(TodolistApplication.class, args);
    }
}
```

这个文件基本就是“告诉 Spring Boot：启动整个应用”。

---

# 6. Model：Todo 长什么样

`src/main/java/com/example/todolist/model/Todo.java`

```java
package com.example.todolist.model;

public class Todo {
    private String id;
    private String title;
    private boolean done;

    public Todo() {
    }

    public Todo(String id, String title, boolean done) {
        this.id = id;
        this.title = title;
        this.done = done;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public boolean isDone() {
        return done;
    }

    public void setDone(boolean done) {
        this.done = done;
    }
}
```

这里我故意把 `id` 设计成 `String`，后面直接用 UUID。
这样能少踩 SQLite 自动主键返回之类的坑。

---

# 7. DTO：前端传进来的请求对象

`src/main/java/com/example/todolist/dto/CreateTodoRequest.java`

```java
package com.example.todolist.dto;

public class CreateTodoRequest {
    private String title;

    public CreateTodoRequest() {
    }

    public CreateTodoRequest(String title) {
        this.title = title;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }
}
```

为什么要单独搞 DTO？

因为：

- `Todo` 是数据库里完整对象
- `CreateTodoRequest` 是“创建请求”
- 新建时用户只传 `title`，不该自己传 `id`

这是很常见的分层思路。

---

# 8. Repository：直接写 SQL

`src/main/java/com/example/todolist/repository/TodoRepository.java`

```java
package com.example.todolist.repository;

import com.example.todolist.model.Todo;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.RowMapper;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public class TodoRepository {

    private final JdbcTemplate jdbcTemplate;

    public TodoRepository(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    private final RowMapper<Todo> todoRowMapper = (rs, rowNum) ->
            new Todo(
                    rs.getString("id"),
                    rs.getString("title"),
                    rs.getBoolean("done")
            );

    public List<Todo> findAll() {
        String sql = "SELECT id, title, done FROM todos ORDER BY rowid DESC";
        return jdbcTemplate.query(sql, todoRowMapper);
    }

    public Optional<Todo> findById(String id) {
        String sql = "SELECT id, title, done FROM todos WHERE id = ?";
        List<Todo> result = jdbcTemplate.query(sql, todoRowMapper, id);
        return result.stream().findFirst();
    }

    public void save(Todo todo) {
        String sql = "INSERT INTO todos(id, title, done) VALUES (?, ?, ?)";
        jdbcTemplate.update(sql, todo.getId(), todo.getTitle(), todo.isDone());
    }

    public int updateDone(String id, boolean done) {
        String sql = "UPDATE todos SET done = ? WHERE id = ?";
        return jdbcTemplate.update(sql, done, id);
    }

    public int deleteById(String id) {
        String sql = "DELETE FROM todos WHERE id = ?";
        return jdbcTemplate.update(sql, id);
    }
}
```

你把 Repository 理解成：

> “所有和数据库相关的脏活累活，都塞这里。”

比如 SQL 查询、插入、删除，全归它管。

---

# 9. Service：业务逻辑层

`src/main/java/com/example/todolist/service/TodoService.java`

```java
package com.example.todolist.service;

import com.example.todolist.dto.CreateTodoRequest;
import com.example.todolist.model.Todo;
import com.example.todolist.repository.TodoRepository;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.UUID;

@Service
public class TodoService {

    private final TodoRepository todoRepository;

    public TodoService(TodoRepository todoRepository) {
        this.todoRepository = todoRepository;
    }

    public List<Todo> getAllTodos() {
        return todoRepository.findAll();
    }

    public Todo getTodoById(String id) {
        return todoRepository.findById(id)
                .orElseThrow(() -> new RuntimeException("Todo not found: " + id));
    }

    public Todo createTodo(CreateTodoRequest request) {
        if (request.getTitle() == null || request.getTitle().trim().isEmpty()) {
            throw new RuntimeException("title cannot be empty");
        }

        Todo todo = new Todo(
                UUID.randomUUID().toString(),
                request.getTitle().trim(),
                false
        );

        todoRepository.save(todo);
        return todo;
    }

    public Todo markDone(String id, boolean done) {
        int affected = todoRepository.updateDone(id, done);
        if (affected == 0) {
            throw new RuntimeException("Todo not found: " + id);
        }
        return getTodoById(id);
    }

    public void deleteTodo(String id) {
        int affected = todoRepository.deleteById(id);
        if (affected == 0) {
            throw new RuntimeException("Todo not found: " + id);
        }
    }
}
```

Service 的职责你就记一句话：

> **Controller 不该直接写 SQL，也不该处理太多业务细节，业务逻辑往 Service 塞。**

比如：

- title 不能为空
- 新增时生成 UUID
- 更新失败时抛异常

这些都属于业务逻辑。

---

# 10. Controller：对外暴露接口

`src/main/java/com/example/todolist/controller/TodoController.java`

```java
package com.example.todolist.controller;

import com.example.todolist.dto.CreateTodoRequest;
import com.example.todolist.model.Todo;
import com.example.todolist.service.TodoService;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/todos")
public class TodoController {

    private final TodoService todoService;

    public TodoController(TodoService todoService) {
        this.todoService = todoService;
    }

    @GetMapping
    public List<Todo> getAllTodos() {
        return todoService.getAllTodos();
    }

    @GetMapping("/{id}")
    public Todo getTodoById(@PathVariable String id) {
        return todoService.getTodoById(id);
    }

    @PostMapping
    public Todo createTodo(@RequestBody CreateTodoRequest request) {
        return todoService.createTodo(request);
    }

    @PatchMapping("/{id}")
    public Todo updateDone(
            @PathVariable String id,
            @RequestBody Map<String, Boolean> body
    ) {
        Boolean done = body.get("done");
        if (done == null) {
            throw new RuntimeException("Field 'done' is required");
        }
        return todoService.markDone(id, done);
    }

    @DeleteMapping("/{id}")
    public String deleteTodo(@PathVariable String id) {
        todoService.deleteTodo(id);
        return "deleted";
    }
}
```

Controller 的工作就是：

- 收请求
- 拿参数
- 调 Service
- 把结果返回给前端

比如：

- `GET /api/todos` → 查全部
- `POST /api/todos` → 新建
- `DELETE /api/todos/{id}` → 删除

---

# 11. SQLite 怎么接进去

## `application.properties`

`src/main/resources/application.properties`

```properties
spring.application.name=todolist

# SQLite 数据库文件
spring.datasource.url=jdbc:sqlite:todo.db
spring.datasource.driver-class-name=org.sqlite.JDBC

# 启动时执行 schema.sql
spring.sql.init.mode=always

# 日志里打印 SQL 初始化信息（可选）
logging.level.org.springframework.jdbc=DEBUG

# 端口可改可不改
server.port=8080
```

这里最关键的是：

```properties
spring.datasource.url=jdbc:sqlite:todo.db
spring.datasource.driver-class-name=org.sqlite.JDBC
```

这表示当前目录下会生成一个 `todo.db` 文件。
SQLite 本质上就是一个**文件数据库**，不像 MySQL 那样要先开服务。

xerial 的 SQLite JDBC 驱动就是提供这个 `org.sqlite.JDBC`。([GitHub][4])

---

# 12. 建表 SQL

`src/main/resources/schema.sql`

```sql
CREATE TABLE IF NOT EXISTS todos (
    id TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    done INTEGER NOT NULL DEFAULT 0
);
```

SQLite 没有很复杂的服务端配置，你就建个表，然后直接操作这个 `.db` 文件。

这里的 `done` 我用 `INTEGER`，因为 SQLite 里布尔值很多时候最终也是按整数风格存。

---

# 13. 整个请求流程到底怎么走

假设你发一个请求：

```http
POST /api/todos
Content-Type: application/json

{
  "title": "learn spring boot"
}
```

内部流程是这样：

1. 请求先到 `TodoController.createTodo()`
2. Controller 把 JSON 转成 `CreateTodoRequest`
3. Controller 调 `todoService.createTodo(request)`
4. Service 检查 title 是否为空，并生成 UUID
5. Service 调 `todoRepository.save(todo)`
6. Repository 执行 SQL：`INSERT INTO todos(...)`
7. SQLite 把数据写进 `todo.db`
8. 返回新建好的 Todo JSON

这就是 MVC/分层真正的意义：

- Controller 不操心 SQL
- Repository 不操心业务规则
- Service 负责把中间逻辑串起来

---

# 14. 怎么运行

在项目根目录执行：

```bash
mvn spring-boot:run
```

或者先打包：

```bash
mvn clean package
java -jar target/todolist-0.0.1-SNAPSHOT.jar
```

Spring Boot 官方文档说明它本来就是这种 stand-alone 应用，可以直接 `java -jar` 跑起来。([Home][2])

---

# 15. 怎么测试接口

## 1）新增 Todo

```bash
curl -X POST http://localhost:8080/api/todos \
  -H "Content-Type: application/json" \
  -d "{\"title\":\"learn spring boot\"}"
```

返回类似：

```json
{
  "id": "2c7d0f57-4e8a-4f16-8db1-7f0e6cdbd4f1",
  "title": "learn spring boot",
  "done": false
}
```

---

## 2）查看全部

```bash
curl http://localhost:8080/api/todos
```

---

## 3）按 id 查看

```bash
curl http://localhost:8080/api/todos/你的id
```

---

## 4）修改完成状态

```bash
curl -X PATCH http://localhost:8080/api/todos/你的id \
  -H "Content-Type: application/json" \
  -d "{\"done\":true}"
```

---

## 5）删除

```bash
curl -X DELETE http://localhost:8080/api/todos/你的id
```

---

# 16. 这一套里面 MVC 到底分别对应谁

你可以直接背这个：

## Model

```text
model/Todo.java
dto/CreateTodoRequest.java
```

表示“数据长什么样”。

## View

```text
现在先没有单独 html 页面
由 @RestController 直接返回 JSON
```

也就是说，你现在做的是“后端 API 版 MVC”。

## Controller

```text
controller/TodoController.java
```

接 URL，请求入口。

## 额外两层

严格说 Service / Repository 不属于经典最原始 MVC 三件套，但在 Spring Boot 里几乎都会这么拆：

- `Service`：业务逻辑层
- `Repository`：数据访问层

所以你现在见到的其实是更实用的：

**Controller → Service → Repository → DB**

---

# 17. 为啥我不建议你第一步就上 JPA + SQLite

因为你现在是学结构，不是折腾生态兼容性。

Spring Data JPA 当然很好，官方也一直推荐它来减少样板代码。([Home][5])

但是 SQLite 在 Hibernate 这边不是最“嫡系”的数据库支持，官方文档把很多这类数据库 dialect 放在 community dialects 里，Hibernate 社区讨论里也明确提到 SQLite dialect 属于 community dialect。([Hibernate 文档][6])

所以：

- **想学 MVC / 分层 / Spring Boot 基本流程**
  先用 `JDBC + SQLite`
- **想学 ORM / JPA / Repository 自动生成查询**
  再换 `MySQL/PostgreSQL + JPA`

这样顺很多。

---

# 18. 你学完这版之后，下一步该干嘛

建议你按这个顺序升级：

## 第 1 步：先把这版跑通

重点理解：

- 项目结构
- 请求怎么流动
- SQL 在哪写

## 第 2 步：加异常处理

比如做一个全局异常处理器：

```java
@RestControllerAdvice
public class GlobalExceptionHandler {
    @ExceptionHandler(RuntimeException.class)
    public Map<String, String> handle(RuntimeException e) {
        return Map.of("error", e.getMessage());
    }
}
```

## 第 3 步：加前端页面

可以加 Thymeleaf，或者干脆前后端分离。

## 第 4 步：再学 JPA

把 `TodoRepository` 从手写 SQL 改成 JPA Repository。

---

# 19. 给你一个“奶奶版”总结

你现在这个项目，本质上就干了 4 件事：

1. **Controller**：门卫，接收请求
2. **Service**：管事的，决定该怎么处理
3. **Repository**：干活的，负责和数据库说话
4. **SQLite**：小本本，把数据记下来

请求来了以后：

**门卫接单 → 管事的安排 → 干活的写入小本本 → 返回结果**

这就是整个 Spring Boot 小项目的骨架。

---

# 20. 最后我给你一个学习建议

你现在最该做的不是一下子背很多注解，而是先把这几个概念钉死：

- `@RestController`：这是接口入口
- `@Service`：这是业务层
- `@Repository`：这是数据库层
- `JdbcTemplate`：这是执行 SQL 的工具
- `application.properties`：这是配置数据库的地方
- `schema.sql`：这是建表的地方

你把这几个点弄明白，Spring Boot 其实就没那么玄乎了。

如果你要，我下一条可以直接继续给你一份：

**“完整可复制的项目源码包（所有文件一次性贴完）”**

或者我也可以接着讲：

**“同一个 TodoList 项目，改成 JPA 版本是怎么写的”**

[1]: https://docs.spring.io/spring-boot/installing.html?utm_source=chatgpt.com "Installing Spring Boot"
[2]: https://docs.spring.io/spring-boot/index.html?utm_source=chatgpt.com "Spring Boot"
[3]: https://docs.spring.io/initializr/docs/current/reference/html/?utm_source=chatgpt.com "Spring Initializr Reference Guide"
[4]: https://github.com/xerial/sqlite-jdbc?utm_source=chatgpt.com "SQLite JDBC Driver"
[5]: https://docs.spring.io/spring-data/jpa/reference/index.html?utm_source=chatgpt.com "Spring Data JPA"
[6]: https://docs.hibernate.org/orm/6.6/dialect/?utm_source=chatgpt.com "Dialects"
