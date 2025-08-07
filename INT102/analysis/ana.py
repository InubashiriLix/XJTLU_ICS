import re
import sys
import math
import matplotlib.pyplot as plt


def extract_data(filename):
    """
    从给定文件中提取 epoch、Train Acc 和 Test Acc 数据。
    文件中的每一行形如：
    [Trial #0] Epoch 1/100 | T=10, lr=3.7420e-04, tau=2.95, batch=128, hidden=163 | Train Acc=9.99%, Test Acc=10.00%
    """
    epochs = []
    train_acc = []
    test_acc = []
    with open(filename, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            epoch_match = re.search(r"Epoch (\d+)/", line)
            train_match = re.search(r"Train Acc=([\d.]+)%", line)
            test_match = re.search(r"Test Acc=([\d.]+)%", line)
            if epoch_match and train_match and test_match:
                epochs.append(int(epoch_match.group(1)))
                train_acc.append(float(train_match.group(1)))
                test_acc.append(float(test_match.group(1)))
    return epochs, train_acc, test_acc


def main():
    if len(sys.argv) < 2:
        print("用法：python plot_combined_accuracy.py file1.txt file2.txt ...")
        sys.exit(1)

    files = sys.argv[1:]
    nfiles = len(files)

    # 设置子图排列，例如每行2列（如果文件很多可以调整此值）
    ncols = 2
    nrows = math.ceil(nfiles / ncols)

    # 设置较大的画布尺寸和较高的 DPI 分辨率
    fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=(16, 8 * nrows), dpi=60)

    # 如果只有一个文件，axes 不是数组，将其包装成列表
    if nfiles == 1:
        axes = [axes]
    else:
        axes = axes.flatten()

    for idx, filename in enumerate(files):
        epochs, train, test = extract_data(filename)
        ax = axes[idx]
        ax.plot(epochs, train, marker="o", label="Train Acc")
        ax.plot(epochs, test, marker="x", label="Test Acc")
        ax.set_xlabel("Epoch")
        ax.set_ylabel("Accuracy (%)")
        ax.set_title(f"Accuracy Curve for {filename}")
        ax.legend()
        ax.grid(True)

    # 如果子图数比文件数多，删除多余的子图
    for j in range(idx + 1, len(axes)):
        fig.delaxes(axes[j])

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()
