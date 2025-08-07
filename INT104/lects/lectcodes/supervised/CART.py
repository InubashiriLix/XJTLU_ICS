from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split, GridSearchCV
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score

from data_loader import X_train, X_test, y_train, y_test

# 3. 构造模型
clf = DecisionTreeClassifier()

# 4. 参数网格（CART 关键参数）
param_grid = {
    "max_depth": [2, 3, 4, 5, None],
    "criterion": ["gini", "entropy"],  # 'gini' 是 CART 默认
    "min_samples_split": [2, 5, 10],
}

# 5. 网格搜索
grid = GridSearchCV(clf, param_grid, cv=5)
grid.fit(X_train, y_train)

# 6. 输出结果
print("最优参数：", grid.best_params_)
y_pred = grid.predict(X_test)
print("预测准确率：", accuracy_score(y_test, y_pred))
