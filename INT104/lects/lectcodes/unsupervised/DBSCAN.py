from data_loader import X

from sklearn.preprocessing import StandardScaler
from sklearn.cluster import DBSCAN
from sklearn.metrics import silhouette_score

import numpy as np

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

best_score = -1
best_params = {}
best_labels: np.ndarray


for eps in np.arange(start=0.3, stop=1.5, step=0.1):
    for min_samples in range(2, 10):
        db = DBSCAN(eps=eps, min_samples=min_samples)
        labels = db.fit_predict(X_scaled)
        n_clustering = len(set(labels)) - (1 if -1 in labels else 0)
        if n_clustering <= 1:
            continue
        try:
            score = silhouette_score(X_scaled, labels)
            if score > best_score:
                best_score = score
                best_params = {"eps": eps, "min_samples": min_samples}
                best_labels = labels
        except Exception:
            pass

print("最优参数：", best_params)
print("Silhouette 得分：", best_score)
print("簇数量：", len(set(best_labels)) - (1 if -1 in best_labels else 0))
