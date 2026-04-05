import java.util.function.Function;

public class StopWatch {
    private long startTime;
    private long endTime;

    public StopWatch() {
        this.startTime = System.currentTimeMillis();
        this.endTime = startTime;
    }

    private void start() {
        this.startTime = System.currentTimeMillis();
    }

    private void stop() {
        this.endTime = System.currentTimeMillis();
    }

    public long getElapseTime(Runnable task) {
        start();
        task.run();
        stop();
        return this.endTime - this.startTime;
    }
}
