# Time Weaver - Python/Netlify Version

A full-stack calendar application converted from C++ to Python, ready for Netlify deployment.

## Features

- **Multi-User System**: Personal, Student, Teacher, and Admin roles
- **Event Management**: Create, edit, delete events with time slots
- **Recurring Events**: Daily, weekly, monthly, and yearly recurrence patterns
- **Holiday Calendar**: Religion-based holiday tracking
- **Search & Filter**: Fast event search with Trie data structure
- **Theme System**: 5 beautiful themes
- **Academic Features**: University management, department system, teacher availability

## Project Structure

```
.
├── app.py                 # Flask application (main server)
├── database.py            # Database manager (SQLite)
├── dsa_structures.py      # Data structures (Trie, LRU Cache, etc.)
├── requirements.txt       # Python dependencies
├── netlify.toml          # Netlify configuration
├── runtime.txt           # Python version
├── HolidayData.csv       # Holiday data
├── calendar.db           # SQLite database (created on first run)
└── public/               # Frontend files
    ├── index.html
    ├── app.js
    ├── app_academic_features.js
    └── style.css
```

## Local Development

### Prerequisites
- Python 3.11+
- pip

### Setup

1. **Install dependencies:**
```bash
pip install -r requirements.txt
```

2. **Run the application:**
```bash
python app.py
```

3. **Access the application:**
```
http://localhost:8080
```

The database will be created automatically on first run.

## Netlify Deployment

### Option 1: Serverless Functions (Recommended)

1. **Create `.netlify/functions/api.py`:**
```python
from app import app

def handler(event, context):
    return app(event, context)
```

2. **Deploy to Netlify:**
   - Connect your repository to Netlify
   - Set build command: `pip install -r requirements.txt`
   - Set publish directory: `public`
   - Deploy!

### Option 2: Flask App (Alternative)

For Netlify, you may need to use a serverless adapter. However, Netlify primarily supports serverless functions.

**Recommended: Use Vercel or Railway for Flask deployment, or convert to serverless functions.**

## API Endpoints

All endpoints are under `/api/`:

- `POST /api/user/add` - Register user
- `POST /api/user/validate` - Login
- `GET /api/events` - Get events
- `POST /api/events` - Create event
- `PUT /api/events/<id>` - Update event
- `DELETE /api/events/<id>` - Delete event
- And many more...

See the original documentation for full API details.

## Database

The application uses SQLite (`calendar.db`). On first run:
- Tables are created automatically
- Universities and departments are populated
- Holidays are loaded from `HolidayData.csv`

## Default Credentials

- **Admin**: username: `admin`, password: `admin123`

## Converting from C++

This project was converted from C++ to Python:
- `server.cpp` → `app.py` (Flask)
- `database.cpp` → `database.py` (SQLite)
- `dsa_structures.h` → `dsa_structures.py` (Python classes)

All functionality has been preserved.

## Notes for Netlify

1. **Database**: SQLite files are ephemeral on Netlify. Consider using:
   - Netlify's serverless functions with external database (PostgreSQL, etc.)
   - Or use a database service like Supabase, PlanetScale, etc.

2. **File Storage**: For production, consider:
   - Moving database to external service
   - Using environment variables for configuration

3. **Serverless Functions**: For better Netlify compatibility, consider converting Flask routes to individual serverless functions.

## License

Educational project - Data Structures & Algorithms course.

