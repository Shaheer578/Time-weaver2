"""
Time Weaver Flask Application
Converted from C++ to Python
"""

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import csv
import os
import json
from database import DatabaseManager, UserProfile, TeacherAvailabilitySlot

app = Flask(__name__, static_folder='public', static_url_path='')
CORS(app)

# Initialize database manager
db_manager = DatabaseManager()

# Initialize database on module import (for serverless compatibility)
def init_database():
    """Initialize database connection and tables"""
    if not db_manager.is_open:
        if not db_manager.open_database():
            print("Failed to open database")
            return False
        db_manager.create_tables()
        populate_holidays()
        db_manager.build_search_index()
    return True

# Initialize on import
init_database()

# Quote management
quotes_map = {
    "academic": {
        1: ["The new year stands before us, like a chapter in a book, waiting to be written."],
        2: ["Develop a passion for learning. If you do, you will never cease to grow."],
        3: ["Spring is a reminder of how beautiful change can truly be. Embrace new knowledge."],
        4: ["Don't watch the clock; do what it does. Keep going."],
        5: ["The beautiful thing about learning is that nobody can take it away from you."],
        6: ["Success is the sum of small efforts, repeated day in and day out."],
        7: ["The expert in anything was once a beginner."],
        8: ["Believe you can and you're halfway there."],
        9: ["Education is the passport to the future, for tomorrow belongs to those who prepare for it today."],
        10: ["Autumn shows us how beautiful it is to let things go... perhaps bad study habits?"],
        11: ["Strive for progress, not perfection."],
        12: ["What we learn with pleasure we never forget."]
    },
    "personal_generic": {
        1: ["The beginning is the most important part of the work."],
        2: ["Kindness is like snow—it beautifies everything it covers."],
        3: ["No winter lasts forever; no spring skips its turn."],
        4: ["The earth laughs in flowers."],
        5: ["Where flowers bloom, so does hope."],
        6: ["Keep your face to the sun and you will never see the shadows."],
        7: ["Live in the sunshine, swim the sea, drink the wild air."],
        8: ["Summer afternoon—summer afternoon; to me those have always been the two most beautiful words."],
        9: ["Life starts all over again when it gets crisp in the fall."],
        10: ["Every leaf speaks bliss to me, fluttering from the autumn tree."],
        11: ["Gratitude turns what we have into enough."],
        12: ["The best and most beautiful things in the world cannot be seen or even touched - they must be felt with the heart."]
    },
    "personal_Christian": {
        1: ["For I know the plans I have for you, declares the Lord, plans for welfare and not for evil, to give you a future and a hope. - Jeremiah 29:11"],
        2: ["Love is patient and kind; love does not envy or boast; it is not arrogant or rude. - 1 Corinthians 13:4"],
        3: ["Therefore, if anyone is in Christ, he is a new creation. The old has passed away; behold, the new has come. - 2 Corinthians 5:17"],
        4: ["He is not here; he has risen, just as he said. - Matthew 28:6 (Easter)"],
        5: ["Ask, and it will be given to you; seek, and you will find; knock, and it will be opened to you. - Matthew 7:7"],
        6: ["I can do all things through him who strengthens me. - Philippians 4:13"],
        7: ["The Lord is my shepherd; I shall not want. - Psalm 23:1"],
        8: ["Be strong and courageous. Do not be frightened, and do not be dismayed, for the Lord your God is with you wherever you go. - Joshua 1:9"],
        9: ["Trust in the Lord with all your heart, and do not lean on your own understanding. - Proverbs 3:5"],
        10: ["Give thanks in all circumstances; for this is the will of God in Christ Jesus for you. - 1 Thessalonians 5:18"],
        11: ["The steadfast love of the Lord never ceases; his mercies never come to an end. - Lamentations 3:22"],
        12: ["For unto you is born this day in the city of David a Savior, who is Christ the Lord. - Luke 2:11 (Christmas)"]
    },
    "personal_Hindu": {
        1: ["As the sun brightens the world, may your life be brightened with knowledge and joy. Happy Makar Sankranti!"],
        2: ["May Lord Shiva shower his blessings on you and your family. Om Namah Shivaya! (Maha Shivaratri)"],
        3: ["Let the colors of Holi spread the message of peace and happiness."],
        4: ["May the divine grace of Lord Ram always be with you. Wish you a very happy and prosperous Rama Navami."],
        5: ["The mind acts like an enemy for those who do not control it. - Bhagavad Gita"],
        6: ["Do your duty, but do not concern yourself with the results. - Bhagavad Gita"],
        7: ["May Lord Krishna steal all your tensions and worries... Happy Janmashtami!"],
        8: ["A brother is a friend given by Nature. Celebrate the bond of Raksha Bandhan."],
        9: ["May Goddess Durga illuminate your life with countless blessings of happiness. (Navaratri)"],
        10: ["May the festival of lights fill your life with the glow of happiness and the sparkle of joy. Happy Diwali!"],
        11: ["There is nothing lost or wasted in this life. - Bhagavad Gita"],
        12: ["Look to this day, for it is life. The very life of life."]
    },
    "personal_Islam": {
        1: ["The best among you are those who have the best manners and character. - Prophet Muhammad (peace be upon him)"],
        2: ["Indeed, Allah is with the patient. - Quran 2:153"],
        3: ["Ramadan is the month whose beginning is mercy, whose middle is forgiveness, and whose end is freedom from the fire."],
        4: ["May the blessings of Allah fill your life with happiness and open all the doors of success now and always. Eid Mubarak! (Eid al-Fitr)"],
        5: ["Speak good or remain silent. - Prophet Muhammad (peace be upon him)"],
        6: ["May His blessings always shine upon you and your family on this auspicious day. Eid Mubarak! (Eid al-Adha)"],
        7: ["Do not lose hope, nor be sad. - Quran 3:139"],
        8: ["He who eats his fill while his neighbor goes hungry is not a believer."],
        9: ["Kindness is a mark of faith, and whoever has not kindness has not faith."],
        10: ["Allah does not burden a soul beyond that it can bear. - Quran 2:286"],
        11: ["The seeking of knowledge is obligatory for every Muslim."],
        12: ["Verily, with hardship, there is relief. - Quran 94:6"]
    },
    "personal_Jewish": {
        1: ["Who is wise? One who learns from every man. - Pirkei Avot"],
        2: ["Just as despair can come to one only from himself, so too can hope be generated by man himself."],
        3: ["On three things the world stands: On Torah, on service [of God], and on acts of human kindness. - Pirkei Avot"],
        4: ["In every generation, one must see oneself as if one personally came out of Egypt. (Passover)"],
        5: ["From the Heights of Sinai, we received the Torah, not just the Ten Commandments, but the totality of Jewish Law and Tradition. (Shavuot)"],
        6: ["Do not be daunted by the enormity of the world's grief. Do justly, now. Love mercy, now. Walk humbly, now."],
        7: ["This is the day the Lord has made; let us rejoice and be glad in it. - Psalm 118:24"],
        8: ["If I am not for myself, who will be for me? But if I am only for myself, what am I? - Hillel"],
        9: ["May you be inscribed and sealed for a good year. Shana Tova! (Rosh Hashanah)"],
        10: ["May your fasting be easy and meaningful. G'mar Chatima Tova. (Yom Kippur)"],
        11: ["The highest form of wisdom is kindness."],
        12: ["A little bit of light dispels a lot of darkness. (Hanukkah)"]
    },
    "personal_Sikh": {
        1: ["Recognize the whole human race as one. - Guru Gobind Singh Ji"],
        2: ["Even Kings and emperors with heaps of wealth and vast dominion cannot compare with an ant filled with the love of God."],
        3: ["Speak only that which will bring you honor."],
        4: ["Let no man in the world live in delusion. Without a Guru, none can cross over to the other shore. (Vaisakhi)"],
        5: ["Conquer your mind and conquer the world."],
        6: ["He who has no faith in himself can never have faith in God."],
        7: ["Dwell in peace in the home of your own being, and the Messenger of Death will not be able to touch you."],
        8: ["The world is a drama, staged in a dream."],
        9: ["Before becoming a Sikh, a Muslim, a Hindu or a Christian, let's become a Human first."],
        10: ["Serve the saints; earn the lasting treasure."],
        11: ["Nanak Naam Chardi Kala, Tere Bhane Sarbat Da Bhala. (May the Holy Name be ever-increasing, May goodwill prevail over all, by Your Grace - Guru Nanak Jayanti)"],
        12: ["Sing the songs of joy to the Lord, serve the Name of the Lord, and become the servant of His servants."]
    }
}


