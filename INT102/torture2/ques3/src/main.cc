#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

// std::string ptn = "TCCTATTCTT";
// std::string ori = "TTATAGATCTGGTATTCTTTTATAGATCTCCTATTCTT";
std::string ptn = "ABC";
std::string ori = "CCABC";

const size_t size_ptn = ptn.size();
const size_t size_ori = ori.size();

bool normal() {
    int cnt = 0;
    for (int i = 0; i <= size_ori - size_ptn; i++) {
        for (int j = 0; j < size_ptn; j++) {
            char char_ptn = ptn[j];
            char char_ori = ori[i + j];
            volatile int flag = 1;
            if (char_ptn != char_ori) {
                cnt = 0;
                break;
            } else {
                cnt++;
                if (cnt >= size_ptn) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool horspool(const std::string& ptn, const std::string& ori) {
    const int size_ptn = ptn.size();
    const int size_ori = ori.size();

    std::unordered_map<char, int> shift;

    for (int i = 0; i < 256; i++) {
        shift[char(i)] = size_ptn;
    }

    for (int i = 0; i < size_ptn; i++) {
        shift[ptn[i]] = size_ptn - 1 - i;
    }

    bool found = false;
    int i = 0;
    while (i <= size_ori - size_ptn) {
        int j = size_ptn - 1;
        while (j >= 0 && ptn[j] == ori[j + i]) j--;
        if (j < 0) {
            std::cout << "found" << std::endl;
            found = true;
            j += size_ptn;
            break;
        } else {
            i += shift[ori[i + size_ptn - 1]];
        }
    }
    return found;
}

int main() {
    std::cout << "Hell Here" << std::endl;
    // std::cout << normal() << std::endl;
    std::cout << horspool(ptn, ori) << std::endl;

    return 0;
}
