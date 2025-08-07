#include <stdio.h>

#include <iostream>
#include <unordered_map>

std::string text = "thisisatestandnolongersufferingthisshit";
std::string ptn = "test";

int main() {
    std::unordered_map<char, int> shift;

    int text_len = text.length();
    int ptn_len = ptn.length();

    for (int i = 0; i < 256; i++) {
        shift[char(i)] = ptn_len;
    }

    for (int i = 0; i < ptn_len - 1; i++) {
        shift[ptn[i]] = ptn_len - 1 - i;
    }

    // for (const auto& pair : shift) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }

    int i = 0;
    bool found = false;

    while (i <= text_len - ptn_len) {  // match from the head to the emd of text ptn
        // compare the ptn from the left  to  right side
        int j = ptn_len - 1;
        while (j >= 0 && ptn[j] == text[j + i]) {
            j--;
        }
        if (j < 0) {
            std::cout << "found one" << std::endl;
            found = true;
            //! skip the ptn len
            j += ptn_len;
            break;

        } else {
            i += shift[text[i + ptn_len - 1]];
        }
    }

    if (!found) std::cout << "not found";

    return 0;
}