def get_quote_for_month(month: int, role: str, religion: str = "") -> str:
    """Get quote for month"""
    quote_key = role
    if role == "personal":
        if religion and religion != "Select Religion":
            quote_key += "_" + religion
        else:
            quote_key += "_generic"
    elif role in ["student", "teacher"]:
        quote_key = "academic"
    else:
        quote_key = "personal_generic"
    
    if quote_key in quotes_map and month in quotes_map[quote_key] and quotes_map[quote_key][month]:
        return quotes_map[quote_key][month][0]
    return "May your month be filled with joy and purpose."


def populate_holidays():
    """Populate holidays from CSV"""
    db_manager.clear_all_holidays()
    try:
        with open("HolidayData.csv", "r", encoding="utf-8") as file:
            reader = csv.reader(file)
            next(reader)  # Skip header
            for row in reader:
                if len(row) >= 3:
                    date = row[0].strip()
                    religion = row[1].strip()
                    name = row[2].strip()
                    if date and religion and name:
                        db_manager.add_holiday(date, religion, name)
    except FileNotFoundError:
        pass


def color_name_to_hex(name: str) -> str:
    """Convert color name to hex"""
    colors = {
        "Red": "#FF0000",
        "Green": "#008000",
        "Blue": "#0000FF",
        "Yellow": "#FFFF00",
        "Orange": "#FFA500",
        "Purple": "#800080"
    }
    return colors.get(name, "")


