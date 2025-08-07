from data_loader import X_train, X_test, y_train, y_test
from sklearn.model_selection import GridSearchCV
from sklearn.svm import SVC
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score

pipe = Pipeline([("scaler", StandardScaler()), ("svc", SVC())])
param_grid = {
    "svc__C": [0.01, 0.1, 1, 10, 100],  # 正则化强度
    "svc__kernel": ["linear", "rbf", "poly"],  # 核函数类型
    "svc__degree": [2, 3, 4],  # 多项式核的阶数
    "svc__gamma": ["scale", "auto"],  # 核系数
}

grid = GridSearchCV(pipe, param_grid)
grid.fit(X_train, y_train)

print(f"the best params: {grid.best_params_}")
y_test_pred = grid.predict(X_test)
print(f"the accuracy: {accuracy_score(y_test_pred, y_test)}")
