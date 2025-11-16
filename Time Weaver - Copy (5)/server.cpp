#include "database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <cstdlib>
using namespace std;

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

DatabaseManager dbManager;

// Quote management
map<string, map<int, vector<string>>> quotesMap;

void initializeQuotes() {
    quotesMap["academic"][1] = {"The new year stands before us, like a chapter in a book, waiting to be written."};
    quotesMap["academic"][2] = {"Develop a passion for learning. If you do, you will never cease to grow."};
    quotesMap["academic"][3] = {"Spring is a reminder of how beautiful change can truly be. Embrace new knowledge."};
    quotesMap["academic"][4] = {"Don't watch the clock; do what it does. Keep going."};
    quotesMap["academic"][5] = {"The beautiful thing about learning is that nobody can take it away from you."};
    quotesMap["academic"][6] = {"Success is the sum of small efforts, repeated day in and day out."};
    quotesMap["academic"][7] = {"The expert in anything was once a beginner."};
    quotesMap["academic"][8] = {"Believe you can and you're halfway there."};
    quotesMap["academic"][9] = {"Education is the passport to the future, for tomorrow belongs to those who prepare for it today."};
    quotesMap["academic"][10] = {"Autumn shows us how beautiful it is to let things go... perhaps bad study habits?"};
    quotesMap["academic"][11] = {"Strive for progress, not perfection."};
    quotesMap["academic"][12] = {"What we learn with pleasure we never forget."};

    quotesMap["personal_generic"][1] = {"The beginning is the most important part of the work."};
    quotesMap["personal_generic"][2] = {"Kindness is like snow—it beautifies everything it covers."};
    quotesMap["personal_generic"][3] = {"No winter lasts forever; no spring skips its turn."};
    quotesMap["personal_generic"][4] = {"The earth laughs in flowers."};
    quotesMap["personal_generic"][5] = {"Where flowers bloom, so does hope."};
    quotesMap["personal_generic"][6] = {"Keep your face to the sun and you will never see the shadows."};
    quotesMap["personal_generic"][7] = {"Live in the sunshine, swim the sea, drink the wild air."};
    quotesMap["personal_generic"][8] = {"Summer afternoon—summer afternoon; to me those have always been the two most beautiful words."};
    quotesMap["personal_generic"][9] = {"Life starts all over again when it gets crisp in the fall."};
    quotesMap["personal_generic"][10] = {"Every leaf speaks bliss to me, fluttering from the autumn tree."};
    quotesMap["personal_generic"][11] = {"Gratitude turns what we have into enough."};
    quotesMap["personal_generic"][12] = {"The best and most beautiful things in the world cannot be seen or even touched - they must be felt with the heart."};

    quotesMap["personal_Christian"][1] = {"For I know the plans I have for you, declares the Lord, plans for welfare and not for evil, to give you a future and a hope. - Jeremiah 29:11"};
    quotesMap["personal_Christian"][2] = {"Love is patient and kind; love does not envy or boast; it is not arrogant or rude. - 1 Corinthians 13:4"};
    quotesMap["personal_Christian"][3] = {"Therefore, if anyone is in Christ, he is a new creation. The old has passed away; behold, the new has come. - 2 Corinthians 5:17"};
    quotesMap["personal_Christian"][4] = {"He is not here; he has risen, just as he said. - Matthew 28:6 (Easter)"};
    quotesMap["personal_Christian"][5] = {"Ask, and it will be given to you; seek, and you will find; knock, and it will be opened to you. - Matthew 7:7"};
    quotesMap["personal_Christian"][6] = {"I can do all things through him who strengthens me. - Philippians 4:13"};
    quotesMap["personal_Christian"][7] = {"The Lord is my shepherd; I shall not want. - Psalm 23:1"};
    quotesMap["personal_Christian"][8] = {"Be strong and courageous. Do not be frightened, and do not be dismayed, for the Lord your God is with you wherever you go. - Joshua 1:9"};
    quotesMap["personal_Christian"][9] = {"Trust in the Lord with all your heart, and do not lean on your own understanding. - Proverbs 3:5"};
    quotesMap["personal_Christian"][10] = {"Give thanks in all circumstances; for this is the will of God in Christ Jesus for you. - 1 Thessalonians 5:18"};
    quotesMap["personal_Christian"][11] = {"The steadfast love of the Lord never ceases; his mercies never come to an end. - Lamentations 3:22"};
    quotesMap["personal_Christian"][12] = {"For unto you is born this day in the city of David a Savior, who is Christ the Lord. - Luke 2:11 (Christmas)"};

    quotesMap["personal_Hindu"][1] = {"As the sun brightens the world, may your life be brightened with knowledge and joy. Happy Makar Sankranti!"};
    quotesMap["personal_Hindu"][2] = {"May Lord Shiva shower his blessings on you and your family. Om Namah Shivaya! (Maha Shivaratri)"};
    quotesMap["personal_Hindu"][3] = {"Let the colors of Holi spread the message of peace and happiness."};
    quotesMap["personal_Hindu"][4] = {"May the divine grace of Lord Ram always be with you. Wish you a very happy and prosperous Rama Navami."};
    quotesMap["personal_Hindu"][5] = {"The mind acts like an enemy for those who do not control it. - Bhagavad Gita"};
    quotesMap["personal_Hindu"][6] = {"Do your duty, but do not concern yourself with the results. - Bhagavad Gita"};
    quotesMap["personal_Hindu"][7] = {"May Lord Krishna steal all your tensions and worries... Happy Janmashtami!"};
    quotesMap["personal_Hindu"][8] = {"A brother is a friend given by Nature. Celebrate the bond of Raksha Bandhan."};
    quotesMap["personal_Hindu"][9] = {"May Goddess Durga illuminate your life with countless blessings of happiness. (Navaratri)"};
    quotesMap["personal_Hindu"][10] = {"May the festival of lights fill your life with the glow of happiness and the sparkle of joy. Happy Diwali!"};
    quotesMap["personal_Hindu"][11] = {"There is nothing lost or wasted in this life. - Bhagavad Gita"};
    quotesMap["personal_Hindu"][12] = {"Look to this day, for it is life. The very life of life."};

    quotesMap["personal_Islam"][1] = {"The best among you are those who have the best manners and character. - Prophet Muhammad (peace be upon him)"};
    quotesMap["personal_Islam"][2] = {"Indeed, Allah is with the patient. - Quran 2:153"};
    quotesMap["personal_Islam"][3] = {"Ramadan is the month whose beginning is mercy, whose middle is forgiveness, and whose end is freedom from the fire."};
    quotesMap["personal_Islam"][4] = {"May the blessings of Allah fill your life with happiness and open all the doors of success now and always. Eid Mubarak! (Eid al-Fitr)"};
    quotesMap["personal_Islam"][5] = {"Speak good or remain silent. - Prophet Muhammad (peace be upon him)"};
    quotesMap["personal_Islam"][6] = {"May His blessings always shine upon you and your family on this auspicious day. Eid Mubarak! (Eid al-Adha)"};
    quotesMap["personal_Islam"][7] = {"Do not lose hope, nor be sad. - Quran 3:139"};
    quotesMap["personal_Islam"][8] = {"He who eats his fill while his neighbor goes hungry is not a believer."};
    quotesMap["personal_Islam"][9] = {"Kindness is a mark of faith, and whoever has not kindness has not faith."};
    quotesMap["personal_Islam"][10] = {"Allah does not burden a soul beyond that it can bear. - Quran 2:286"};
    quotesMap["personal_Islam"][11] = {"The seeking of knowledge is obligatory for every Muslim."};
    quotesMap["personal_Islam"][12] = {"Verily, with hardship, there is relief. - Quran 94:6"};

    quotesMap["personal_Jewish"][1] = {"Who is wise? One who learns from every man. - Pirkei Avot"};
    quotesMap["personal_Jewish"][2] = {"Just as despair can come to one only from himself, so too can hope be generated by man himself."};
    quotesMap["personal_Jewish"][3] = {"On three things the world stands: On Torah, on service [of God], and on acts of human kindness. - Pirkei Avot"};
    quotesMap["personal_Jewish"][4] = {"In every generation, one must see oneself as if one personally came out of Egypt. (Passover)"};
    quotesMap["personal_Jewish"][5] = {"From the Heights of Sinai, we received the Torah, not just the Ten Commandments, but the totality of Jewish Law and Tradition. (Shavuot)"};
    quotesMap["personal_Jewish"][6] = {"Do not be daunted by the enormity of the world's grief. Do justly, now. Love mercy, now. Walk humbly, now."};
    quotesMap["personal_Jewish"][7] = {"This is the day the Lord has made; let us rejoice and be glad in it. - Psalm 118:24"};
    quotesMap["personal_Jewish"][8] = {"If I am not for myself, who will be for me? But if I am only for myself, what am I? - Hillel"};
    quotesMap["personal_Jewish"][9] = {"May you be inscribed and sealed for a good year. Shana Tova! (Rosh Hashanah)"};
    quotesMap["personal_Jewish"][10] = {"May your fasting be easy and meaningful. G'mar Chatima Tova. (Yom Kippur)"};
    quotesMap["personal_Jewish"][11] = {"The highest form of wisdom is kindness."};
    quotesMap["personal_Jewish"][12] = {"A little bit of light dispels a lot of darkness. (Hanukkah)"};

    quotesMap["personal_Sikh"][1] = {"Recognize the whole human race as one. - Guru Gobind Singh Ji"};
    quotesMap["personal_Sikh"][2] = {"Even Kings and emperors with heaps of wealth and vast dominion cannot compare with an ant filled with the love of God."};
    quotesMap["personal_Sikh"][3] = {"Speak only that which will bring you honor."};
    quotesMap["personal_Sikh"][4] = {"Let no man in the world live in delusion. Without a Guru, none can cross over to the other shore. (Vaisakhi)"};
    quotesMap["personal_Sikh"][5] = {"Conquer your mind and conquer the world."};
    quotesMap["personal_Sikh"][6] = {"He who has no faith in himself can never have faith in God."};
    quotesMap["personal_Sikh"][7] = {"Dwell in peace in the home of your own being, and the Messenger of Death will not be able to touch you."};
    quotesMap["personal_Sikh"][8] = {"The world is a drama, staged in a dream."};
    quotesMap["personal_Sikh"][9] = {"Before becoming a Sikh, a Muslim, a Hindu or a Christian, let's become a Human first."};
    quotesMap["personal_Sikh"][10] = {"Serve the saints; earn the lasting treasure."};
    quotesMap["personal_Sikh"][11] = {"Nanak Naam Chardi Kala, Tere Bhane Sarbat Da Bhala. (May the Holy Name be ever-increasing, May goodwill prevail over all, by Your Grace - Guru Nanak Jayanti)"};
    quotesMap["personal_Sikh"][12] = {"Sing the songs of joy to the Lord, serve the Name of the Lord, and become the servant of His servants."};
}