# Static file serving
@app.route('/')
@app.route('/index.html')
def index():
    return send_from_directory('public', 'index.html')


@app.route('/<path:path>')
def serve_static(path):
    """Serve static files"""
    return send_from_directory('public', path)


# API Routes
@app.route('/api/user/add', methods=['POST'])
def add_user():
    """Add a new user"""
    data = request.get_json()
    username = data.get('username', '').strip()
    password = data.get('password', '').strip()
    role = data.get('role', '').strip()
    display_name = data.get('displayName', '').strip()
    university = data.get('university', '').strip()
    department = data.get('department', '').strip()
    
    exists = db_manager.user_exists(username, role, university)
    success = False
    
    if not exists:
        success = db_manager.add_user(username, password, role, university)
        # For academic users, also upsert profile
        if success and role in ["student", "teacher"]:
            profile = UserProfile()
            profile.username = username
            profile.role = role
            profile.display_name = display_name or username
            profile.university = university
            profile.department = department
            db_manager.upsert_user_profile(profile)
    
    return jsonify({"success": success, "exists": exists})


@app.route('/api/user/validate', methods=['POST'])
def validate_user():
    """Validate user credentials"""
    data = request.get_json()
    username = data.get('username', '').strip()
    password = data.get('password', '').strip()
    role = data.get('role', '').strip()
    university = data.get('university', '').strip()
    department = data.get('department', '').strip()
    
    # Allow built-in admin
    valid = False
    if username.lower() == "admin" and password == "admin123":
        valid = True
    else:
        valid = db_manager.validate_user(username, password, role, university)
    
    if not valid:
        return jsonify({"valid": False, "error": "Invalid credentials"}), 401
    
    # For academic users, return profile data
    if role in ["student", "teacher"]:
        profile = db_manager.get_user_profile(username, role)
        if profile:
            return jsonify({
                "valid": True,
                "displayName": profile.display_name,
                "university": profile.university,
                "department": profile.department
            })
        else:
            return jsonify({"valid": False, "error": "Invalid credentials"}), 401
    
    return jsonify({"valid": True})


@app.route('/api/user/profile', methods=['POST'])
def upsert_profile():
    """Upsert user profile"""
    data = request.get_json()
    username = data.get('username', '').strip()
    role = data.get('role', '').strip()
    display_name = data.get('displayName', '').strip()
    university = data.get('university', '').strip()
    department = data.get('department', '').strip()
    
    if not username or not role:
        return jsonify({"success": False, "error": "Missing parameters"}), 400
    
    profile = UserProfile()
    profile.username = username
    profile.role = role
    profile.display_name = display_name or username
    profile.university = university
    profile.department = department
    
    success = db_manager.upsert_user_profile(profile)
    return jsonify({"success": success})


@app.route('/api/users', methods=['GET'])
def list_users():
    """List all users (admin only)"""
    admin = request.args.get('admin')
    if admin != "true":
        return jsonify({"error": "Unauthorized"}), 403
    
    users = db_manager.list_users()
    result = []
    for user_str in users:
        parts = user_str.split('|')
        if len(parts) >= 2:
            result.append({"username": parts[0], "role": parts[1]})
    
    return jsonify({"users": result})


