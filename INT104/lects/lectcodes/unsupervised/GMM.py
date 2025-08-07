from re import S
from data_loader import X_train, X_test, y_test, y_train
from sklearn.mixture import GaussianMixture
from sklearn.metrics import silhouette_score
from sklearn.preprocessing import StandardScaler

import numpy as np

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X_train)

lowest_bic = np.inf
best_gmm = GaussianMixture()
best_params = {}

for n_components in range(1, 11):
    for cov_type in ["full", "tied", "diag", "spherical"]:
        gmm = GaussianMixture(n_components=n_components, random_state=42)
        gmm.fit(X_scaled)

        bic = gmm.bic(X_scaled)

        if bic < lowest_bic:
            lowest_bic = bic
            best_gmm = gmm
            best_params = {
                "n_components": n_components,
                "covariance_type": cov_type,
                "bic": bic,
            }

print("最优参数：", best_params)
labels = best_gmm.predict(X_scaled)
sil_score = silhouette_score(X_scaled, labels)
print("Silhouette 得分：", sil_score)
