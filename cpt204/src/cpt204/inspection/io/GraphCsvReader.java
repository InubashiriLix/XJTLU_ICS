package cpt204.inspection.io;

import cpt204.inspection.model.Graph;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

/**
 * 读取 paths.csv 并构建无向加权图。
 * 预期列：from_location, to_location, weight
 */
public class GraphCsvReader {
    public Graph read(String filePath) throws IOException {
        Graph graph = new Graph();

        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            String line;
            boolean firstLine = true;
            while ((line = reader.readLine()) != null) {
                if (line.trim().isEmpty()) {
                    continue;
                }
                if (firstLine) {
                    firstLine = false;
                    if (line.toLowerCase().contains("from_location")) {
                        continue;
                    }
                }
                String[] parts = line.split(",");
                if (parts.length < 3) {
                    throw new IOException("Invalid graph CSV row: " + line);
                }
                String fromLocation = parts[0].trim();
                String toLocation = parts[1].trim();
                double weight = Double.parseDouble(parts[2].trim());
                graph.addUndirectedEdge(fromLocation, toLocation, weight);
            }
        }

        return graph;
    }
}
