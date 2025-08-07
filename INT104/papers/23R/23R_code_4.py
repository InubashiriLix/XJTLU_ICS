# 4. Write a Python script that compares the clustering results of K-means
#    with the following initialisation methods: 1) totally random; 2) pick random
#    samples. The data should be randomly generated and contains 1000 samples
#    for 2 clusters, which is named as a variable X. k hence should be set to 2. Use
#    silhouette index to evaluate the performance of clustering.
#    The appendix of the exam provides a set of API that may be used in this
#    question
#
import numpy as np
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
from sklearn.datasets import make_blobs
from sklearn.utils import check_random_state
from typing import Any, cast

X: np.ndarray
y_true = np.ndarray
X, y_true = make_blobs(  # pyright: ignore[reportAssignmentType]
    n_samples=1000,
    n_features=2,
    centers=2,
    cluster_std=0.75,
    random_state=42,
    return_centers=False,
)

k = 2
rng = check_random_state(42)

# method1: totally random -> split the centers around [min, max]
mins = X.min(axis=0)
maxs = X.max(axis=0)
initial_centers_rand = rng.uniform(mins, maxs, size=(k, X.shape[1]))
kmeans_rand = KMeans(
    n_clusters=k,
    init=cast(Any, initial_centers_rand),
    n_init=cast(Any, 1),
    random_state=42,
).fit(X)

sil_rand = silhouette_score(X, kmeans_rand.labels_)
print(f"all random around the cneter: {sil_rand}")

# method2 random samples -> sklearn with init="random"
kmeans_sample = KMeans(
    n_clusters=k,
    init="random",
    n_init=cast(Any, 10),
    random_state=42,
).fit(X)
sil_sample = silhouette_score(X, kmeans_sample.labels_)
print(f"random samples: {sil_sample}")

better = "all random" if sil_rand > sil_sample else "random samples"
print(f"result: {better}")
