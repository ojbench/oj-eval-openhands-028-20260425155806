
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cmath>

using namespace std;

struct Student {
    string name;
    char gender;
    int class_num;
    vector<int> scores;
    
    Student() : scores(9, 0) {}
    
    Student(const string& n, char g, int c, const vector<int>& s) 
        : name(n), gender(g), class_num(c), scores(s) {}
    
    int get_avg_score() const {
        int sum = 0;
        for (int score : scores) {
            sum += score;
        }
        return sum / scores.size();  // floor division
    }
    
    // Comparison operator for ranking
    bool operator<(const Student& other) const {
        int my_avg = get_avg_score();
        int other_avg = other.get_avg_score();
        
        if (my_avg != other_avg) {
            return my_avg > other_avg;  // Higher average score first
        }
        
        // Compare scores from 0 to 8
        for (int i = 0; i < 9; i++) {
            if (scores[i] != other.scores[i]) {
                return scores[i] > other.scores[i];
            }
        }
        
        // If still tied, compare name lexicographically
        return name < other.name;
    }
};

class StudentManager {
private:
    unordered_map<string, Student> students;
    set<Student> ranking;
    bool started = false;
    
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
        
        Student student(name, gender, class_num, scores);
        students[name] = student;
    }
    
    void start_statistics() {
        started = true;
        // Build initial ranking
        for (const auto& pair : students) {
            ranking.insert(pair.second);
        }
    }
    
    void update_score(const string& name, int code, int score) {
        auto it = students.find(name);
        if (it == students.end()) {
            cout << "[Error]Update failed.\n";
            return;
        }
        
        // Remove from ranking if started
        if (started) {
            auto rank_it = ranking.find(it->second);
            if (rank_it != ranking.end()) {
                ranking.erase(rank_it);
            }
        }
        
        // Update score
        it->second.scores[code] = score;
        
        // Add back to ranking if started
        if (started) {
            ranking.insert(it->second);
        }
    }
    
    void flush_ranking() {
        // Rebuild ranking from current student data
        ranking.clear();
        for (const auto& pair : students) {
            ranking.insert(pair.second);
        }
    }
    
    void print_list() const {
        int rank = 1;
        for (const Student& student : ranking) {
            cout << rank << " " << student.name << " " 
                 << (student.gender == 'M' ? "male" : "female") << " "
                 << student.class_num << " " << student.get_avg_score() << "\n";
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
        
        // Find rank in the set
        int rank = 1;
        for (const Student& student : ranking) {
            if (student.name == name) {
                cout << "STUDENT " << name << " NOW AT RANKING " << rank << "\n";
                return;
            }
            rank++;
        }
    }
    
    bool is_started() const {
        return started;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
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