string getQuoteForMonth(int month, const string& role, const string& religion) {
string quoteKey = role;
    if (role == "personal") {
        if (!religion.empty() && religion != "Select Religion") {
            quoteKey += "_" + religion;
        } else {
            quoteKey += "_generic";
        }
    } else if (role == "student" || role == "teacher") {
        quoteKey = "academic";
    } else {
        quoteKey = "personal_generic";
    }
    if (quotesMap.find(quoteKey) != quotesMap.end() && 
        quotesMap[quoteKey].find(month) != quotesMap[quoteKey].end() &&
        !quotesMap[quoteKey][month].empty()) {
        return quotesMap[quoteKey][month][0];
    }
    return "May your month be filled with joy and purpose.";
}

void populateHolidays() {
    dbManager.clearAllHolidays();
ifstream file("HolidayData.csv");
    if (!file.is_open()) return;
string line;
    bool firstLine = true;
    while (getline(file, line)) {
        if (firstLine) { firstLine = false; continue; }
        if (line.empty()) continue;
istringstream iss(line);
string date, religion, name;
        if (getline(iss, date, ',') && getline(iss, religion, ',') && getline(iss, name)) {
            date.erase(0, date.find_first_not_of(" \t"));
            date.erase(date.find_last_not_of(" \t") + 1);
            religion.erase(0, religion.find_first_not_of(" \t"));
            religion.erase(religion.find_last_not_of(" \t") + 1);
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            if (!date.empty() && !religion.empty() && !name.empty()) {
                dbManager.addHoliday(date, religion, name);
            }
        }
    }
    file.close();
}

