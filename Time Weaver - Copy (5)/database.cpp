#include "database.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <regex>
#include <ctime>
using namespace std;

DatabaseManager::DatabaseManager() : db(nullptr), isOpen(false) {
    searchTrie = new EventSearchTrie();
    holidayCache = new LRUCache(100);
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
    delete searchTrie;
    delete holidayCache;
}

bool DatabaseManager::openDatabase() {
    if (isOpen) return true;
    
    int rc = sqlite3_open("calendar.db", &db);
    if (rc != SQLITE_OK) {
        return false;
    }
    isOpen = true;
    return true;
}

void DatabaseManager::closeDatabase() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    isOpen = false;
}

bool DatabaseManager::createTables() {
    if (!isOpen) return false;

    const char* usersTable = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "password TEXT, "
        "role TEXT NOT NULL, "
        "university TEXT, "
        "UNIQUE(username, role, university));";

    const char* eventsTable = 
        "CREATE TABLE IF NOT EXISTS events ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "event_date DATE NOT NULL, "
        "event_description TEXT NOT NULL, "
        "start_time TEXT, "
        "end_time TEXT, "
        "color TEXT, "
        "is_personal BOOLEAN NOT NULL, "
        "recurrence_type TEXT NOT NULL DEFAULT 'none', "
        "recurrence_count INTEGER DEFAULT 0, "
        "category TEXT, "
        "category_icon TEXT, "
        "department TEXT);";

    const char* holidaysTable = 
        "CREATE TABLE IF NOT EXISTS holidays ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "religion TEXT NOT NULL, "
        "holiday_date DATE NOT NULL, "
        "holiday_name TEXT NOT NULL);";

    const char* tasksTable =
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "title TEXT NOT NULL, "
        "due_date DATE, "
        "completed INTEGER NOT NULL DEFAULT 0);";

    // Academic user profiles
    const char* profilesTable =
        "CREATE TABLE IF NOT EXISTS user_profiles ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "role TEXT NOT NULL, "
        "display_name TEXT, "
        "university TEXT, "
        "department TEXT, "
        "UNIQUE(username, role, university));";

    // Teacher availability (weekly slots)
    const char* availabilityTable =
        "CREATE TABLE IF NOT EXISTS teacher_availability ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "teacher_username TEXT NOT NULL, "
        "day_of_week INTEGER NOT NULL, "
        "start_time TEXT NOT NULL, "
        "end_time TEXT NOT NULL, "
        "note TEXT);";

    const char* universitiesTable =
        "CREATE TABLE IF NOT EXISTS universities ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE);";

    const char* departmentsTable =
        "CREATE TABLE IF NOT EXISTS departments ("
        "code TEXT PRIMARY KEY, "
        "name TEXT NOT NULL);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, usersTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db, eventsTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db, holidaysTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db, tasksTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, profilesTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, availabilityTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, universitiesTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, departmentsTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    
    populateUniversitiesAndDepartments();
    
    // Add category columns if they don't exist (migration for existing databases)
    const char* addCategoryColumn = "ALTER TABLE events ADD COLUMN category TEXT;";
    const char* addCategoryIconColumn = "ALTER TABLE events ADD COLUMN category_icon TEXT;";
    const char* addDepartmentColumn = "ALTER TABLE events ADD COLUMN department TEXT;";
    
    // These may fail if columns already exist, which is fine
    sqlite3_exec(db, addCategoryColumn, nullptr, nullptr, &errMsg);
    if (errMsg) sqlite3_free(errMsg);
    
    sqlite3_exec(db, addCategoryIconColumn, nullptr, nullptr, &errMsg);
    if (errMsg) sqlite3_free(errMsg);

    sqlite3_exec(db, addDepartmentColumn, nullptr, nullptr, &errMsg);
    if (errMsg) sqlite3_free(errMsg);
    
    return true;
}

bool DatabaseManager::addUser(const string& username, const string& password, const string& role, const string& university) {
    if (userExists(username, role, university)) {
        return false;  // User already exists - return false for all roles
    }

    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password, role, university) VALUES (?, ?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, university.c_str(), -1, SQLITE_STATIC);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::userExists(const string& username, const string& role, const string& university) {
    sqlite3_stmt* stmt;
    const char* sql;
    if (university.empty()) {
        // For backward compatibility (personal/admin users)
        sql = "SELECT username FROM users WHERE username = ? AND role = ? AND (university IS NULL OR university = '')";
    } else {
        sql = "SELECT username FROM users WHERE username = ? AND role = ? AND university = ?";
    }
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_STATIC);
    if (!university.empty()) {
        sqlite3_bind_text(stmt, 3, university.c_str(), -1, SQLITE_STATIC);
    }

    bool exists = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return exists;
}

bool DatabaseManager::validateUser(const string& username, const string& password, const string& role, const string& university) {
    sqlite3_stmt* stmt;
    const char* sql;
    if (university.empty()) {
        // For backward compatibility (personal/admin users)
        sql = "SELECT password FROM users WHERE username = ? AND role = ? AND (university IS NULL OR university = '')";
    } else {
        sql = "SELECT password FROM users WHERE username = ? AND role = ? AND university = ?";
    }
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_STATIC);
    if (!university.empty()) {
        sqlite3_bind_text(stmt, 3, university.c_str(), -1, SQLITE_STATIC);
    }

    bool valid = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* storedPassword = (const char*)sqlite3_column_text(stmt, 0);
        if (storedPassword && password == storedPassword) {
            valid = true;
        }
    }
    sqlite3_finalize(stmt);
    return valid;
}