@app.route('/api/users/password', methods=['PUT'])
def update_password():
    """Update user password (admin only)"""
    data = request.get_json()
    admin = data.get('admin')
    if admin != "true":
        return jsonify({"error": "Unauthorized"}), 403
    
    username = data.get('username', '').strip()
    role = data.get('role', '').strip()
    new_password = data.get('password', '').strip()
    
    success = db_manager.update_password(username, role, new_password)
    return jsonify({"success": success})


@app.route('/api/users', methods=['DELETE'])
def delete_user():
    """Delete user (admin only)"""
    admin = request.args.get('admin')
    if admin != "true":
        return jsonify({"error": "Unauthorized"}), 403
    
    username = request.args.get('username', '').strip()
    role = request.args.get('role', '').strip()
    
    success = db_manager.delete_user(username, role)
    return jsonify({"success": success})


@app.route('/api/events', methods=['GET'])
def get_events():
    """Get events for a date"""
    username = request.args.get('username', '').strip()
    date = request.args.get('date', '').strip()
    
    if not username or not date:
        return jsonify({"error": "Missing parameters"}), 400
    
    events = db_manager.get_events(username, date)
    result = []
    for event in events:
        result.append({
            "id": event.id,
            "description": event.description,
            "startTime": event.start_time,
            "endTime": event.end_time,
            "color": event.color,
            "isPersonal": event.is_personal,
            "recurrence": event.recurrence,
            "category": event.category,
            "categoryIcon": event.category_icon
        })
    
    return jsonify({"events": result})


@app.route('/api/events', methods=['POST'])
def add_event():
    """Add a new event"""
    data = request.get_json()
    username = data.get('username', '').strip()
    date = data.get('date', '').strip()
    description = data.get('description', '').strip()
    start_time = data.get('startTime', '').strip()
    end_time = data.get('endTime', '').strip()
    color = data.get('color', '').strip()
    recurrence = data.get('recurrence', 'none').strip()
    recurrence_count_str = data.get('recurrenceCount', '0').strip()
    category = data.get('category', '').strip()
    category_icon = data.get('categoryIcon', '').strip()
    department = data.get('department', '').strip()
    is_personal = data.get('isPersonal', False)
    
    if not recurrence:
        recurrence = "none"
    
    recurrence_count = 0
    if recurrence_count_str:
        try:
            recurrence_count = int(recurrence_count_str)
        except ValueError:
            recurrence_count = 0
    
    if color and not color.startswith('#'):
        color = color_name_to_hex(color)
    
    success = db_manager.add_event(
        username, date, description, start_time, end_time, color,
        is_personal, recurrence, category, category_icon, department, recurrence_count
    )
    
    return jsonify({"success": success})


@app.route('/api/events/<int:event_id>', methods=['PUT'])
def update_event(event_id):
    """Update an event"""
    data = request.get_json()
    description = data.get('description', '').strip()
    start_time = data.get('startTime', '').strip()
    end_time = data.get('endTime', '').strip()
    color = data.get('color', '').strip()
    
    if color and not color.startswith('#'):
        color = color_name_to_hex(color)
    
    success = db_manager.update_event(event_id, description, start_time, end_time, color)
    return jsonify({"success": success})


@app.route('/api/events/<int:event_id>', methods=['DELETE'])
def delete_event(event_id):
    """Delete an event"""
    success = db_manager.delete_event(event_id)
    return jsonify({"success": success})


@app.route('/api/events/search', methods=['GET'])
def search_events():
    """Search events"""
    username = request.args.get('username', '').strip()
    term = request.args.get('term', '').strip()
    religion = request.args.get('religion', '').strip()
    
    if not username or not term:
        return jsonify({"error": "Missing parameters"}), 400
    
    results = db_manager.search_events(username, term, religion)
    result_list = []
    for de in results:
        result_list.append({
            "date": de.date,
            "description": de.description
        })
    
    return jsonify({"results": result_list})