string colorNameToHex(const string& name) {
    if (name == "Red") return "#FF0000";
    if (name == "Green") return "#008000";
    if (name == "Blue") return "#0000FF";
    if (name == "Yellow") return "#FFFF00";
    if (name == "Orange") return "#FFA500";
    if (name == "Purple") return "#800080";
    return "";
}

string readFile(const string& path) {
ifstream file(path);
    if (!file.is_open()) return "";
string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return content;
}

string jsonEscape(const string& str) {
string result;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else if (c == '\n') result += "\\n";
        else if (c == '\r') result += "\\r";
        else if (c == '\t') result += "\\t";
        else result += c;
    }
    return result;
}

string buildJsonArray(const vector<string>& items) {
string result = "[";
    for (size_t i = 0; i < items.size(); i++) {
        if (i > 0) result += ",";
        result += "\"" + jsonEscape(items[i]) + "\"";
    }
    result += "]";
    return result;
}

string getValueFromJson(const string& json, const string& key) {
string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == string::npos) return "";
    pos = json.find(":", pos);
    if (pos == string::npos) return "";
    pos++; // Skip the colon
    // Skip whitespace after colon
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    
    // Check if value is a string (starts with quote)
    if (pos < json.length() && json[pos] == '"') {
        pos++; // Skip opening quote
        size_t end = json.find("\"", pos);
        if (end == string::npos) return "";
        return json.substr(pos, end - pos);
    } else {
        // Value is a number, boolean, or null - extract until comma, }, or whitespace
        size_t start = pos;
        while (pos < json.length() && json[pos] != ',' && json[pos] != '}' && json[pos] != ']' && json[pos] != ' ' && json[pos] != '\t' && json[pos] != '\n' && json[pos] != '\r') {
            pos++;
        }
        string result = json.substr(start, pos - start);
        // Trim whitespace
        while (!result.empty() && (result.back() == ' ' || result.back() == '\t')) {
            result.pop_back();
        }
        return result;
    }
}

string parseUrlParams(const string& url) {
    size_t pos = url.find("?");
    if (pos == string::npos) return "";
    return url.substr(pos + 1);
}

