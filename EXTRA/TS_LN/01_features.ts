const hello: string = "fuck you";
let ciallo = "ciallo!!"

console.log(hello);
console.log(ciallo)

interface IPerson {
    name: string
    age: number
    shit(): void
    hello(): void
    add(name: string, age: number): void
}

class Person implements IPerson {
    name: string
    age: number

    constructor(name: string, age: number) {
        this.name = name
        this.age = age
    }

    shit(): void {
        console.log(this.name + ", you are a piece of shit!")
    }

    hello(): void {
        console.log(this.name, ", you are", this.age, "now, and you are still a piece of shit!")
    }

    add(name: string, age: number): void {
        console.log("both of you are shits!");
    }
    toString(): string {
        return `Person: ${this.name}`;
    }
}

let shit1 = new Person("shit1", 20)
let shit2 = new Person("shit2", 20)

shit1.hello()
shit2.hello()
shit1.add(shit2.name, shit2.age)

type Shit = IPerson | string;
var shit3: Shit = new Person("shityou", 232)
// var shit4: Shit = new Shit("shit", 2324) // the Shit is only a alias, it can not use as a value or constructor function
console.log(shit3)

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
}

console.log(Direction.DOWN)

let point: [number, number] = [10, 10]
console.log(point)

class Point {
    public name: string
    private value: [number, number]
    public shit(): void {
        console.log(`name: ${this.name}`)
    }
    public constructor(name: string, value1: number, value2: number) {
        this.name = name;
        this.value = [value1, value2]
    }
}

var point2: Point = new Point("shitpoint2", 100, 100);
point2.shit()

function printshit(shit: string | number | Person): void {
    if (typeof shit === "string" || typeof shit === "number") {
        console.log(shit);
        return;
    }
    if (shit instanceof Person) {
        console.log(shit.toString());
        return;
    }
    throw new TypeError("Expected a string, number, or Person");
}


printshit(shit1)

let user1 = { name: "Alice", address: { city: "xiabeize", postcode: "114514" } };
let user2 = { name: "Alice", address: null };
console.log(user1?.address?.city);
console.log(user2?.address?.city ?? "city does not existed");  // undefined, we use ?? to redirect to "city does not existed"


export { };


