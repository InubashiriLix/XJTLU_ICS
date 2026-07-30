package cpt204.inspection.util;

/**
 * 基于 System.nanoTime() 的简单计时器封装。
 */
public class Timer {
    private long startTimeNs;

    public void start() {
        startTimeNs = System.nanoTime();
    }

    public long stopNanoseconds() {
        return System.nanoTime() - startTimeNs;
    }
}
