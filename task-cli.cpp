#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cstdio>

struct Task {
    int id;
    std::string description;
    std::string status; // "todo", "in-progress", "done"
    std::string createdAt;
    std::string updatedAt;
};

// Helper: Escape JSON string
// Helper: Get current date/time as string  
std::string now() {  
   time_t t = time(nullptr);  
   struct tm tm;  
   localtime_s(&tm, &t); // Use localtime_s to safely convert time_t to tm  
   char buf[32];  
   strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm); // Pass the tm struct to strftime  
   return buf;  
}
std::string json_escape(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        if (c == '\"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else out += c;
    }
    out += "\"";
    return out;
}

// Helper: Parse a simple JSON array of objects (very basic, for this use case)
std::vector<Task> load_tasks(const std::string& filename) {
    std::vector<Task> tasks;
    std::ifstream f(filename);
    if (!f) return tasks;
    std::string line, json;
    while (std::getline(f, line)) json += line;
    size_t pos = 0;
    while ((pos = json.find("{", pos)) != std::string::npos) {
        Task t;
        size_t end = json.find("}", pos);
        std::string obj = json.substr(pos, end - pos + 1);
        // Parse fields
        size_t p;
        if ((p = obj.find("\"id\"")) != std::string::npos)
            t.id = std::stoi(obj.substr(obj.find(":", p) + 1));
        if ((p = obj.find("\"description\"")) != std::string::npos) {
            size_t q1 = obj.find("\"", p + 14) + 1;
            size_t q2 = obj.find("\"", q1);
            t.description = obj.substr(q1, q2 - q1);
        }
        if ((p = obj.find("\"status\"")) != std::string::npos) {
            size_t q1 = obj.find("\"", p + 8) + 1;
            size_t q2 = obj.find("\"", q1);
            t.status = obj.substr(q1, q2 - q1);
        }
        if ((p = obj.find("\"createdAt\"")) != std::string::npos) {
            size_t q1 = obj.find("\"", p + 11) + 1;
            size_t q2 = obj.find("\"", q1);
            t.createdAt = obj.substr(q1, q2 - q1);
        }
        if ((p = obj.find("\"updatedAt\"")) != std::string::npos) {
            size_t q1 = obj.find("\"", p + 11) + 1;
            size_t q2 = obj.find("\"", q1);
            t.updatedAt = obj.substr(q1, q2 - q1);
        }
        tasks.push_back(t);
        pos = end + 1;
    }
    return tasks;
}

// Helper: Save tasks to JSON file
void save_tasks(const std::string& filename, const std::vector<Task>& tasks) {
    std::ofstream f(filename, std::ios::trunc);
    f << "[\n";
    for (size_t i = 0; i < tasks.size(); ++i) {
        const Task& t = tasks[i];
        f << "  {\n";
        f << "    \"id\": " << t.id << ",\n";
        f << "    \"description\": " << json_escape(t.description) << ",\n";
        f << "    \"status\": " << json_escape(t.status) << ",\n";
        f << "    \"createdAt\": " << json_escape(t.createdAt) << ",\n";
        f << "    \"updatedAt\": " << json_escape(t.updatedAt) << "\n";
        f << "  }" << (i + 1 < tasks.size() ? "," : "") << "\n";
    }
    f << "]\n";
}

// Helper: Find task by id
std::vector<Task>::iterator find_task(std::vector<Task>& tasks, int id) {
    return std::find_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
}

// Command: Add
void cmd_add(const std::string& filename, const std::string& desc) {
    std::vector<Task> tasks = load_tasks(filename);
    int max_id = 0;
    for (const auto& t : tasks) if (t.id > max_id) max_id = t.id;
    Task t;
    t.id = max_id + 1;
    t.description = desc;
    t.status = "todo";
    t.createdAt = t.updatedAt = now();
    tasks.push_back(t);
    save_tasks(filename, tasks);
    std::cout << "Task added successfully (ID: " << t.id << ")\n";
}

// Command: Update
void cmd_update(const std::string& filename, int id, const std::string& desc) {
    std::vector<Task> tasks = load_tasks(filename);
    auto it = find_task(tasks, id);
    if (it == tasks.end()) {
        std::cout << "Task not found.\n";
        return;
    }
    it->description = desc;
    it->updatedAt = now();
    save_tasks(filename, tasks);
    std::cout << "Task updated successfully.\n";
}

// Command: Delete
void cmd_delete(const std::string& filename, int id) {
    std::vector<Task> tasks = load_tasks(filename);
    auto it = find_task(tasks, id);
    if (it == tasks.end()) {
        std::cout << "Task not found.\n";
        return;
    }
    tasks.erase(it);
    save_tasks(filename, tasks);
    std::cout << "Task deleted successfully.\n";
}

// Command: Mark in-progress/done
void cmd_mark(const std::string& filename, int id, const std::string& status) {
    std::vector<Task> tasks = load_tasks(filename);
    auto it = find_task(tasks, id);
    if (it == tasks.end()) {
        std::cout << "Task not found.\n";
        return;
    }
    it->status = status;
    it->updatedAt = now();
    save_tasks(filename, tasks);
    std::cout << "Task marked as " << status << ".\n";
}

// Command: List
void cmd_list(const std::string& filename, const std::string& filter = "") {
    std::vector<Task> tasks = load_tasks(filename);
    if (tasks.empty()) {
        std::cout << "No tasks found.\n";
        return;
    }
    for (const auto& t : tasks) {
        if (!filter.empty() && t.status != filter) continue;
        std::cout << "ID: " << t.id << "\n";
        std::cout << "  Description: " << t.description << "\n";
        std::cout << "  Status: " << t.status << "\n";
        std::cout << "  Created At: " << t.createdAt << "\n";
        std::cout << "  Updated At: " << t.updatedAt << "\n";
    }
}

int main(int argc, char* argv[]) {
    const std::string filename = "tasks.json";
    if (argc < 2) {
        std::cout << "Usage: task-cli <command> [args...]\n";
        return 1;
    }
    std::string cmd = argv[1];
    try {
        if (cmd == "add" && argc >= 3) {
            cmd_add(filename, argv[2]);
        }
        else if (cmd == "update" && argc >= 4) {
            cmd_update(filename, std::stoi(argv[2]), argv[3]);
        }
        else if (cmd == "delete" && argc >= 3) {
            cmd_delete(filename, std::stoi(argv[2]));
        }
        else if (cmd == "mark-in-progress" && argc >= 3) {
            cmd_mark(filename, std::stoi(argv[2]), "in-progress");
        }
        else if (cmd == "mark-done" && argc >= 3) {
            cmd_mark(filename, std::stoi(argv[2]), "done");
        }
        else if (cmd == "list") {
            if (argc == 2) cmd_list(filename);
            else if (argc == 3) {
                std::string filter = argv[2];
                if (filter == "done" || filter == "in-progress" || filter == "todo")
                    cmd_list(filename, filter);
                else
                    std::cout << "Unknown status filter.\n";
            }
            else {
                std::cout << "Usage: task-cli list [done|todo|in-progress]\n";
            }
        }
        else {
            std::cout << "Unknown command or wrong arguments.\n";
        }
    }
    catch (...) {
        std::cout << "Error: Invalid arguments or file error.\n";
    }
    return 0;
}