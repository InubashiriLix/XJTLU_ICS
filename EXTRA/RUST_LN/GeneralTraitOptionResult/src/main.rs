enum Axis {
    X,
    Y,
}

#[derive(Debug, Copy, Clone, PartialEq)]
struct Point {
    x: f64,
    y: f64,
}

impl Point {
    /// constructor a new point
    ///
    /// * `x`: The x-coordinate of the point
    /// * `y`: The y-coordinate of the point
    fn new(x: f64, y: f64) -> Self {
        Point { x, y }
    }

    fn move_by(&mut self, axis: &Axis, delta: f64) {
        match axis {
            Axis::X => self.x += delta,
            Axis::Y => self.y += delta,
        };
    }

    fn distance_euclidean(&self, p2: &Point) -> f64 {
        ((self.x - p2.x).powi(2) + (self.y - p2.y).powi(2)).sqrt()
    }

    fn distance_manhattan(&self, p2: &Point) -> f64 {
        (self.x - p2.x).abs() + (self.y - p2.y).abs()
    }
}

#[derive(Debug, Copy, Clone)]
struct Rectangle {
    point_rt: Point,
    point_lb: Point,
}

impl Rectangle {
    fn new(point_rt: Point, point_lb: Point) -> Option<Self> {
        if point_rt.x <= point_lb.x || point_rt.y <= point_lb.y {
            return None;
        }
        Some(Rectangle { point_rt, point_lb })
    }
}

#[derive(Debug, Copy, Clone)]
struct Circle {
    center: Point,
    radius: f64,
}
// NOTE: we intentionally leave Circle implementation empty for optinal test

#[derive(Debug, Copy, Clone)]
struct Vector2D {
    point_s: Point,
    point_e: Point,
}

enum Shape {
    Rectangle(Rectangle),
    Circle(Circle),
    Vector2D(Vector2D),
}

impl Shape {
    fn area(&self) -> f64 {
        match self {
            Shape::Rectangle(rect) => {
                (rect.point_rt.x - rect.point_lb.x)
                    * (rect.point_rt.y - rect.point_lb.y)
            }
            Shape::Circle(circle) => {
                circle.radius * circle.radius * std::f64::consts::PI
            }
            _ => 0.0,
        }
    }
}

fn total_area(shape_vec: &[Shape]) -> f64 {
    let mut total: f64 = 0.0;
    shape_vec.iter().for_each(|shape| {
        total += shape.area();
    });
    total
}

fn main() {
    let rectangle: Option<Rectangle> =
        Rectangle::new(Point::new(10.0, 10.0), Point::new(0.0, 0.0));
    let circle: Circle = Circle { center: Point::new(5.0, 5.0), radius: 5.0 };
    let vector: Vector2D = Vector2D {
        point_s: Point::new(0.0, 0.0),
        point_e: Point::new(10.0, 10.0),
    };

    let mut shape_vec: Vec<Shape> = vec![];
    if let Some(r) = rectangle {
        shape_vec.push(Shape::Rectangle(r));
    }
    shape_vec.push(Shape::Circle(circle));
    shape_vec.push(Shape::Vector2D(vector));

    println!("Total area is {}", total_area(&shape_vec));
}
