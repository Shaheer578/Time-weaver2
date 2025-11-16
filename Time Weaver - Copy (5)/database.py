"""
Database Manager for Time Weaver
Converted from C++ to Python
"""

import sqlite3
import re
from typing import List, Optional, Tuple, Dict
from datetime import datetime, timedelta
from dsa_structures import EventSearchTrie, LRUCache, RecurrenceEngine


class EventDetails:
    """Event details structure"""
    def __init__(self):
        self.id: int = 0
        self.description: str = ""
        self.start_time: str = ""
        self.end_time: str = ""
        self.color: str = ""
        self.is_personal: bool = False
        self.recurrence: str = ""
        self.category: str = ""
        self.category_icon: str = ""


class DateEvent:
    """Date-event pair"""
    def __init__(self):
        self.date: str = ""
        self.description: str = ""


class UserProfile:
    """Academic user profile"""
    def __init__(self):
        self.username: str = ""
        self.role: str = ""
        self.display_name: str = ""
        self.university: str = ""
        self.department: str = ""


class TeacherAvailabilitySlot:
    """Teacher availability slot"""
    def __init__(self):
        self.id: int = 0
        self.teacher_username: str = ""
        self.day_of_week: int = 0
        self.start_time: str = ""
        self.end_time: str = ""
        self.note: str = ""