@app.route('/api/events/month', methods=['GET'])
def get_event_dates_for_month():
    """Get event dates for a month"""
    username = request.args.get('username', '').strip()
    year_str = request.args.get('year', '').strip()
    month_str = request.args.get('month', '').strip()
    
    if not username or not year_str or not month_str:
        return jsonify({"error": "Missing parameters"}), 400
    
    try:
        year = int(year_str)
        month = int(month_str)
        if month < 1 or month > 12:
            return jsonify({"error": "Invalid month"}), 400
        
        dates = db_manager.get_event_dates_for_month(username, year, month)
        return jsonify({"dates": dates})
    except ValueError:
        return jsonify({"error": "Invalid parameters"}), 400


@app.route('/api/events/all', methods=['GET'])
def get_all_events():
    """Get all events for a user"""
    username = request.args.get('username', '').strip()
    if not username:
        return jsonify({"error": "Missing parameters"}), 400
    
    all_events = db_manager.get_all_events(username)
    result = []
    for de in all_events:
        result.append({
            "date": de.date,
            "description": de.description
        })
    
    return jsonify({"events": result})


@app.route('/api/events/recurrence', methods=['GET'])
def get_events_with_recurrence():
    """Get events with recurrence"""
    username = request.args.get('username', '').strip()
    date = request.args.get('date', '').strip()
    
    if not username or not date:
        return jsonify({"error": "Missing parameters"}), 400
    
    events = db_manager.get_events_with_recurrence(username, date)
    result = []
    for event in events:
        result.append({
            "id": event.id,
            "description": event.description,
            "startTime": event.start_time,
            "endTime": event.end_time,
            "color": event.color,
            "isPersonal": event.is_personal,
            "recurrence": event.recurrence,
            "category": event.category,
            "categoryIcon": event.category_icon
        })
    
    return jsonify({"events": result})


@app.route('/api/holidays', methods=['GET'])
def get_holidays():
    """Get holidays for a date"""
    date = request.args.get('date', '').strip()
    religion = request.args.get('religion', '').strip()
    
    if not date:
        return jsonify({"error": "Missing parameters"}), 400
    
    holidays = db_manager.get_holidays(date, religion)
    return jsonify({"holidays": holidays})


@app.route('/api/holidays/month', methods=['GET'])
def get_holiday_dates_for_month():
    """Get holiday dates for a month"""
    religion = request.args.get('religion', '').strip()
    year_str = request.args.get('year', '').strip()
    month_str = request.args.get('month', '').strip()
    
    if not year_str or not month_str:
        return jsonify({"error": "Missing parameters"}), 400
    
    try:
        year = int(year_str)
        month = int(month_str)
        if month < 1 or month > 12:
            return jsonify({"error": "Invalid month"}), 400
        
        dates = db_manager.get_holiday_dates_for_month(religion, year, month)
        return jsonify({"dates": dates})
    except ValueError:
        return jsonify({"error": "Invalid parameters"}), 400


@app.route('/api/tasks', methods=['GET'])
def list_tasks():
    """List tasks for a user"""
    username = request.args.get('username', '').strip()
    if not username:
        return jsonify({"error": "Missing parameters"}), 400
    
    tasks = db_manager.list_tasks(username)
    result = []
    for task_str in tasks:
        parts = task_str.split('|')
        if len(parts) >= 4:
            result.append({
                "id": int(parts[0]),
                "title": parts[1],
                "due": parts[2],
                "completed": parts[3] == "1"
            })
    
    return jsonify({"tasks": result})


@app.route('/api/tasks', methods=['POST'])
def add_task():
    """Add a task"""
    data = request.get_json()
    username = data.get('username', '').strip()
    title = data.get('title', '').strip()
    due = data.get('due', '').strip()
    
    success = db_manager.add_task(username, title, due)
    return jsonify({"success": success})


@app.route('/api/tasks/<int:task_id>', methods=['PUT'])
def toggle_task(task_id):
    """Toggle task completion"""
    data = request.get_json()
    completed = data.get('completed', False)
    
    success = db_manager.toggle_task(task_id, completed)
    return jsonify({"success": success})


@app.route('/api/tasks/<int:task_id>', methods=['DELETE'])
def delete_task(task_id):
    """Delete a task"""
    success = db_manager.delete_task(task_id)
    return jsonify({"success": success})


@app.route('/api/teachers/search', methods=['GET'])
def search_teachers():
    """Search for teachers"""
    name = request.args.get('name', '').strip()
    university = request.args.get('university', '').strip()
    department = request.args.get('department', '').strip()
    
    teachers = db_manager.search_teachers(name, university, department)
    result = []
    for teacher in teachers:
        result.append({
            "username": teacher.username,
            "displayName": teacher.display_name,
            "university": teacher.university,
            "department": teacher.department
        })
    
    return jsonify({"teachers": result})


