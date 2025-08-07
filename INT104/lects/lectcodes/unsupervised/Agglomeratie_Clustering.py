from data_loader import X, Y, X_train, X_test, y_train, y_test

from sklearn.preprocessing import StandardScaler
from sklearn.cluster import AgglomerativeClustering
from sklearn.metrics import silhouette_score

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

best_score = -1
best_k = -1
best_model: AgglomerativeClustering


for k in range(2, 11):
    model = AgglomerativeClustering(
        n_clusters=k, linkage="ward"
    )  # linkage: ward, complete, average, single
    labels = model.fit_predict(X_scaled)
    score = silhouette_score(X_scaled, labels)

    if score > best_score:
        best_score = score
        best_k = k
        best_model = model

# 4. 输出最佳参数与聚类结果
print("最优簇数 k：", best_k)
print("Silhouette 得分：", best_score)
print("部分聚类标签：", best_model.labels_[:10])
