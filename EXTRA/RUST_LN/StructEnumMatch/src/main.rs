struct Point {
    x: f64,
    y: f64,
}

fn move_right(p: &mut Point, dx: f64) {
    p.x += dx;
}

fn move_up(p: &mut Point, dy: f64) {
    p.y += dy;
}

struct Circle {
    r: f64,
    center: Point,
}

struct Rectangle {
    rt_point: Point,
    lb_point: Point,
}

enum Shape {
    Circle(f64, Point),
    Rectangle(Point, Point),
}

fn area(shape: &Shape) -> f64 {
    match shape {
        Shape::Circle(r, _point) => std::f64::consts::PI * r * r,
        Shape::Rectangle(rt, lb) => (rt.y - lb.y) * (rt.x - lb.x),
    }
}

#[derive(Debug)]
enum Message {
    Quit,
    Write(String),
}

fn process(msg: &Message) {
    match msg {
        Message::Quit => println!("Quit message received"),
        Message::Write(text) => println!("Text message: {}", text),
    }
}

fn main() {
    let point_rt = Point { x: 10.0, y: 10.0 };
    let point_lb = Point { x: 0.0, y: 0.0 };
    let rectangle = Shape::Rectangle(point_rt, point_lb);

    let mut point = Point { x: 0.0, y: 7.0 };
    point.x = 5.0;
    println!("point x: {}, y: {}", point.x, point.y);
    let center_point = Shape::Circle(5.0, point);

    println!("the area of rectangle is {}", area(&rectangle));
    println!("the area of the circle is {}", area(&center_point));

    println!("practice time");
    println!("practice 1");
    let mut point1 = Point { x: 3.0, y: 4.0 };
    move_right(&mut point1, 5.0);
    move_up(&mut point1, 10.0);
    assert!(point1.x == 8.0);
    assert!(point1.y == 14.0);
    move_right(&mut point1, -3.0);
    assert!(point1.x == 5.0);

    println!("practice 3");
    let m_write = Message::Write(String::from("hello"));
    let m_quit = Message::Quit;
    process(&m_write);
    process(&m_quit);

    println!("{:?}", m_write);
    // implement Debug (what is debug derive?)
}