class DatabaseManager:
    """Database manager class"""
    
    def __init__(self):
        self.db: Optional[sqlite3.Connection] = None
        self.is_open: bool = False
        self.search_trie = EventSearchTrie()
        self.holiday_cache = LRUCache(100)
    
    def open_database(self) -> bool:
        """Open database connection"""
        if self.is_open:
            return True
        
        try:
            self.db = sqlite3.connect("calendar.db", check_same_thread=False)
            self.is_open = True
            return True
        except sqlite3.Error:
            return False
    
    def close_database(self) -> None:
        """Close database connection"""
        if self.db:
            self.db.close()
            self.db = None
        self.is_open = False
    
    def create_tables(self) -> bool:
        """Create all database tables"""
        if not self.is_open:
            return False
        
        tables = [
            """CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT NOT NULL,
                password TEXT,
                role TEXT NOT NULL,
                university TEXT,
                UNIQUE(username, role, university)
            )""",
            """CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT NOT NULL,
                event_date DATE NOT NULL,
                event_description TEXT NOT NULL,
                start_time TEXT,
                end_time TEXT,
                color TEXT,
                is_personal BOOLEAN NOT NULL,
                recurrence_type TEXT NOT NULL DEFAULT 'none',
                recurrence_count INTEGER DEFAULT 0,
                category TEXT,
                category_icon TEXT,
                department TEXT
            )""",
            """CREATE TABLE IF NOT EXISTS holidays (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                religion TEXT NOT NULL,
                holiday_date DATE NOT NULL,
                holiday_name TEXT NOT NULL
            )""",
            """CREATE TABLE IF NOT EXISTS tasks (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT NOT NULL,
                title TEXT NOT NULL,
                due_date DATE,
                completed INTEGER NOT NULL DEFAULT 0
            )""",
            """CREATE TABLE IF NOT EXISTS user_profiles (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT NOT NULL,
                role TEXT NOT NULL,
                display_name TEXT,
                university TEXT,
                department TEXT,
                UNIQUE(username, role, university)
            )""",
            """CREATE TABLE IF NOT EXISTS teacher_availability (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                teacher_username TEXT NOT NULL,
                day_of_week INTEGER NOT NULL,
                start_time TEXT NOT NULL,
                end_time TEXT NOT NULL,
                note TEXT
            )""",
            """CREATE TABLE IF NOT EXISTS universities (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL UNIQUE
            )""",
            """CREATE TABLE IF NOT EXISTS departments (
                code TEXT PRIMARY KEY,
                name TEXT NOT NULL
            )"""
        ]
        
        try:
            cursor = self.db.cursor()
            for table_sql in tables:
                cursor.execute(table_sql)
            
            # Add columns if they don't exist (migration)
            try:
                cursor.execute("ALTER TABLE events ADD COLUMN category TEXT")
            except sqlite3.OperationalError:
                pass
            
            try:
                cursor.execute("ALTER TABLE events ADD COLUMN category_icon TEXT")
            except sqlite3.OperationalError:
                pass
            
            try:
                cursor.execute("ALTER TABLE events ADD COLUMN department TEXT")
            except sqlite3.OperationalError:
                pass
            
            self.db.commit()
            self.populate_universities_and_departments()
            return True
        except sqlite3.Error:
            return False
    
    def add_user(self, username: str, password: str, role: str, university: str = "") -> bool:
        """Add a new user"""
        if self.user_exists(username, role, university):
            return False
        
        try:
            cursor = self.db.cursor()
            cursor.execute(
                "INSERT INTO users (username, password, role, university) VALUES (?, ?, ?, ?)",
                (username, password, role, university)
            )
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def user_exists(self, username: str, role: str, university: str = "") -> bool:
        """Check if user exists"""
        try:
            cursor = self.db.cursor()
            if university:
                cursor.execute(
                    "SELECT username FROM users WHERE username = ? AND role = ? AND university = ?",
                    (username, role, university)
                )
            else:
                cursor.execute(
                    "SELECT username FROM users WHERE username = ? AND role = ? AND (university IS NULL OR university = '')",
                    (username, role)
                )
            return cursor.fetchone() is not None
        except sqlite3.Error:
            return False
    
    def validate_user(self, username: str, password: str, role: str, university: str = "") -> bool:
        """Validate user credentials"""
        try:
            cursor = self.db.cursor()
            if university:
                cursor.execute(
                    "SELECT password FROM users WHERE username = ? AND role = ? AND university = ?",
                    (username, role, university)
                )
            else:
                cursor.execute(
                    "SELECT password FROM users WHERE username = ? AND role = ? AND (university IS NULL OR university = '')",
                    (username, role)
                )
            row = cursor.fetchone()
            if row and row[0] == password:
                return True
            return False
        except sqlite3.Error:
            return False
    
    def add_event(self, username: str, date: str, event_desc: str, start_time: str = "",
                  end_time: str = "", color: str = "", is_personal: bool = False,
                  recurrence: str = "none", category: str = "", category_icon: str = "",
                  department: str = "", recurrence_count: int = 0) -> bool:
        """Add a new event"""
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """INSERT INTO events (username, event_date, event_description, start_time, 
                    end_time, color, is_personal, recurrence_type, recurrence_count, category, 
                    category_icon, department) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                (username, date, event_desc, start_time or None, end_time or None, color or None,
                 is_personal, recurrence, recurrence_count, category or None, category_icon or None,
                 department or None)
            )
            event_id = cursor.lastrowid
            self.db.commit()
            # Index event for search
            self.index_event(event_id, event_desc)
            return True
        except sqlite3.Error:
            return False
    
    def get_events(self, username: str, date: str) -> List[EventDetails]:
        """Get events for a specific date"""
        events = []
        
        # Determine department from username
        department_code = ""
        if username.startswith("CT-"):
            department_code = "CT"
        elif username.startswith("ME-"):
            department_code = "ME"
        elif username.startswith("EE-"):
            department_code = "EE"
        elif username.startswith("CHE-"):
            department_code = "CHE"
        
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """SELECT id, event_description, start_time, end_time, color, is_personal, 
                    recurrence_type, event_date, category, category_icon, recurrence_count 
                    FROM events 
                    WHERE ((username = ? AND is_personal = 1) 
                    OR (username = ? AND is_personal = 0) 
                    OR (? <> '' AND is_personal = 0 AND department = ?))""",
                (username, username, department_code, department_code)
            )
            
            for row in cursor.fetchall():
                event_id, desc, st, et, col, is_pers, rec_type, event_date, cat, cat_icon, rec_count = row
                
                # Check if event occurs on the requested date
                occurs = False
                if rec_type == "none" and event_date == date:
                    occurs = True
                elif rec_type == "daily" and event_date <= date:
                    try:
                        start_dt = datetime.strptime(event_date, "%Y-%m-%d")
                        target_dt = datetime.strptime(date, "%Y-%m-%d")
                        days_diff = (target_dt - start_dt).days
                        if rec_count == 0 or (days_diff >= 0 and (days_diff + 1) <= rec_count):
                            occurs = True
                    except ValueError:
                        pass
                elif rec_type == "weekly" and event_date <= date:
                    try:
                        start_dt = datetime.strptime(event_date, "%Y-%m-%d")
                        target_dt = datetime.strptime(date, "%Y-%m-%d")
                        if start_dt.weekday() == target_dt.weekday():
                            weeks_diff = (target_dt - start_dt).days // 7
                            if rec_count == 0 or (weeks_diff >= 0 and (weeks_diff + 1) <= rec_count):
                                occurs = True
                    except ValueError:
                        pass
                elif rec_type == "monthly" and event_date <= date:
                    if event_date[8:] == date[8:]:  # Same day of month
                        try:
                            start_dt = datetime.strptime(event_date, "%Y-%m-%d")
                            target_dt = datetime.strptime(date, "%Y-%m-%d")
                            months_diff = (target_dt.year - start_dt.year) * 12 + (target_dt.month - start_dt.month)
                            if rec_count == 0 or (months_diff >= 0 and (months_diff + 1) <= rec_count):
                                occurs = True
                        except ValueError:
                            pass
                elif rec_type == "yearly" and event_date <= date:
                    if event_date[5:] == date[5:]:  # Same month and day
                        try:
                            start_dt = datetime.strptime(event_date, "%Y-%m-%d")
                            target_dt = datetime.strptime(date, "%Y-%m-%d")
                            years_diff = target_dt.year - start_dt.year
                            if rec_count == 0 or (years_diff >= 0 and (years_diff + 1) <= rec_count):
                                occurs = True
                        except ValueError:
                            pass
                
                if occurs:
                    event = EventDetails()
                    event.id = event_id
                    event.description = desc or ""
                    event.start_time = st or ""
                    event.end_time = et or ""
                    event.color = col or ""
                    event.is_personal = bool(is_pers)
                    event.recurrence = rec_type or "none"
                    event.category = cat or ""
                    event.category_icon = cat_icon or ""
                    events.append(event)
            
            return events
        except sqlite3.Error:
            return []
    
    def update_event(self, event_id: int, new_description: str, new_start_time: str = "",
                    new_end_time: str = "", new_color: str = "") -> bool:
        """Update an event"""
        try:
            cursor = self.db.cursor()
            cursor.execute(
                "UPDATE events SET event_description = ?, start_time = ?, end_time = ?, color = ? WHERE id = ?",
                (new_description, new_start_time or None, new_end_time or None, new_color or None, event_id)
            )
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def delete_event(self, event_id: int) -> bool:
        """Delete an event"""
        try:
            cursor = self.db.cursor()
            cursor.execute("DELETE FROM events WHERE id = ?", (event_id,))
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def search_events(self, username: str, search_term: str, religion: str = "") -> List[DateEvent]:
        """Search events by term"""
        results = []
        like_term = f"%{search_term}%"
        
        # Determine department from username
        department_code = ""
        if username.startswith("CT-"):
            department_code = "CT"
        elif username.startswith("ME-"):
            department_code = "ME"
        elif username.startswith("EE-"):
            department_code = "EE"
        elif username.startswith("CHE-"):
            department_code = "CHE"
        
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """SELECT event_date, event_description FROM events 
                    WHERE event_description LIKE ? AND 
                    ((username = ? AND is_personal = 1) OR 
                    (username = ? AND is_personal = 0) OR 
                    (? <> '' AND is_personal = 0 AND department = ?))""",
                (like_term, username, username, department_code, department_code)
            )
            
            for row in cursor.fetchall():
                de = DateEvent()
                de.date = row[0]
                de.description = row[1]
                results.append(de)
            
            # Also search holidays if personal user
            is_academic = username.startswith("CT-") or username == "teacher"
            if not is_academic and religion and religion != "Select Religion":
                cursor.execute(
                    "SELECT holiday_date, holiday_name FROM holidays WHERE holiday_name LIKE ? AND religion = ?",
                    (like_term, religion)
                )
                for row in cursor.fetchall():
                    de = DateEvent()
                    de.date = row[0]
                    de.description = f"[Holiday] {row[1]}"
                    results.append(de)
            
            results.sort(key=lambda x: x.date)
            return results
        except sqlite3.Error:
            return []
    
    def get_event_dates_for_month(self, username: str, year: int, month: int) -> List[str]:
        """Get all dates with events in a month"""
        dates = []
        
        # Determine department from username
        department_code = ""
        if username.startswith("CT-"):
            department_code = "CT"
        elif username.startswith("ME-"):
            department_code = "ME"
        elif username.startswith("EE-"):
            department_code = "EE"
        elif username.startswith("CHE-"):
            department_code = "CHE"
        
        # Calculate first and last day of month
        first_day = f"{year:04d}-{month:02d}-01"
        if month == 2:
            days_in_month = 29 if (year % 4 == 0 and (year % 100 != 0 or year % 400 == 0)) else 28
        elif month in [4, 6, 9, 11]:
            days_in_month = 30
        else:
            days_in_month = 31
        last_day = f"{year:04d}-{month:02d}-{days_in_month:02d}"
        
        try:
            cursor = self.db.cursor()
            # Get non-recurring events
            cursor.execute(
                """SELECT event_date FROM events 
                    WHERE ((username = ? AND is_personal = 1) OR 
                    (username = ? AND is_personal = 0) OR 
                    (? <> '' AND is_personal = 0 AND department = ?)) 
                    AND recurrence_type = 'none' AND event_date BETWEEN ? AND ?""",
                (username, username, department_code, department_code, first_day, last_day)
            )
            for row in cursor.fetchall():
                dates.append(row[0])
            
            # Get recurring events
            cursor.execute(
                """SELECT event_date, recurrence_type, recurrence_count FROM events 
                    WHERE ((username = ? AND is_personal = 1) OR 
                    (username = ? AND is_personal = 0) OR 
                    (? <> '' AND is_personal = 0 AND department = ?)) 
                    AND recurrence_type != 'none' AND event_date <= ?""",
                (username, username, department_code, department_code, last_day)
            )
            
            for row in cursor.fetchall():
                event_date, rec_type, rec_count = row
                try:
                    start_dt = datetime.strptime(event_date, "%Y-%m-%d")
                    for day in range(1, days_in_month + 1):
                        try:
                            current_dt = datetime(year, month, day)
                            if current_dt < start_dt:
                                continue
                            
                            occurs = False
                            if rec_type == "daily":
                                days_diff = (current_dt - start_dt).days
                                if rec_count == 0 or (days_diff + 1) <= rec_count:
                                    occurs = True
                            elif rec_type == "weekly":
                                if start_dt.weekday() == current_dt.weekday():
                                    weeks_diff = (current_dt - start_dt).days // 7
                                    if rec_count == 0 or (weeks_diff + 1) <= rec_count:
                                        occurs = True
                            elif rec_type == "monthly":
                                if start_dt.day == current_dt.day:
                                    months_diff = (current_dt.year - start_dt.year) * 12 + (current_dt.month - start_dt.month)
                                    if rec_count == 0 or (months_diff + 1) <= rec_count:
                                        occurs = True
                            elif rec_type == "yearly":
                                if start_dt.month == current_dt.month and start_dt.day == current_dt.day:
                                    years_diff = current_dt.year - start_dt.year
                                    if rec_count == 0 or (years_diff + 1) <= rec_count:
                                        occurs = True
                            
                            if occurs:
                                dates.append(current_dt.strftime("%Y-%m-%d"))
                        except ValueError:
                            pass
                except ValueError:
                    pass
            
            # Remove duplicates and sort
            dates = sorted(list(set(dates)))
            return dates
        except sqlite3.Error:
            return []
    
    def get_all_events(self, username: str) -> List[DateEvent]:
        """Get all events for a user"""
        all_events = []
        
        # Determine department from username
        department_code = ""
        if username.startswith("CT-"):
            department_code = "CT"
        elif username.startswith("ME-"):
            department_code = "ME"
        elif username.startswith("EE-"):
            department_code = "EE"
        elif username.startswith("CHE-"):
            department_code = "CHE"
        
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """SELECT event_date, event_description FROM events 
                    WHERE (username = ? AND is_personal = 1) OR 
                    (username = ? AND is_personal = 0) OR 
                    (? <> '' AND is_personal = 0 AND department = ?)""",
                (username, username, department_code, department_code)
            )
            
            for row in cursor.fetchall():
                de = DateEvent()
                de.date = row[0]
                de.description = row[1]
                all_events.append(de)
            
            all_events.sort(key=lambda x: x.date)
            return all_events
        except sqlite3.Error:
            return []
    
    def get_all_searchable_strings(self, username: str, role: str, religion: str = "") -> List[str]:
        """Get all searchable strings for a user"""
        searchable_items = []
        
        # Determine department from username
        department_code = ""
        if username.startswith("CT-"):
            department_code = "CT"
        elif username.startswith("ME-"):
            department_code = "ME"
        elif username.startswith("EE-"):
            department_code = "EE"
        elif username.startswith("CHE-"):
            department_code = "CHE"
        
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """SELECT DISTINCT event_description FROM events 
                    WHERE (username = ? AND is_personal = 1) OR 
                    (username = ? AND is_personal = 0) OR 
                    (? <> '' AND is_personal = 0 AND department = ?)""",
                (username, username, department_code, department_code)
            )
            
            for row in cursor.fetchall():
                searchable_items.append(row[0])
            
            # Include holidays if personal user
            if role == "personal" and religion and religion != "Select Religion":
                cursor.execute(
                    "SELECT DISTINCT holiday_name FROM holidays WHERE religion = ?",
                    (religion,)
                )
                for row in cursor.fetchall():
                    searchable_items.append(row[0])
            
            return searchable_items
        except sqlite3.Error:
            return []
    
    def add_holiday(self, date: str, religion: str, holiday_name: str) -> bool:
        """Add a holiday"""
        try:
            cursor = self.db.cursor()
            # Check if exists
            cursor.execute(
                "SELECT id FROM holidays WHERE religion = ? AND holiday_date = ? AND holiday_name = ?",
                (religion, date, holiday_name)
            )
            if cursor.fetchone():
                return True  # Already exists
            
            cursor.execute(
                "INSERT INTO holidays (religion, holiday_date, holiday_name) VALUES (?, ?, ?)",
                (religion, date, holiday_name)
            )
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def get_holidays(self, date: str, religion: str = "") -> List[str]:
        """Get holidays for a date"""
        holidays = []
        
        try:
            cursor = self.db.cursor()
            if not religion or religion == "Select Religion":
                cursor.execute(
                    "SELECT holiday_name FROM holidays WHERE religion = 'General' AND holiday_date = ?",
                    (date,)
                )
            else:
                cursor.execute(
                    "SELECT holiday_name FROM holidays WHERE religion = ? AND holiday_date = ?",
                    (religion, date)
                )
            
            for row in cursor.fetchall():
                holidays.append(row[0])
            
            return holidays
        except sqlite3.Error:
            return []
    
    def get_holiday_dates_for_month(self, religion: str, year: int, month: int) -> List[str]:
        """Get holiday dates for a month"""
        dates = []
        
        first_day = f"{year:04d}-{month:02d}-01"
        if month == 2:
            days_in_month = 29 if (year % 4 == 0 and (year % 100 != 0 or year % 400 == 0)) else 28
        elif month in [4, 6, 9, 11]:
            days_in_month = 30
        else:
            days_in_month = 31
        last_day = f"{year:04d}-{month:02d}-{days_in_month:02d}"
        
        try:
            cursor = self.db.cursor()
            if not religion or religion == "Select Religion":
                cursor.execute(
                    "SELECT holiday_date FROM holidays WHERE religion = 'General' AND holiday_date BETWEEN ? AND ?",
                    (first_day, last_day)
                )
            else:
                cursor.execute(
                    "SELECT holiday_date FROM holidays WHERE religion = ? AND holiday_date BETWEEN ? AND ?",
                    (religion, first_day, last_day)
                )
            
            for row in cursor.fetchall():
                dates.append(row[0])
            
            return dates
        except sqlite3.Error:
            return []
    
    def clear_all_holidays(self) -> None:
        """Clear all holidays"""
        try:
            cursor = self.db.cursor()
            cursor.execute("DELETE FROM holidays")
            self.db.commit()
        except sqlite3.Error:
            pass
    
    def list_users(self) -> List[str]:
        """List all users"""
        users = []
        try:
            cursor = self.db.cursor()
            cursor.execute("SELECT username, role FROM users ORDER BY role, username")
            for row in cursor.fetchall():
                users.append(f"{row[0]}|{row[1]}")
            return users
        except sqlite3.Error:
            return []
    
    def delete_user(self, username: str, role: str) -> bool:
        """Delete a user"""
        try:
            cursor = self.db.cursor()
            cursor.execute("DELETE FROM users WHERE username = ? AND role = ?", (username, role))
            self.db.commit()
            return cursor.rowcount > 0
        except sqlite3.Error:
            return False
    
    def update_password(self, username: str, role: str, new_password: str) -> bool:
        """Update user password"""
        try:
            cursor = self.db.cursor()
            cursor.execute(
                "UPDATE users SET password = ? WHERE username = ? AND role = ?",
                (new_password, username, role)
            )
            self.db.commit()
            return cursor.rowcount > 0
        except sqlite3.Error:
            return False
    
    def upsert_user_profile(self, profile: UserProfile) -> bool:
        """Insert or update user profile"""
        try:
            cursor = self.db.cursor()
            # Try update first
            cursor.execute(
                """UPDATE user_profiles SET display_name = ?, university = ?, department = ? 
                    WHERE username = ? AND role = ?""",
                (profile.display_name, profile.university, profile.department, profile.username, profile.role)
            )
            if cursor.rowcount > 0:
                self.db.commit()
                return True
            
            # Insert if update didn't affect any row
            cursor.execute(
                """INSERT INTO user_profiles (username, role, display_name, university, department) 
                    VALUES (?, ?, ?, ?, ?)""",
                (profile.username, profile.role, profile.display_name, profile.university, profile.department)
            )
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def get_user_profile(self, username: str, role: str) -> Optional[UserProfile]:
        """Get user profile"""
        try:
            cursor = self.db.cursor()
            cursor.execute(
                "SELECT display_name, university, department FROM user_profiles WHERE username = ? AND role = ?",
                (username, role)
            )
            row = cursor.fetchone()
            if row:
                profile = UserProfile()
                profile.username = username
                profile.role = role
                profile.display_name = row[0] or ""
                profile.university = row[1] or ""
                profile.department = row[2] or ""
                return profile
            return None
        except sqlite3.Error:
            return None
    
    def search_teachers(self, name_like: str = "", university: str = "", department: str = "") -> List[UserProfile]:
        """Search for teachers"""
        teachers = []
        
        sql = "SELECT username, role, display_name, university, department FROM user_profiles WHERE role = 'teacher'"
        params = []
        
        if name_like:
            sql += " AND (display_name LIKE ? OR username LIKE ?)"
            like = f"%{name_like}%"
            params.extend([like, like])
        if university:
            sql += " AND university = ?"
            params.append(university)
        if department:
            sql += " AND department = ?"
            params.append(department)
        
        try:
            cursor = self.db.cursor()
            cursor.execute(sql, params)
            for row in cursor.fetchall():
                p = UserProfile()
                p.username = row[0]
                p.role = row[1]
                p.display_name = row[2] or ""
                p.university = row[3] or ""
                p.department = row[4] or ""
                teachers.append(p)
            return teachers
        except sqlite3.Error:
            return []
    
    def clear_teacher_availability(self, teacher_username: str) -> bool:
        """Clear teacher availability"""
        try:
            cursor = self.db.cursor()
            cursor.execute("DELETE FROM teacher_availability WHERE teacher_username = ?", (teacher_username,))
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def add_teacher_availability_slot(self, slot: TeacherAvailabilitySlot) -> bool:
        """Add teacher availability slot"""
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """INSERT INTO teacher_availability (teacher_username, day_of_week, start_time, end_time, note) 
                    VALUES (?, ?, ?, ?, ?)""",
                (slot.teacher_username, slot.day_of_week, slot.start_time, slot.end_time, slot.note or "")
            )
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def get_teacher_availability(self, teacher_username: str) -> List[TeacherAvailabilitySlot]:
        """Get teacher availability"""
        slots = []
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """SELECT id, day_of_week, start_time, end_time, note FROM teacher_availability 
                    WHERE teacher_username = ? ORDER BY day_of_week, start_time""",
                (teacher_username,)
            )
            for row in cursor.fetchall():
                s = TeacherAvailabilitySlot()
                s.id = row[0]
                s.teacher_username = teacher_username
                s.day_of_week = row[1]
                s.start_time = row[2] or ""
                s.end_time = row[3] or ""
                s.note = row[4] or ""
                slots.append(s)
            return slots
        except sqlite3.Error:
            return []
    
    def add_task(self, username: str, title: str, due_date: str = "") -> bool:
        """Add a task"""
        try:
            cursor = self.db.cursor()
            cursor.execute(
                "INSERT INTO tasks (username, title, due_date, completed) VALUES (?, ?, ?, 0)",
                (username, title, due_date or None)
            )
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def list_tasks(self, username: str) -> List[str]:
        """List tasks for a user"""
        tasks = []
        try:
            cursor = self.db.cursor()
            cursor.execute(
                "SELECT id, title, due_date, completed FROM tasks WHERE username = ? ORDER BY completed, due_date",
                (username,)
            )
            for row in cursor.fetchall():
                tasks.append(f"{row[0]}|{row[1] or ''}|{row[2] or ''}|{1 if row[3] else 0}")
            return tasks
        except sqlite3.Error:
            return []
    
    def toggle_task(self, task_id: int, completed: bool) -> bool:
        """Toggle task completion"""
        try:
            cursor = self.db.cursor()
            cursor.execute("UPDATE tasks SET completed = ? WHERE id = ?", (1 if completed else 0, task_id))
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def delete_task(self, task_id: int) -> bool:
        """Delete a task"""
        try:
            cursor = self.db.cursor()
            cursor.execute("DELETE FROM tasks WHERE id = ?", (task_id,))
            self.db.commit()
            return True
        except sqlite3.Error:
            return False
    
    def index_event(self, event_id: int, description: str) -> None:
        """Index event for search"""
        words = description.split()
        for word in words:
            self.search_trie.insert(word, event_id)
    
    def build_search_index(self) -> None:
        """Build search index from all events"""
        try:
            cursor = self.db.cursor()
            cursor.execute("SELECT id, event_description FROM events")
            for row in cursor.fetchall():
                event_id, desc = row
                if desc:
                    self.index_event(event_id, desc)
        except sqlite3.Error:
            pass
    
    def search_events_with_trie(self, search_term: str) -> List[int]:
        """Search events using trie"""
        return self.search_trie.search(search_term)
    
    def get_events_with_recurrence(self, username: str, target_date: str) -> List[EventDetails]:
        """Get events with recurrence calculation"""
        all_events = []
        try:
            cursor = self.db.cursor()
            cursor.execute(
                """SELECT id, event_description, start_time, end_time, color, is_personal, 
                    recurrence_type, category, category_icon, event_date FROM events 
                    WHERE username = ? OR is_personal = 0""",
                (username,)
            )
            
            for row in cursor.fetchall():
                event_id, desc, st, et, col, is_pers, rec_type, cat, cat_icon, event_date = row
                
                rec_type_enum = RecurrenceEngine.string_to_type(rec_type or "none")
                if RecurrenceEngine.matches_recurrence(event_date or "", target_date, rec_type_enum):
                    event = EventDetails()
                    event.id = event_id
                    event.description = desc or ""
                    event.start_time = st or ""
                    event.end_time = et or ""
                    event.color = col or ""
                    event.is_personal = bool(is_pers)
                    event.recurrence = rec_type or "none"
                    event.category = cat or ""
                    event.category_icon = cat_icon or ""
                    all_events.append(event)
            
            all_events.sort(key=lambda x: x.start_time)
            return all_events
        except sqlite3.Error:
            return []
    
    def populate_universities_and_departments(self) -> None:
        """Populate universities and departments"""
        try:
            cursor = self.db.cursor()
            # Check if already populated
            cursor.execute("SELECT COUNT(*) FROM universities")
            if cursor.fetchone()[0] > 0:
                return
            
            universities = [
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
            ]
            
            for uni in universities:
                cursor.execute("INSERT OR IGNORE INTO universities (name) VALUES (?)", (uni,))
            
            departments = [
                ("CT", "Computer Science"), ("SE", "Software Engineering"),
                ("ME", "Mechanical Engineering"), ("EE", "Electrical Engineering"),
                ("CE", "Civil Engineering"), ("CHE", "Chemical Engineering"),
                ("AI", "Artificial Intelligence"), ("DS", "Data Science"),
                ("CS", "Cyber Security"), ("TE", "Telecommunication Engineering"),
                ("AE", "Aerospace Engineering"), ("IE", "Industrial Engineering"),
                ("PE", "Petroleum Engineering"), ("BM", "Biomedical Engineering"),
                ("ENV", "Environmental Engineering"), ("AR", "Architecture"),
                ("PHY", "Physics"), ("MA", "Mathematics"),
                ("CH", "Chemistry"), ("BI", "Biology"),
                ("BT", "Biotechnology"), ("MB", "Microbiology"),
                ("BC", "Biochemistry"), ("BA", "Business Administration"),
                ("EC", "Economics"), ("FN", "Finance"),
                ("MK", "Marketing"), ("HR", "Human Resources"),
                ("AC", "Accounting"), ("MG", "Management Sciences"),
                ("EN", "English"), ("UR", "Urdu"),
                ("IS", "Islamic Studies"), ("PS", "Political Science"),
                ("SO", "Sociology"), ("PY", "Psychology"),
                ("ED", "Education"), ("LA", "Law"),
                ("MD", "Medicine"), ("PH", "Pharmacy"),
                ("NS", "Nursing"), ("DT", "Dentistry")
            ]
            
            for code, name in departments:
                cursor.execute("INSERT OR IGNORE INTO departments (code, name) VALUES (?, ?)", (code, name))
            
            self.db.commit()
        except sqlite3.Error:
            pass
    
    def get_department_from_roll(self, roll_number: str) -> str:
        """Extract department code from roll number"""
        match = re.match(r"^([A-Z]{2,3})-", roll_number)
        if match:
            return match.group(1)
        return ""
    
    def is_valid_roll_number(self, roll_number: str) -> bool:
        """Validate roll number format"""
        pattern = r"^[A-Z]{2,3}-[0-9]+$"
        return bool(re.match(pattern, roll_number))
    
    def get_department_name(self, code: str) -> str:
        """Get department name from code"""
        try:
            cursor = self.db.cursor()
            cursor.execute("SELECT name FROM departments WHERE code = ?", (code,))
            row = cursor.fetchone()
            if row:
                return row[0]
            return code
        except sqlite3.Error:
            return code
    
    def get_all_universities(self) -> List[str]:
        """Get all universities"""
        universities = []
        try:
            cursor = self.db.cursor()
            cursor.execute("SELECT name FROM universities ORDER BY name")
            for row in cursor.fetchall():
                universities.append(row[0])
            return universities
        except sqlite3.Error:
            return []
    
    def get_all_departments(self) -> List[Tuple[str, str]]:
        """Get all departments"""
        departments = []
        try:
            cursor = self.db.cursor()
            cursor.execute("SELECT code, name FROM departments ORDER BY code")
            for row in cursor.fetchall():
                departments.append((row[0], row[1]))
            return departments
        except sqlite3.Error:
            return []