bool DatabaseManager::addEvent(const string& username, const string& date, const string& event,
                               const string& startTime, const string& endTime, const string& color,
                               bool isPersonal, const string& recurrence, const string& category, const string& categoryIcon,
                               const string& department, int recurrenceCount) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO events (username, event_date, event_description, start_time, end_time, color, is_personal, recurrence_type, recurrence_count, category, category_icon, department) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, event.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, startTime.empty() ? nullptr : startTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, endTime.empty() ? nullptr : endTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, color.empty() ? nullptr : color.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, isPersonal ? 1 : 0);
    sqlite3_bind_text(stmt, 8, recurrence.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 9, recurrenceCount);
    sqlite3_bind_text(stmt, 10, category.empty() ? nullptr : category.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, categoryIcon.empty() ? nullptr : categoryIcon.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, department.empty() ? nullptr : department.c_str(), -1, SQLITE_STATIC);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<EventDetails> DatabaseManager::getEvents(const string& username, const string& date) {
vector<EventDetails> events;

    // Determine department from username (roll prefix) for academic students.
string departmentCode;
    if (username.rfind("CT-", 0) == 0) departmentCode = "CT";
    else if (username.rfind("ME-", 0) == 0) departmentCode = "ME";
    else if (username.rfind("EE-", 0) == 0) departmentCode = "EE";
    else if (username.rfind("CHE-", 0) == 0) departmentCode = "CHE";

    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, event_description, start_time, end_time, color, is_personal, recurrence_type, event_date, category, category_icon, recurrence_count "
                      "FROM events "
                      "WHERE ((username = ? AND is_personal = 1) "
                      "OR (username = ? AND is_personal = 0) "
                      "OR (? <> '' AND is_personal = 0 AND department = ?))";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return events;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, departmentCode.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, departmentCode.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        EventDetails details;
        details.id = sqlite3_column_int(stmt, 0);
        details.description = (const char*)sqlite3_column_text(stmt, 1);
        const char* startTime = (const char*)sqlite3_column_text(stmt, 2);
        details.startTime = startTime ? startTime : "";
        const char* endTime = (const char*)sqlite3_column_text(stmt, 3);
        details.endTime = endTime ? endTime : "";
        const char* color = (const char*)sqlite3_column_text(stmt, 4);
        details.color = color ? color : "";
        details.isPersonal = sqlite3_column_int(stmt, 5) != 0;
        details.recurrence = (const char*)sqlite3_column_text(stmt, 6);
        string eventStartDate = (const char*)sqlite3_column_text(stmt, 7);
        const char* category = (const char*)sqlite3_column_text(stmt, 8);
        details.category = category ? category : "";
        const char* categoryIcon = (const char*)sqlite3_column_text(stmt, 9);
        details.categoryIcon = categoryIcon ? categoryIcon : "";
        int recurrenceCount = sqlite3_column_int(stmt, 10);

        // Check if event occurs on the requested date based on recurrence
        bool occursOnDate = false;
        if (details.recurrence == "none" && eventStartDate == date) {
            occursOnDate = true;
        } else if (details.recurrence == "daily" && eventStartDate <= date) {
            // Calculate days since start
            struct tm start = {0}, current = {0};
            sscanf(eventStartDate.c_str(), "%d-%d-%d", &start.tm_year, &start.tm_mon, &start.tm_mday);
            start.tm_year -= 1900; start.tm_mon -= 1;
            sscanf(date.c_str(), "%d-%d-%d", &current.tm_year, &current.tm_mon, &current.tm_mday);
            current.tm_year -= 1900; current.tm_mon -= 1;
            
            time_t startTime = mktime(&start);
            time_t currentTime = mktime(&current);
            int daysDiff = (currentTime - startTime) / (60 * 60 * 24);
            
            // Check if within occurrence limit (0 = unlimited)
            // daysDiff is 0-indexed, so occurrence number is daysDiff + 1
            if (recurrenceCount == 0 || (daysDiff >= 0 && (daysDiff + 1) <= recurrenceCount)) {
                occursOnDate = true;
            }
        } else if (details.recurrence == "weekly") {
            // Parse dates manually
            int y1, m1, d1, y2, m2, d2;
            sscanf(eventStartDate.c_str(), "%d-%d-%d", &y1, &m1, &d1);
            sscanf(date.c_str(), "%d-%d-%d", &y2, &m2, &d2);
            
            // Calculate day of week (Zeller's congruence)
            auto dayOfWeek = [](int year, int month, int day) {
                if (month < 3) { month += 12; year--; }
                int k = year % 100;
                int j = year / 100;
                int h = (day + (13 * (month + 1)) / 5 + k + k / 4 + j / 4 - 2 * j) % 7;
                return (h + 5) % 7; // 0 = Sunday, 6 = Saturday
            };
            
            if (dayOfWeek(y1, m1, d1) == dayOfWeek(y2, m2, d2) && eventStartDate <= date) {
                // Calculate weeks since start
                struct tm start = {0}, current = {0};
                sscanf(eventStartDate.c_str(), "%d-%d-%d", &start.tm_year, &start.tm_mon, &start.tm_mday);
                start.tm_year -= 1900; start.tm_mon -= 1;
                sscanf(date.c_str(), "%d-%d-%d", &current.tm_year, &current.tm_mon, &current.tm_mday);
                current.tm_year -= 1900; current.tm_mon -= 1;
                mktime(&start);
                mktime(&current);
                
                time_t startTime = mktime(&start);
                time_t currentTime = mktime(&current);
                int weeksDiff = (currentTime - startTime) / (60 * 60 * 24 * 7);
                
                // Check if within occurrence limit (0 = unlimited)
                // weeksDiff is 0-indexed, so occurrence number is weeksDiff + 1
                if (recurrenceCount == 0 || (weeksDiff >= 0 && (weeksDiff + 1) <= recurrenceCount)) {
                    occursOnDate = true;
                }
            }
        } else if (details.recurrence == "monthly") {
            // Same day of month
            if (eventStartDate.substr(8) == date.substr(8) && eventStartDate <= date) {
                // Calculate months since start
                int eYear, eMonth, eDay, tYear, tMonth, tDay;
                sscanf(eventStartDate.c_str(), "%d-%d-%d", &eYear, &eMonth, &eDay);
                sscanf(date.c_str(), "%d-%d-%d", &tYear, &tMonth, &tDay);
                int monthsDiff = (tYear - eYear) * 12 + (tMonth - eMonth);
                
                // Check if within occurrence limit (0 = unlimited)
                // monthsDiff is 0-indexed, so occurrence number is monthsDiff + 1
                if (recurrenceCount == 0 || (monthsDiff >= 0 && (monthsDiff + 1) <= recurrenceCount)) {
                    occursOnDate = true;
                }
            }
        } else if (details.recurrence == "yearly") {
            // Same month and day (anniversary)
            if (eventStartDate.substr(5) == date.substr(5) && eventStartDate <= date) {
                // Calculate years since start
                int eYear, eMonth, eDay, tYear, tMonth, tDay;
                sscanf(eventStartDate.c_str(), "%d-%d-%d", &eYear, &eMonth, &eDay);
                sscanf(date.c_str(), "%d-%d-%d", &tYear, &tMonth, &tDay);
                int yearsDiff = tYear - eYear;
                
                // Check if within occurrence limit (0 = unlimited)
                // yearsDiff is 0-indexed, so occurrence number is yearsDiff + 1
                if (recurrenceCount == 0 || (yearsDiff >= 0 && (yearsDiff + 1) <= recurrenceCount)) {
                    occursOnDate = true;
                }
            }
        }

        if (occursOnDate) {
            events.push_back(details);
        }
    }

    sqlite3_finalize(stmt);
    return events;
}

