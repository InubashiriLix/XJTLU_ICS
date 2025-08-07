package Comparable;

import java.util.Collections;
import java.util.List;
import java.util.ArrayList;

public class FaceExample {
    private List<FaceComparable> faces = new ArrayList<>();

    public FaceExample() {
        faces.add(new FaceComparable(1.0, 2.0));
        faces.add(new FaceComparable(2.0, 3.0));
        faces.add(new FaceComparable(3.0, 4.0));
        faces.add(new FaceComparable(4.0, 5.0));

        Collections.sort(faces, new FaceComparator());

        for (var face : faces) {
            System.out.println("Face size: " + face.size());
        }
    }

}
