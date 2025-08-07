package Map;

import java.util.Map;
import java.util.HashMap;

public class MapExample {

    private Map<String, Integer> wordMap = new HashMap<String, Integer>();

    public MapExample(String input) {
        CountWordFrequency(input);
        IterWordMap();
        findMaxCount(wordMap);
    }

    private void CountWordFrequency(final String input) {
        for (String word : input.split(" ")) {
            if (wordMap.containsKey(word)) {
                wordMap.put(word, wordMap.get(word) + 1);
            } else {
                wordMap.put(word, 1);
            }
        }
    }

    private void IterWordMap() {
        StringBuilder sb = new StringBuilder();
        // NOTE: method 1
        // for (String key : wordMap.keySet()) {
        // var value = wordMap.get(key);
        // if (value != null) {
        // sb.append(key + " " + value + "\n");
        // }
        // }
        // NOTE: method 2, but the key is not supported
        // for (int value : wordMap.values()) {
        // sb.append(value + "\n");
        // }
        // NOTE: method 3
        for (Map.Entry<String, Integer> entry : wordMap.entrySet()) {
            sb.append(entry.getKey() + ":" + entry.getValue() + "\n");
        }
        System.out.println(sb.toString());
    }

    private String findMaxCount(final Map<String, Integer> counts) {
        String rtnKey = "";
        int rtnCnt = 0;

        if (wordMap.isEmpty()) {
            System.out.println("the wordMap is empty");
            return null;
        }

        for (var entry : wordMap.entrySet()) {
            if (entry.getValue() > rtnCnt) {
                rtnKey = entry.getKey();
                rtnCnt = entry.getValue();
            }
        }

        System.out.println("found max frequency count: " + rtnKey + " : " + rtnCnt);
        return rtnKey;
    }
}
