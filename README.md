## 1. Class/Struct Design
Task Struct
Represents a single task with all required properties.
```
struct Task {
    int id;
    std::string description;
    std::string status;      // "todo", "in-progress", "done"
    std::string createdAt;
    std::string updatedAt;
};
```
## 2. Functions (Procedural Design)
Since the project is simple, it uses free functions for operations:
•	Add Task: Creates a new Task object, assigns values, and appends to the task list.
•	Update Task: Finds a Task by id and updates its fields.
•	Delete Task: Removes a Task from the list by id.
•	Mark Task: Changes the status of a Task.
•	List Tasks: Iterates over the vector of Task objects and prints them.
Example function signature:

```
void cmd_add(const std::string& filename, const std::string& desc);
```

## 3. Object Management
•	Task List:
Uses a std::vector<Task> to store all tasks in memory.
•	Persistence:
Reads from and writes to a JSON file (tasks.json) to persist the vector of Task objects.
---
## 4. File I/O and Serialization
•	Loading:
Reads the JSON file, parses it, and populates the std::vector<Task>.
•	Saving:
Serializes the std::vector<Task> back to JSON and writes to the file.
---
## 5. Command Dispatching
•	Main Function:
Parses command-line arguments and calls the appropriate function (e.g., cmd_add, cmd_update).
---
## 6. Error Handling
•	Checks for file existence, invalid arguments, and missing tasks.
•	Returns user-friendly messages.
---
## In summary:
•	The project uses a Task struct as the main data object.
•	All tasks are managed in a std::vector<Task>.
•	Operations are implemented as free functions.
•	File I/O is used for persistence.
•	The main function acts as the command dispatcher.
