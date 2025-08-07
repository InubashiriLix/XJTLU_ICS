import heapq
from collections import defaultdict


class HuffmanNode:
    def __init__(self, char, freq):
        self.char = char
        self.freq = freq
        self.left = None
        self.right = None

    def __lt__(self, other):
        return self.freq < other.freq


def build_huffman_tree(frequency):
    heap = [HuffmanNode(char, freq) for char, freq in frequency.items()]
    heapq.heapify(heap)

    while len(heap) > 1:
        left = heapq.heappop(heap)
        right = heapq.heappop(heap)
        merged = HuffmanNode(None, left.freq + right.freq)
        merged.left = left
        merged.right = right
        heapq.heappush(heap, merged)

    return heap[0]


def generate_huffman_codes(node, code="", codebook={}):
    if node:
        if node.char is not None:
            codebook[node.char] = code
        generate_huffman_codes(node.left, code + "0", codebook)
        generate_huffman_codes(node.right, code + "1", codebook)
    return codebook


def huffman_encode(text, codebook):
    return "".join(codebook[char] for char in text)


def huffman_decode(encoded_text, tree):
    decoded_text = []
    node = tree
    for bit in encoded_text:
        node = node.left if bit == "0" else node.right
        if node.char is not None:
            decoded_text.append(node.char)
            node = tree
    return "".join(decoded_text)


# 示例输入文本
text = "hello huffman"

# 统计字符频率
frequency = defaultdict(int)
for char in text:
    frequency[char] += 1

# 构建 Huffman 树
huffman_tree = build_huffman_tree(frequency)

# 生成 Huffman 编码表
huffman_codebook = generate_huffman_codes(huffman_tree)

# 编码
encoded_text = huffman_encode(text, huffman_codebook)

# 解码
decoded_text = huffman_decode(encoded_text, huffman_tree)

# 输出结果
print("原始文本:", text)
print("字符频率:", dict(frequency))
print("Huffman 编码表:", huffman_codebook)
print("编码后的文本:", encoded_text)
print("解码后的文本:", decoded_text)