string getParam(const string& params, const string& key) {
    size_t pos = params.find(key + "=");
    if (pos == string::npos) return "";
    pos += key.length() + 1;
    size_t end = params.find("&", pos);
    if (end == string::npos) end = params.length();
string value = params.substr(pos, end - pos);
    // URL decode
string result;
    for (size_t i = 0; i < value.length(); i++) {
        if (value[i] == '%' && i + 2 < value.length()) {
            int hex = 0;
            sscanf(value.substr(i + 1, 2).c_str(), "%x", &hex);
            result += (char)hex;
            i += 2;
        } else if (value[i] == '+') {
            result += ' ';
        } else {
            result += value[i];
        }
    }
    return result;
}

void sendResponse(int clientSocket, const string& content, const string& contentType, int statusCode = 200) {
ostringstream response;
    response << "HTTP/1.1 " << statusCode << " OK\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
    response << "Access-Control-Allow-Headers: Content-Type\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << content;
    
string responseStr = response.str();
#ifdef _WIN32
    send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
    closesocket(clientSocket);
#else
    write(clientSocket, responseStr.c_str(), responseStr.length());
    close(clientSocket);
#endif
}

void handleRequest(int clientSocket, const string& request) {
istringstream iss(request);
string method, path, version;
    iss >> method >> path >> version;
    
    if (method == "OPTIONS") {
        sendResponse(clientSocket, "", "text/plain", 204);
        return;
    }
    
    // Parse path
    size_t queryPos = path.find("?");
string pathOnly = (queryPos != string::npos) ? path.substr(0, queryPos) : path;
string query = (queryPos != string::npos) ? path.substr(queryPos + 1) : "";
    
    // Get body if POST/PUT
string body;
    size_t bodyPos = request.find("\r\n\r\n");
    if (bodyPos != string::npos) {
        body = request.substr(bodyPos + 4);
    }
    
    // Serve static files
    if (pathOnly == "/" || pathOnly == "/index.html") {
string content = readFile("public/index.html");
        sendResponse(clientSocket, content, "text/html");
        return;
    }
    if (pathOnly == "/style.css") {
string content = readFile("public/style.css");
        sendResponse(clientSocket, content, "text/css");
        return;
    }
    if (pathOnly == "/app.js") {
string content = readFile("public/app.js");
        sendResponse(clientSocket, content, "application/javascript");
        return;
    }
    if (pathOnly == "/app_academic_features.js") {
string content = readFile("public/app_academic_features.js");
        sendResponse(clientSocket, content, "application/javascript");
        return;
    }
    if (pathOnly == "/universities.js") {
string content = readFile("public/universities.js");
        sendResponse(clientSocket, content, "application/javascript");
        return;
    }
    
    // API Routes
    if (pathOnly == "/api/user/add" && method == "POST") {
string username = getValueFromJson(body, "username");
string password = getValueFromJson(body, "password");
string role = getValueFromJson(body, "role");
string displayName = getValueFromJson(body, "displayName");
string university = getValueFromJson(body, "university");
string department = getValueFromJson(body, "department");
        bool exists = dbManager.userExists(username, role, university);
        bool success = false;
        if (!exists) {
            success = dbManager.addUser(username, password, role, university);
        }
        // For academic users, also upsert profile
        if (success && (role == "student" || role == "teacher")) {
            UserProfile profile;
            profile.username = username;
            profile.role = role;
            profile.displayName = displayName.empty() ? username : displayName;
            profile.university = university;
            profile.department = department;
            dbManager.upsertUserProfile(profile);
        }
        sendResponse(clientSocket, "{\"success\":" + string(success ? "true" : "false") + ",\"exists\":" + string(exists ? "true" : "false") + "}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/user/validate" && method == "POST") {
string username = getValueFromJson(body, "username");
string password = getValueFromJson(body, "password");
string role = getValueFromJson(body, "role");
string university = getValueFromJson(body, "university");
string department = getValueFromJson(body, "department");
        
        // trim spaces
        auto trim = [](string s) {
            size_t a = s.find_first_not_of(" \t\r\n");
            size_t b = s.find_last_not_of(" \t\r\n");
            if (a == string::npos) return string();
            return s.substr(a, b - a + 1);
        };
        username = trim(username);
        password = trim(password);
        role = trim(role);
        university = trim(university);
        department = trim(department);
        
        // lowercase helper
        auto toLower = [](string s) {
transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return (char)tolower(c); });
            return s;
        };
        bool valid = false;
        // Allow built-in admin irrespective of role casing/value for reliability
        if (toLower(username) == "admin" && password == "admin123") {
            valid = true;
        } else {
            // For academic users, validate with university
            valid = dbManager.validateUser(username, password, role, university);
        }
        if (!valid) {
            sendResponse(clientSocket, "{\"valid\":false,\"error\":\"Invalid credentials\"}", "application/json");
            return;
        }

        // For academic users, return profile data
        if (role == "student" || role == "teacher") {
            UserProfile profile;
            if (dbManager.getUserProfile(username, role, profile)) {
                // Return profile data
string json = "{\"valid\":true";
                json += ",\"displayName\":\"" + jsonEscape(profile.displayName) + "\"";
                json += ",\"university\":\"" + jsonEscape(profile.university) + "\"";
                json += ",\"department\":\"" + jsonEscape(profile.department) + "\"}";
                sendResponse(clientSocket, json, "application/json");
                return;
            } else {
                // Profile not found for academic user
                sendResponse(clientSocket, "{\"valid\":false,\"error\":\"Invalid credentials\"}", "application/json");
                return;
            }
        }
        
        // For non-academic users (personal, admin)
        sendResponse(clientSocket, "{\"valid\":true}", "application/json");
        return;
    }

    // Academic user profile upsert (student/teacher)
    if (pathOnly == "/api/user/profile" && method == "POST") {
string username = getValueFromJson(body, "username");
string role = getValueFromJson(body, "role");
string displayName = getValueFromJson(body, "displayName");
string university = getValueFromJson(body, "university");
string department = getValueFromJson(body, "department");
        if (username.empty() || role.empty()) {
            sendResponse(clientSocket, "{\"success\":false,\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        UserProfile profile;
        profile.username = username;
        profile.role = role;
        profile.displayName = displayName.empty() ? username : displayName;
        profile.university = university;
        profile.department = department;
        bool ok = dbManager.upsertUserProfile(profile);
        sendResponse(clientSocket, "{\"success\":" + string(ok ? "true" : "false") + "}", "application/json");
        return;
    }

    // Admin: list users
    if (pathOnly == "/api/users" && method == "GET") {
string adminUser = getParam(query, "admin");
        if (adminUser != "true") {
            sendResponse(clientSocket, "{\"error\":\"Unauthorized\"}", "application/json", 403);
            return;
        }
        auto users = dbManager.listUsers();
string json = "{\"users\":[";
        for (size_t i = 0; i < users.size(); i++) {
            if (i) json += ",";
            // users[i] = "username|role"
            size_t p = users[i].find("|");
string u = users[i].substr(0, p);
string r = (p != string::npos) ? users[i].substr(p + 1) : "";
            json += "{\"username\":\"" + jsonEscape(u) + "\",\"role\":\"" + jsonEscape(r) + "\"}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }

    // Admin: update password
    if (pathOnly == "/api/users/password" && method == "PUT") {
string adminUser = getValueFromJson(body, "admin");
        if (adminUser != "true") {
            sendResponse(clientSocket, "{\"error\":\"Unauthorized\"}", "application/json", 403);
            return;
        }
string username = getValueFromJson(body, "username");
string role = getValueFromJson(body, "role");
string newPassword = getValueFromJson(body, "password");
        bool ok = dbManager.updatePassword(username, role, newPassword);
        sendResponse(clientSocket, "{\"success\":" + string(ok ? "true" : "false") + "}", "application/json");
        return;
    }

    // Admin: delete user
    if (pathOnly == "/api/users" && method == "DELETE") {
string adminUser = getParam(query, "admin");
        if (adminUser != "true") {
            sendResponse(clientSocket, "{\"error\":\"Unauthorized\"}", "application/json", 403);
            return;
        }
string username = getParam(query, "username");
string role = getParam(query, "role");
        bool ok = dbManager.deleteUser(username, role);
        sendResponse(clientSocket, "{\"success\":" + string(ok ? "true" : "false") + "}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/events" && method == "GET") {
string username = getParam(query, "username");
string date = getParam(query, "date");
        if (username.empty() || date.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        auto events = dbManager.getEvents(username, date);
string json = "{\"events\":[";
        for (size_t i = 0; i < events.size(); i++) {
            if (i > 0) json += ",";
            json += "{";
            json += "\"id\":" + to_string(events[i].id) + ",";
            json += "\"description\":\"" + jsonEscape(events[i].description) + "\",";
            json += "\"startTime\":\"" + jsonEscape(events[i].startTime) + "\",";
            json += "\"endTime\":\"" + jsonEscape(events[i].endTime) + "\",";
            json += "\"color\":\"" + jsonEscape(events[i].color) + "\",";
            json += "\"isPersonal\":" + string(events[i].isPersonal ? "true" : "false") + ",";
            json += "\"recurrence\":\"" + jsonEscape(events[i].recurrence) + "\",";
            json += "\"category\":\"" + jsonEscape(events[i].category) + "\",";
            json += "\"categoryIcon\":\"" + jsonEscape(events[i].categoryIcon) + "\"";
            json += "}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    if (pathOnly == "/api/events" && method == "POST") {
string username = getValueFromJson(body, "username");
string date = getValueFromJson(body, "date");
string description = getValueFromJson(body, "description");
string startTime = getValueFromJson(body, "startTime");
string endTime = getValueFromJson(body, "endTime");
string color = getValueFromJson(body, "color");
string recurrence = getValueFromJson(body, "recurrence");
string recurrenceCountStr = getValueFromJson(body, "recurrenceCount");
string category = getValueFromJson(body, "category");
string categoryIcon = getValueFromJson(body, "categoryIcon");
string department = getValueFromJson(body, "department");
        if (recurrence.empty()) recurrence = "none";
        
        int recurrenceCount = 0;
        if (!recurrenceCountStr.empty()) {
            recurrenceCount = atoi(recurrenceCountStr.c_str());
        }
        
        bool isPersonal = false;
        if (body.find("\"isPersonal\":true") != string::npos) isPersonal = true;
        
        if (!color.empty() && color.find("#") != 0) {
            color = colorNameToHex(color);
        }
        
        bool success = dbManager.addEvent(username, date, description, startTime, endTime, color, isPersonal, recurrence, category, categoryIcon, department, recurrenceCount);
        sendResponse(clientSocket, "{\"success\":" + string(success ? "true" : "false") + "}", "application/json");
        return;
    }
    
    if (pathOnly.find("/api/events/") == 0 && method == "PUT") {
        size_t pos = pathOnly.find_last_of("/");
        int eventId = atoi(pathOnly.substr(pos + 1).c_str());
string description = getValueFromJson(body, "description");
string startTime = getValueFromJson(body, "startTime");
string endTime = getValueFromJson(body, "endTime");
string color = getValueFromJson(body, "color");
        if (!color.empty() && color.find("#") != 0) {
            color = colorNameToHex(color);
        }
        bool success = dbManager.updateEvent(eventId, description, startTime, endTime, color);
        sendResponse(clientSocket, "{\"success\":" + string(success ? "true" : "false") + "}", "application/json");
        return;
    }
    
    if (pathOnly.find("/api/events/") == 0 && method == "DELETE") {
        size_t pos = pathOnly.find_last_of("/");
        int eventId = atoi(pathOnly.substr(pos + 1).c_str());
        bool success = dbManager.deleteEvent(eventId);
        sendResponse(clientSocket, "{\"success\":" + string(success ? "true" : "false") + "}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/events/search" && method == "GET") {
string username = getParam(query, "username");
string term = getParam(query, "term");
string religion = getParam(query, "religion");
        if (username.empty() || term.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        auto results = dbManager.searchEvents(username, term, religion);
string json = "{\"results\":[";
        for (size_t i = 0; i < results.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"date\":\"" + jsonEscape(results[i].date) + "\",";
            json += "\"description\":\"" + jsonEscape(results[i].description) + "\"}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    if (pathOnly == "/api/events/month" && method == "GET") {
string username = getParam(query, "username");
string yearStr = getParam(query, "year");
string monthStr = getParam(query, "month");
        if (username.empty() || yearStr.empty() || monthStr.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        int year = atoi(yearStr.c_str());
        int month = atoi(monthStr.c_str());
        if (month < 1 || month > 12) {
            sendResponse(clientSocket, "{\"error\":\"Invalid month\"}", "application/json", 400);
            return;
        }
        auto dates = dbManager.getEventDatesForMonth(username, year, month);
        sendResponse(clientSocket, "{\"dates\":" + buildJsonArray(dates) + "}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/holidays" && method == "GET") {
string date = getParam(query, "date");
string religion = getParam(query, "religion");
        if (date.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        auto holidays = dbManager.getHolidays(date, religion);
        sendResponse(clientSocket, "{\"holidays\":" + buildJsonArray(holidays) + "}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/holidays/month" && method == "GET") {
string religion = getParam(query, "religion");
string yearStr = getParam(query, "year");
string monthStr = getParam(query, "month");
        if (yearStr.empty() || monthStr.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        int year = atoi(yearStr.c_str());
        int month = atoi(monthStr.c_str());
        if (month < 1 || month > 12) {
            sendResponse(clientSocket, "{\"error\":\"Invalid month\"}", "application/json", 400);
            return;
        }
        auto dates = dbManager.getHolidayDatesForMonth(religion, year, month);
        sendResponse(clientSocket, "{\"dates\":" + buildJsonArray(dates) + "}", "application/json");
        return;
    }

    // Tasks
    if (pathOnly == "/api/tasks" && method == "GET") {
string username = getParam(query, "username");
        if (username.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        auto tasks = dbManager.listTasks(username);
string json = "{\"tasks\":[";
        for (size_t i = 0; i < tasks.size(); i++) {
            if (i) json += ",";
            // id|title|due|completed
istringstream iss(tasks[i]);
string id, title, due, completed;
getline(iss, id, '|');
getline(iss, title, '|');
getline(iss, due, '|');
getline(iss, completed, '|');
            json += "{\"id\":" + id + ",\"title\":\"" + jsonEscape(title) + "\",\"due\":\"" + jsonEscape(due) + "\",\"completed\":" + (completed == "1" ? "true" : "false") + "}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    if (pathOnly == "/api/tasks" && method == "POST") {
string username = getValueFromJson(body, "username");
string title = getValueFromJson(body, "title");
string due = getValueFromJson(body, "due");
        bool ok = dbManager.addTask(username, title, due);
        sendResponse(clientSocket, "{\"success\":" + string(ok ? "true" : "false") + "}", "application/json");
        return;
    }
    if (pathOnly.find("/api/tasks/") == 0 && method == "PUT") {
        size_t pos = pathOnly.find_last_of("/");
        int taskId = atoi(pathOnly.substr(pos + 1).c_str());
        bool completed = body.find("\"completed\":true") != string::npos;
        bool ok = dbManager.toggleTask(taskId, completed);
        sendResponse(clientSocket, "{\"success\":" + string(ok ? "true" : "false") + "}", "application/json");
        return;
    }
    if (pathOnly.find("/api/tasks/") == 0 && method == "DELETE") {
        size_t pos = pathOnly.find_last_of("/");
        int taskId = atoi(pathOnly.substr(pos + 1).c_str());
        bool ok = dbManager.deleteTask(taskId);
        sendResponse(clientSocket, "{\"success\":" + string(ok ? "true" : "false") + "}", "application/json");
        return;
    }

    // Teacher search & availability
    if (pathOnly == "/api/teachers/search" && method == "GET") {
string name = getParam(query, "name");
string university = getParam(query, "university");
string department = getParam(query, "department");
        auto teachers = dbManager.searchTeachers(name, university, department);
string json = "{\"teachers\":[";
        for (size_t i = 0; i < teachers.size(); ++i) {
            if (i) json += ",";
            json += "{\"username\":\"" + jsonEscape(teachers[i].username) + "\",";
            json += "\"displayName\":\"" + jsonEscape(teachers[i].displayName) + "\",";
            json += "\"university\":\"" + jsonEscape(teachers[i].university) + "\",";
            json += "\"department\":\"" + jsonEscape(teachers[i].department) + "\"}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }

    if (pathOnly == "/api/teachers/availability" && method == "GET") {
string teacher = getParam(query, "teacher");
        if (teacher.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing teacher\"}", "application/json", 400);
            return;
        }
        auto slots = dbManager.getTeacherAvailability(teacher);
string json = "{\"slots\":[";
        for (size_t i = 0; i < slots.size(); ++i) {
            if (i) json += ",";
            json += "{\"id\":" + to_string(slots[i].id) + ",";
            json += "\"dayOfWeek\":" + to_string(slots[i].dayOfWeek) + ",";
            json += "\"startTime\":\"" + jsonEscape(slots[i].startTime) + "\",";
            json += "\"endTime\":\"" + jsonEscape(slots[i].endTime) + "\",";
            json += "\"note\":\"" + jsonEscape(slots[i].note) + "\"}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }

    if (pathOnly == "/api/teachers/availability" && method == "POST") {
string teacher = getValueFromJson(body, "username");
string dayStr = getValueFromJson(body, "dayOfWeek");
string startTime = getValueFromJson(body, "startTime");
string endTime = getValueFromJson(body, "endTime");
string note = getValueFromJson(body, "note");
string clearExisting = getValueFromJson(body, "clearExisting");
        if (teacher.empty() || dayStr.empty() || startTime.empty() || endTime.empty()) {
            sendResponse(clientSocket, "{\"success\":false,\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        int day = atoi(dayStr.c_str());
        if (clearExisting == "true") {
            dbManager.clearTeacherAvailability(teacher);
        }
        TeacherAvailabilitySlot slot;
        slot.id = 0;
        slot.teacherUsername = teacher;
        slot.dayOfWeek = day;
        slot.startTime = startTime;
        slot.endTime = endTime;
        slot.note = note;
        bool ok = dbManager.addTeacherAvailabilitySlot(slot);
        sendResponse(clientSocket, "{\"success\":" + string(ok ? "true" : "false") + "}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/quote" && method == "GET") {
string monthStr = getParam(query, "month");
string role = getParam(query, "role");
string religion = getParam(query, "religion");
        if (monthStr.empty() || role.empty()) {
            sendResponse(clientSocket, "{\"quote\":\"May your month be filled with joy and purpose.\"}", "application/json");
            return;
        }
        int month = atoi(monthStr.c_str());
        if (month < 1 || month > 12) month = 1;
string quote = getQuoteForMonth(month, role, religion);
        sendResponse(clientSocket, "{\"quote\":\"" + jsonEscape(quote) + "\"}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/events/all" && method == "GET") {
string username = getParam(query, "username");
        if (username.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        auto allEvents = dbManager.getAllEvents(username);
string json = "{\"events\":[";
        for (size_t i = 0; i < allEvents.size(); i++) {
            if (i > 0) json += ",";
            json += "{";
            json += "\"date\":\"" + jsonEscape(allEvents[i].date) + "\",";
            json += "\"description\":\"" + jsonEscape(allEvents[i].description) + "\"";
            json += "}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    if (pathOnly == "/api/search/suggestions" && method == "GET") {
string username = getParam(query, "username");
string role = getParam(query, "role");
string religion = getParam(query, "religion");
        if (username.empty() || role.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        auto suggestions = dbManager.getAllSearchableStrings(username, role, religion);
        sendResponse(clientSocket, "{\"suggestions\":" + buildJsonArray(suggestions) + "}", "application/json");
        return;
    }
    
    if (pathOnly == "/api/events/recurrence" && method == "GET") {
string username = getParam(query, "username");
string date = getParam(query, "date");
        if (username.empty() || date.empty()) {
            sendResponse(clientSocket, "{\"error\":\"Missing parameters\"}", "application/json", 400);
            return;
        }
        
        auto events = dbManager.getEventsWithRecurrence(username, date);
string json = "{\"events\":[";
        for (size_t i = 0; i < events.size(); i++) {
            if (i > 0) json += ",";
            json += "{";
            json += "\"id\":" + to_string(events[i].id) + ",";
            json += "\"description\":\"" + jsonEscape(events[i].description) + "\",";
            json += "\"startTime\":\"" + jsonEscape(events[i].startTime) + "\",";
            json += "\"endTime\":\"" + jsonEscape(events[i].endTime) + "\",";
            json += "\"color\":\"" + jsonEscape(events[i].color) + "\",";
            json += "\"isPersonal\":" + string(events[i].isPersonal ? "true" : "false") + ",";
            json += "\"recurrence\":\"" + jsonEscape(events[i].recurrence) + "\",";
            json += "\"category\":\"" + jsonEscape(events[i].category) + "\",";
            json += "\"categoryIcon\":\"" + jsonEscape(events[i].categoryIcon) + "\"";
            json += "}";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    if (pathOnly == "/api/search/trie" && method == "GET") {
string term = getParam(query, "term");
        if (term.empty()) {
            sendResponse(clientSocket, "{\"eventIds\":[]}", "application/json");
            return;
        }
        
        auto eventIds = dbManager.searchEventsWithTrie(term);
string json = "{\"eventIds\":[";
        for (size_t i = 0; i < eventIds.size(); i++) {
            if (i > 0) json += ",";
            json += to_string(eventIds[i]);
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    if (pathOnly == "/api/universities" && method == "GET") {
        auto universities = dbManager.getAllUniversities();
string json = "{\"universities\":[";
        for (size_t i = 0; i < universities.size(); i++) {
            if (i > 0) json += ",";
            json += "\"" + jsonEscape(universities[i]) + "\"";
        }
        json += "]}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    if (pathOnly == "/api/departments" && method == "GET") {
        auto departments = dbManager.getAllDepartments();
string json = "{\"departments\":{";
        for (size_t i = 0; i < departments.size(); i++) {
            if (i > 0) json += ",";
            json += "\"" + jsonEscape(departments[i].first) + "\":\"" + jsonEscape(departments[i].second) + "\"";
        }
        json += "}}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    if (pathOnly == "/api/validate/roll" && method == "GET") {
string roll = getParam(query, "roll");
        bool valid = dbManager.isValidRollNumber(roll);
string dept = dbManager.getDepartmentFromRoll(roll);
string deptName = dbManager.getDepartmentName(dept);
        
string json = "{\"valid\":" + string(valid ? "true" : "false");
        json += ",\"department\":\"" + jsonEscape(dept) + "\"";
        json += ",\"departmentName\":\"" + jsonEscape(deptName) + "\"}";
        sendResponse(clientSocket, json, "application/json");
        return;
    }
    
    sendResponse(clientSocket, "Not Found", "text/plain", 404);
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
cerr << "WSAStartup failed" << endl;
        return 1;
    }
#endif
    
    // Initialize database
    if (!dbManager.openDatabase()) {
cerr << "Failed to open database" << endl;
        return 1;
    }
    dbManager.createTables();
    populateHolidays();
    initializeQuotes();
    dbManager.buildSearchIndex();
    
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
cerr << "Socket creation failed" << endl;
        return 1;
    }
    
    // Set socket options
    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    
    // Bind socket
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    // Get port from environment variable (for cloud deployment) or use default 8080
    int port = 8080;
    const char* portEnv = getenv("PORT");
    if (portEnv != nullptr) {
        port = atoi(portEnv);
        if (port <= 0 || port > 65535) port = 8080; // Validate port range
    }
    serverAddr.sin_port = htons(port);
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
cerr << "Bind failed" << endl;
        return 1;
    }
    
    // Listen
    if (listen(serverSocket, 10) < 0) {
cerr << "Listen failed" << endl;
        return 1;
    }
    
cout << "Time Weaver server running on port " << port << endl;
    
    // Accept connections
    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) continue;
        
        // Read request
        char buffer[8192];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
string request(buffer);
            handleRequest(clientSocket, request);
        }
        
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
    }
    
    dbManager.closeDatabase();
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
