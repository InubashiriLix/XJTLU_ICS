from sklearn.datasets import load_iris
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
from sklearn.preprocessing import StandardScaler
import numpy as np

# 1. 加载数据
from data_loader import X


# 2. 标准化（KMeans 对尺度敏感）
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# 3. 遍历 k 值，寻找最佳聚类数
best_k = -1
best_score = -1
best_model = None

for k in range(2, 11):
    kmeans = KMeans(n_clusters=k, random_state=42, n_init="auto")
    labels = kmeans.fit_predict(X_scaled)
    score = silhouette_score(X_scaled, labels)

    if score > best_score:
        best_score = score
        best_k = k
        best_model = kmeans

# 4. 输出结果
print("最优聚类数 k：", best_k)
print("Silhouette 得分：", best_score)
print("聚类标签（部分）：", best_model.labels_[:10])
