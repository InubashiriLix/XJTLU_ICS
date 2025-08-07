package TodoList;

public class Task {
    private String detail;
    private Priority priority;

    public String getDetail() {
        return detail;
    }

    public Priority getPriority() {
        return priority;
    }

    public Task(Priority pri, String detail) {
        this.detail = detail;
        this.priority = pri;
    }

    public boolean equals(Task other) {
        if (this.detail.equals(other.detail) && this.priority.equals(other.priority)) {
            return true;
        }
        return false;
    }
}
