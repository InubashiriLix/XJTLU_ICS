package cpt204.inspection.gui;

import cpt204.inspection.graph.AStarSolver;
import cpt204.inspection.graph.BellmanFordSolver;
import cpt204.inspection.graph.BidirectionalDijkstraSolver;
import cpt204.inspection.graph.DijkstraSolver;
import cpt204.inspection.graph.GreedyBestFirstSolver;
import cpt204.inspection.graph.LandmarkHeuristic;
import cpt204.inspection.graph.LearnedHeuristic;
import cpt204.inspection.graph.LinearDijkstraSolver;
import cpt204.inspection.graph.QLearningRouteSolver;
import cpt204.inspection.graph.RouteQuery;
import cpt204.inspection.graph.RouteQueryService;
import cpt204.inspection.graph.SpfaSolver;
import cpt204.inspection.graph.UnweightedBreadthFirstSolver;
import cpt204.inspection.io.CandidateCsvReader;
import cpt204.inspection.io.GraphCsvReader;
import cpt204.inspection.model.CandidateLocation;
import cpt204.inspection.model.Edge;
import cpt204.inspection.model.Graph;
import cpt204.inspection.model.PathResult;
import cpt204.inspection.sorting.MergeSorter;
import cpt204.inspection.sorting.HybridAdaptiveSorter;
import cpt204.inspection.sorting.IntroSorter;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.geometry.Insets;
import javafx.geometry.Point2D;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.TextArea;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.stage.Stage;
import javafx.util.Duration;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class GuiMain extends Application {
    private static final String COLOR_SURFACE = "#f4f5f6";
    private static final String COLOR_PANEL = "#e9edf0";
    private static final String COLOR_TEXT = "#20262b";
    private static final String COLOR_MUTED_TEXT = "#5f6b73";
    private static final String COLOR_BAR = "#4f6f7d";
    private static final String COLOR_ACTIVE = "#c26a2e";
    private static final String COLOR_ROUTE = "#1f6fb2";
    private static final String COLOR_START = "#2f6f4e";
    private static final String COLOR_WAYPOINT = "#9b7b2f";
    private static final String COLOR_DESTINATION = "#8a4f3d";

    private static final String[] CANDIDATE_FILES = {
            "data/candidates_A.csv",
            "data/candidates_B.csv",
            "data/candidates_C.csv"
    };
    private static final String PATHS_CSV = "data/paths.csv";
    private static final int SORT_SAMPLE_SIZE = 36;
    private static final int MAX_CONTEXT_NEIGHBORS_PER_PATH_NODE = 5;
    private static final double GRAPH_CANVAS_WIDTH = 940;
    private static final double GRAPH_CANVAS_HEIGHT = 560;
    private static final String ROUTE_LINEAR_DIJKSTRA = "Linear Dijkstra";
    private static final String ROUTE_HEAP_DIJKSTRA = "Heap Dijkstra (PriorityQueue)";
    private static final String ROUTE_BIDIRECTIONAL_DIJKSTRA = "Bidirectional Dijkstra";
    private static final String ROUTE_SPFA = "SPFA";
    private static final String ROUTE_BFS = "Unweighted BFS (approximate)";
    private static final String ROUTE_ASTAR_LANDMARK = "A* Landmark";
    private static final String ROUTE_GREEDY_LANDMARK = "Greedy Best-First Landmark (approximate)";
    private static final String ROUTE_ASTAR_LEARNED = "A* Learned Heuristic (experimental)";
    private static final String ROUTE_GREEDY_LEARNED = "Greedy Best-First Learned (experimental)";
    private static final String ROUTE_Q_LEARNING = "Q-Learning Routing (experimental)";
    private static final String ROUTE_BELLMAN_FORD = "Bellman-Ford";

    private final CandidateCsvReader candidateReader = new CandidateCsvReader();
    private final GraphCsvReader graphReader = new GraphCsvReader();

    private Canvas sortCanvas;
    private TextArea sortNarration;
    private ComboBox<String> sortDatasetSelect;
    private ComboBox<String> sortAlgorithmSelect;
    private Slider sortSpeedSlider;
    private List<SortFrame> sortFrames = new ArrayList<>();
    private int sortFrameIndex;
    private Timeline sortTimeline;

    private Canvas graphCanvas;
    private TextArea graphNarration;
    private ComboBox<String> routeSelect;
    private ComboBox<String> routeAlgorithmSelect;
    private Graph cachedGraph;
    private List<RouteQuery> cachedQueries;

    @Override
    public void start(Stage stage) {
        TabPane tabs = new TabPane(createSortingTab(), createRouteTab());

        BorderPane root = new BorderPane();
        root.setTop(createHeader());
        root.setCenter(tabs);

        Scene scene = new Scene(root, 1180, 760);
        stage.setTitle("Urban Infrastructure Inspection - Algorithm Visualizer");
        stage.setScene(scene);
        stage.show();

        loadSortingFrames();
        drawEmptyGraph();
    }

    private HBox createHeader() {
        Label title = new Label("Urban Infrastructure Inspection Algorithm Visualizer");
        title.setFont(Font.font("Serif", 22));
        Label subtitle = new Label("Sorting process and shortest-path routing, shown as an educational prototype");
        subtitle.setTextFill(Color.web(COLOR_MUTED_TEXT));

        VBox text = new VBox(2, title, subtitle);
        HBox header = new HBox(text);
        header.setPadding(new Insets(14, 18, 10, 18));
        header.setStyle("-fx-background-color: " + COLOR_PANEL + ";");
        return header;
    }

    private Tab createSortingTab() {
        sortDatasetSelect = new ComboBox<>(FXCollections.observableArrayList(CANDIDATE_FILES));
        sortDatasetSelect.getSelectionModel().select("data/candidates_B.csv");

        sortAlgorithmSelect = new ComboBox<>(FXCollections.observableArrayList(
                "Bubble Sort",
                "Quick Sort",
                "Merge Sort",
                "Intro Sort",
                "Hybrid Adaptive Sort"
        ));
        sortAlgorithmSelect.getSelectionModel().selectFirst();

        sortSpeedSlider = new Slider(20, 500, 90);
        sortSpeedSlider.setShowTickLabels(true);
        sortSpeedSlider.setPrefWidth(180);

        Button loadButton = new Button("Load Frames");
        Button stepButton = new Button("Step");
        Button playButton = new Button("Play");
        Button pauseButton = new Button("Pause");
        Button resetButton = new Button("Reset");

        loadButton.setOnAction(event -> loadSortingFrames());
        stepButton.setOnAction(event -> showNextSortFrame());
        playButton.setOnAction(event -> playSorting());
        pauseButton.setOnAction(event -> stopSorting());
        resetButton.setOnAction(event -> resetSorting());

        HBox controls = new HBox(10,
                new Label("Dataset:"), sortDatasetSelect,
                new Label("Algorithm:"), sortAlgorithmSelect,
                new Label("Speed ms:"), sortSpeedSlider,
                loadButton, stepButton, playButton, pauseButton, resetButton);
        controls.setPadding(new Insets(0, 0, 8, 0));

        sortCanvas = new Canvas(1040, 470);
        sortNarration = new TextArea();
        sortNarration.setEditable(false);
        sortNarration.setPrefRowCount(5);

        VBox content = new VBox(10, controls, sortCanvas, sortNarration);
        content.setPadding(new Insets(14));
        VBox.setVgrow(sortNarration, Priority.ALWAYS);
        return new Tab("Task A Visual Sorting", content);
    }

    private Tab createRouteTab() {
        routeSelect = new ComboBox<>(FXCollections.observableArrayList("Case 1", "Case 2", "Case 3", "Case 4"));
        routeSelect.getSelectionModel().select("Case 2");

        routeAlgorithmSelect = new ComboBox<>(FXCollections.observableArrayList(
                ROUTE_LINEAR_DIJKSTRA,
                ROUTE_HEAP_DIJKSTRA,
                ROUTE_BIDIRECTIONAL_DIJKSTRA,
                ROUTE_SPFA,
                ROUTE_BFS,
                ROUTE_ASTAR_LANDMARK,
                ROUTE_GREEDY_LANDMARK,
                ROUTE_ASTAR_LEARNED,
                ROUTE_GREEDY_LEARNED,
                ROUTE_Q_LEARNING,
                ROUTE_BELLMAN_FORD
        ));
        routeAlgorithmSelect.getSelectionModel().select(ROUTE_HEAP_DIJKSTRA);
        routeAlgorithmSelect.setPrefWidth(360);

        Button visualizeButton = new Button("Visualize Route");
        visualizeButton.setOnAction(event -> visualizeRoute());

        HBox controls = new HBox(10,
                new Label("Route:"), routeSelect,
                new Label("Algorithm:"), routeAlgorithmSelect,
                visualizeButton);
        controls.setPadding(new Insets(0, 0, 8, 0));

        graphCanvas = new Canvas(GRAPH_CANVAS_WIDTH, GRAPH_CANVAS_HEIGHT);
        graphNarration = new TextArea();
        graphNarration.setEditable(false);
        graphNarration.setPrefRowCount(6);

        VBox content = new VBox(10, controls, graphCanvas, graphNarration);
        content.setPadding(new Insets(14));
        VBox.setVgrow(graphNarration, Priority.ALWAYS);
        return new Tab("Task B Route Map", content);
    }

    private void loadSortingFrames() {
        stopSorting();
        sortFrameIndex = 0;
        try {
            CandidateLocation[] sample = readSortingSample(sortDatasetSelect.getValue());
            String algorithm = sortAlgorithmSelect.getValue();

            if ("Quick Sort".equals(algorithm)) {
                sortFrames = quickSortFrames(sample);
            } else if ("Merge Sort".equals(algorithm)) {
                sortFrames = mergeSortFrames(sample);
            } else if ("Intro Sort".equals(algorithm)) {
                sortFrames = introSortFrames(sample);
            } else if ("Hybrid Adaptive Sort".equals(algorithm)) {
                sortFrames = hybridAdaptiveFrames(sample);
            } else {
                sortFrames = bubbleSortFrames(sample);
            }

            drawSortFrame(sortFrames.get(0));
            sortNarration.setText("Loaded " + sortFrames.size() + " animation frames for " + algorithm
                    + ". Each bar is one candidate location; taller bars have higher priority score.");
        } catch (Exception e) {
            sortNarration.setText("Failed to load sorting frames: " + e.getMessage());
        }
    }

    private CandidateLocation[] readSortingSample(String csvPath) throws IOException {
        List<CandidateLocation> locations = candidateReader.read(csvPath);
        return locations.subList(0, Math.min(SORT_SAMPLE_SIZE, locations.size())).toArray(new CandidateLocation[0]);
    }

    private List<SortFrame> bubbleSortFrames(CandidateLocation[] input) {
        List<SortFrame> frames = new ArrayList<>();
        CandidateLocation[] data = Arrays.copyOf(input, input.length);
        frames.add(new SortFrame(data, -1, -1, "Initial sample before Bubble Sort."));

        for (int i = 0; i < data.length - 1; i++) {
            boolean swapped = false;
            for (int j = 0; j < data.length - 1 - i; j++) {
                frames.add(new SortFrame(data, j, j + 1, "Compare " + data[j] + " and " + data[j + 1] + "."));
                if (data[j].compareTo(data[j + 1]) > 0) {
                    swap(data, j, j + 1);
                    swapped = true;
                    frames.add(new SortFrame(data, j, j + 1, "Swap because the left item should come later."));
                }
            }
            if (!swapped) {
                frames.add(new SortFrame(data, -1, -1, "No swap in this pass, so Bubble Sort stops early."));
                break;
            }
        }
        frames.add(new SortFrame(data, -1, -1, "Bubble Sort complete. Top candidates are now at the left."));
        return frames;
    }

    private List<SortFrame> quickSortFrames(CandidateLocation[] input) {
        List<SortFrame> frames = new ArrayList<>();
        CandidateLocation[] data = Arrays.copyOf(input, input.length);
        frames.add(new SortFrame(data, -1, -1, "Initial sample before Quick Sort."));
        quickSort(data, 0, data.length - 1, frames);
        frames.add(new SortFrame(data, -1, -1, "Quick Sort complete. Partitions have been combined in-place."));
        return frames;
    }

    private void quickSort(CandidateLocation[] data, int low, int high, List<SortFrame> frames) {
        if (low >= high) {
            return;
        }

        CandidateLocation pivot = data[high];
        int i = low - 1;
        frames.add(new SortFrame(data, high, high, "Choose pivot " + pivot + " for range " + low + "-" + high + "."));

        for (int j = low; j < high; j++) {
            frames.add(new SortFrame(data, j, high, "Compare " + data[j] + " with pivot " + pivot + "."));
            if (data[j].compareTo(pivot) <= 0) {
                i++;
                swap(data, i, j);
                frames.add(new SortFrame(data, i, j, "Move item into the left partition."));
            }
        }

        swap(data, i + 1, high);
        frames.add(new SortFrame(data, i + 1, high, "Place pivot between the two partitions."));
        quickSort(data, low, i, frames);
        quickSort(data, i + 2, high, frames);
    }

    private List<SortFrame> introSortFrames(CandidateLocation[] input) {
        List<SortFrame> frames = new ArrayList<>();
        CandidateLocation[] data = Arrays.copyOf(input, input.length);
        int depthLimit = 2 * floorLog2(Math.max(1, data.length));
        frames.add(new SortFrame(data, -1, -1,
                "Initial sample before Intro Sort. Start with quicksort; use insertion for small ranges; heapsort if depth is too high."));
        introSort(data, 0, data.length - 1, depthLimit, frames);
        insertionSortRange(data, 0, data.length, frames, "Final insertion pass cleans up small partitions.");
        frames.add(new SortFrame(data, -1, -1, "Intro Sort complete."));
        return frames;
    }

    private void introSort(CandidateLocation[] data, int low, int high, int depthLimit, List<SortFrame> frames) {
        while (high - low + 1 > 10) {
            if (depthLimit == 0) {
                frames.add(new SortFrame(data, low, high,
                        "Depth limit reached. Intro Sort switches to Heap Sort to avoid quicksort worst case."));
                heapSortRange(data, low, high, frames);
                return;
            }

            int mid = low + (high - low) / 2;
            int pivotIndex = medianOfThree(data, low, mid, high);
            frames.add(new SortFrame(data, pivotIndex, pivotIndex,
                    "Median-of-three chooses pivot " + data[pivotIndex] + " for range " + low + "-" + high + "."));
            swap(data, pivotIndex, high);

            int partitionIndex = introPartition(data, low, high, frames);
            frames.add(new SortFrame(data, partitionIndex, partitionIndex,
                    "Pivot placed. Left side <= pivot, right side >= pivot."));

            depthLimit--;
            introSort(data, partitionIndex + 1, high, depthLimit, frames);
            high = partitionIndex - 1;
        }

        if (low < high) {
            frames.add(new SortFrame(data, low, high,
                    "Small range " + low + "-" + high + " is left for insertion sort."));
        }
    }

    private int introPartition(CandidateLocation[] data, int low, int high, List<SortFrame> frames) {
        CandidateLocation pivot = data[high];
        int i = low - 1;

        for (int j = low; j < high; j++) {
            frames.add(new SortFrame(data, j, high, "Intro Sort compares " + data[j] + " with pivot " + pivot + "."));
            if (data[j].compareTo(pivot) <= 0) {
                i++;
                swap(data, i, j);
                frames.add(new SortFrame(data, i, j, "Move item into the lower partition."));
            }
        }

        swap(data, i + 1, high);
        return i + 1;
    }

    private List<SortFrame> hybridAdaptiveFrames(CandidateLocation[] input) {
        List<SortFrame> frames = new ArrayList<>();
        CandidateLocation[] data = Arrays.copyOf(input, input.length);
        DataProfile profile = profile(data);
        frames.add(new SortFrame(data, -1, -1,
                "Hybrid starts by profiling the input: local break ratio = " + formatRatio(profile.breakRatio())
                        + ", duplicate score ratio = " + formatRatio(profile.duplicateScoreRatio()) + "."));

        if (profile.breakRatio() <= 0.08) {
            frames.add(new SortFrame(data, -1, -1,
                    "Detected nearly sorted data. Use local repair instead of full O(n log n) sorting."));
            repairLocalBreaks(data, frames);
            frames.add(new SortFrame(data, -1, -1, "Hybrid Adaptive Sort complete after local repair."));
            return frames;
        }

        if (profile.duplicateScoreRatio() >= 0.20) {
            frames.add(new SortFrame(data, -1, -1,
                    "Detected many equal priority scores. Use stable Merge Sort to preserve clear tie handling."));
            CandidateLocation[] temp = new CandidateLocation[data.length];
            mergeSort(data, temp, 0, data.length - 1, frames);
            frames.add(new SortFrame(data, -1, -1, "Hybrid Adaptive Sort complete after Merge Sort branch."));
            return frames;
        }

        frames.add(new SortFrame(data, -1, -1,
                "Detected mostly random data. Use STL-style Intro Sort for robust general performance."));
        introSort(data, 0, data.length - 1, 2 * floorLog2(Math.max(1, data.length)), frames);
        insertionSortRange(data, 0, data.length, frames, "Final insertion pass after Intro branch.");
        frames.add(new SortFrame(data, -1, -1, "Hybrid Adaptive Sort complete after Intro Sort branch."));
        return frames;
    }

    private void repairLocalBreaks(CandidateLocation[] data, List<SortFrame> frames) {
        for (int i = 1; i < data.length; i++) {
            frames.add(new SortFrame(data, i - 1, i, "Check whether this adjacent pair is a local break."));
            if (data[i - 1].compareTo(data[i]) > 0) {
                CandidateLocation current = data[i];
                int j = i - 1;
                while (j >= 0 && data[j].compareTo(current) > 0) {
                    data[j + 1] = data[j];
                    frames.add(new SortFrame(data, j, j + 1, "Shift larger item right to repair local break."));
                    j--;
                }
                data[j + 1] = current;
                frames.add(new SortFrame(data, j + 1, i, "Insert the displaced item into its correct local position."));
            }
        }
    }

    private void insertionSortRange(CandidateLocation[] data, int fromInclusive, int toExclusive,
            List<SortFrame> frames, String messagePrefix) {
        frames.add(new SortFrame(data, fromInclusive, toExclusive - 1, messagePrefix));
        for (int i = fromInclusive + 1; i < toExclusive; i++) {
            CandidateLocation current = data[i];
            int j = i - 1;
            while (j >= fromInclusive && data[j].compareTo(current) > 0) {
                data[j + 1] = data[j];
                frames.add(new SortFrame(data, j, j + 1, "Insertion Sort shifts item right."));
                j--;
            }
            data[j + 1] = current;
            frames.add(new SortFrame(data, j + 1, i, "Insertion Sort places the held item."));
        }
    }

    private void heapSortRange(CandidateLocation[] data, int low, int high, List<SortFrame> frames) {
        int size = high - low + 1;

        for (int i = size / 2 - 1; i >= 0; i--) {
            heapify(data, size, i, low, frames);
        }

        for (int i = size - 1; i > 0; i--) {
            swap(data, low, low + i);
            frames.add(new SortFrame(data, low, low + i, "Heap Sort moves current maximum to the end of the range."));
            heapify(data, i, 0, low, frames);
        }
    }

    private void heapify(CandidateLocation[] data, int size, int root, int offset, List<SortFrame> frames) {
        int largest = root;
        int left = 2 * root + 1;
        int right = 2 * root + 2;

        if (left < size && data[offset + left].compareTo(data[offset + largest]) > 0) {
            largest = left;
        }
        if (right < size && data[offset + right].compareTo(data[offset + largest]) > 0) {
            largest = right;
        }

        if (largest != root) {
            swap(data, offset + root, offset + largest);
            frames.add(new SortFrame(data, offset + root, offset + largest, "Heapify swaps parent with larger child."));
            heapify(data, size, largest, offset, frames);
        }
    }

    private int medianOfThree(CandidateLocation[] data, int first, int second, int third) {
        CandidateLocation a = data[first];
        CandidateLocation b = data[second];
        CandidateLocation c = data[third];

        if (a.compareTo(b) < 0) {
            if (b.compareTo(c) < 0) {
                return second;
            }
            return a.compareTo(c) < 0 ? third : first;
        }

        if (a.compareTo(c) < 0) {
            return first;
        }
        return b.compareTo(c) < 0 ? third : second;
    }

    private int floorLog2(int value) {
        int log = 0;
        while (value > 1) {
            value >>= 1;
            log++;
        }
        return log;
    }

    private DataProfile profile(CandidateLocation[] data) {
        int orderBreaks = 0;
        for (int i = 0; i < data.length - 1; i++) {
            if (data[i].compareTo(data[i + 1]) > 0) {
                orderBreaks++;
            }
        }

        int[] scores = new int[data.length];
        for (int i = 0; i < data.length; i++) {
            scores[i] = data[i].getPriorityScore();
        }
        Arrays.sort(scores);

        int duplicateScoreCount = 0;
        int currentRun = 1;
        for (int i = 1; i < scores.length; i++) {
            if (scores[i] == scores[i - 1]) {
                currentRun++;
            } else {
                if (currentRun > 1) {
                    duplicateScoreCount += currentRun;
                }
                currentRun = 1;
            }
        }
        if (currentRun > 1) {
            duplicateScoreCount += currentRun;
        }

        return new DataProfile(
                orderBreaks / (double) Math.max(1, data.length - 1),
                duplicateScoreCount / (double) data.length
        );
    }

    private String formatRatio(double value) {
        return String.format("%.2f%%", value * 100.0);
    }

    private List<SortFrame> mergeSortFrames(CandidateLocation[] input) {
        List<SortFrame> frames = new ArrayList<>();
        CandidateLocation[] data = Arrays.copyOf(input, input.length);
        CandidateLocation[] temp = new CandidateLocation[data.length];
        frames.add(new SortFrame(data, -1, -1, "Initial sample before Merge Sort."));
        mergeSort(data, temp, 0, data.length - 1, frames);
        frames.add(new SortFrame(data, -1, -1, "Merge Sort complete. The sorted array is rebuilt from sorted halves."));
        return frames;
    }

    private void mergeSort(CandidateLocation[] data, CandidateLocation[] temp, int left, int right,
            List<SortFrame> frames) {
        if (left >= right) {
            return;
        }

        int mid = left + (right - left) / 2;
        mergeSort(data, temp, left, mid, frames);
        mergeSort(data, temp, mid + 1, right, frames);

        int i = left;
        int j = mid + 1;
        int k = left;
        while (i <= mid && j <= right) {
            if (data[i].compareTo(data[j]) <= 0) {
                temp[k++] = data[i++];
            } else {
                temp[k++] = data[j++];
            }
        }
        while (i <= mid) {
            temp[k++] = data[i++];
        }
        while (j <= right) {
            temp[k++] = data[j++];
        }
        for (int index = left; index <= right; index++) {
            data[index] = temp[index];
            frames.add(new SortFrame(data, index, -1, "Write merged item at index " + index + "."));
        }
    }

    private void showNextSortFrame() {
        if (sortFrames.isEmpty()) {
            return;
        }
        sortFrameIndex = Math.min(sortFrameIndex + 1, sortFrames.size() - 1);
        drawSortFrame(sortFrames.get(sortFrameIndex));
    }

    private void playSorting() {
        stopSorting();
        sortTimeline = new Timeline(new KeyFrame(Duration.millis(sortSpeedSlider.getValue()), event -> {
            if (sortFrameIndex >= sortFrames.size() - 1) {
                stopSorting();
            } else {
                showNextSortFrame();
            }
        }));
        sortTimeline.setCycleCount(Timeline.INDEFINITE);
        sortTimeline.play();
    }

    private void stopSorting() {
        if (sortTimeline != null) {
            sortTimeline.stop();
        }
    }

    private void resetSorting() {
        stopSorting();
        sortFrameIndex = 0;
        if (!sortFrames.isEmpty()) {
            drawSortFrame(sortFrames.get(0));
        }
    }

    private void drawSortFrame(SortFrame frame) {
        GraphicsContext gc = sortCanvas.getGraphicsContext2D();
        double width = sortCanvas.getWidth();
        double height = sortCanvas.getHeight();
        gc.setFill(Color.web(COLOR_SURFACE));
        gc.fillRect(0, 0, width, height);

        CandidateLocation[] data = frame.data();
        int min = Arrays.stream(data).mapToInt(CandidateLocation::getPriorityScore).min().orElse(0);
        int max = Arrays.stream(data).mapToInt(CandidateLocation::getPriorityScore).max().orElse(1);
        double range = Math.max(1, max - min);
        double chartHeight = height - 110;
        double barWidth = (width - 48) / data.length;

        gc.setFill(Color.web(COLOR_TEXT));
        gc.setFont(Font.font("Serif", 18));
        gc.fillText("Frame " + (sortFrameIndex + 1) + " / " + sortFrames.size(), 24, 28);
        gc.setFont(Font.font("Serif", 14));
        gc.fillText(frame.message(), 24, 52);

        for (int i = 0; i < data.length; i++) {
            double normalized = (data[i].getPriorityScore() - min) / range;
            double barHeight = 28 + normalized * (chartHeight - 28);
            double x = 24 + i * barWidth;
            double y = height - 46 - barHeight;

            if (i == frame.activeA() || i == frame.activeB()) {
                gc.setFill(Color.web(COLOR_ACTIVE));
            } else {
                gc.setFill(Color.web(COLOR_BAR));
            }
            gc.fillRect(x, y, Math.max(3, barWidth - 3), barHeight);
            gc.setFill(Color.web(COLOR_TEXT));
            if (barWidth > 20) {
                gc.fillText(data[i].getLocationId().substring(1), x, height - 24);
            }
        }
    }

    private void visualizeRoute() {
        try {
            Graph graph = getGraph();
            List<RouteQuery> queries = getQueries();
            RouteQuery query = queries.get(routeSelect.getSelectionModel().getSelectedIndex());
            RouteQueryService service = createRouteService(routeAlgorithmSelect.getValue(), graph, queries);
            PathResult result = runQuery(graph, service, query);
            drawGraph(graph, query, result);
            graphNarration.setText(routeExplanation(query, result, routeAlgorithmSelect.getValue()));
        } catch (Exception e) {
            graphNarration.setText("Failed to visualize route: " + e.getMessage());
        }
    }

    private RouteQueryService createRouteService(String algorithm, Graph graph, List<RouteQuery> queries) {
        if (ROUTE_LINEAR_DIJKSTRA.equals(algorithm)) {
            return new RouteQueryService(new LinearDijkstraSolver());
        }
        if (ROUTE_HEAP_DIJKSTRA.equals(algorithm)) {
            return new RouteQueryService(new DijkstraSolver());
        }
        if (ROUTE_BIDIRECTIONAL_DIJKSTRA.equals(algorithm)) {
            return new RouteQueryService(new BidirectionalDijkstraSolver());
        }
        if (ROUTE_SPFA.equals(algorithm)) {
            return new RouteQueryService(new SpfaSolver());
        }
        if (ROUTE_BFS.equals(algorithm)) {
            return new RouteQueryService(new UnweightedBreadthFirstSolver());
        }
        if (ROUTE_ASTAR_LANDMARK.equals(algorithm)) {
            return new RouteQueryService(new AStarSolver(new LandmarkHeuristic(graph, buildLandmarks())));
        }
        if (ROUTE_GREEDY_LANDMARK.equals(algorithm)) {
            return new RouteQueryService(new GreedyBestFirstSolver(new LandmarkHeuristic(graph, buildLandmarks())));
        }
        if (ROUTE_ASTAR_LEARNED.equals(algorithm)) {
            return new RouteQueryService(new AStarSolver(new LearnedHeuristic(graph, buildLandmarks())));
        }
        if (ROUTE_GREEDY_LEARNED.equals(algorithm)) {
            return new RouteQueryService(new GreedyBestFirstSolver(new LearnedHeuristic(graph, buildLandmarks())));
        }
        if (ROUTE_Q_LEARNING.equals(algorithm)) {
            QLearningRouteSolver solver = new QLearningRouteSolver();
            solver.train(graph, collectRouteDestinations(queries));
            return new RouteQueryService(solver);
        }
        if (ROUTE_BELLMAN_FORD.equals(algorithm)) {
            return new RouteQueryService(new BellmanFordSolver());
        }
        return new RouteQueryService(new DijkstraSolver());
    }

    private Graph getGraph() throws IOException {
        if (cachedGraph == null) {
            cachedGraph = graphReader.read(PATHS_CSV);
        }
        return cachedGraph;
    }

    private List<RouteQuery> getQueries() throws IOException {
        if (cachedQueries == null) {
            cachedQueries = buildRequiredQueries();
        }
        return cachedQueries;
    }

    private List<RouteQuery> buildRequiredQueries() throws IOException {
        CandidateLocation[] topA = readTopLocations("data/candidates_A.csv");
        CandidateLocation[] topB = readTopLocations("data/candidates_B.csv");
        CandidateLocation[] topC = readTopLocations("data/candidates_C.csv");

        String a1 = topA[0].getLocationId();
        String a10 = topA[9].getLocationId();
        String b1 = topB[0].getLocationId();
        String b5 = topB[4].getLocationId();
        String c1 = topC[0].getLocationId();
        String c5 = topC[4].getLocationId();

        return Arrays.asList(
                RouteQuery.direct("Case 1", "A1 to A1", a1, a1),
                RouteQuery.direct("Case 2", "A1 to A10", a1, a10),
                RouteQuery.via("Case 3", "A1 to B1 via B5", a1, b1, b5),
                RouteQuery.via("Case 4", "A1 to C1 via B5 and C5", a1, c1, b5, c5)
        );
    }

    private CandidateLocation[] readTopLocations(String csvPath) throws IOException {
        List<CandidateLocation> locations = candidateReader.read(csvPath);
        CandidateLocation[] sorted = locations.toArray(new CandidateLocation[0]);
        new MergeSorter().sort(sorted);
        return Arrays.copyOf(sorted, 10);
    }

    private PathResult runQuery(Graph graph, RouteQueryService service, RouteQuery query) {
        String[] waypoints = query.getWaypoints();
        if (waypoints.length == 0) {
            return service.direct(graph, query.getStartLocation(), query.getDestinationLocation());
        }
        return service.via(graph, query.getStartLocation(), query.getDestinationLocation(), waypoints);
    }

    private void drawEmptyGraph() {
        GraphicsContext gc = graphCanvas.getGraphicsContext2D();
        gc.setFill(Color.web(COLOR_SURFACE));
        gc.fillRect(0, 0, graphCanvas.getWidth(), graphCanvas.getHeight());
        gc.setFill(Color.web(COLOR_MUTED_TEXT));
        gc.setFont(Font.font("Serif", 18));
        gc.fillText("Choose a route and algorithm, then visualize the graph.", 28, 42);
    }

    private void drawGraph(Graph graph, RouteQuery query, PathResult result) {
        GraphicsContext gc = graphCanvas.getGraphicsContext2D();
        double width = graphCanvas.getWidth();
        double height = graphCanvas.getHeight();
        gc.setFill(Color.web(COLOR_SURFACE));
        gc.fillRect(0, 0, width, height);

        RouteSubgraph subgraph = buildRouteSubgraph(graph, result);
        Map<String, Point2D> positions = buildRouteFocusedPositions(subgraph, width, height);
        drawContextEdges(gc, graph, subgraph, positions);
        drawContextNodes(gc, subgraph, positions);

        if (result.isReachable()) {
            drawRoute(gc, result.getPath(), positions);
        }
        drawRouteMarkers(gc, query, positions);

        gc.setFill(Color.web(COLOR_TEXT));
        gc.setFont(Font.font("Serif", 17));
        gc.fillText(query.getCaseName() + "  " + query.getDescription(), 24, 28);
        gc.setFont(Font.font("Serif", 13));
        gc.setFill(Color.web(COLOR_MUTED_TEXT));
        gc.fillText("Showing route-focused subgraph: path nodes plus nearby connected context nodes.", 24, 48);
    }

    private RouteSubgraph buildRouteSubgraph(Graph graph, PathResult result) {
        if (!result.isReachable()) {
            return new RouteSubgraph(new ArrayList<>(), new HashSet<>(), new HashSet<>(), new HashSet<>());
        }

        List<String> path = result.getPath();
        Set<String> pathNodes = new HashSet<>(path);
        Set<String> contextNodes = new HashSet<>(pathNodes);
        Set<String> contextEdges = new HashSet<>();

        for (int i = 0; i < path.size() - 1; i++) {
            contextEdges.add(edgeKey(path.get(i), path.get(i + 1)));
        }

        for (String pathNode : path) {
            int addedNeighbors = 0;
            for (Edge edge : graph.getEdgesFrom(pathNode)) {
                String neighbor = edge.getToLocation();
                if (!pathNodes.contains(neighbor) && addedNeighbors >= MAX_CONTEXT_NEIGHBORS_PER_PATH_NODE) {
                    continue;
                }

                contextNodes.add(neighbor);
                contextEdges.add(edgeKey(pathNode, neighbor));
                if (!pathNodes.contains(neighbor)) {
                    addedNeighbors++;
                }
            }
        }

        return new RouteSubgraph(path, pathNodes, contextNodes, contextEdges);
    }

    private Map<String, Point2D> buildRouteFocusedPositions(RouteSubgraph subgraph, double width, double height) {
        Map<String, Point2D> positions = new HashMap<>();
        List<String> path = subgraph.path();
        double left = 70;
        double right = width - 70;
        double centerY = height * 0.52;
        double step = path.size() <= 1 ? 0 : (right - left) / (path.size() - 1);

        for (int i = 0; i < path.size(); i++) {
            double x = path.size() <= 1 ? width / 2.0 : left + i * step;
            double y = centerY + Math.sin(i * 0.9) * 34;
            positions.put(path.get(i), new Point2D(x, y));
        }

        Map<String, Integer> neighborCounts = new HashMap<>();
        for (String node : subgraph.contextNodes()) {
            if (positions.containsKey(node)) {
                continue;
            }

            String anchor = nearestPathAnchor(node, subgraph.pathNodes(), path);
            Point2D anchorPoint = positions.get(anchor);
            int count = neighborCounts.getOrDefault(anchor, 0);
            neighborCounts.put(anchor, count + 1);

            int side = count % 2 == 0 ? -1 : 1;
            int layer = count / 2 + 1;
            double offsetX = (side * 24) + ((layer % 2 == 0) ? 12 : -12);
            double offsetY = side * (52 + layer * 26);
            positions.put(node, new Point2D(
                    clamp(anchorPoint.getX() + offsetX, 28, width - 28),
                    clamp(anchorPoint.getY() + offsetY, 72, height - 34)
            ));
        }

        return positions;
    }

    private String nearestPathAnchor(String node, Set<String> pathNodes, List<String> path) {
        String bestAnchor = path.isEmpty() ? node : path.get(0);
        int bestDistance = Integer.MAX_VALUE;

        for (String pathNode : pathNodes) {
            int distance = Math.abs(parseLocationNumber(node) - parseLocationNumber(pathNode));
            if (distance < bestDistance) {
                bestDistance = distance;
                bestAnchor = pathNode;
            }
        }

        return bestAnchor;
    }

    private void drawContextEdges(GraphicsContext gc, Graph graph, RouteSubgraph subgraph, Map<String, Point2D> positions) {
        gc.setLineWidth(1.0);

        for (String from : subgraph.contextNodes()) {
            Point2D fromPoint = positions.get(from);
            if (fromPoint == null) {
                continue;
            }
            for (Edge edge : graph.getEdgesFrom(from)) {
                String to = edge.getToLocation();
                String key = edgeKey(from, to);
                if (subgraph.contextEdges().contains(key)) {
                    Point2D toPoint = positions.get(to);
                    if (toPoint != null) {
                        gc.setStroke(subgraph.pathNodes().contains(from) && subgraph.pathNodes().contains(to)
                                ? Color.rgb(83, 96, 105, 0.34)
                                : Color.rgb(95, 107, 115, 0.18));
                        gc.strokeLine(fromPoint.getX(), fromPoint.getY(), toPoint.getX(), toPoint.getY());
                    }
                }
            }
        }
    }

    private void drawContextNodes(GraphicsContext gc, RouteSubgraph subgraph, Map<String, Point2D> positions) {
        for (String locationId : subgraph.contextNodes()) {
            Point2D point = positions.get(locationId);
            if (point == null) {
                continue;
            }
            if (subgraph.pathNodes().contains(locationId)) {
                gc.setFill(Color.rgb(71, 86, 96, 0.82));
                gc.fillOval(point.getX() - 4.0, point.getY() - 4.0, 8.0, 8.0);
            } else {
                gc.setFill(Color.rgb(90, 104, 112, 0.30));
                gc.fillOval(point.getX() - 2.8, point.getY() - 2.8, 5.6, 5.6);
            }
        }
    }

    private void drawRoute(GraphicsContext gc, List<String> path, Map<String, Point2D> positions) {
        gc.setStroke(Color.web(COLOR_ROUTE));
        gc.setLineWidth(3.2);
        for (int i = 0; i < path.size() - 1; i++) {
            Point2D from = positions.get(path.get(i));
            Point2D to = positions.get(path.get(i + 1));
            gc.strokeLine(from.getX(), from.getY(), to.getX(), to.getY());
        }

        gc.setFill(Color.web(COLOR_ROUTE));
        for (String locationId : path) {
            Point2D point = positions.get(locationId);
            gc.fillOval(point.getX() - 4.0, point.getY() - 4.0, 8.0, 8.0);
        }
    }

    private void drawRouteMarkers(GraphicsContext gc, RouteQuery query, Map<String, Point2D> positions) {
        drawMarker(gc, positions.get(query.getStartLocation()), query.getStartLocation(), Color.web(COLOR_START));
        for (String waypoint : query.getWaypoints()) {
            drawMarker(gc, positions.get(waypoint), waypoint, Color.web(COLOR_WAYPOINT));
        }
        drawMarker(gc, positions.get(query.getDestinationLocation()), query.getDestinationLocation(),
                Color.web(COLOR_DESTINATION));
    }

    private void drawMarker(GraphicsContext gc, Point2D point, String label, Color color) {
        if (point == null) {
            return;
        }
        gc.setFill(color);
        gc.fillOval(point.getX() - 7, point.getY() - 7, 14, 14);
        gc.setFill(Color.web(COLOR_TEXT));
        gc.setFont(Font.font("Serif", 13));
        gc.fillText(label, point.getX() + 8, point.getY() - 8);
    }

    private String routeExplanation(RouteQuery query, PathResult result, String algorithm) {
        if (!result.isReachable()) {
            return algorithm + " could not find a route for " + query.getCaseName() + ".";
        }

        return algorithm + " visualized for " + query.getCaseName()
                + "\nStart: " + query.getStartLocation()
                + "\nDestination: " + query.getDestinationLocation()
                + "\nWaypoints: " + query.getWaypointsLabel()
                + "\nTotal cost: " + result.getTotalCost()
                + "\nPath: " + String.join(" -> ", result.getPath())
                + "\nAlgorithm note: " + routeAlgorithmNote(algorithm)
                + "\n\nThe pale network shows only the route-focused subgraph: path nodes and nearby connected context nodes. The red line is the selected route. "
                + "Green marks the start, gold marks waypoint nodes, and red marks the destination.";
    }

    private String routeAlgorithmNote(String algorithm) {
        if (ROUTE_LINEAR_DIJKSTRA.equals(algorithm)) {
            return "Exact shortest path; scans unsettled nodes linearly, so it is mainly a baseline.";
        }
        if (ROUTE_HEAP_DIJKSTRA.equals(algorithm)) {
            return "Exact shortest path; uses a PriorityQueue and is the main formal solver.";
        }
        if (ROUTE_BIDIRECTIONAL_DIJKSTRA.equals(algorithm)) {
            return "Exact shortest path for this undirected non-negative graph; searches from both ends.";
        }
        if (ROUTE_SPFA.equals(algorithm)) {
            return "Exact shortest path variant based on Bellman-Ford relaxation with a queue.";
        }
        if (ROUTE_BFS.equals(algorithm)) {
            return "Approximate for weighted graphs; minimizes hop count rather than total edge cost.";
        }
        if (ROUTE_ASTAR_LANDMARK.equals(algorithm)) {
            return "Exact when the landmark heuristic remains admissible; uses precomputed landmark distances.";
        }
        if (ROUTE_GREEDY_LANDMARK.equals(algorithm)) {
            return "Approximate; follows the heuristic aggressively and does not guarantee minimum cost.";
        }
        if (ROUTE_ASTAR_LEARNED.equals(algorithm)) {
            return "Experimental exact-search style using a learned heuristic; quality depends on the heuristic.";
        }
        if (ROUTE_GREEDY_LEARNED.equals(algorithm)) {
            return "Experimental approximate search; fast but does not guarantee minimum cost.";
        }
        if (ROUTE_Q_LEARNING.equals(algorithm)) {
            return "Experimental learned routing policy; training runs before visualization.";
        }
        if (ROUTE_BELLMAN_FORD.equals(algorithm)) {
            return "Exact shortest path; supports negative edges in general, but is slower on this non-negative graph.";
        }
        return "Default exact route solver.";
    }

    private String edgeKey(String first, String second) {
        return first.compareTo(second) < 0 ? first + "-" + second : second + "-" + first;
    }

    private double clamp(double value, double min, double max) {
        return Math.max(min, Math.min(max, value));
    }

    private List<String> buildLandmarks() {
        return Arrays.asList("L0001", "L0010", "L0101", "L0110", "L0201", "L0210");
    }

    private List<String> collectRouteDestinations(List<RouteQuery> queries) {
        List<String> destinations = new ArrayList<>();
        for (RouteQuery query : queries) {
            for (String waypoint : query.getWaypoints()) {
                if (!destinations.contains(waypoint)) {
                    destinations.add(waypoint);
                }
            }
            if (!destinations.contains(query.getDestinationLocation())) {
                destinations.add(query.getDestinationLocation());
            }
        }
        return destinations;
    }

    private int parseLocationNumber(String locationId) {
        try {
            return Integer.parseInt(locationId.substring(1));
        } catch (RuntimeException e) {
            return 0;
        }
    }

    private void swap(CandidateLocation[] data, int first, int second) {
        CandidateLocation temp = data[first];
        data[first] = data[second];
        data[second] = temp;
    }

    public static void main(String[] args) {
        launch(args);
    }

    private record SortFrame(CandidateLocation[] data, int activeA, int activeB, String message) {
        private SortFrame {
            data = Arrays.copyOf(data, data.length);
        }
    }

    private record DataProfile(double breakRatio, double duplicateScoreRatio) {
    }

    private record RouteSubgraph(List<String> path, Set<String> pathNodes, Set<String> contextNodes,
            Set<String> contextEdges) {
    }
}
