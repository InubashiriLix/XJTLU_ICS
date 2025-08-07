import pandas as pd
import struct
import numpy as np
from sklearn.cluster import KMeans
from sklearn.neighbors import KNeighborsClassifier
from sklearn.model_selection import train_test_split
from datetime import datetime
from sklearn.metrics import pairwise_distances



def evaluate_classification(predicted_labels, true_bin_path):
    """
    Calculate classification accuracy by comparing predicted labels with true labels

    Args:
        predicted_labels (list/np.array): Model's output predictions
        true_bin_path (str): Path to binary file containing true labels

    Returns:
        float: Accuracy percentage (0-100)
    """
    # Read true labels from binary file
    true_labels = []
    with open(true_bin_path, 'rb') as bin_file:
        while True:
            byte_data = bin_file.read(4)  # 4 bytes per integer
            if not byte_data:
                break
            true_labels.append(struct.unpack('i', byte_data)[0])

    # Convert to numpy arrays for vectorized operations
    true_array = np.array(true_labels)
    pred_array = np.array(predicted_labels)

    # Validate lengths match
    if len(true_array) != len(pred_array):
        raise ValueError(f"Length mismatch: {len(true_array)} true vs {len(pred_array)} predicted")

    # Calculate accuracy
    matches = np.sum(true_array == pred_array)
    accuracy = (matches / len(true_array)) * 100

    current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print("\n")
    print("***************** Show This to TA********************")
    print(f"Execution time: {current_time} - Accuracy: {accuracy:.2f}%")
    print("***************** Show This to TA********************")
    print("\n")
    return accuracy


if __name__ == "__main__":
    true_labels = []
    with open("checks_Friday.bin", 'rb') as bin_file:
        while True:
            byte_data = bin_file.read(4)  # 4 bytes per integer
            if not byte_data:
                break
            true_labels.append(struct.unpack('i', byte_data)[0])

    # Convert to numpy arrays for vectorized operations
    true_array = np.array(true_labels)

    print(true_labels)
