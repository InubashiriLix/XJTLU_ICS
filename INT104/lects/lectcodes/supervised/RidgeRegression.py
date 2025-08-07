from sklearn.datasets import fetch_california_housing
from sklearn.linear_model import Ridge
from sklearn.model_selection import train_test_split, GridSearchCV
from sklearn.metrics import mean_squared_error
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline

# 1. 加载数据
data = fetch_california_housing()
X = data.data
y = data.target

# 2. 划分训练集与测试集
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.3, random_state=42
)

# 3. 构建管道（标准化 + 岭回归）
pipe = Pipeline([("scaler", StandardScaler()), ("ridge", Ridge())])

# 4. 参数网格
param_grid = {
    "ridge__alpha": [0.01, 0.1, 1, 10, 100],  # 正则化强度
    "ridge__solver": ["auto", "saga", "lsqr"],
}

# 5. GridSearchCV
grid = GridSearchCV(pipe, param_grid, cv=5)
grid.fit(X_train, y_train)

# 6. 输出最优参数和测试误差
print("最优参数：", grid.best_params_)
y_pred = grid.predict(X_test)
print("测试集均方误差 MSE：", mean_squared_error(y_test, y_pred))
