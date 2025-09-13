import matplotlib.pyplot as plt

# Coordinates
p1 = (5, 11)
p2 = (13, 8)

# Create plot
plt.figure(figsize=(6, 6))
plt.plot([p1[0], p2[0]], [p1[1], p2[1]], marker="o", color="blue", label="Line Segment")
plt.scatter(*p1, color="red")
plt.scatter(*p2, color="green")
plt.text(p1[0], p1[1] + 0.3, "p1(5,11)", color="red")
plt.text(p2[0], p2[1] + 0.3, "p2(13,8)", color="green")

plt.xlim(0, 15)
plt.ylim(0, 15)
plt.xlabel("x")
plt.ylabel("y")
plt.grid(True)
plt.legend()
plt.title("Line Segment from p1(5,11) to p2(13,8)")
plt.savefig("part1_ques1.png")
