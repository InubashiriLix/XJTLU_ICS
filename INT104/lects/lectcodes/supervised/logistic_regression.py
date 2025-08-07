from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import accuracy_score
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline

from data_loader import X_train, X_test, y_train, y_test

pipe = Pipeline(
    [("scaler", StandardScaler()), ("logreg", LogisticRegression(max_iter=1000))]
)

param_grid = {
    "logreg__C": [0.01, 0.1, 1, 10, 100],  # 正则化强度
    "logreg__penalty": ["l2"],  # 正则项（L2 正则）
    "logreg__solver": ["lbfgs", "liblinear", "saga"],  # 求解器
    "logreg__multi_class": ["auto"],
}

grid = GridSearchCV(pipe, param_grid=param_grid, cv=5)
grid.fit(X_train, y_train)

print(f"the best params: {grid.best_params_}")
y_pred = grid.predict(X_test)
print(f"accuracy: {accuracy_score(y_test, y_pred)}")
