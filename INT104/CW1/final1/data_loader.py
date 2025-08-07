import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA


def load_data(file_path="training_set.xlsx"):
    """
    Load student data from file and construct feature sets for clustering.

    Feature sets constructed (excluding the true label 'Programme'):
      - Scores: Q1–Q5 scores, standardized (z-score normalization).
      - Demo: Demographic features (Gender_bin, Grade_bin) only.
      - PCA2D: All available features (scores + demographic) reduced to 2 dimensions via PCA.
      - PCA3D: All available features reduced to 3 dimensions via PCA.

    Returns:
        X_sets (dict): Dictionary with keys 'Scores', 'Demo', 'PCA2D', 'PCA3D' mapping to feature matrices (numpy arrays).
        y (numpy.ndarray): Array of true labels (Programme) for each student.
    """
    # Read the dataset
    df = pd.read_excel(file_path)

    # Separate the true labels (Programme) and drop them from features
    y = df['Programme'].values  # true labels
    # Ensure 'Programme' is not included in any feature set
    feature_df = df.drop(columns=['Programme'])

    # Construct demographic binary features
    # Gender: assuming original coding 1 and 2 (map to 0/1)
    gender = feature_df['Gender'].values
    gender_bin = (gender != 1).astype(int)  # 1->0, 2 (and others if any) ->1
    # Grade: map to binary (assuming two categories present, e.g., 2 and 3)
    grade = feature_df['Grade'].values
    grade_bin = (grade != 2).astype(int)  # 2->0, others ->1

    # Insert binary encoded demo features into feature_df (and drop original if needed)
    feature_df['Gender_bin'] = gender_bin
    feature_df['Grade_bin'] = grade_bin
    feature_df = feature_df.drop(columns=['Gender', 'Grade'])

    # Feature set 1: Scores (Q1–Q5) normalized
    scores_cols = ['Q1', 'Q2', 'Q3', 'Q4', 'Q5']
    # Ensure these columns exist
    scores_data = feature_df[scores_cols].values.astype(float)
    scaler = StandardScaler()
    X_scores = scaler.fit_transform(scores_data)

    # Feature set 2: Demo (Gender_bin + Grade_bin only)
    demo_cols = ['Gender_bin', 'Grade_bin']
    X_demo = feature_df[demo_cols].values.astype(float)
    # (These are already binary 0/1, so no further scaling is done to avoid distorting binary values.)

    # Prepare data for PCA (all features except Programme)
    # Combine standardized scores and demographic features for PCA
    # We will standardize all features to ensure PCA is not biased by scale
    all_features = feature_df[scores_cols + demo_cols].values.astype(float)
    scaler_all = StandardScaler()
    X_all_scaled = scaler_all.fit_transform(all_features)

    # Feature set 3: PCA2D (all features reduced to 2 dimensions)
    pca2 = PCA(n_components=2)
    X_pca2d = pca2.fit_transform(X_all_scaled)

    # Feature set 4: PCA3D (all features reduced to 3 dimensions)
    pca3 = PCA(n_components=3)
    X_pca3d = pca3.fit_transform(X_all_scaled)

    # Package all feature sets into a dictionary
    X_sets = {
        "Scores": X_scores,
        "Demo": X_demo,
        "PCA2D": X_pca2d,
        "PCA3D": X_pca3d
    }
    return X_sets, y
