#ifndef DSA_STRUCTURES_H
#define DSA_STRUCTURES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <ctime>
#include <iostream>
using namespace std;

struct Event {
    int id;
    string username;
    string date;
    string description;
    string startTime;
    string endTime;
    string color;
    bool isPersonal;
    string recurrence;
    string category;
    string categoryIcon;
    string department;
};

class EventSearchTrie {
private:
    struct TrieNode {
        unordered_map<char, TrieNode*> children;
        vector<int> eventIds;
        bool isEndOfWord;
        
        TrieNode() : isEndOfWord(false) {}
    };
    
    TrieNode* root;
    
    void toLowerStr(string& str) {
        transform(str.begin(), str.end(), str.begin(), 
            [](unsigned char c){ return tolower(c); });
    }
    
public:
    EventSearchTrie() {
        root = new TrieNode();
    }
    
    void insert(const string& keyword, int eventId) {
        string lowerKeyword = keyword;
        toLowerStr(lowerKeyword);
        
        TrieNode* current = root;
        for (char c : lowerKeyword) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
        }
        current->isEndOfWord = true;
        current->eventIds.push_back(eventId);
    }
    
    vector<int> search(const string& prefix) {
        string lowerPrefix = prefix;
        toLowerStr(lowerPrefix);
        
        TrieNode* current = root;
        for (char c : lowerPrefix) {
            if (current->children.find(c) == current->children.end()) {
                return vector<int>();
            }
            current = current->children[c];
        }
        
        vector<int> results;
        collectAllEventIds(current, results);
        return results;
    }
    
    void collectAllEventIds(TrieNode* node, vector<int>& results) {
        if (node->isEndOfWord) {
            results.insert(results.end(), node->eventIds.begin(), node->eventIds.end());
        }
        for (auto& pair : node->children) {
            collectAllEventIds(pair.second, results);
        }
    }
    
    ~EventSearchTrie() {
        deleteTrie(root);
    }
    
    void deleteTrie(TrieNode* node) {
        if (!node) return;
        for (auto& pair : node->children) {
            deleteTrie(pair.second);
        }
        delete node;
    }
};

class EventScheduler {
private:
    struct EventComparator {
        bool operator()(const Event& a, const Event& b) {
            if (a.date != b.date) {
                return a.date > b.date;
            }
            return a.startTime > b.startTime;
        }
    };
    
    priority_queue<Event, vector<Event>, EventComparator> eventHeap;
    
public:
    void addEvent(const Event& e) {
        eventHeap.push(e);
    }
    
    vector<Event> getEventsForDate(const string& date) {
        vector<Event> result;
        vector<Event> temp;
        
        while (!eventHeap.empty()) {
            Event e = eventHeap.top();
            eventHeap.pop();
            temp.push_back(e);
            
            if (e.date == date) {
                result.push_back(e);
            }
        }
        
        for (const Event& e : temp) {
            eventHeap.push(e);
        }
        
        sort(result.begin(), result.end(), [](const Event& a, const Event& b) {
            return a.startTime < b.startTime;
        });
        
        return result;
    }
    
    Event getNextEvent() {
        if (eventHeap.empty()) {
            return Event();
        }
        return eventHeap.top();
    }
    
    bool isEmpty() {
        return eventHeap.empty();
    }
};

class LRUCache {
private:
    struct Node {
        string key;
        string value;
        Node* prev;
        Node* next;
        
        Node(const string& k, const string& v) 
            : key(k), value(v), prev(nullptr), next(nullptr) {}
    };
    
    int capacity;
    unordered_map<string, Node*> cache;
    Node* head;
    Node* tail;
    
    void addToFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }
    
    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    void moveToFront(Node* node) {
        removeNode(node);
        addToFront(node);
    }
    
public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node("", "");
        tail = new Node("", "");
        head->next = tail;
        tail->prev = head;
    }
    
    string get(const string& key) {
        if (cache.find(key) == cache.end()) {
            return "";
        }
        Node* node = cache[key];
        moveToFront(node);
        return node->value;
    }
    
    void put(const string& key, const string& value) {
        if (cache.find(key) != cache.end()) {
            Node* node = cache[key];
            node->value = value;
            moveToFront(node);
            return;
        }
        
        if (cache.size() >= (size_t)capacity) {
            Node* lru = tail->prev;
            removeNode(lru);
            cache.erase(lru->key);
            delete lru;
        }
        
        Node* newNode = new Node(key, value);
        cache[key] = newNode;
        addToFront(newNode);
    }
    
    ~LRUCache() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
};

