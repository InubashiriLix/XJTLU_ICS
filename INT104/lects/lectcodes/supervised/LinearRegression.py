from sklearn.linear_model import LinearRegression
from sklearn.model_selection import GridSearchCV
from sklearn.tree import DecisionTreeRegressor
from sklearn.metrics import accuracy_score

from data_loader import X_train, X_test, y_train, y_test

clf = DecisionTreeRegressor()
param_gird = {
    "max_depth": [2, 3, 4, 5, None],
    "criterion": ["gini", "entropy"],
    "min_samples_split": [2, 5, 10],
}

grid = GridSearchCV(clf, param_grid=param_gird, cv=5)
grid.fit(X_train, y_train)

print("最优参数：", grid.best_params_)
y_pred = grid.predict(X_test)
print("预测准确率：", accuracy_score(y_test, y_pred))