bool DatabaseManager::updateEvent(int eventId, const string& newDescription,
                                 const string& newStartTime, const string& newEndTime, const string& newColor) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE events SET event_description = ?, start_time = ?, end_time = ?, color = ? WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, newDescription.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, newStartTime.empty() ? nullptr : newStartTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, newEndTime.empty() ? nullptr : newEndTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, newColor.empty() ? nullptr : newColor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, eventId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::deleteEvent(int eventId) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM events WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, eventId);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<DateEvent> DatabaseManager::searchEvents(const string& username, const string& searchTerm, const string& religion) {
vector<DateEvent> results;
string likeTerm = "%" + searchTerm + "%";

    // Determine department from username (roll prefix) for academic students.
string departmentCode;
    if (username.rfind("CT-", 0) == 0) departmentCode = "CT";
    else if (username.rfind("ME-", 0) == 0) departmentCode = "ME";
    else if (username.rfind("EE-", 0) == 0) departmentCode = "EE";
    else if (username.rfind("CHE-", 0) == 0) departmentCode = "CHE";

    sqlite3_stmt* stmt;
    const char* sql = "SELECT event_date, event_description FROM events "
                      "WHERE event_description LIKE ? AND "
                      "((username = ? AND is_personal = 1) OR "
                      "(username = ? AND is_personal = 0) OR "
                      "(? <> '' AND is_personal = 0 AND department = ?))";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return results;
    }

    sqlite3_bind_text(stmt, 1, likeTerm.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, departmentCode.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, departmentCode.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DateEvent de;
        de.date = (const char*)sqlite3_column_text(stmt, 0);
        de.description = (const char*)sqlite3_column_text(stmt, 1);
        results.push_back(de);
    }
    sqlite3_finalize(stmt);

    // Also search holidays if personal user and religion is selected
    bool isAcademicUser = (username.find("CT-") == 0 || username == "teacher");
    if (!isAcademicUser && !religion.empty() && religion != "Select Religion") {
        sqlite3_stmt* holidayStmt;
        const char* holidaySql = "SELECT holiday_date, holiday_name FROM holidays WHERE holiday_name LIKE ? AND religion = ?";
        if (sqlite3_prepare_v2(db, holidaySql, -1, &holidayStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(holidayStmt, 1, likeTerm.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(holidayStmt, 2, religion.c_str(), -1, SQLITE_STATIC);
            while (sqlite3_step(holidayStmt) == SQLITE_ROW) {
                DateEvent de;
                de.date = (const char*)sqlite3_column_text(holidayStmt, 0);
                de.description = "[Holiday] " + string((const char*)sqlite3_column_text(holidayStmt, 1));
                results.push_back(de);
            }
            sqlite3_finalize(holidayStmt);
        }
    }

    // Sort by date
sort(results.begin(), results.end(), [](const DateEvent& a, const DateEvent& b) {
        return a.date < b.date;
    });

    return results;
}

vector<string> DatabaseManager::getEventDatesForMonth(const string& username, int year, int month) {
vector<string> dates;
    bool isAcademicUser = (username.find("CT-") == 0 || username.find("ME-") == 0 ||
                           username.find("EE-") == 0 || username.find("CHE-") == 0 ||
                           username == "teacher");

    // Format month range
ostringstream firstDay, lastDay;
    firstDay << setfill('0') << setw(4) << year << "-"
             << setw(2) << month << "-01";
    
    // Calculate last day of month
    int daysInMonth = 31;
    if (month == 2) daysInMonth = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    else if (month == 4 || month == 6 || month == 9 || month == 11) daysInMonth = 30;
    
    lastDay << setfill('0') << setw(4) << year << "-"
            << setw(2) << month << "-" << setw(2) << daysInMonth;

    // Determine department from username for academic users
string departmentCode;
    if (username.rfind("CT-", 0) == 0) departmentCode = "CT";
    else if (username.rfind("ME-", 0) == 0) departmentCode = "ME";
    else if (username.rfind("EE-", 0) == 0) departmentCode = "EE";
    else if (username.rfind("CHE-", 0) == 0) departmentCode = "CHE";

    // Get non-recurring events
    sqlite3_stmt* stmt;
    const char* sql = "SELECT event_date FROM events "
                      "WHERE ((username = ? AND is_personal = 1) OR "
                      "(username = ? AND is_personal = 0) OR "
                      "(? <> '' AND is_personal = 0 AND department = ?)) "
                      "AND recurrence_type = 'none' AND event_date BETWEEN ? AND ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, departmentCode.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, departmentCode.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, firstDay.str().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, lastDay.str().c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            dates.push_back((const char*)sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }

    // Get recurring events with recurrence count limit
    sqlite3_stmt* recurStmt;
    const char* recurSql = "SELECT event_date, recurrence_type, recurrence_count FROM events "
                           "WHERE ((username = ? AND is_personal = 1) OR "
                           "(username = ? AND is_personal = 0) OR "
                           "(? <> '' AND is_personal = 0 AND department = ?)) "
                           "AND recurrence_type != 'none' AND event_date <= ?";
    if (sqlite3_prepare_v2(db, recurSql, -1, &recurStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(recurStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(recurStmt, 2, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(recurStmt, 3, departmentCode.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(recurStmt, 4, departmentCode.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(recurStmt, 5, lastDay.str().c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(recurStmt) == SQLITE_ROW) {
string eventStartDate = (const char*)sqlite3_column_text(recurStmt, 0);
string recurrence = (const char*)sqlite3_column_text(recurStmt, 1);
            int recurrenceCount = sqlite3_column_int(recurStmt, 2);
            
            // Parse event start date
            int eYear, eMonth, eDay;
            sscanf(eventStartDate.c_str(), "%d-%d-%d", &eYear, &eMonth, &eDay);
            
            // Add dates based on recurrence (with count limit)
            if (recurrence == "daily") {
                // Add all days from start to end of month
                for (int day = 1; day <= daysInMonth; day++) {
ostringstream dateStr;
                    dateStr << setfill('0') << setw(4) << year << "-"
                            << setw(2) << month << "-" << setw(2) << day;
                    if (eventStartDate <= dateStr.str()) {
                        // Calculate days since start
                        struct tm start = {0}, current = {0};
                        sscanf(eventStartDate.c_str(), "%d-%d-%d", &start.tm_year, &start.tm_mon, &start.tm_mday);
                        start.tm_year -= 1900; start.tm_mon -= 1;
                        sscanf(dateStr.str().c_str(), "%d-%d-%d", &current.tm_year, &current.tm_mon, &current.tm_mday);
                        current.tm_year -= 1900; current.tm_mon -= 1;
                        
                        time_t startTime = mktime(&start);
                        time_t currentTime = mktime(&current);
                        int daysDiff = (currentTime - startTime) / (60 * 60 * 24);
                        
                        // Check if within occurrence limit (0 = unlimited)
                        // daysDiff is 0-indexed, so occurrence number is daysDiff + 1
                        if (recurrenceCount == 0 || (daysDiff + 1) <= recurrenceCount) {
                            dates.push_back(dateStr.str());
                        }
                    }
                }
            } else if (recurrence == "weekly") {
                // Get day of week for event start date
                struct tm eTm = {0};
                eTm.tm_year = eYear - 1900;
                eTm.tm_mon = eMonth - 1;
                eTm.tm_mday = eDay;
                mktime(&eTm);
                int eventDayOfWeek = eTm.tm_wday;
                
                // Add all matching weekdays in the month
                for (int day = 1; day <= daysInMonth; day++) {
                    struct tm tTm = {0};
                    tTm.tm_year = year - 1900;
                    tTm.tm_mon = month - 1;
                    tTm.tm_mday = day;
                    mktime(&tTm);
                    
ostringstream dateStr;
                    dateStr << setfill('0') << setw(4) << year << "-"
                            << setw(2) << month << "-" << setw(2) << day;
                    
                    // Check if same day of week and after start date
                    if (tTm.tm_wday == eventDayOfWeek && eventStartDate <= dateStr.str()) {
                        // Calculate weeks since start
                        struct tm start = {0};
                        sscanf(eventStartDate.c_str(), "%d-%d-%d", &start.tm_year, &start.tm_mon, &start.tm_mday);
                        start.tm_year -= 1900; start.tm_mon -= 1;
                        mktime(&start);
                        
                        time_t startTime = mktime(&start);
                        time_t currentTime = mktime(&tTm);
                        int weeksDiff = (currentTime - startTime) / (60 * 60 * 24 * 7);
                        
                        // Check if within occurrence limit (0 = unlimited)
                        // weeksDiff is 0-indexed, so occurrence number is weeksDiff + 1
                        if (recurrenceCount == 0 || (weeksDiff + 1) <= recurrenceCount) {
                            dates.push_back(dateStr.str());
                        }
                    }
                }
            } else if (recurrence == "monthly") {
                // Add same day of month if it exists and after start date
                if (eDay <= daysInMonth) {
ostringstream dateStr;
                    dateStr << setfill('0') << setw(4) << year << "-"
                            << setw(2) << month << "-" << setw(2) << eDay;
                    
                    if (eventStartDate <= dateStr.str()) {
                        // Calculate months since start
                        int monthsDiff = (year - eYear) * 12 + (month - eMonth);
                        
                        // Check if within occurrence limit (0 = unlimited)
                        // monthsDiff is 0-indexed, so occurrence number is monthsDiff + 1
                        if (recurrenceCount == 0 || (monthsDiff + 1) <= recurrenceCount) {
                            dates.push_back(dateStr.str());
                        }
                    }
                }
            } else if (recurrence == "yearly") {
                // Add same month and day (anniversary) if it exists and after start date
                if (eMonth == month && eDay <= daysInMonth) {
ostringstream dateStr;
                    dateStr << setfill('0') << setw(4) << year << "-"
                            << setw(2) << month << "-" << setw(2) << eDay;
                    
                    if (eventStartDate <= dateStr.str()) {
                        // Calculate years since start
                        int yearsDiff = year - eYear;
                        
                        // Check if within occurrence limit (0 = unlimited)
                        // yearsDiff is 0-indexed, so occurrence number is yearsDiff + 1
                        if (recurrenceCount == 0 || (yearsDiff >= 0 && (yearsDiff + 1) <= recurrenceCount)) {
                            dates.push_back(dateStr.str());
                        }
                    }
                }
            }
        }
        sqlite3_finalize(recurStmt);
    }

    // Remove duplicates and sort
sort(dates.begin(), dates.end());
    dates.erase(unique(dates.begin(), dates.end()), dates.end());
    
    return dates;
}

vector<DateEvent> DatabaseManager::getAllEvents(const string& username) {
vector<DateEvent> allEvents;
    bool isAcademicUser = (username.find("CT-") == 0 || username.find("ME-") == 0 ||
                           username.find("EE-") == 0 || username.find("CHE-") == 0 ||
                           username == "teacher");

    // Determine department from username for academic users
string departmentCode;
    if (username.rfind("CT-", 0) == 0) departmentCode = "CT";
    else if (username.rfind("ME-", 0) == 0) departmentCode = "ME";
    else if (username.rfind("EE-", 0) == 0) departmentCode = "EE";
    else if (username.rfind("CHE-", 0) == 0) departmentCode = "CHE";

    sqlite3_stmt* stmt;
    const char* sql = "SELECT event_date, event_description FROM events "
                      "WHERE (username = ? AND is_personal = 1) OR "
                      "(username = ? AND is_personal = 0) OR "
                      "(? <> '' AND is_personal = 0 AND department = ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, departmentCode.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, departmentCode.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            DateEvent de;
            de.date = (const char*)sqlite3_column_text(stmt, 0);
            de.description = (const char*)sqlite3_column_text(stmt, 1);
            allEvents.push_back(de);
        }
        sqlite3_finalize(stmt);
    }

sort(allEvents.begin(), allEvents.end(), [](const DateEvent& a, const DateEvent& b) {
        return a.date < b.date;
    });

    return allEvents;
}

vector<string> DatabaseManager::getAllSearchableStrings(const string& username, const string& role, const string& religion) {
vector<string> searchableItems;
    bool isAcademicUser = (role == "teacher" || (role == "student" &&
                           (username.find("CT-") == 0 || username.find("ME-") == 0 ||
                            username.find("EE-") == 0 || username.find("CHE-") == 0)));

    // Determine department from username for academic users
string departmentCode;
    if (username.rfind("CT-", 0) == 0) departmentCode = "CT";
    else if (username.rfind("ME-", 0) == 0) departmentCode = "ME";
    else if (username.rfind("EE-", 0) == 0) departmentCode = "EE";
    else if (username.rfind("CHE-", 0) == 0) departmentCode = "CHE";

    sqlite3_stmt* stmt;
    const char* sql = "SELECT DISTINCT event_description FROM events "
                      "WHERE (username = ? AND is_personal = 1) OR "
                      "(username = ? AND is_personal = 0) OR "
                      "(? <> '' AND is_personal = 0 AND department = ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, departmentCode.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, departmentCode.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            searchableItems.push_back((const char*)sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }

    // Include holiday names only when a religion is selected
    if (role == "personal" && !religion.empty() && religion != "Select Religion") {
        sqlite3_stmt* holidayStmt;
        const char* holidaySql = "SELECT DISTINCT holiday_name FROM holidays WHERE religion = ?";
        if (sqlite3_prepare_v2(db, holidaySql, -1, &holidayStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(holidayStmt, 1, religion.c_str(), -1, SQLITE_STATIC);
            while (sqlite3_step(holidayStmt) == SQLITE_ROW) {
                searchableItems.push_back((const char*)sqlite3_column_text(holidayStmt, 0));
            }
            sqlite3_finalize(holidayStmt);
        }
    }

    return searchableItems;
}

bool DatabaseManager::addHoliday(const string& date, const string& religion, const string& holidayName) {
    // Check if already exists
    sqlite3_stmt* checkStmt;
    const char* checkSql = "SELECT id FROM holidays WHERE religion = ? AND holiday_date = ? AND holiday_name = ?";
    if (sqlite3_prepare_v2(db, checkSql, -1, &checkStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(checkStmt, 1, religion.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(checkStmt, 2, date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(checkStmt, 3, holidayName.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(checkStmt) == SQLITE_ROW) {
            sqlite3_finalize(checkStmt);
            return true; // Already exists
        }
        sqlite3_finalize(checkStmt);
    }

    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO holidays (religion, holiday_date, holiday_name) VALUES (?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, religion.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, holidayName.c_str(), -1, SQLITE_STATIC);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<string> DatabaseManager::getHolidays(const string& date, const string& religion) {
vector<string> holidays;

    sqlite3_stmt* stmt = nullptr;
    if (religion.empty() || religion == "Select Religion") {
        const char* sqlGen = "SELECT holiday_name FROM holidays WHERE religion = 'General' AND holiday_date = ?";
        if (sqlite3_prepare_v2(db, sqlGen, -1, &stmt, nullptr) != SQLITE_OK) {
            return holidays;
        }
        sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_STATIC);
    } else {
        const char* sql = "SELECT holiday_name FROM holidays WHERE religion = ? AND holiday_date = ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            return holidays;
        }
        sqlite3_bind_text(stmt, 1, religion.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        holidays.push_back((const char*)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);
    return holidays;
}

vector<string> DatabaseManager::getHolidayDatesForMonth(const string& religion, int year, int month) {
vector<string> dates;

ostringstream firstDay, lastDay;
    firstDay << setfill('0') << setw(4) << year << "-"
             << setw(2) << month << "-01";
    
    int daysInMonth = 31;
    if (month == 2) daysInMonth = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    else if (month == 4 || month == 6 || month == 9 || month == 11) daysInMonth = 30;
    
    lastDay << setfill('0') << setw(4) << year << "-"
            << setw(2) << month << "-" << setw(2) << daysInMonth;

    sqlite3_stmt* stmt;
    if (religion.empty() || religion == "Select Religion") {
        const char* sqlGen = "SELECT holiday_date FROM holidays WHERE religion = 'General' AND holiday_date BETWEEN ? AND ?";
        if (sqlite3_prepare_v2(db, sqlGen, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, firstDay.str().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, lastDay.str().c_str(), -1, SQLITE_STATIC);

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                dates.push_back((const char*)sqlite3_column_text(stmt, 0));
            }
            sqlite3_finalize(stmt);
        }
    } else {
        const char* sql = "SELECT holiday_date FROM holidays WHERE religion = ? AND holiday_date BETWEEN ? AND ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, religion.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, firstDay.str().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, lastDay.str().c_str(), -1, SQLITE_STATIC);

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                dates.push_back((const char*)sqlite3_column_text(stmt, 0));
            }
            sqlite3_finalize(stmt);
        }
    }

    return dates;
}

void DatabaseManager::clearAllHolidays() {
    sqlite3_exec(db, "DELETE FROM holidays", nullptr, nullptr, nullptr);
}

vector<string> DatabaseManager::listUsers() {
vector<string> users;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT username, role FROM users ORDER BY role, username";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
string u = (const char*)sqlite3_column_text(stmt, 0);
string r = (const char*)sqlite3_column_text(stmt, 1);
            users.push_back(u + "|" + r);
        }
        sqlite3_finalize(stmt);
    }
    return users;
}

bool DatabaseManager::deleteUser(const string& username, const string& role) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM users WHERE username = ? AND role = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_STATIC);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::updatePassword(const string& username, const string& role, const string& newPassword) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE users SET password = ? WHERE username = ? AND role = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, newPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_STATIC);
    bool stepOk = sqlite3_step(stmt) == SQLITE_DONE;
    // Ensure at least one row was actually updated; otherwise, report failure
    int changes = sqlite3_changes(db);
    bool success = stepOk && changes > 0;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::upsertUserProfile(const UserProfile& profile) {
    // Try update first
    sqlite3_stmt* stmt;
    const char* updateSql = "UPDATE user_profiles SET display_name = ?, university = ?, department = ? "
                            "WHERE username = ? AND role = ?";
    if (sqlite3_prepare_v2(db, updateSql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, profile.displayName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, profile.university.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, profile.department.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, profile.username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, profile.role.c_str(), -1, SQLITE_STATIC);

    bool updateOk = sqlite3_step(stmt) == SQLITE_DONE;
    int changes = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    if (updateOk && changes > 0) {
        return true; // Updated existing profile
    }

    // Insert new profile if update did not affect any row
    const char* insertSql = "INSERT INTO user_profiles (username, role, display_name, university, department) "
                            "VALUES (?, ?, ?, ?, ?)";
    if (sqlite3_prepare_v2(db, insertSql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, profile.username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, profile.role.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, profile.displayName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, profile.university.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, profile.department.c_str(), -1, SQLITE_STATIC);

    bool insertOk = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return insertOk;
}

bool DatabaseManager::getUserProfile(const string& username, const string& role, UserProfile& outProfile) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT display_name, university, department FROM user_profiles WHERE username = ? AND role = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_STATIC);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* dn = (const char*)sqlite3_column_text(stmt, 0);
        const char* univ = (const char*)sqlite3_column_text(stmt, 1);
        const char* dept = (const char*)sqlite3_column_text(stmt, 2);
        outProfile.username = username;
        outProfile.role = role;
        outProfile.displayName = dn ? dn : "";
        outProfile.university = univ ? univ : "";
        outProfile.department = dept ? dept : "";
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

vector<UserProfile> DatabaseManager::searchTeachers(const string& nameLike, const string& university, const string& department) {
vector<UserProfile> teachers;

string sql = "SELECT username, role, display_name, university, department FROM user_profiles WHERE role = 'teacher'";
vector<string> params;

    if (!nameLike.empty()) {
        sql += " AND (display_name LIKE ? OR username LIKE ?)";
string like = "%" + nameLike + "%";
        params.push_back(like);
        params.push_back(like);
    }
    if (!university.empty()) {
        sql += " AND university = ?";
        params.push_back(university);
    }
    if (!department.empty()) {
        sql += " AND department = ?";
        params.push_back(department);
    }

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return teachers;
    }

    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, (int)i + 1, params[i].c_str(), -1, SQLITE_STATIC);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        UserProfile p;
        p.username = (const char*)sqlite3_column_text(stmt, 0);
        p.role = (const char*)sqlite3_column_text(stmt, 1);
        const char* dn = (const char*)sqlite3_column_text(stmt, 2);
        const char* univ = (const char*)sqlite3_column_text(stmt, 3);
        const char* dept = (const char*)sqlite3_column_text(stmt, 4);
        p.displayName = dn ? dn : "";
        p.university = univ ? univ : "";
        p.department = dept ? dept : "";
        teachers.push_back(p);
    }
    sqlite3_finalize(stmt);
    return teachers;
}