class RecurrenceEngine {
public:
    enum RecurrenceType {
        NONE,
        DAILY,
        WEEKLY,
        MONTHLY
    };
    
    static RecurrenceType stringToType(const string& str) {
        if (str == "daily") return DAILY;
        if (str == "weekly") return WEEKLY;
        if (str == "monthly") return MONTHLY;
        return NONE;
    }
    
    static vector<string> generateRecurringDates(
        const string& startDate, 
        RecurrenceType type, 
        int count = 365
    ) {
        vector<string> dates;
        if (type == NONE) {
            dates.push_back(startDate);
            return dates;
        }
        
        struct tm tm = {};
        sscanf(startDate.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        
        for (int i = 0; i < count; i++) {
            time_t t = mktime(&tm);
            struct tm* result = localtime(&t);
            
            char buffer[11];
            sprintf(buffer, "%04d-%02d-%02d", 
                result->tm_year + 1900, 
                result->tm_mon + 1, 
                result->tm_mday);
            dates.push_back(string(buffer));
            
            switch (type) {
                case DAILY:
                    tm.tm_mday += 1;
                    break;
                case WEEKLY:
                    tm.tm_mday += 7;
                    break;
                case MONTHLY:
                    tm.tm_mon += 1;
                    break;
                default:
                    break;
            }
        }
        
        return dates;
    }
    
    static bool matchesRecurrence(
        const string& eventDate,
        const string& targetDate,
        RecurrenceType type
    ) {
        if (type == NONE) {
            return eventDate == targetDate;
        }
        
        int eYear, eMonth, eDay;
        int tYear, tMonth, tDay;
        
        sscanf(eventDate.c_str(), "%d-%d-%d", &eYear, &eMonth, &eDay);
        sscanf(targetDate.c_str(), "%d-%d-%d", &tYear, &tMonth, &tDay);
        
        if (targetDate < eventDate) return false;
        
        switch (type) {
            case DAILY:
                return true;
            case WEEKLY: {
                struct tm eTm = {0, 0, 0, eDay, eMonth - 1, eYear - 1900};
                struct tm tTm = {0, 0, 0, tDay, tMonth - 1, tYear - 1900};
                mktime(&eTm);
                mktime(&tTm);
                return eTm.tm_wday == tTm.tm_wday;
            }
            case MONTHLY:
                return eDay == tDay;
            default:
                return false;
        }
    }
};

class EventBST {
private:
    struct Node {
        string time;
        vector<int> eventIds;
        Node* left;
        Node* right;
        
        Node(const string& t, int id) : time(t), left(nullptr), right(nullptr) {
            eventIds.push_back(id);
        }
    };
    
    Node* root;
    
    Node* insert(Node* node, const string& time, int eventId) {
        if (!node) return new Node(time, eventId);
        
        if (time < node->time) {
            node->left = insert(node->left, time, eventId);
        } else if (time > node->time) {
            node->right = insert(node->right, time, eventId);
        } else {
            node->eventIds.push_back(eventId);
        }
        return node;
    }
    
    void findInRange(Node* node, const string& start, const string& end, vector<int>& result) {
        if (!node) return;
        
        if (node->time > start) {
            findInRange(node->left, start, end, result);
        }
        
        if (node->time >= start && node->time <= end) {
            result.insert(result.end(), node->eventIds.begin(), node->eventIds.end());
        }
        
        if (node->time < end) {
            findInRange(node->right, start, end, result);
        }
    }
    
    void deleteTree(Node* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
    
public:
    EventBST() : root(nullptr) {}
    
    ~EventBST() {
        deleteTree(root);
    }
    
    void insert(const string& time, int eventId) {
        root = insert(root, time, eventId);
    }
    
    vector<int> findEventsInRange(const string& start, const string& end) {
        vector<int> result;
        findInRange(root, start, end, result);
        return result;
    }
    
    vector<int> findConflicts(const string& time) {
        return findEventsInRange(time, time);
    }
};

#endif
