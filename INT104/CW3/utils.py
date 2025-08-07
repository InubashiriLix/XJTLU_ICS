
import numpy as np
import pandas as pd
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import accuracy_score, confusion_matrix
from sklearn.base import clone

def evaluate_model_cv(model, X, y, model_name, feat_name, config_name, cv=5):
    """Perform stratified CV and return list of result dicts."""
    results = []
    skf = StratifiedKFold(n_splits=cv, shuffle=True, random_state=42)
    X_arr = np.array(X)
    y_arr = np.array(y)
    fold = 0
    for train_idx, test_idx in skf.split(X_arr, y_arr):
        fold += 1
        X_train, X_test = X_arr[train_idx], X_arr[test_idx]
        y_train, y_test = y_arr[train_idx], y_arr[test_idx]
        # clone model to reset it for each fold
        clf = clone(model)
        clf.fit(X_train, y_train)
        y_pred = clf.predict(X_test)
        acc = accuracy_score(y_test, y_pred)
        results.append({
            "Model": model_name,
            "Feature_Set": feat_name,
            "Configuration": config_name,
            "Fold": fold,
            "Accuracy": acc
        })
    return results

def save_results_csv(df, filepath):
    df.to_csv(filepath, index=False)

def get_confusion_matrix_plot(model, X, y, cv=5):
    from sklearn.model_selection import cross_val_predict
    clf = clone(model)
    y_pred = cross_val_predict(clf, X, y, cv=cv)
    cm = confusion_matrix(y, y_pred)
    return cm