bool DatabaseManager::clearTeacherAvailability(const string& teacherUsername) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM teacher_availability WHERE teacher_username = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, teacherUsername.c_str(), -1, SQLITE_STATIC);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool DatabaseManager::addTeacherAvailabilitySlot(const TeacherAvailabilitySlot& slot) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO teacher_availability (teacher_username, day_of_week, start_time, end_time, note) "
                      "VALUES (?, ?, ?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, slot.teacherUsername.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, slot.dayOfWeek);
    sqlite3_bind_text(stmt, 3, slot.startTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, slot.endTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, slot.note.c_str(), -1, SQLITE_STATIC);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

vector<TeacherAvailabilitySlot> DatabaseManager::getTeacherAvailability(const string& teacherUsername) {
vector<TeacherAvailabilitySlot> slots;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, day_of_week, start_time, end_time, note FROM teacher_availability "
                      "WHERE teacher_username = ? ORDER BY day_of_week, start_time";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return slots;
    }

    sqlite3_bind_text(stmt, 1, teacherUsername.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TeacherAvailabilitySlot s;
        s.id = sqlite3_column_int(stmt, 0);
        s.teacherUsername = teacherUsername;
        s.dayOfWeek = sqlite3_column_int(stmt, 1);
        const char* st = (const char*)sqlite3_column_text(stmt, 2);
        const char* et = (const char*)sqlite3_column_text(stmt, 3);
        const char* note = (const char*)sqlite3_column_text(stmt, 4);
        s.startTime = st ? st : "";
        s.endTime = et ? et : "";
        s.note = note ? note : "";
        slots.push_back(s);
    }
    sqlite3_finalize(stmt);
    return slots;
}

