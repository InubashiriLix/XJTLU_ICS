from sklearn.cluster import KMeans, AgglomerativeClustering, DBSCAN
from sklearn.mixture import GaussianMixture


# Define functions for each clustering algorithm to ensure a consistent interface
def run_kmeans(X, n_clusters, random_state=42):
    """Run KMeans clustering on data X with specified number of clusters."""
    model = KMeans(n_clusters=n_clusters, random_state=random_state, n_init=10)
    labels = model.fit_predict(X)
    return labels


def run_agglomerative(X, n_clusters, linkage="ward"):
    """Run Agglomerative (hierarchical) clustering on data X with specified clusters and linkage."""
    model = AgglomerativeClustering(n_clusters=n_clusters, linkage=linkage)
    labels = model.fit_predict(X)
    return labels


def run_dbscan(X, eps=0.5, min_samples=5):
    """Run DBSCAN clustering on data X with specified epsilon and min_samples."""
    model = DBSCAN(eps=eps, min_samples=min_samples)
    labels = model.fit_predict(X)
    return labels


def run_gaussian_mixture(X, n_components, random_state=42):
    """Run Gaussian Mixture Model clustering on data X with specified number of components (clusters)."""
    model = GaussianMixture(n_components=n_components, random_state=random_state)
    model.fit(X)
    labels = model.predict(X)
    return labels


# Define the clustering algorithms and their parameter grids for experiments
clustering_algorithms = [
    {
        "name": "KMeans",
        "function": run_kmeans,
        "params": {"n_clusters": [2, 3, 4, 5, 6]}
    },
    {
        "name": "Agglomerative",
        "function": run_agglomerative,
        "params": {"n_clusters": [2, 3, 4, 5, 6], "linkage": ["ward", "complete"]}
    },
    {
        "name": "DBSCAN",
        "function": run_dbscan,
        "params": {"eps": [0.5, 1.0, 1.5], "min_samples": [5]}
    },
    {
        "name": "GaussianMixture",
        "function": run_gaussian_mixture,
        "params": {"n_components": [2, 3, 4, 5, 6]}
    }
]
