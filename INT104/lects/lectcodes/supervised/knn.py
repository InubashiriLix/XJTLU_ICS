import numpy as np
from sklearn.neighbors import KNeighborsClassifier as KNN
from sklearn.model_selection import GridSearchCV

from sklearn.metrics import accuracy_score

from data_loader import X_train, X_test, y_train, y_test, X, Y


def knn(neighbors=3) -> float:
    print(f"neighbors: {neighbors}")
    model = KNN(n_neighbors=neighbors)
    model.fit(X_train, y_train)

    y_test_pred = model.predict(X_test)
    return accuracy_score(y_test, y_test_pred)


if __name__ == "__main__":
    temp_max = 0.0
    temp_n = 0
    for n in [3, 5, 7, 9, 11]:
        temp = knn(n)
        if temp > temp_max:
            temp_max = temp
            temp_n = n
    print(f"final: {temp_max} with num of neighbors: {temp_n}")