bool DatabaseManager::addTask(const string& username, const string& title, const string& dueDate) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO tasks (username, title, due_date, completed) VALUES (?, ?, ?, 0)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, dueDate.empty() ? nullptr : dueDate.c_str(), -1, SQLITE_STATIC);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<string> DatabaseManager::listTasks(const string& username) {
vector<string> tasks;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, title, due_date, completed FROM tasks WHERE username = ? ORDER BY completed, due_date";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* title = (const char*)sqlite3_column_text(stmt, 1);
            const char* due = (const char*)sqlite3_column_text(stmt, 2);
            int completed = sqlite3_column_int(stmt, 3);
ostringstream oss;
            oss << id << "|" << (title ? title : "") << "|" << (due ? due : "") << "|" << (completed ? "1" : "0");
            tasks.push_back(oss.str());
        }
        sqlite3_finalize(stmt);
    }
    return tasks;
}

bool DatabaseManager::toggleTask(int taskId, bool completed) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE tasks SET completed = ? WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, completed ? 1 : 0);
    sqlite3_bind_int(stmt, 2, taskId);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::deleteTask(int taskId) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM tasks WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, taskId);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

void DatabaseManager::indexEvent(int eventId, const string& description) {
istringstream iss(description);
string word;
    while (iss >> word) {
transform(word.begin(), word.end(), word.begin(), ::tolower);
        searchTrie->insert(word, eventId);
    }
}

