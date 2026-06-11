package IterableTest;

import java.util.Comparator;

public class Chronos implements Comparable<Chronos> {
    private int hours;
    private int minutes;

    public static final Chronos ZERO = new Chronos(0, 0);

    public static final Comparator<Chronos> BY_HOURS = Comparator.comparingInt(Chronos::getHours);
    public static final Comparator<Chronos> BY_MINUTES = Comparator.comparingInt(Chronos::getMinutes);
    public static final Comparator<Chronos> BY_TOTAL_MINUTES = Comparator.comparingInt(c -> c.hours * 60 + c.minutes);

    public Chronos() {
        this.hours = 0;
        this.minutes = 0;
    }

    public Chronos(int hours, int minutes) {
        this.hours = hours;
        this.minutes = minutes;
    }

    public int getHours() {
        return hours;
    }

    public int getMinutes() {
        return minutes;
    }

    public void setHours(int hours) {
        this.hours = hours;
    }

    public void setMinutes(int minutes) {
        this.minutes = minutes;
    }

    public void addMinutes(int minutesToAdd) {
        int totalMinutes = this.minutes + minutesToAdd;
        this.hours += totalMinutes / 60;
        this.minutes = totalMinutes % 60;
    }

    public void addHours(int hoursToAdd) {
        this.hours += hoursToAdd;
    }

    public void addTime(Chronos other) {
        if (other == null)
            throw new IllegalArgumentException("Other Chronos object cannot be null.");
        this.addHours(other.getHours());
        this.addMinutes(other.getMinutes());
    }

    public String toString() {
        return String.format("%02d:%02d", hours, minutes);
    }

    public int compareTo(Chronos other) {
        if (this.hours != other.hours) {
            return this.hours - other.hours;
        } else {
            return this.minutes - other.minutes;
        }
    }

    public Chronos clone() {
        return new Chronos(this.hours, this.minutes);
    }

    public Chronos subtract(int hour, int minute) {
        int totalMinutes = this.hours * 60 + this.minutes - (hour * 60 + minute);
        if (totalMinutes < 0) {
            totalMinutes = 0; // Prevent negative time
        }
        this.hours = totalMinutes / 60;
        this.minutes = totalMinutes % 60;
        return this;
    }

    public boolean titik() {
        subtract(1, 0);
        if (this.equals(ZERO)) {
            System.out.println("Stuck in this mement");
            return true;
        }
        return false;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (!(obj instanceof Chronos))
            return false;
        Chronos other = (Chronos) obj;
        return this.hours == other.hours && this.minutes == other.minutes;
    }
}
