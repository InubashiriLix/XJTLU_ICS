from sklearn.mixture import GaussianMixture
import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import load_iris

iris = load_iris()
X = iris.data[:, :2]

gmm = GaussianMixture(n_components=3)
gmm.fit(X)

labels = gmm.predict(X)

plt.scatter(X[:, 0], X[:, 1], c=labels, cmap="viridis")
plt.xlabel("Sepal Length")
plt.ylabel("Sepal width")
plt.title("Gaussian Mixture Model Clustering")
plt.show()
