
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>

using namespace std;

struct Student {
    string name;
    char gender;
    int class_num;
    int scores[9];
    int avg_score;
    bool dirty;  // flag to indicate if scores need recalculation
    
    Student() : scores{0}, avg_score(0), dirty(false) {}
    
    void calculate_avg() {
        int sum = 0;
        for (int i = 0; i < 9; i++) {
            sum += scores[i];
        }
        avg_score = sum / 9;
        dirty = false;
    }
    
    void set_score(int code, int score) {
        scores[code] = score;
        dirty = true;
    }
    
    int get_avg_score() const {
        if (dirty) {
            const_cast<Student*>(this)->calculate_avg();
        }
        return avg_score;
    }
};

struct StudentComparator {
    bool operator()(const Student* a, const Student* b) const {
        int avg_a = a->get_avg_score();
        int avg_b = b->get_avg_score();
        
        if (avg_a != avg_b) {
            return avg_a > avg_b;  // Higher average score first
        }
        
        // Compare scores from 0 to 8
        for (int i = 0; i < 9; i++) {
            if (a->scores[i] != b->scores[i]) {
                return a->scores[i] > b->scores[i];
            }
        }
        
        // If still tied, compare name lexicographically
        return a->name < b->name;
    }
};

class StudentManager {
private:
    unordered_map<string, Student> students;
    set<Student*, StudentComparator> ranking;
    unordered_map<string, int> cached_ranks;
    bool started = false;
    bool ranks_dirty = false;
    
    void update_all_ranks() {
        if (!ranks_dirty) return;
        
        int rank = 1;
        for (const Student* student : ranking) {
            cached_ranks[student->name] = rank;
            rank++;
        }
        ranks_dirty = false;
    }
    
public:
    void add_student(const string& name, char gender, int class_num, const vector<int>& scores) {
        if (started) {
            cout << "[Error]Cannot add student now.\n";
            return;
        }
        
        if (students.find(name) != students.end()) {
            cout << "[Error]Add failed.\n";
            return;
        }
        
        Student student;
        student.name = name;
        student.gender = gender;
        student.class_num = class_num;
        for (int i = 0; i < 9; i++) {
            student.scores[i] = scores[i];
        }
        student.calculate_avg();
        
        students[name] = student;
    }
    
    void start_statistics() {
        started = true;
        // Build initial ranking
        for (auto& pair : students) {
            ranking.insert(&pair.second);
        }
        ranks_dirty = true;
    }
    
    void update_score(const string& name, int code, int score) {
        auto it = students.find(name);
        if (it == students.end()) {
            cout << "[Error]Update failed.\n";
            return;
        }
        
        // Update score but don't modify ranking until FLUSH
        it->second.set_score(code, score);
    }
    
    void flush_ranking() {
        // Only rebuild if there are dirty students
        bool has_dirty = false;
        for (const auto& pair : students) {
            if (pair.second.dirty) {
                has_dirty = true;
                break;
            }
        }
        
        if (!has_dirty) {
            // Just update ranks if no scores changed
            ranks_dirty = true;
            return;
        }
        
        // Rebuild ranking from current student data
        ranking.clear();
        for (auto& pair : students) {
            if (pair.second.dirty) {
                pair.second.calculate_avg();
            }
            ranking.insert(&pair.second);
        }
        ranks_dirty = true;
    }
    
    void print_list() const {
        int rank = 1;
        for (const Student* student : ranking) {
            cout << rank << " " << student->name << " " 
                 << (student->gender == 'M' ? "male" : "female") << " "
                 << student->class_num << " " << student->get_avg_score() << "\n";
            rank++;
        }
    }
    
    void query_student(const string& name) const {
        auto it = students.find(name);
        if (it == students.end()) {
            cout << "[Error]Query failed.\n";
            return;
        }
        
        if (!started) {
            cout << "[Error]Query failed.\n";
            return;
        }
        
        // Update ranks if needed and use cached value
        const_cast<StudentManager*>(this)->update_all_ranks();
        auto rank_it = cached_ranks.find(name);
        if (rank_it != cached_ranks.end()) {
            cout << "STUDENT " << name << " NOW AT RANKING " << rank_it->second << "\n";
        }
    }
    
    bool is_started() const {
        return started;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    
    StudentManager manager;
    string command;
    
    while (cin >> command) {
        if (command == "ADD") {
            string name;
            char gender;
            int class_num;
            vector<int> scores(9);
            
            cin >> name >> gender >> class_num;
            for (int i = 0; i < 9; i++) {
                cin >> scores[i];
            }
            
            manager.add_student(name, gender, class_num, scores);
        }
        else if (command == "START") {
            manager.start_statistics();
        }
        else if (command == "UPDATE") {
            string name;
            int code, score;
            cin >> name >> code >> score;
            manager.update_score(name, code, score);
        }
        else if (command == "FLUSH") {
            manager.flush_ranking();
        }
        else if (command == "PRINTLIST") {
            manager.print_list();
        }
        else if (command == "QUERY") {
            string name;
            cin >> name;
            manager.query_student(name);
        }
        else if (command == "END") {
            break;
        }
    }
    
    return 0;
}
