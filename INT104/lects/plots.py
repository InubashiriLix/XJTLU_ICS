from matplotlib.pyplot import boxplot
import numpy as np
from sklearn.utils.extmath import density
from tables import table

rng = np.random.default_rng(42)  # For reproducibility
data1 = rng.normal(loc=0, scale=1, size=800)
data2 = rng.normal(loc=5, scale=0.5, size=200)

data = np.hstack([data1, data2])


def frequency_table():
    bin_edges = np.linspace(
        start=np.min(data), stop=np.max(data), num=11
    )  # 11 edges, 10 box inside

    counts, edges = np.histogram(data, bins=bin_edges)

    print("edges")
    for i in range(len(counts)):
        left, right = edges[i], edges[i + 1]
        print(f"[{left:.2f}, {right:.2f}] -> count = {counts[i]}")


def histogram():
    import matplotlib.pyplot as plt

    plt.figure(figsize=(6, 4))
    plt.hist(data, bins=10, edgecolor="black", alpha=0.7)
    plt.title("histogram of data (counts)")
    plt.xlabel("value")
    plt.ylabel("Count")
    plt.grid(axis="y", linestyle="--", alpha=0.5)
    plt.tight_layout()
    plt.show()


def boxplt():
    import matplotlib.pyplot as plt

    plt.figure(figsize=(4, 6))
    plt.boxplot(
        x=data,
        vert=True,
        patch_artist=True,
        boxprops=dict(facecolor="lightblue", color="black"),
        medianprops=dict(color="navy"),
    )
    plt.show()


def density_plot():
    import numpy as np
    import matplotlib.pyplot as plt
    from scipy.stats import gaussian_kde

    # use gaussian kde to collect the data
    kde = gaussian_kde(dataset=data)

    # generate evaluation points, which cover all the range from the data
    x_min, x_max = np.min(data), np.max(data)
    x_values = np.linspace(x_min - 1, x_max - 1, 500)

    density_plot = kde(x_values)

    plt.figure(figsize=(6, 4))
    plt.plot(x_values, density_plot, color="blue", label="Density Plot")
    plt.fill_between(x_values, density_plot, color="lightblue", alpha=0.5)
    plt.title("Kernel density Etimate of data")
    plt.xlabel("Value")
    plt.ylabel("Density")
    plt.grid(axis="both", linestyle="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    print("all datas: ", data.shape)
    frequency_table()
    # histogram()
    # boxplt()
    density_plot()
