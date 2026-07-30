package cpt204.inspection.io;

import cpt204.inspection.model.CandidateLocation;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * 读取候选 CSV 文件。
 * 预期列：location_id, priority_score
 */
public class CandidateCsvReader {
    public List<CandidateLocation> read(String filePath) throws IOException {
        List<CandidateLocation> locations = new ArrayList<>();

        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            String line;
            boolean firstLine = true;
            while ((line = reader.readLine()) != null) {
                if (line.trim().isEmpty()) {
                    continue;
                }
                if (firstLine) {
                    firstLine = false;
                    if (line.toLowerCase().contains("location_id")) {
                        continue;
                    }
                }
                String[] parts = line.split(",");
                if (parts.length < 2) {
                    throw new IOException("Invalid candidate CSV row: " + line);
                }
                String locationId = parts[0].trim();
                int priorityScore = Integer.parseInt(parts[1].trim());
                locations.add(new CandidateLocation(locationId, priorityScore));
            }
        }

        return locations;
    }
}
