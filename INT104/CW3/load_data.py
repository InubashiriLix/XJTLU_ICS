import re
import pandas as pd

def load_data(filepath):
    """Load data from Excel file and preprocess."""
    df = pd.read_excel(filepath)
    df = df.dropna() # drop none
    return df

def generate_feature_sets(X_full):
    """Generate multiple feature set combinations."""
    feature_sets = {}
    # Feature sets with and without Gender and Grade
    if "Gender" in X_full.columns and "Grade" in X_full.columns:

        feature_sets["All features"] = X_full.copy()
        feature_sets["No Gender"] = X_full.drop(columns=["Gender"])
        feature_sets["No Grade"] = X_full.drop(columns=["Grade"])
        feature_sets["No Gender/Grade"] = X_full.drop(columns=["Gender", "Grade"])

        # Gender and Grade Only
        feature_sets["Demographics only"] = X_full[["Gender", "Grade"]]

    # Subsets of Q1-Q5
    qm_cols = sorted(
        [c for c in X_full.columns if re.fullmatch(r"[QM]\d+", c)],
        key=lambda s: int(s[1:])
    )
    if qm_cols:
        for i in range(len(qm_cols)):
            for j in range(i + 1, len(qm_cols)):  # 至少两个题目
                cols = qm_cols[i:j + 1]
                name = f"{cols[0]}-{cols[-1]} only"
                feature_sets[name] = X_full[cols]

    feature_sets["Q1-5"] = X_full.drop(columns=["Gender", "Grade", "MCQ"])

    return feature_sets
