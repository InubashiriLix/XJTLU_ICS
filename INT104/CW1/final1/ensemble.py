import numpy as np
from sklearn.preprocessing import OneHotEncoder
from sklearn.cluster import KMeans


def voting_ensemble(cluster_labels_list, y_true):
    """
    Ensemble clustering by majority voting.
    Each clustering in cluster_labels_list is first aligned to true labels (majority vote within each cluster to assign a class label),
    then each sample's predicted class is determined by majority vote of the aligned predictions.

    Parameters:
        cluster_labels_list (list of arrays): List of cluster label arrays from different clustering algorithms (each array length = n_samples).
        y_true (array-like): True labels of samples (for mapping clusters to classes).
    Returns:
        np.ndarray: Ensemble predicted labels (aligned to the true label space).
    """
    if len(cluster_labels_list) == 0:
        return np.array([])  # no cluster results

    cluster_labels_list = [np.array(labels) for labels in cluster_labels_list]
    y_true = np.array(y_true)
    n_samples = len(y_true)
    # Align each clustering's labels to actual class labels by majority vote
    predicted_class_list = []  # list of arrays of predicted class labels for each algorithm
    for labels in cluster_labels_list:
        pred_classes = np.full(n_samples, -1, dtype=int)
        unique_clusters = np.unique(labels)
        for cluster_id in unique_clusters:
            # Determine majority class for this cluster (ignoring noise label -1 for mapping purposes)
            if cluster_id == -1:
                # leave noise as -1 (will be ignored in final voting)
                continue
            mask = (labels == cluster_id)
            if np.sum(mask) == 0:
                continue
            # Find the most frequent true label in this cluster
            true_subset = y_true[mask]
            # If no true labels (shouldn't happen unless mask empty), skip
            if true_subset.size == 0:
                continue
            # Determine majority class
            # Using np.bincount for performance (requires non-negative ints)
            # Shift true labels if they are not starting at 0
            true_subset_shifted = true_subset - np.min(y_true)  # shift so smallest label becomes 0
            counts = np.bincount(true_subset_shifted)
            majority_idx = counts.argmax()
            majority_label = majority_idx + np.min(y_true)
            pred_classes[mask] = majority_label
        predicted_class_list.append(pred_classes)
    # Now perform majority vote across algorithms for each sample
    predicted_class_matrix = np.vstack(predicted_class_list)  # shape (n_algorithms, n_samples)
    predicted_class_matrix = predicted_class_matrix.T  # shape (n_samples, n_algorithms)
    ensemble_pred = np.full(n_samples, -1, dtype=int)
    for i, preds in enumerate(predicted_class_matrix):
        # Exclude any -1 (unassigned/noise) predictions from voting
        valid_preds = preds[preds != -1]
        if valid_preds.size == 0:
            # If no algorithm assigned a class (all noise), leave as -1
            ensemble_pred[i] = -1
        else:
            # Count votes among valid predictions
            vals, counts = np.unique(valid_preds, return_counts=True)
            max_count = counts.max()
            # Candidates with max votes
            winners = vals[counts == max_count]
            if winners.size == 1:
                ensemble_pred[i] = winners[0]
            else:
                # Tie-breaker: choose the smallest label (deterministic)
                ensemble_pred[i] = winners.min()
    return ensemble_pred


def stacking_ensemble(cluster_labels_list, n_clusters):
    """
    Ensemble clustering by stacking (feature-based).
    The cluster labels from each algorithm are one-hot encoded and concatenated to form a new feature representation for each sample.
    Then KMeans clustering is applied on this representation to produce final clusters.

    Parameters:
        cluster_labels_list (list of arrays): List of cluster label arrays from different clustering algorithms (each of length n_samples).
        n_clusters (int): Number of clusters to form in the final ensemble clustering.
    Returns:
        np.ndarray: Final cluster labels from the ensemble clustering.
    """
    if len(cluster_labels_list) == 0:
        return np.array([])
    # Prepare matrix with each column as cluster labels from one algorithm
    cluster_labels_list = [np.array(labels) for labels in cluster_labels_list]
    labels_matrix = np.column_stack(cluster_labels_list)
    # One-hot encode each algorithm's cluster assignments
    # Use handle_unknown='ignore' for compatibility in case of labels not seen in fit.
    try:
        encoder = OneHotEncoder(sparse_output=False, handle_unknown='ignore')
    except TypeError:
        # for older versions of sklearn where 'sparse' is used instead of 'sparse_output'
        encoder = OneHotEncoder(sparse_output=False, handle_unknown='ignore')
    label_onehot = encoder.fit_transform(labels_matrix)
    # Perform clustering on the encoded features
    model = KMeans(n_clusters=n_clusters, random_state=42, n_init=10)
    final_labels = model.fit_predict(label_onehot)
    return final_labels
