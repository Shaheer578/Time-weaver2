#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <ctime>
#include "dsa_structures.h"
using namespace std;

struct EventDetails {
    int id;
    string description;
    string startTime;  // HH:mm format or empty
    string endTime;    // HH:mm format or empty
    string color;      // Hex color or empty
    bool isPersonal;
    string recurrence; // "none", "daily", "weekly", "monthly"
    string category;   // Category name
    string categoryIcon; // Category emoji icon
};

struct DateEvent {
    string date;       // YYYY-MM-DD
    string description;
};

// Academic user profile (student/teacher meta-data)
struct UserProfile {
    string username;
    string role;        // "student" or "teacher"
    string displayName; // Full name or label
    string university;  // University name
    string department;  // Department code such as CT, ME, EE, CHE
};

// Teacher availability slot (simple recurring weekly slot)
struct TeacherAvailabilitySlot {
    int id;
    string teacherUsername;
    int dayOfWeek;           // 0-6 (Sunday-Saturday)
    string startTime;   // HH:mm
    string endTime;     // HH:mm
    string note;        // Optional note
};

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();
    bool openDatabase();
    void closeDatabase();
    bool createTables();

    // User management
    bool addUser(const string& username, const string& password, const string& role, const string& university = "");
    bool userExists(const string& username, const string& role, const string& university = "");
    bool validateUser(const string& username, const string& password, const string& role, const string& university = "");
    // Admin/user utilities
    vector<string> listUsers(); // returns "username|role"
    bool deleteUser(const string& username, const string& role);
    bool updatePassword(const string& username, const string& role, const string& newPassword);

    // Academic user profiles (university, department, display name)
    bool upsertUserProfile(const UserProfile& profile);
    bool getUserProfile(const string& username, const string& role, UserProfile& outProfile);
    vector<UserProfile> searchTeachers(const string& nameLike, const string& university, const string& department);

    // Event management
    bool addEvent(const string& username, const string& date, const string& event,
                  const string& startTime, const string& endTime, const string& color,
                  bool isPersonal, const string& recurrence, const string& category = "", const string& categoryIcon = "",
                  const string& department = "", int recurrenceCount = 0);
    vector<EventDetails> getEvents(const string& username, const string& date);
    bool updateEvent(int eventId, const string& newDescription,
                     const string& newStartTime, const string& newEndTime, const string& newColor);
    bool deleteEvent(int eventId);
    vector<DateEvent> searchEvents(const string& username, const string& searchTerm, const string& religion);
    vector<string> getEventDatesForMonth(const string& username, int year, int month);
    vector<DateEvent> getAllEvents(const string& username);
    vector<string> getAllSearchableStrings(const string& username, const string& role, const string& religion);

    // Teacher availability
    bool clearTeacherAvailability(const string& teacherUsername);
    bool addTeacherAvailabilitySlot(const TeacherAvailabilitySlot& slot);
    vector<TeacherAvailabilitySlot> getTeacherAvailability(const string& teacherUsername);

    // Holiday management
    bool addHoliday(const string& date, const string& religion, const string& holidayName);
    vector<string> getHolidays(const string& date, const string& religion);
    vector<string> getHolidayDatesForMonth(const string& religion, int year, int month);
    void clearAllHolidays();

    // Task management (student task manager)
    bool addTask(const string& username, const string& title, const string& dueDate);
    vector<string> listTasks(const string& username); // returns "id|title|dueDate|completed"
    bool toggleTask(int taskId, bool completed);
    bool deleteTask(int taskId);
    
    vector<EventDetails> getEventsWithRecurrence(const string& username, const string& targetDate);
    vector<int> searchEventsWithTrie(const string& searchTerm);
    void buildSearchIndex();
    
    string getDepartmentFromRoll(const string& rollNumber);
    string getDepartmentName(const string& code);
    bool isValidRollNumber(const string& rollNumber);
    vector<string> getAllUniversities();
    vector<pair<string, string>> getAllDepartments();

private:
    sqlite3* db;
    bool isOpen;
    EventSearchTrie* searchTrie;
    LRUCache* holidayCache;
    
    void indexEvent(int eventId, const string& description);
    void populateUniversitiesAndDepartments();
};

#endif // DATABASE_H
