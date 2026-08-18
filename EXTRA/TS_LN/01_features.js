"use strict";
var _a, _b, _c;
Object.defineProperty(exports, "__esModule", { value: true });
var hello = "fuck you";
var ciallo = "ciallo!!";
console.log(hello);
console.log(ciallo);
var Person = /** @class */ (function () {
    function Person(name, age) {
        this.name = name;
        this.age = age;
    }
    Person.prototype.shit = function () {
        console.log(this.name + ", you are a piece of shit!");
    };
    Person.prototype.hello = function () {
        console.log(this.name, ", you are", this.age, "now, and you are still a piece of shit!");
    };
    Person.prototype.add = function (name, age) {
        console.log("both of you are shits!");
    };
    Person.prototype.toString = function () {
        return "Person: ".concat(this.name);
    };
    return Person;
}());
var shit1 = new Person("shit1", 20);
var shit2 = new Person("shit2", 20);
shit1.hello();
shit2.hello();
shit1.add(shit2.name, shit2.age);
var shit3 = new Person("shityou", 232);
// var shit4: Shit = new Shit("shit", 2324) // the Shit is only a alias, it can not use as a value or constructor function
console.log(shit3);
var Direction;
(function (Direction) {
    Direction[Direction["UP"] = 0] = "UP";
    Direction[Direction["DOWN"] = 1] = "DOWN";
    Direction[Direction["RIGHT"] = 2] = "RIGHT";
    Direction[Direction["LEFT"] = 3] = "LEFT";
})(Direction || (Direction = {}));
console.log(Direction.DOWN);
var point = [10, 10];
console.log(point);
var Point = /** @class */ (function () {
    function Point(name, value1, value2) {
        this.name = name;
        this.value = [value1, value2];
    }
    Point.prototype.shit = function () {
        console.log("name: ".concat(this.name));
    };
    return Point;
}());
var point2 = new Point("shitpoint2", 100, 100);
point2.shit();
function printshit(shit) {
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
printshit(shit1);
var user1 = { name: "Alice", address: { city: "xiabeize", postcode: "114514" } };
var user2 = { name: "Alice", address: null };
console.log((_a = user1 === null || user1 === void 0 ? void 0 : user1.address) === null || _a === void 0 ? void 0 : _a.city);
console.log((_c = (_b = user2 === null || user2 === void 0 ? void 0 : user2.address) === null || _b === void 0 ? void 0 : _b.city) !== null && _c !== void 0 ? _c : "city does not existed"); // undefined
