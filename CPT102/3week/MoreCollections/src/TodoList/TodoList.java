package TodoList;

import ArrayList.List;

import javax.naming.directory.NoSuchAttributeException;

import ArrayList.ArrayList;

public class TodoList {
    private List<Task> todoList;

    public TodoList() {
        todoList = new ArrayList<Task>();
    }

    private Priority getPri(int pri) throws NoSuchAttributeException {
        switch (pri) {
            case 0: {
                return Priority.HIGH;
            }
            case 1: {
                return Priority.MID;
            }
            case 2: {
                return Priority.LOW;
            }
            case 3: {
                return Priority.TINY;
            }
            default: {
                throw new NoSuchAttributeException("No such priority");
            }
        }

    }

    public int isInList(Task targetTask) {
        int tmp_idx = -1;
        for (var task : todoList) {
            tmp_idx++;
            if (task.equals(targetTask))
                return tmp_idx;
        }
        return tmp_idx;
    }

    public boolean addTask(String detail, int pri) {
        try {
            var priority = getPri(pri);
            var newTask = new Task(priority, detail);
            todoList.add(newTask);
            return true;
        } catch (NoSuchAttributeException e) {
            return false;
        }
    }

    public boolean removeTask(String detail, int pri) {
        try {
            var priority = getPri(pri);
            var newTask = new Task(priority, detail);
            return todoList.remove(newTask);
        } catch (NoSuchAttributeException e) {
            return false;
        }
    }

}
