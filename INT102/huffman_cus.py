char_dict = {}
codebook = {}
test_string = """
this is a test das est ein test
sadfkhgok;;l]=[poiuytrewqzxcvbnm,.//bin/]=-098976543weasz567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=567890--102=x
ASDFGHJKL;'QWERTYUIOP[]1234567890-ZXCVBNM,./
"""

for char in test_string:
    if char not in char_dict.keys():
        char_dict[char] = 1
    else:
        char_dict[char] += 1


class Node:
    def __init__(self, char, freq):
        self.char = char
        self.freq = freq
        self.left: Node = None
        self.right: Node = None

    def __lt__(self, other):
        return self.freq < other.freq


def find_min_two():
    char1, char2 = 0, 0
    min1, min2 = 0, 0
    for char, v in char_dict.items():
        if v < min1:
            min2 = min1
            char2 = char1
            min1 = v
            char1 = char
        elif v < min2:
            min2 = v
            char2 = char
        char_dict.pop(char1)
        char_dict.pop(char2)

    return (char1, min1), (char2, min2)


def find_min():
    min = 0
    for char, freq in char_dict:
        if freq < min:
            min = freq
            char_min = char
    return char_min, min


node = None
node_list = [Node(char, freq) for char, freq in char_dict.items()]


# sort the nodelist
def sorted() -> bool:
    for i in range(len(node_list) - 1):
        if node_list[i] > node_list[i + 1]:
            return False
    return True


def sort() -> None:
    while not sorted():
        for i in range(len(node_list) - 1):
            # pop sort
            if node_list[i] > node_list[i + 1]:
                node_list[i], node_list[i + 1] = node_list[i + 1], node_list[i]


sort()

while len(node_list) > 1:
    right = node_list.pop(0)
    left = node_list.pop(0)

    merged = Node(None, left.freq + right.freq)
    merged.left = left
    merged.right = right
    node_list.append(merged)
    sort()


def node2codebook(node: Node, code="", codebook={}):
    if node:
        if node.char is not None:
            codebook[node] = code
        node2codebook(node.left, code + "0", codebook)
        node2codebook(node.right, code + "1", codebook)
    return codebook


codebook = node2codebook(node_list[0])


def codebook2code(char):
    for node, code in codebook.items():
        if node.char == char:
            return code
    return "ERROR"


for node, code in codebook.items():
    print(f"{node.char}: {code}")


def encode(string: str):
    rtn = ""
    for char in string:
        rtn += codebook2code(char) + " "

    return rtn


print(encode("woxiang CAO DA BI"))