void DatabaseManager::buildSearchIndex() {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, event_description FROM events";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* desc = (const char*)sqlite3_column_text(stmt, 1);
            if (desc) {
                indexEvent(id, string(desc));
            }
        }
        sqlite3_finalize(stmt);
    }
}

vector<int> DatabaseManager::searchEventsWithTrie(const string& searchTerm) {
    return searchTrie->search(searchTerm);
}

vector<EventDetails> DatabaseManager::getEventsWithRecurrence(const string& username, const string& targetDate) {
vector<EventDetails> allEvents;
    sqlite3_stmt* stmt;
    
    const char* sql = "SELECT id, event_description, start_time, end_time, color, is_personal, "
                      "recurrence_type, category, category_icon, event_date FROM events "
                      "WHERE username = ? OR is_personal = 0";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            EventDetails event;
            event.id = sqlite3_column_int(stmt, 0);
            
            const char* desc = (const char*)sqlite3_column_text(stmt, 1);
            event.description = desc ? desc : "";
            
            const char* st = (const char*)sqlite3_column_text(stmt, 2);
            event.startTime = st ? st : "";
            
            const char* et = (const char*)sqlite3_column_text(stmt, 3);
            event.endTime = et ? et : "";
            
            const char* col = (const char*)sqlite3_column_text(stmt, 4);
            event.color = col ? col : "";
            
            event.isPersonal = sqlite3_column_int(stmt, 5) != 0;
            
            const char* rec = (const char*)sqlite3_column_text(stmt, 6);
            event.recurrence = rec ? rec : "none";
            
            const char* cat = (const char*)sqlite3_column_text(stmt, 7);
            event.category = cat ? cat : "";
            
            const char* icon = (const char*)sqlite3_column_text(stmt, 8);
            event.categoryIcon = icon ? icon : "";
            
            const char* eventDate = (const char*)sqlite3_column_text(stmt, 9);
string evtDateStr = eventDate ? eventDate : "";
            
            RecurrenceEngine::RecurrenceType recType = RecurrenceEngine::stringToType(event.recurrence);
            
            if (RecurrenceEngine::matchesRecurrence(evtDateStr, targetDate, recType)) {
                allEvents.push_back(event);
            }
        }
        sqlite3_finalize(stmt);
    }
    
