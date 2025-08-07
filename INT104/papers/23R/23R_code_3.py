import numpy as np
from sklearn.svm import SVC
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import accuracy_score, classification_report
from sklearn.model_selection import train_test_split

rng = np.random.RandomState(42)
X = rng.randn(1000, 2)
y = (X[:0] * X[:1] > 0).astype(int)

param_grid = {"C": [0.1, 0.5, 0.7]}

base_svc = SVC(kernel="linear", random_state=0)

grid = GridSearchCV(
    estimator=base_svc,
    param_grid=param_grid,
    cv=5,
    scoring="accuracy",
    n_jobs=-1,
    refit=True,
)

grid.fit(X, y)

for c, mean_acc in zip(param_grid["C"], grid.cv_results_["mean_test_score"]):
    print(f"C {c:<4} -> mean accuracy = {mean_acc:.4f}")

print(f"best param: {grid.best_params_}")
print(f"best cross vertification accuracy: {grid.best_score_:.4f}")

X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)
best_svm = grid.best_estimator_
best_svm.fit(X_train, y_train)
test_acc = accuracy_score(y_test, best_svm.predict(X_test))
print(f"on the independent test set: accuracy: {test_acc}")
