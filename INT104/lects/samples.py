import numpy as np
import random

population = np.arange(1, 101)  # 100 sample
n_sampale = 10  # 10 samples


def simple_sample():
    simple_sample = np.random.choice(population, size=n_sampale, replace=False)
    print("simple Random sample:", simple_sample)


def stratified_sample():
    N = 100
    # construat a population of 100 smaples
    data = np.arange(N)

    # construct a 100 labels
    labels = np.zeros(N, dtype=float)
    labels[30:60] = 1.0
    labels[60:] = 2.0

    # specify the number we want from each
    samples_sizes = {0.0: 6, 1.0: 3, 2.0: 1}

    stratified_indices = []
    for stratum, size in samples_sizes.items():
        indices_in_stratum = np.where(labels == stratum)[0]
        chosen = np.random.choice(indices_in_stratum, size=size, replace=False)
        stratified_indices.append(chosen)

    # put every indices in the stratum into together
    stratified_indices = np.concatenate(stratified_indices)
    stratified_sample = data[stratified_indices]
    stratified_labels = labels[stratified_indices]

    print(f"the samples are: {stratified_sample}")
    print(f"stratified labels: {stratified_labels}")


def proportional_startified_sampling():
    N = 100
    # construat a population of 100 smaples
    data = np.arange(N)

    # construct a 100 labels
    labels = np.zeros(N, dtype=float)
    labels[30:60] = 1.0
    labels[60:] = 2.0

    fraction = 0.2
    proportional_indices = []

    for stratum in np.unique(labels):
        # find all the sample in curent labels
        idx_in_stratum = np.where(labels == stratum)[0]
        # pick randomly k indices from this stratum
        chosen = np.random.choice(
            idx_in_stratum, size=int(fraction * len(idx_in_stratum)), replace=False
        )
        proportional_indices.append(chosen)

    stratified_indices = np.concatenate(proportional_indices)
    stratified_data = data[stratified_indices]
    stratified_labels = labels[stratified_indices]

    print(f"stratified sample: {stratified_data}")
    print(f"stratified labels: {stratified_labels}")


if __name__ == "__main__":
    # simple_sample()
    # stratified_sample()
    proportional_startified_sampling()