@app.route('/api/teachers/availability', methods=['GET'])
def get_teacher_availability():
    """Get teacher availability"""
    teacher = request.args.get('teacher', '').strip()
    if not teacher:
        return jsonify({"error": "Missing teacher"}), 400
    
    slots = db_manager.get_teacher_availability(teacher)
    result = []
    for slot in slots:
        result.append({
            "id": slot.id,
            "dayOfWeek": slot.day_of_week,
            "startTime": slot.start_time,
            "endTime": slot.end_time,
            "note": slot.note
        })
    
    return jsonify({"slots": result})


@app.route('/api/teachers/availability', methods=['POST'])
def add_teacher_availability():
    """Add teacher availability slot"""
    data = request.get_json()
    teacher = data.get('username', '').strip()
    day_str = data.get('dayOfWeek', '').strip()
    start_time = data.get('startTime', '').strip()
    end_time = data.get('endTime', '').strip()
    note = data.get('note', '').strip()
    clear_existing = data.get('clearExisting', '').strip()
    
    if not teacher or not day_str or not start_time or not end_time:
        return jsonify({"success": False, "error": "Missing parameters"}), 400
    
    try:
        day = int(day_str)
        if clear_existing == "true":
            db_manager.clear_teacher_availability(teacher)
        
        slot = TeacherAvailabilitySlot()
        slot.teacher_username = teacher
        slot.day_of_week = day
        slot.start_time = start_time
        slot.end_time = end_time
        slot.note = note
        
        success = db_manager.add_teacher_availability_slot(slot)
        return jsonify({"success": success})
    except ValueError:
        return jsonify({"success": False, "error": "Invalid day"}), 400


@app.route('/api/quote', methods=['GET'])
def get_quote():
    """Get quote for month"""
    month_str = request.args.get('month', '').strip()
    role = request.args.get('role', '').strip()
    religion = request.args.get('religion', '').strip()
    
    if not month_str or not role:
        return jsonify({"quote": "May your month be filled with joy and purpose."})
    
    try:
        month = int(month_str)
        if month < 1 or month > 12:
            month = 1
        quote = get_quote_for_month(month, role, religion)
        return jsonify({"quote": quote})
    except ValueError:
        return jsonify({"quote": "May your month be filled with joy and purpose."})


@app.route('/api/search/suggestions', methods=['GET'])
def get_search_suggestions():
    """Get search suggestions"""
    username = request.args.get('username', '').strip()
    role = request.args.get('role', '').strip()
    religion = request.args.get('religion', '').strip()
    
    if not username or not role:
        return jsonify({"error": "Missing parameters"}), 400
    
    suggestions = db_manager.get_all_searchable_strings(username, role, religion)
    return jsonify({"suggestions": suggestions})


@app.route('/api/search/trie', methods=['GET'])
def search_trie():
    """Search using trie"""
    term = request.args.get('term', '').strip()
    if not term:
        return jsonify({"eventIds": []})
    
    event_ids = db_manager.search_events_with_trie(term)
    return jsonify({"eventIds": event_ids})


@app.route('/api/universities', methods=['GET'])
def get_universities():
    """Get all universities"""
    universities = db_manager.get_all_universities()
    return jsonify({"universities": universities})


@app.route('/api/departments', methods=['GET'])
def get_departments():
    """Get all departments"""
    departments = db_manager.get_all_departments()
    result = {}
    for code, name in departments:
        result[code] = name
    return jsonify({"departments": result})


@app.route('/api/validate/roll', methods=['GET'])
def validate_roll():
    """Validate roll number"""
    roll = request.args.get('roll', '').strip()
    valid = db_manager.is_valid_roll_number(roll)
    dept = db_manager.get_department_from_roll(roll)
    dept_name = db_manager.get_department_name(dept)
    
    return jsonify({
        "valid": valid,
        "department": dept,
        "departmentName": dept_name
    })


if __name__ == '__main__':
    # Ensure database is initialized
    if not init_database():
        exit(1)
    
    # Run Flask app
    port = int(os.environ.get('PORT', 8080))
    app.run(host='0.0.0.0', port=port, debug=False)