sort(allEvents.begin(), allEvents.end(), [](const EventDetails& a, const EventDetails& b) {
        return a.startTime < b.startTime;
    });
    
    return allEvents;
}

void DatabaseManager::populateUniversitiesAndDepartments() {
    sqlite3_stmt* stmt;
    const char* checkSql = "SELECT COUNT(*) FROM universities";
    if (sqlite3_prepare_v2(db, checkSql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            if (count > 0) return;
        } else {
            sqlite3_finalize(stmt);
        }
    }
    
    const char* universities[] = {
        "National University of Sciences and Technology (NUST)",
        "Lahore University of Management Sciences (LUMS)",
        "Pakistan Institute of Engineering and Applied Sciences (PIEAS)",
        "COMSATS University Islamabad",
        "University of Engineering and Technology (UET) Lahore",
        "University of Engineering and Technology (UET) Taxila",
        "University of Engineering and Technology (UET) Peshawar",
        "Ghulam Ishaq Khan Institute (GIKI)",
        "National University of Computer and Emerging Sciences (FAST-NUCES)",
        "Quaid-i-Azam University Islamabad",
        "University of the Punjab",
        "University of Karachi",
        "NED University of Engineering and Technology",
        "Mehran University of Engineering and Technology",
        "Bahria University",
        "Air University",
        "Institute of Business Administration (IBA) Karachi",
        "Institute of Business Administration (IBA) Sukkur"
    };
    
    const char* sql = "INSERT OR IGNORE INTO universities (name) VALUES (?)";
    for (const char* uni : universities) {
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, uni, -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
    
    struct Department {
        const char* code;
        const char* name;
    };
    
    Department departments[] = {
        {"CT", "Computer Science"}, {"SE", "Software Engineering"},
        {"ME", "Mechanical Engineering"}, {"EE", "Electrical Engineering"},
        {"CE", "Civil Engineering"}, {"CHE", "Chemical Engineering"},
        {"AI", "Artificial Intelligence"}, {"DS", "Data Science"},
        {"CS", "Cyber Security"}, {"TE", "Telecommunication Engineering"},
        {"AE", "Aerospace Engineering"}, {"IE", "Industrial Engineering"},
        {"PE", "Petroleum Engineering"}, {"BM", "Biomedical Engineering"},
        {"ENV", "Environmental Engineering"}, {"AR", "Architecture"},
        {"PHY", "Physics"}, {"MA", "Mathematics"},
        {"CH", "Chemistry"}, {"BI", "Biology"},
        {"BT", "Biotechnology"}, {"MB", "Microbiology"},
        {"BC", "Biochemistry"}, {"BA", "Business Administration"},
        {"EC", "Economics"}, {"FN", "Finance"},
        {"MK", "Marketing"}, {"HR", "Human Resources"},
        {"AC", "Accounting"}, {"MG", "Management Sciences"},
        {"EN", "English"}, {"UR", "Urdu"},
        {"IS", "Islamic Studies"}, {"PS", "Political Science"},
        {"SO", "Sociology"}, {"PY", "Psychology"},
        {"ED", "Education"}, {"LA", "Law"},
        {"MD", "Medicine"}, {"PH", "Pharmacy"},
        {"NS", "Nursing"}, {"DT", "Dentistry"}
    };
    
    const char* deptSql = "INSERT OR IGNORE INTO departments (code, name) VALUES (?, ?)";
    for (const Department& dept : departments) {
        if (sqlite3_prepare_v2(db, deptSql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, dept.code, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, dept.name, -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
}

string DatabaseManager::getDepartmentFromRoll(const string& rollNumber) {
regex pattern("^([A-Z]{2,3})-");
smatch match;
    if (regex_search(rollNumber, match, pattern)) {
        return match[1].str();
    }
    return "";
}

bool DatabaseManager::isValidRollNumber(const string& rollNumber) {
regex pattern("^[A-Z]{2,3}-[0-9]+$");
    return regex_match(rollNumber, pattern);
}

string DatabaseManager::getDepartmentName(const string& code) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT name FROM departments WHERE code = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* name = (const char*)sqlite3_column_text(stmt, 0);
string result = name ? name : code;
            sqlite3_finalize(stmt);
            return result;
        }
        sqlite3_finalize(stmt);
    }
    return code;
}

vector<string> DatabaseManager::getAllUniversities() {
vector<string> universities;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT name FROM universities ORDER BY name";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* name = (const char*)sqlite3_column_text(stmt, 0);
            if (name) {
                universities.push_back(string(name));
            }
        }
        sqlite3_finalize(stmt);
    }
    return universities;
}

vector<pair<string, string>> DatabaseManager::getAllDepartments() {
vector<pair<string, string>> departments;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT code, name FROM departments ORDER BY code";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* code = (const char*)sqlite3_column_text(stmt, 0);
            const char* name = (const char*)sqlite3_column_text(stmt, 1);
            if (code && name) {
                departments.push_back(make_pair(string(code), string(name)));
            }
        }
        sqlite3_finalize(stmt);
    }
    return departments;
}
